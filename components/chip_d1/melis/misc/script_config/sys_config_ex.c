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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <log.h>
#include <script.h>
#include "typedef.h"
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
intptr_t* esCFG_Init_Ex(char *path)
{
    intptr_t*   parser = NULL;
    __hdle      fp = NULL;
    char        *CfgVAddr = NULL;
    uint32_t    size = 0;
    uint32_t    readnum = 0;

    __wrn("esCFG_Init_Ex, path=%s", path);

    fp = fopen(path, "rb");
    if (NULL == fp)
    {
        __wrn("file does not exist:%s", path);
        parser = NULL;
        goto end;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    CfgVAddr = (char *)malloc(size);
    if (NULL == CfgVAddr)
    {
        __wrn("esMEMS_Balloc fail...");
        parser = NULL;
        goto end;
    }

    readnum = fread(CfgVAddr, 1, size, fp);
    if (readnum != size)
    {
        __wrn("esFSYS_fread fail...");
        parser = NULL;
        goto end;
    }

    parser = script_parser_init((char *)(CfgVAddr), size);
    if (NULL == parser)
    {
        __wrn("script_parser_init fail...");
        parser = NULL;
        goto end;
    }

end:
    if (fp)
    {
        fclose((__hdle)fp);
        fp = NULL;
    }

    if (CfgVAddr && size > 0)
    {
        free(CfgVAddr);
        CfgVAddr = NULL;
    }

    __wrn("parser=0x%x", parser);

    return parser;
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
int32_t esCFG_Exit_Ex(intptr_t* parser)
{
    if (NULL != parser)
    {
        script_parser_exit(parser);
        //parser = NULL; //do not change the outside parameter
    }

    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                       �����������ƺ��Ӽ����ƻ�ȡ�ű�����
*
* Description:
*
* Arguments  :    KeyName : sec_name/key_name  ��������/�Ӽ�����
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
int32_t esCFG_GetKeyValue_Ex(intptr_t* parser, char *KeyName, int32_t Value[], int32_t Count)
{
    char    sec_name[32];
    char    key_name[32];
    char    *pblash;

    if (NULL == parser)
    {
        return EPDK_FAIL;
    }

    memset(sec_name, 0, sizeof(sec_name));
    memset(key_name, 0, sizeof(key_name));
    pblash = (char *)strchr(KeyName, '/');
    if (NULL == pblash)
    {
        __wrn("KeyName format incorrect, / not found...");
        return EPDK_FAIL;
    }
    strncpy(sec_name, KeyName, pblash - KeyName);
    strcpy(key_name, pblash + 1);
    __wrn("sec_name=%s", sec_name);
    __wrn("key_name=%s", key_name);
    return  script_parser_fetch(parser, sec_name, key_name, (int32_t *)Value, Count);
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
int32_t esCFG_GetSecKeyCount_Ex(intptr_t* parser, char *SecName)
{
    if (NULL == parser)
    {
        return EPDK_FAIL;
    }

    return  script_parser_subkey_count(parser, SecName);
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
int32_t esCFG_GetSecCount_Ex(intptr_t* parser)
{
    if (NULL == parser)
    {
        return EPDK_FAIL;
    }

    return  script_parser_mainkey_count(parser);
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
int32_t esCFG_GetGPIOSecKeyCount_Ex(intptr_t* parser, char *GPIOSecName)
{
    if (NULL == parser)
    {
        return EPDK_FAIL;
    }

    return  script_parser_mainkey_get_gpio_count(parser, GPIOSecName);
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
int32_t esCFG_GetGPIOSecData_Ex(intptr_t* parser, char *GPIOSecName, void *pGPIOCfg, int32_t GPIONum)
{
    if (NULL == parser)
    {
        return EPDK_FAIL;
    }

    return  script_parser_mainkey_get_gpio_cfg(parser, GPIOSecName, pGPIOCfg, GPIONum);
}

