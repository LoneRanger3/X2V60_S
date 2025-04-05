#ifndef _XM_IA_CVCHG_INTERFACE_H_
#define _XM_IA_CVCHG_INTERFACE_H_
#ifdef  __cplusplus
extern "C" {
#endif

    typedef enum
    {
        XM_IA_CVCHG_CLOCKWISE = 0,         /* 顺时针旋转90度 */
        XM_IA_CVCHG_CLOCKWISE_INVERT = 1   /* 逆时针旋转90度/顺时针旋转270度 */

    }XM_IA_CVCHG_TYPE_E;

    int XM_IA_Resize_run_easy(void *imageDataIn, void *imageDataOut, int pos, int mirror, XM_IA_CVCHG_TYPE_E eRotate);
#ifdef  __cplusplus
}
#endif
#endif

