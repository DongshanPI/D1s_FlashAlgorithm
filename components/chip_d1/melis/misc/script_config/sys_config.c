/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                Config system
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : sys_config.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-4-28
* Descript: system config sevice functions.
* Update  : date                auther      ver     notes
*           2011-4-28 20:57:56  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include <log.h>
#include <script.h>
#include "typedef.h"

extern int32_t      esCFG_Exit(void);
extern int32_t      esCFG_Exit_Ex(intptr_t* parser);
extern int32_t      esCFG_GetGPIOSecData(char *GPIOSecName, void *pGPIOCfg, int32_t GPIONum);
extern int32_t      esCFG_GetGPIOSecData_Ex(intptr_t* parser, char *GPIOSecName, void *pGPIOCfg, int32_t GPIONum);
extern int32_t      esCFG_GetGPIOSecKeyCount(char *GPIOSecName);
extern int32_t      esCFG_GetGPIOSecKeyCount_Ex(intptr_t* parser, char *GPIOSecName);
extern int32_t      esCFG_GetKeyValue(char *SecName, char *KeyName, int32_t Value[], int32_t Count);
extern int32_t      esCFG_GetKeyValue_Ex(intptr_t* parser, char *KeyName, int32_t Value[], int32_t Count);
extern int32_t      esCFG_GetSecCount(void);
extern int32_t      esCFG_GetSecCount_Ex(intptr_t* parser);
extern int32_t      esCFG_GetSecKeyCount(char *SecName);
extern int32_t      esCFG_GetSecKeyCount_Ex(intptr_t* parser, char *SecName);
extern int32_t      esCFG_Init(uint8_t *CfgVAddr, uint32_t size);
extern intptr_t*    esCFG_Init_Ex(char *path);

static intptr_t* g_parser = NULL;
/*
*********************************************************************************************************
*                                       INITIALIZE CONFIG SYSTEM
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int32_t esCFG_Init(uint8_t *CfgVAddr, uint32_t size)
{
    g_parser = script_parser_init((char *)(CfgVAddr), size);
    if (NULL == g_parser)
    {
        __err("initialize config system failed.");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       EXIT CONFIG SYSTEM
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Note       :
*********************************************************************************************************
*/
int32_t esCFG_Exit(void)
{
    if (NULL != g_parser)
    {
        script_parser_exit(g_parser);
        g_parser = NULL;
    }

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       �����������ƺ��Ӽ����ƻ�ȡ�ű�����
*
* Description:
*
* Arguments  : main_name    ��������
*
*              sub_name     �Ӽ�����
*
*              value        ������ݵ�buffer
*
*              count        buffer��������
*
*
* Returns    : ��ȡ�����Ƿ�ɹ�
*
* Notes      :
*********************************************************************************************************
*/
int32_t esCFG_GetKeyValue(char *SecName, char *KeyName, int32_t Value[], int32_t Count)
{
    if (NULL == g_parser)
    {
        return EPDK_FAIL;
    }

    return  script_parser_fetch(g_parser, SecName, KeyName, (int *)Value, Count);
}

/*
*********************************************************************************************************
*                                       �����������ƣ���ȡ�����µ��Ӽ��ܹ�����
*
* Description:
*
* Arguments  : main_name    ��������
*
*
*
* Returns    : ����ɹ��������Ӽ�����
*              ���ʧ�ܣ����ظ���
*
* Notes      :
*********************************************************************************************************
*/
int32_t esCFG_GetSecKeyCount(char *SecName)
{
    if (NULL == g_parser)
    {
        return EPDK_FAIL;
    }

    return  script_parser_subkey_count(g_parser, SecName);
}

/*
*********************************************************************************************************
*                                       ��ȡ�ܹ������ĸ���
*
* Description:
*
* Arguments  :
*
*
*
* Returns    : ����ɹ���������������
*              ���ʧ�ܣ����ظ���
*
* Notes      :
*********************************************************************************************************
*/
int32_t esCFG_GetSecCount(void)
{
    if (NULL == g_parser)
    {
        return EPDK_FAIL;
    }

    return  script_parser_mainkey_count(g_parser);
}

/*
*********************************************************************************************************
*                                       �����������ƣ���ȡ�����µ�GPIO�����ܹ�����
*
* Description:
*
* Arguments  : main_name    ��������
*
*
*
* Returns    : ����ɹ���������������
*              ���ʧ�ܣ����ظ���
*
* Notes      :
*********************************************************************************************************
*/
int32_t esCFG_GetGPIOSecKeyCount(char *GPIOSecName)
{
    if (NULL == g_parser)
    {
        return EPDK_FAIL;
    }

    return  script_parser_mainkey_get_gpio_count(g_parser, GPIOSecName);
}

/*
*********************************************************************************************************
*                                       �����������ƣ���ȡ�����µ�GPIO���͵���������
*
* Description:
*
* Arguments  : main_name    ��������
*
*              gpio_cfg     ���GPIO������Ϣ��buffer
*
*              gpio_count   GPIO���ܹ�����
*
*
* Returns    : ����ɹ������سɹ���־
*              ���ʧ�ܣ����ظ���
*
* Notes      :
*********************************************************************************************************
*/
int32_t esCFG_GetGPIOSecData(char *GPIOSecName, void *pGPIOCfg, int32_t GPIONum)
{
    if (NULL == g_parser)
    {
        return EPDK_FAIL;
    }

    return  script_parser_mainkey_get_gpio_cfg(g_parser, GPIOSecName, pGPIOCfg, GPIONum);
}
