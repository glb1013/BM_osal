#ifndef TYPE_H
#define TYPE_H

enum {
    RET_FAILURE = 0,
    RET_ERROR = 0,
    RET_SUCCESS = 1,
    RET_TASK_INVALID,
    RET_EVENT_INVALID_ID,
    RET_TIMER_INVALID,
    RET_MSG_INVALID_POINTER,
    RET_MSG_BUFFER_NOT_AVAIL,
};
enum {
    TASK_ID_INVALID = 0,
    TASK_ID_USER_START = 1,
    TASK_ID_USER_END = 255
};

typedef unsigned char       bool;

//Ð¾Æ¬Ó²¼þ×Ö³¤
typedef unsigned int        halDataAlign_t;

// Unsigned numbers
typedef unsigned char       uint8;
typedef unsigned char       byte;
typedef unsigned short      uint16;
typedef unsigned short      int16U;
typedef unsigned int        uint32;
typedef unsigned int        int32U;

// Signed numbers
typedef signed char         int8;
typedef signed short        int16;
typedef signed int          int32;

#ifndef ARRAY_NULL
#define ARRAY_NULL '\0'
#endif

#ifndef TRUE
#define TRUE       1
#endif

#ifndef FALSE
#define FALSE       0
#endif

#ifndef OPEN
#define OPEN       1
#endif

#ifndef CLOSE
#define CLOSE      0
#endif

#ifndef NULL
#define NULL       ((void*) 0 )
#endif

#ifndef HIGH
#define HIGH       1
#endif

#ifndef LOW
#define LOW        0
#endif

#ifndef SUCCESS
#define SUCCESS     1
#endif

#ifndef ERROR
#define ERROR       0
#endif

#define BIT_MASK(n)         (1U << (n))
#define BIT_SET(v,n)        ((v) | BIT_MASK(n))
#define BIT_CLR(v,n)        ((v) & ~BIT_MASK(n))

#endif
