/*****************************************************************************
------------------------------------------------------------------------------
xm_ia_rule.h
产品名:    算法规则
模块名:    公共文件
生成日期:  2020-08-28
作者:     冯世益
文件描述:  规则对外公共头文件
*****************************************************************************/
#ifndef _XM_IA_RULE_H_
#define _XM_IA_RULE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "xm_ia_comm.h"

#define XM_IA_RULE_NUM_MAX           (4)         /* 规则人形算法的最大规则数 */

/* 规则选项 */
typedef enum
{
    IA_TRIPWIRE,                        /* 绊线 */
    IA_PERIMETER,                       /* 禁区 */
    IA_PERIMETER_V1,                    /* 新禁区 */
    IA_RULE_NUM,
}XM_IA_RULE_E;

/* 触发报警点选项 */
typedef enum
{
    IA_ALARM_GROUND,                    /* 以地面为参照，报警根据目标是否在警戒框所对应的地面区域内 */
    IA_ALARM_BOX,                       /* 报警根据目标框对应的四个顶点是否在警戒框内 */
    IA_ALARM_NUM,
}XM_IA_ALARM_POINT_E;

/* 禁区状态 */
typedef enum
{
    IA_RULE_STATUS_INSIDE,                        /* 禁区内 */
    IA_RULE_STATUS_OUTSIDE,                       /* 禁区外 */
}XM_IA_STATUS_PERIMETER_E;

/* 拌线状态 */
typedef enum
{
    IA_RULE_STATUS_LEFT,                        /* 绊线左侧 */
    IA_RULE_STATUS_RIGHT,                       /* 绊线右侧 */
}XM_IA_STATUS_TRIPWIRE_E;

/* 状态联合体 */
typedef struct
{
    XM_IA_RULE_E eRuleType;             /* 规则类型 */
    union
    {
        XM_IA_STATUS_TRIPWIRE_E eStatusTripwire;       /* 绊线状态 */
        XM_IA_STATUS_PERIMETER_E eStatusPerimeter;     /* 禁区状态 */
    }unRuleStatus;

    unsigned int uiReserved[16];        /* 保留规则 */
}XM_IA_RULE_STATUS_S;

/* 线规则结构体 */
typedef struct
{
    XM_IA_LINE_S stLine;                /* 绊线位置X1,Y1为起始点,X2,Y2为终止点(百分比表示0~8192对应0～100%) */
    XM_IA_DIRECTION_E eAlarmDirect;     /* 顺起始点终止点法线方向为正向,逆起始点终止点法线方向为反向 */

    unsigned int uiReserved[16];        /* 保留规则 */
}XM_IA_RULE_LINE_S;

/* 区域规则结构体 */
typedef struct
{
    int iBoundaryPtsNum;                /* 禁区描点数 */
    XM_IA_POINT_S astBoundaryPts[8];    /* 禁区描点位置(百分比表示0~8192对应0～100%) */
    XM_IA_DIRECTION_E eAlarmDirect;     /* 进入禁区为正向,离开禁区为反向 */

    unsigned int uiReserved[16];        /* 保留规则 */
}XM_IA_RULE_REGION_S;

/* 区域规则结构体 */
typedef struct
{
    int iBoundaryPtsNum;                    /* 禁区描点数 */
    XM_IA_POINT_S astBoundaryPts[16];       /* 禁区描点位置(百分比表示0~8192对应0～100%) */
    XM_IA_DIRECTION_E eAlarmDirect;         /* 进入禁区为正向,离开禁区为反向 */

    unsigned int uiReserved[8];             /* 保留规则 */
}XM_IA_RULE_REGIONV1_S;

/* 规则联合体 */
typedef union
{
    XM_IA_RULE_LINE_S stRuleLine;               /* 绊线规则 */
    XM_IA_RULE_REGION_S stRuleRegion;           /* 禁区规则 */
    XM_IA_RULE_REGIONV1_S stRuleRegionV1;     /* 新禁区规则，增加禁区端点数目到16 */
    unsigned int uiReserved[16];                /* 保留规则 */
}XM_IA_PD_RULE_U;

/* 所有规则结构体 */
typedef struct
{
    unsigned char ucRuleEnable;         /* 规则是否使能:0不使能/1使能 */

    XM_IA_RULE_E eRuleType;             /* 规则类型 */
    XM_IA_PD_RULE_U unPdRule;           /* 具体规则 */
    XM_IA_ALARM_POINT_E eAlarmType;     /* 触发报警点选项 */

    unsigned int uiReserved[15];        /* 保留规则:使能 */
}XM_IA_PD_RULES_S;

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif
