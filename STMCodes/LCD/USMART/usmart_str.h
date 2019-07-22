#ifndef __USMART_STR_H
#define __USMART_STR_H	 
#include "stm32f4xx.h"
//////////////////////////////////////////////////////////////////////////////////	 
//STM32F4工程-库函数版本
//淘宝店铺：http://mcudev.taobao.com	
/////////////////////////////////////////////////////////////////////////////////////
 
 
u8 usmart_get_parmpos(u8 num);						//得到某个参数在参数列里面的起始位置
u8 usmart_strcmp(u8*str1,u8 *str2);					//对比两个字符串是否相等
u32 usmart_pow(u8 m,u8 n);							//M^N次方
u8 usmart_str2num(u8*str,u32 *res);					//字符串转为数字
u8 usmart_get_cmdname(u8*str,u8*cmdname,u8 *nlen,u8 maxlen);//从str中得到指令名,并返回指令长度
u8 usmart_get_fname(u8*str,u8*fname,u8 *pnum,u8 *rval);		//从str中得到函数名
u8 usmart_get_aparm(u8 *str,u8 *fparm,u8 *ptype); 	//从str中得到一个函数参数
u8 usmart_get_fparam(u8*str,u8 *parn);  			//得到str中所有的函数参数.
#endif











