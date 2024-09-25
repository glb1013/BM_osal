
#include "osal_type.h"
#include "osal.h"
#include "osal_memory.h"

#if ( MAXMEMHEAP >= 32768 )             //�ڴ����Ĭ��ʹ��15λ���ݱ�ʶ������ܹ���32768�ֽ�
#error MAXMEMHEAP is too big to manage!
#endif

//���ҵ����ʵ��ڴ��֮�󣬾�Ҫ�����Ƿ�Դ��ڴ����зָ
//����ڴ�����Ļ���Ȼ������ڴ���˷ѡ�
//����ڴ��Ĵ�С��ȥҪ������ڴ���ֵ����OSALMEM_MIN_BLKSZ(4byte)��
//��ָ���ڴ�顣����ʹ���ָ�������ڴ������ƿ�ͷ��
#if !defined ( OSALMEM_MIN_BLKSZ )
#define OSALMEM_MIN_BLKSZ    4
#endif

#if !defined ( OSALMEM_SMALL_BLKSZ )
#define OSALMEM_SMALL_BLKSZ  16     //�̶��ڴ��������Ĺ̶����ȣ�16�ֽ�
#endif

#if !defined ( OSALMEM_GUARD )
#define OSALMEM_GUARD  TRUE         // TBD - Hacky workaround til Bugzilla 1252 is fixed!
#define OSALMEM_READY  0xE2
#endif

//�ڴ���䷵�ص���һ��ָ����������ָ�룬ָ��ĳ������ڴ����ͷ���ڴ���뷽ʽ�нϴ��һ����
//�����������Ҳ����С���䵥Ԫ�ĳ��ȡ���OSAL�����ĳ�����16bit��
//�˴���Ҫ����ʵ�ʱ��뻷���޸ģ�ȷ��osalMemHdr_t����Ϊ16bit������
typedef halDataAlign_t  osalMemHdr_t;

/*********************************************************************
 * CONSTANTS
 */

#define OSALMEM_IN_USE  0x8000        //�ڴ����ͷ��16λ�����λ��ʶ���ڴ���Ƿ�ʹ��

/* This number sets the size of the small-block bucket. Although profiling
 * shows max simultaneous alloc of 16x18, timing without profiling overhead
 * shows that the best worst case is achieved with the following.
   ���ݳ���ĸ�Ҫ����������ͬʱ����Ĵ�С��16x18, ��������г����Ҫ������
   �ڶ�ʱ����ϵͳ�Ŀ���ʱ��������������������ڴ�����С��232(byte)�ĳ��ȡ�
   ���԰ѹ̶���������ĳ��ȶ���Ϊ232��byte����
 */
#define SMALLBLKHEAP    232

//�ڴ���䷵�ص���һ��ָ����������ָ�룬ָ��ĳ������ڴ����ͷ���ڴ���뷽ʽ�нϴ��һ����
//�����������Ҳ����С���䵥Ԫ�ĳ��ȡ���ЩOSAL�����ĳ�����16bit��
//�˴���Ҫ����ʵ�ʱ��뻷���޸ģ�ȷ��osalMemHdr_t����Ϊ16bit������
// To maintainalignment of the pointer returned, reserve the greater
// space for the memory block header.
#define HDRSZ  ( (sizeof ( halDataAlign_t ) > sizeof( osalMemHdr_t )) ? \
                  sizeof ( halDataAlign_t ) : sizeof( osalMemHdr_t ) )

#if ( OSALMEM_GUARD )
static byte ready = 0;
#endif

static osalMemHdr_t *ff1;  // First free block in the small-block bucket.
static osalMemHdr_t *ff2;  // First free block after the small-block bucket.

#if defined( EXTERNAL_RAM )
static byte  *theHeap = (byte *)EXT_RAM_BEG;
#else
static halDataAlign_t _theHeap[ MAXMEMHEAP / sizeof(halDataAlign_t) ];
// static __align(32) halDataAlign_t _theHeap[ MAXMEMHEAP / sizeof( halDataAlign_t ) ];
//����ʵ��ʹ�õ�оƬ�趨�ڴ���룬__align(32)���ǳ���Ҫ������
static byte  *theHeap = (byte *)_theHeap;
#endif

#if OSALMEM_METRICS
static uint16 blkMax;  // Max cnt of all blocks ever seen at once.
static uint16 blkCnt;  // Current cnt of all blocks.
static uint16 blkFree; // Current cnt of free blocks.
static uint16 memAlo;  // Current total memory allocated.
static uint16 memMax;  // Max total memory ever allocated at once.
#endif
/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn osal_mem_init
 *
 * @brief   Initialize the heap memory management system.
 *
 * @param   void
 *
 * @return  void
 */
