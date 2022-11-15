/*
**********************************************************************************************************************
*											        eGon
*						                     the Embedded System
*									       script parser sub-system
*
*						  Copyright(C), 2006-2010, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : script.c
*
* By      : Jerry
*
* Version : V2.00
*
* Date	  :
*
* Descript:
**********************************************************************************************************************
*/
#include "script.h"
#include <stdlib.h>
#include <string.h>
#include "typedef.h"
#include "log.h"

/*
************************************************************************************************************
*
*                                             _test_str_length
*
*    �������ƣ����Դ������ַ����ĳ���
*
*    �����б���str
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
static int32_t _test_str_length(char *str)
{
	int32_t length = 0;

	while(str[length++])
	{
		if(length > 32)
		{
			length = 32;
			break;
		}
	}

	return length;
}
/*
************************************************************************************************************
*
*                                             script_parser_init
*
*    �������ƣ�
*
*    �����б���script_buf: �ű����ݳز�����malloc������ size:�ű������ֽ���
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
void* script_parser_init(char *script_buf, unsigned long size)
{
    script_parser_t* parser;
	script_head_t   *script_head;    

	if(script_buf)
	{
        parser = malloc(sizeof(script_parser_t));
        if(NULL == parser)
        {
            return NULL;
        }
        parser->script_mod_buf = malloc(size);
        if(NULL == parser->script_mod_buf)
        {
            return NULL;
        }

        parser->script_mod_buf_size = size;

        {
            char* p;
                        
            p = parser->script_mod_buf;
            while(size--)
            {
                *p++ = *script_buf++;
            }            
        }

        script_head = (script_head_t *)(parser->script_mod_buf);

		parser->script_main_key_count = script_head->main_key_count;
                
		return (void*)parser;
	}
	else
	{
		return NULL;
	}
}
/*
************************************************************************************************************
*
*                                             script_parser_exit
*
*    �������ƣ�
*
*    �����б���NULL
*
*    ����ֵ  ��
*
*    ˵��    ��
*
*
************************************************************************************************************
*/
int32_t script_parser_exit(void* hparser )
{
    script_parser_t* parser;

    parser = (script_parser_t*)hparser;
    if(NULL == parser)
    {
        return SCRIPT_PARSER_EMPTY_BUFFER;
    }
    
    if(parser->script_mod_buf)
	{
        free(parser->script_mod_buf);
        parser->script_mod_buf = NULL;
    }
	parser->script_main_key_count = 0;
    parser->script_mod_buf_size = 0;

    free(parser);
    
	return SCRIPT_PARSER_OK;
}

