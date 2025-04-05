#ifndef __ISP_TYPE_H__
#define __ISP_TYPE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*----------------------------------------------*
 * The common data type, will be used in the whole project.*
 *----------------------------------------------*/

typedef unsigned char           ISP_U8;
typedef unsigned short          ISP_U16;
typedef unsigned int            ISP_U32;

typedef signed char             ISP_S8;
typedef short                   ISP_S16;
typedef int                     ISP_S32;

typedef unsigned long long  	ISP_U64;
typedef long long           	ISP_S64;

typedef char                    ISP_CHAR;
#define ISP_VOID                 void

/*----------------------------------------------*
 * const defination                             *
 *----------------------------------------------*/
typedef enum {
    ISP_FALSE = 0,
    ISP_TRUE  = 1,
} ISP_BOOL;

#ifndef NULL
    #define NULL    0L
#endif

#define ISP_NULL     0L
#define ISP_SUCCESS  0
#define ISP_FAILURE  (-1)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __ISP_TYPE_H__ */