void osal_mem_init(void)
{
    osalMemHdr_t *tmp;
    // Setup a NULL block at the end of the heap for fast comparisons with zero.
    //�����ڴ�����������ֽ����㣬��������ڴ�������
    tmp = (osalMemHdr_t *)theHeap + (MAXMEMHEAP / HDRSZ) - 1;
    *tmp = 0;

    // Setup a small-block bucket.
    //���ù̶������ڴ������ȣ�232�ֽڣ��ڴ�����һ���ֱ����������
    tmp = (osalMemHdr_t *)theHeap;
    *tmp = SMALLBLKHEAP;

    // Setup the wilderness.
    //���ÿɱ�����ڴ������ȣ��ڴ�����һ���ֱ����������
    tmp = (osalMemHdr_t *)theHeap + (SMALLBLKHEAP / HDRSZ);
    *tmp = ((MAXMEMHEAP / HDRSZ) * HDRSZ) - SMALLBLKHEAP - HDRSZ;

#if ( OSALMEM_GUARD )
    ready = OSALMEM_READY;
#endif

    // Setup a NULL block that is never freed so that the small-block bucket
    // is never coalesced with the wilderness.
    ff1 = tmp;
    ff2 = osal_mem_alloc(0);
    ff1 = (osalMemHdr_t *)theHeap;
    /*
    ������佫�ڹ̶����ȷ�������Ϳɱ��������֮������һ��0��С���ڴ�飬
    �൱���ڹ̶���������Ϳɱ��������֮�䱣����һ��һֱ����ʹ��״̬��
    ����ָ�򳤶�Ϊ0��һ���ڴ������ƿ顣
    ����ڴ����ǽ���������������뿪������Ϳɱ䳤�ȷ�������ϲ���
    */

#if ( OSALMEM_METRICS )
    /* Start with the small-block bucket and the wilderness - don't count the
     * end-of-heap NULL block nor the end-of-small-block NULL block.
     */
    blkCnt = blkFree = 2;
    memAlo = 0;
#endif
}

/*********************************************************************
 * @fn osal_mem_kick
 *
 * @brief   Kick the ff1 pointer out past the long-lived OSAL Task blocks.
 *          Invoke this once after all long-lived blocks have been allocated -
 *          presently at the end of osal_init_system().
 *          ʹFF1�����̶���������ָ��FF2(�ɱ䳤������)��Ҳ���������˼��
 *          ����ڹ̶�����������û�������ڴ�ɹ���
 *          ���ô˺����������޸�ָ��̶����������ָ��ʹָ��ָ��ɱ��������
 *          Ȼ���ٵ���osal_mem_alloc �����ڿɱ䳤�ȷ��������н����ڴ���䡣
 *
 * @param   void
 *
 * @return  void
 */
void osal_mem_kick(void)
{
    //halIntState_t  intState;
    HAL_ENTER_CRITICAL_SECTION();  // Hold off interrupts.

    /* Logic in osal_mem_free() will ratchet ff1 back down to the first free
     * block in the small-block bucket.
     */
    ff1 = ff2;

    HAL_EXIT_CRITICAL_SECTION();  // Re-enable interrupts.
}

/*********************************************************************
 * @fn osal_mem_alloc
 *
 * @brief   Implementation of the allocator functionality.
 *
 * @param   size - number of bytes to allocate from the heap.
 *
 * @return  void * - pointer to the heap allocation; NULL if error or failure.
 */