/*
************************************************************************************************************
*
*                                             script_parser_fetch
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    �����ݴ������������Ӽ���ȡ�ö�Ӧ����ֵ
*
*
************************************************************************************************************
*/
int32_t script_parser_fetch(void* hparser, char *main_name, char *sub_name, int value[], int count)
{
	char   main_bkname[32], sub_bkname[32];
	char   *main_char, *sub_char;
	script_main_key_t  *main_key = NULL;
	script_sub_key_t   *sub_key = NULL;
	int32_t    i, j;
	int32_t    pattern, word_count;
    script_parser_t* parser;

    parser = (script_parser_t*)hparser;
    if(NULL == parser)
    {
        return SCRIPT_PARSER_EMPTY_BUFFER;
    }

	//���ű�buffer�Ƿ����
	if(!parser->script_mod_buf)
	{
		return SCRIPT_PARSER_EMPTY_BUFFER;
	}
	//����������ƺ��Ӽ������Ƿ�Ϊ��
	if((main_name == NULL) || (sub_name == NULL))
	{
		return SCRIPT_PARSER_KEYNAME_NULL;
	}
	//�������buffer�Ƿ�Ϊ��
	if(value == NULL)
	{
		return SCRIPT_PARSER_DATA_VALUE_NULL;
	}
	//�����������ƺ��Ӽ����ƣ��������31�ֽ����ȡ31�ֽ�
	main_char = main_name;
	if(_test_str_length(main_name) > 31)
	{
	    memset(main_bkname, 0, 32);
		strncpy(main_bkname, main_name, 31);
		main_char = main_bkname;
	}
    sub_char = sub_name;
	if(_test_str_length(sub_name) > 31)
	{
		memset(sub_bkname, 0, 32);
		strncpy(sub_bkname, sub_name, 31);
		sub_char = sub_bkname;
	}

	for(i=0;i<parser->script_main_key_count;i++)
	{
		main_key = (script_main_key_t *)(parser->script_mod_buf + (sizeof(script_head_t)) + i * sizeof(script_main_key_t));
		if(strcmp(main_key->main_name, main_char))    //���������ƥ�䣬Ѱ����һ������
		{
			continue;
		}
		//����ƥ�䣬Ѱ���Ӽ�����ƥ��
		for(j=0;j<main_key->lenth;j++)
		{
			sub_key = (script_sub_key_t *)(parser->script_mod_buf + (main_key->offset<<2) + (j * sizeof(script_sub_key_t)));
			if(strcmp(sub_key->sub_name, sub_char))    //���������ƥ�䣬Ѱ����һ������
			{
				continue;
			}
			pattern    = (sub_key->pattern>>16) & 0xffff;             //��ȡ���ݵ�����
			word_count = (sub_key->pattern>> 0) & 0xffff;             //��ȡ��ռ�õ�word����
			//ȡ������
			switch(pattern)
			{
				case DATA_TYPE_SINGLE_WORD:                           //��word��������
					value[0] = *(int *)(parser->script_mod_buf + (sub_key->offset<<2));
					break;

				case DATA_TYPE_STRING:     							  //�ַ�����������
					if(count < word_count)
					{
						word_count = count;
					}
					memcpy((char *)value, parser->script_mod_buf + (sub_key->offset<<2), word_count << 2);
					break;

				case DATA_TYPE_MULTI_WORD:
					break;
				case DATA_TYPE_GPIO_WORD:							 //��word��������
				{
					user_gpio_set_t  *user_gpio_cfg = (user_gpio_set_t *)value;
                    //������GPIO���ͣ�����Ƿ��㹻����û�����
                    if(sizeof(user_gpio_set_t) > (count<<2))
                    {
                        return SCRIPT_PARSER_BUFFER_NOT_ENOUGH;
                    }
					strncpy( user_gpio_cfg->gpio_name, sub_char, strlen(sub_char) + 1);
					memcpy(&user_gpio_cfg->port, parser->script_mod_buf + (sub_key->offset<<2),  sizeof(user_gpio_set_t) - 32);

					break;
			    }
			}

			return SCRIPT_PARSER_OK;
		}
	}

	return SCRIPT_PARSER_KEY_NOT_FIND;
}
/*
************************************************************************************************************
*
*                                             script_parser_subkey_count
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    �����ݴ�����������ȡ�������µ��Ӽ�����
*
*
************************************************************************************************************
*/
int32_t script_parser_subkey_count(void* hparser, char *main_name)
{
	char   main_bkname[32];
	char   *main_char;
	script_main_key_t  *main_key = NULL;
	int32_t    i;
    script_parser_t* parser;

    parser = (script_parser_t*)hparser;
    if(NULL == parser)
    {
        return SCRIPT_PARSER_EMPTY_BUFFER;
    }

	//���ű�buffer�Ƿ����
	if(!parser->script_mod_buf)
	{
		return SCRIPT_PARSER_EMPTY_BUFFER;
	}
	//����������ƺ��Ӽ������Ƿ�Ϊ��
	if(main_name == NULL)
	{
		return SCRIPT_PARSER_KEYNAME_NULL;
	}
	//�����������ƺ��Ӽ����ƣ��������31�ֽ����ȡ31�ֽ�
	main_char = main_name;
	if(_test_str_length(main_name) > 31)
	{
	    memset(main_bkname, 0, 32);
		strncpy(main_bkname, main_name, 31);
		main_char = main_bkname;
	}

	for(i=0;i<parser->script_main_key_count;i++)
	{
		main_key = (script_main_key_t *)(parser->script_mod_buf + (sizeof(script_head_t)) + i * sizeof(script_main_key_t));
		if(strcmp(main_key->main_name, main_char))    //���������ƥ�䣬Ѱ����һ������
		{
			continue;
		}

		return main_key->lenth;    //���ص�ǰ�����µ��Ӽ�����
	}

	return -1;                     //-1 ��ʾû�ж�Ӧ������
}
/*
************************************************************************************************************
*
*                                             script_parser_mainkey_count
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    �����ݴ�����������ȡ�������ĸ���
*
*
************************************************************************************************************
*/
int32_t script_parser_mainkey_count(void* hparser)
{
    script_parser_t* parser;

    parser = (script_parser_t*)hparser;
    if(NULL == parser)
    {
        return SCRIPT_PARSER_EMPTY_BUFFER;
    }
    
	//���ű�buffer�Ƿ����
	if(!parser->script_mod_buf)
	{
		return SCRIPT_PARSER_EMPTY_BUFFER;
	}

	return 	parser->script_main_key_count;
}
/*
************************************************************************************************************
*
*                                             script_parser_mainkey_get_gpio_count
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    �����ݴ�����������ȡ��������GPIO����
*
*
************************************************************************************************************
*/
int32_t script_parser_mainkey_get_gpio_count(void* hparser, char *main_name)
{
	char   main_bkname[32];
	char   *main_char;
	script_main_key_t  *main_key = NULL;
	script_sub_key_t   *sub_key = NULL;
	int32_t    i, j;
	int32_t    pattern, gpio_count = 0;
    script_parser_t* parser;

    parser = (script_parser_t*)hparser;
    if(NULL == parser)
    {
        return SCRIPT_PARSER_EMPTY_BUFFER;
    }

	//���ű�buffer�Ƿ����
	if(!parser->script_mod_buf)
	{
		return SCRIPT_PARSER_EMPTY_BUFFER;
	}
	//����������ƺ��Ӽ������Ƿ�Ϊ��
	if(main_name == NULL)
	{
		return SCRIPT_PARSER_KEYNAME_NULL;
	}
	//�����������ƺ��Ӽ����ƣ��������31�ֽ����ȡ31�ֽ�
	main_char = main_name;
	if(_test_str_length(main_name) > 31)
	{
	    memset(main_bkname, 0, 32);
		strncpy(main_bkname, main_name, 31);
		main_char = main_bkname;
	}

	for(i=0;i<parser->script_main_key_count;i++)
	{
		main_key = (script_main_key_t *)(parser->script_mod_buf + (sizeof(script_head_t)) + i * sizeof(script_main_key_t));
		if(strcmp(main_key->main_name, main_char))    //���������ƥ�䣬Ѱ����һ������
		{
			continue;
		}
		//����ƥ�䣬Ѱ���Ӽ�����ƥ��
		for(j=0;j<main_key->lenth;j++)
		{
			sub_key = (script_sub_key_t *)(parser->script_mod_buf + (main_key->offset<<2) + (j * sizeof(script_sub_key_t)));

			pattern    = (sub_key->pattern>>16) & 0xffff;             //��ȡ���ݵ�����
			//ȡ������
			if(DATA_TYPE_GPIO_WORD == pattern)
			{
				gpio_count ++;
			}
		}
	}

	return gpio_count;
}

