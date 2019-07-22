/*
程序里面有一点magic number是懒得写file header然后直接取定的。
视频每一帧二值化像素，使用8个像素1字节的压缩方法，所以320*240的图像使用320*240/8=9600 bytes
9600字节分6次读取，最后一次先把数据还原成u16 array(captionBack),然后再读取目标字幕的压缩数据，对应写进captionBack
从而实现一个比较小内存使用的双缓冲绘图，避免字幕闪瞎狗眼。
然后使用Tim3定时器中断，频率为音频的采样率。中断后通过Dac1和Dac2输出音频的采样值。
文件结构：
每一帧：
{
	图像压缩数据:9600 bytes,
	音频采样数据:3200 bytes,
	[
		{
			通道字幕长度: 4 bytes,
			通道字幕压缩数据:通道字幕长度 bytes
		}
	]
}
*/

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "sram.h"   
#include "malloc.h" 
#include "usmart.h"  
#include "sdio_sdcard.h"    
#include "malloc.h" 
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"    
#include "utils.h"
#include "MyLCDLib.h"
#include "Music.h"
 

void sth_difficult_to_init(){
	u8 res=0;	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	LED_Init();					//初始化LED
	usmart_dev.init(84);		//初始化USMART
 	LCD_Init();					//LCD初始化

 	KEY_Init();					//按键初始化
	W25QXX_Init();				//初始化W25Q128
	

	
	printf("W25Q128 Init Finished\n");
	my_mem_init(SRAMIN);		//初始化内部内存池
	my_mem_init(SRAMCCM);		//初始化CCM内存池
	printf("Mem Init Finished\n");

 	POINT_COLOR=RED;//设置字体为红色

	LCD_ShowString(30,130,200,16,16,"Use USMART for test");
 	while(SD_Init()){ //检测不到SD卡
		LCD_ShowString(30,150,200,16,16,"SD Card Error!");
		delay_ms(500);
		LCD_ShowString(30,150,200,16,16,"Please Check! ");
		delay_ms(500);
		LED0=!LED0;//DS0闪烁
	}
 	exfuns_init();							//为fatfs相关变量申请内存
	f_mount(fs[0],"0:",1); 					//挂载SD卡
 	res=f_mount(fs[1],"1:",1); 				//挂载FLASH
	if(res==0X0D){ //FLASH磁盘,FAT文件系统错误,重新格式化FLASH
	
		LCD_ShowString(30,150,200,16,16,"Flash Disk Formatting...");	//格式化FLASH
		res=f_mkfs("1:",1,4096);//格式化FLASH,1,盘符;1,不需要引导区,8个扇区为1个簇
		if(res==0){
			f_setlabel((const TCHAR *)"1:mcudev");	//设置Flash磁盘的名字为：mcudev
			LCD_ShowString(30,150,200,16,16,"Flash Disk Format Finish");	//格式化完成
		}else
			LCD_ShowString(30,150,200,16,16,"Flash Disk Format Error ");	//格式化失败
		delay_ms(1000);
	}
	printf("SD Init Finished\n");
	LCD_Display_Dir(U2D_R2L);
}


int main(void){
	u8 t=0;	
	u8 res=0;	
	u8 j=0;
	FIL fp;
	u16*imgBuf;

	u32 readLen=0;

	__IO u32 tTemp=0; //this variable is used to store the clock cnt;
	
	sth_difficult_to_init();
	
	delay_ms(2000);
	musicIOInit();
	printf("musicIOInit Finished\n");
	dacMusicTimInit(AUDIO_SAMPLE_RATE);
	printf("dacMusicTimInit Finished\n");

	
	while(1){
		LCD_Clear(GREEN);
		delay_ms(1000);
	
		res = f_open(&fp, "0:/cxk.dat",FA_OPEN_ALWAYS|FA_READ);
		imgBuf=(u16*)mymalloc(SRAMIN,IMG_BUF_LEN);
		musicBufInit(AUDIO_BUF_LEN);
		printf("Buf Init Finished\n");
		
		if(res!=FR_OK){
			printf("\r\n f_open() fail .. \r\n");
			delay_ms(1000);
			continue;
		}else{
			printf("\r\n f_open() success .. \r\n");
		}
		while(1){

			SysTick->LOAD=(u32)(PERIOD_PER_FRAME-1)*FAC_MS;//时间加载(SysTick->LOAD为24bit)
			SysTick->VAL =0x00;           //清空计数器
			SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数
			
			
			if(!readMusicFromFile(&fp,AUDIO_BUF_LEN)){
				goto end;
			}
			for(j=0;j<6;j++){
				res=f_read(&fp,imgBuf,25600,&readLen);
				if(readLen==0) goto end;
				// printf("%s",strBuf);
				LCD_Color_Fill(0,j*40,319,j*40+39,imgBuf);
			}
			

			
			//wait for this frame end
			do{
				tTemp=SysTick->CTRL;
			}while((tTemp&0x01)&&!(tTemp&(1<<16)));//等待时间到达
			SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
			SysTick->VAL =0X00;       //清空计数器
			//delay_ms(50);
		}
		end:
		
		SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
		SysTick->VAL =0X00;       //清空计数器
		myfree(SRAMIN,imgBuf);
		musicBufDeinit();
		f_close(&fp);
		LCD_Clear(BLACK);
		while(KEY_Scan(0)!=KEY0_PRES) delay_ms(100);
	}

}