void *osal_mem_alloc(uint16 size)
{
    osalMemHdr_t  *prev = NULL;
    osalMemHdr_t  *hdr;
    uint16  tmp;
    byte coal = 0;

#if ( OSALMEM_GUARD )
    // Try to protect against premature use by HAL / OSAL.
    if(ready != OSALMEM_READY)
    {
        osal_mem_init();
    }
#endif

    size += HDRSZ;

    // Calculate required bytes to add to 'size' to align to halDataAlign_t.
    //����ʵ�ʵ�оƬ���ֳ�halDataAlign_t�����ֽڶ���
    if(sizeof(halDataAlign_t) == 2)
    {
        size += (size & 0x01);
    }
    else if(sizeof(halDataAlign_t) != 1)
    {
        const byte mod = size % sizeof(halDataAlign_t);

        if(mod != 0)
        {
            size += (sizeof(halDataAlign_t) - mod);
        }
    }

    HAL_ENTER_CRITICAL_SECTION();       // Hold off interrupts.

    // Smaller allocations are first attempted in the small-block bucket.
    if(size <= OSALMEM_SMALL_BLKSZ)
    {
        hdr = ff1;
    }
    else
    {
        hdr = ff2;
    }
    tmp = *hdr;

    do
    {
        if(tmp & OSALMEM_IN_USE)
        {
            tmp ^= OSALMEM_IN_USE;          //��Ƭ�ѱ�ʹ�ã��ó�����
            coal = 0;                       //�ҵ����ڴ��־���㣬��Ϊ������һƬ��ʹ�õ��ڴ�
        }
        else                                //��Ƭδ��ʹ��
        {
            if(coal != 0)                   //���ֲ������ҵ����ڴ棬���ռ䲻��
            {
#if ( OSALMEM_METRICS )
                blkCnt--;                   //�ڴ�ϲ������ڴ�������һ
                blkFree--;                  //�ڴ�ϲ��������ڴ�������һ
#endif
                *prev += *hdr;              //���ϱ����ҵ��Ŀ��ڴ��С

                if(*prev >= size)           //���Ϻ��ڴ��С����������Ҫ
                {
                    hdr = prev;             //���ظ��ڴ��
                    tmp = *hdr;             //�ó�����
                    break;
                }
            }
            else                            //���ֲ���δ�ҵ����ڴ�
            {
                if(tmp >= size)             //��Ƭ�ڴ��С����������������ѭ��
                {
                    break;
                }

                coal = 1;                   //��Ƭ�ڴ��С���������󣬱���ҵ�һ����ڴ�
                prev = hdr;                 //��¼���ڴ�
            }
        }

        hdr = (osalMemHdr_t *)((byte *)hdr + tmp);  //ƫ������һƬ�ڴ�����

        tmp = *hdr;                         //��ȡ�����򳤶�
        if(tmp == 0)
        {
            hdr = ((void *)NULL);
            break;
        }
    } while(1);

    if(hdr != ((void *)NULL))
    {
        tmp -= size;                        //���������ʣ�೤��
        // Determine whether the threshold for splitting is met.
        if(tmp >= OSALMEM_MIN_BLKSZ)        //ʣ��ռ������С����ռ䣬�ָ��ڴ湩�´�����
        {
            // Split the block before allocating it.
            osalMemHdr_t *next = (osalMemHdr_t *)((byte *)hdr + size);  //ƫ��
            *next = tmp;                    //��¼δʹ������ʣ�೤��
            *hdr = (size | OSALMEM_IN_USE); //��־�������������ѱ�ʹ�ã�����¼����ʹ�ó���

#if ( OSALMEM_METRICS )
            blkCnt++;                       //�ڴ�ָ���ڴ�������һ
            if(blkMax < blkCnt)
            {
                blkMax = blkCnt;            //�����ڴ���������ֵ
            }
            memAlo += size;                 //���������ڴ��С
#endif
        }
        else
        {
#if ( OSALMEM_METRICS )
            memAlo += *hdr;
            blkFree--;                      //�ڴ治�ָ�����ڴ�������һ
#endif

            *hdr |= OSALMEM_IN_USE;
        }

#if ( OSALMEM_METRICS )
        if(memMax < memAlo)
        {
            memMax = memAlo;
        }
#endif

        hdr++;                              //ƫ�ƣ�����ʵ��������ڴ��ַ
    }

    HAL_EXIT_CRITICAL_SECTION();            // Re-enable interrupts.

    return (void *)hdr;
}

/*********************************************************************
 * @fn osal_mem_free
 *
 * @brief   Implementation of the de-allocator functionality.
 *
 * @param   ptr - pointer to the memory to free.
 *
 * @return  void
 */
void osal_mem_free(void *ptr)
{
    osalMemHdr_t  *currHdr;
    //halIntState_t   intState;

#if ( OSALMEM_GUARD )
    // Try to protect against premature use by HAL / OSAL.
    if(ready != OSALMEM_READY)
    {
        osal_mem_init();
    }
#endif

    HAL_ENTER_CRITICAL_SECTION();  // Hold off interrupts.

    currHdr = (osalMemHdr_t *)ptr - 1;

    *currHdr &= ~OSALMEM_IN_USE;

    if(ff1 > currHdr)
    {
        ff1 = currHdr;
    }

#if OSALMEM_METRICS
    memAlo -= *currHdr;
    blkFree++;
#endif

    HAL_EXIT_CRITICAL_SECTION();  // Re-enable interrupts.
}

#if OSALMEM_METRICS
/*********************************************************************
 * @fn osal_heap_block_max
 *
 * @brief   Return the maximum number of blocks ever allocated at once.
 *
 * @param   none
 *
 * @return  Maximum number of blocks ever allocated at once.
 */
uint16 osal_heap_block_max(void)
{
    return blkMax;
}

/*********************************************************************
 * @fn osal_heap_block_cnt
 *
 * @brief   Return the current number of blocks now allocated.
 *
 * @param   none
 *
 * @return  Current number of blocks now allocated.
 */
uint16 osal_heap_block_cnt(void)
{
    return blkCnt;
}

/*********************************************************************
 * @fn osal_heap_block_free
 *
 * @brief   Return the current number of free blocks.
 *
 * @param   none
 *
 * @return  Current number of free blocks.
 */
uint16 osal_heap_block_free(void)
{
    return blkFree;
}

/*********************************************************************
 * @fn osal_heap_mem_used
 *
 * @brief   Return the current number of bytes allocated.
 *
 * @param   none
 *
 * @return  Current number of bytes allocated.
 */
uint16 osal_heap_mem_used(void)
{
    return memAlo;
}

/*********************************************************************
 * @fn osal_heap_high_water
 *
 * @brief   Return the highest byte ever allocated in the heap.
 *
 * @param   none
 *
 * @return  Highest number of bytes ever used by the stack.
 */
uint16 osal_heap_high_water(void)
{
#if ( OSALMEM_METRICS )
    return memMax;
#else
    return MAXMEMHEAP;
#endif
}

//�����ڴ�ʹ����
uint16 osal_heap_mem_usage_rate(void)
{
    return (uint16)(memAlo / (MAXMEMHEAP / 100));
}

#endif