/*
************************************************************************************************************
*
*                                             script_parser_mainkey_get_gpio_cfg
*
*    �������ƣ�
*
*    �����б���
*
*    ����ֵ  ��
*
*    ˵��    �����ݴ�����������ȡ��������GPIO��������Ϣ
*
*
************************************************************************************************************
*/
int32_t script_parser_mainkey_get_gpio_cfg(void* hparser, char *main_name, void *gpio_cfg, int gpio_count)
{
	char   main_bkname[32];
	char   *main_char;
	script_main_key_t  *main_key = NULL;
	script_sub_key_t   *sub_key = NULL;
	user_gpio_set_t  *user_gpio_cfg = (user_gpio_set_t *)gpio_cfg;
	int32_t    i, j;
	int32_t    pattern, user_index;
    script_parser_t* parser;

    parser = (script_parser_t*)hparser;
    if(NULL == parser)
    {
        return SCRIPT_PARSER_EMPTY_BUFFER;
    }

	//���ű�buffer�Ƿ����
	if(!parser->script_mod_buf)
	{
		return SCRIPT_PARSER_EMPTY_BUFFER;
	}
	//����������ƺ��Ӽ������Ƿ�Ϊ��
	if(main_name == NULL)
	{
		return SCRIPT_PARSER_KEYNAME_NULL;
	}
	//��������û�buffer
	memset(user_gpio_cfg, 0, sizeof(user_gpio_set_t) * gpio_count);
	//�����������ƺ��Ӽ����ƣ��������31�ֽ����ȡ31�ֽ�
	main_char = main_name;
	if(_test_str_length(main_name) > 31)
	{
	    memset(main_bkname, 0, 32);
		strncpy(main_bkname, main_name, 31);
		main_char = main_bkname;
	}

	for(i=0;i<parser->script_main_key_count;i++)
	{
		main_key = (script_main_key_t *)(parser->script_mod_buf + (sizeof(script_head_t)) + i * sizeof(script_main_key_t));
		if(strcmp(main_key->main_name, main_char))    //���������ƥ�䣬Ѱ����һ������
		{
			continue;
		}
		//����ƥ�䣬Ѱ���Ӽ�����ƥ��
		user_index = 0;
		for(j=0;j<main_key->lenth;j++)
		{
			sub_key = (script_sub_key_t *)(parser->script_mod_buf + (main_key->offset<<2) + (j * sizeof(script_sub_key_t)));
			pattern    = (sub_key->pattern>>16) & 0xffff;             //��ȡ���ݵ�����
			//ȡ������
			if(DATA_TYPE_GPIO_WORD == pattern)
			{
			    strncpy( user_gpio_cfg[user_index].gpio_name, sub_key->sub_name, strlen(sub_key->sub_name) + 1);
				memcpy(&user_gpio_cfg[user_index].port, parser->script_mod_buf + (sub_key->offset<<2), sizeof(user_gpio_set_t) - 32);
				user_index++;
				if(user_index >= gpio_count)
				{
					break;
				}
			}
		}
		return SCRIPT_PARSER_OK;
	}

	return SCRIPT_PARSER_KEY_NOT_FIND;
}

