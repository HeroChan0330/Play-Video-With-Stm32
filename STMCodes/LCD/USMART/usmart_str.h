#ifndef __USMART_STR_H
#define __USMART_STR_H	 
#include "stm32f4xx.h"
//////////////////////////////////////////////////////////////////////////////////	 
//STM32F4����-�⺯���汾
//�Ա����̣�http://mcudev.taobao.com	
/////////////////////////////////////////////////////////////////////////////////////
 
 
u8 usmart_get_parmpos(u8 num);						//�õ�ĳ�������ڲ������������ʼλ��
u8 usmart_strcmp(u8*str1,u8 *str2);					//�Ա������ַ����Ƿ����
u32 usmart_pow(u8 m,u8 n);							//M^N�η�
u8 usmart_str2num(u8*str,u32 *res);					//�ַ���תΪ����
u8 usmart_get_cmdname(u8*str,u8*cmdname,u8 *nlen,u8 maxlen);//��str�еõ�ָ����,������ָ���
u8 usmart_get_fname(u8*str,u8*fname,u8 *pnum,u8 *rval);		//��str�еõ�������
u8 usmart_get_aparm(u8 *str,u8 *fparm,u8 *ptype); 	//��str�еõ�һ����������
u8 usmart_get_fparam(u8*str,u8 *parn);  			//�õ�str�����еĺ�������.
#endif











