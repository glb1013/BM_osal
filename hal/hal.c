/**
 * @file hal.c
 * @brief osal hal adapter
 * @version 0.1
 * @date 2019-07-25
 * @author WatWu
 */
#include <string.h>
#include "osal.h"
#include "osal_memory.h"

 /*********************************************************************
  * @fn osal_strlen
  *
  * @brief
  *
  *   Calculates the length of a string.  The string must be null
  *   terminated.
  *
  * @param   char *pString - pointer to text string
  *
  * @return  int - number of characters
  */
int osal_strlen(char* pString)
{
    return (int)(strlen(pString));
}

/*********************************************************************
 * @fn osal_memcpy
 *
 * @brief
 *
 *   Generic memory copy.
 *
 *   Note: This function differs from the standard memcpy(), since
 *         it returns the pointer to the next destination uint8. The
 *         standard memcpy() returns the original destination address.
 *
 * @param   dst - destination address
 * @param   src - source address
 * @param   len - number of bytes to copy
 *
 * @return  pointer to end of destination buffer
 */
void* osal_memcpy(void* dst, const void* src, unsigned int len)
{
    uint8* pDst;
    const uint8* pSrc;

    pSrc = src;
    pDst = dst;

    while (len--)
        *pDst++ = *pSrc++;

    return (pDst);
}

/*********************************************************************
 * @fn osal_revmemcpy
 *
 * @brief   Generic reverse memory copy.  Starts at the end of the
 *   source buffer, by taking the source address pointer and moving
 *   pointer ahead "len" bytes, then decrementing the pointer.
 *
 *   Note: This function differs from the standard memcpy(), since
 *         it returns the pointer to the next destination uint8. The
 *         standard memcpy() returns the original destination address.
 *
 * @param   dst - destination address
 * @param   src - source address
 * @param   len - number of bytes to copy
 *
 * @return  pointer to end of destination buffer
 */
void* osal_revmemcpy(void* dst, const void* src, unsigned int len)
{
    uint8* pDst;
    const uint8* pSrc;

    pSrc = src;
    pSrc += (len - 1);
    pDst = dst;

    while (len--)
        *pDst++ = *pSrc--;

    return (pDst);
}

/*********************************************************************
 * @fn osal_memdup
 *
 * @brief   Allocates a buffer [with osal_mem_alloc()] and copies
 *          the src buffer into the newly allocated space.
 *
 * @param   src - source address
 * @param   len - number of bytes to copy
 *
 * @return  pointer to the new allocated buffer, or NULL if
 *          allocation problem.
 */
void* osal_memdup(const void* src, unsigned int len)
{
    uint8* pDst;

    pDst = osal_mem_alloc(len);
    if (pDst)
    {
        osal_memcpy(pDst, src, len);
    }

    return ((void*)pDst);
}

/*********************************************************************
 * @fn osal_memcmp
 *
 * @brief
 *
 *   Generic memory compare.
 *
 * @param   src1 - source 1 addrexx
 * @param   src2 - source 2 address
 * @param   len - number of bytes to compare
 *
 * @return  TRUE - same, FALSE - different
 */
uint8 osal_memcmp(const void* src1, const void* src2, unsigned int len)
{
    const uint8* pSrc1;
    const uint8* pSrc2;

    pSrc1 = src1;
    pSrc2 = src2;

    while (len--)
    {
        if (*pSrc1++ != *pSrc2++)
            return FALSE;
    }
    return TRUE;
}

/*********************************************************************
 * @fn osal_memset
 *
 * @brief
 *
 *   Set memory buffer to value.
 *
 * @param   dest - pointer to buffer
 * @param   value - what to set each uint8 of the message
 * @param   size - how big
 *
 * @return  pointer to destination buffer
 */
void* osal_memset(void* dest, uint8 value, int len)
{
    return memset(dest, value, len);
}
