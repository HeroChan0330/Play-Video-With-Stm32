/*
����������һ��magic number������дfile headerȻ��ֱ��ȡ���ġ�
��Ƶÿһ֡��ֵ�����أ�ʹ��8������1�ֽڵ�ѹ������������320*240��ͼ��ʹ��320*240/8=9600 bytes
9600�ֽڷ�6�ζ�ȡ�����һ���Ȱ����ݻ�ԭ��u16 array(captionBack),Ȼ���ٶ�ȡĿ����Ļ��ѹ�����ݣ���Ӧд��captionBack
�Ӷ�ʵ��һ���Ƚ�С�ڴ�ʹ�õ�˫�����ͼ��������Ļ��Ϲ���ۡ�
Ȼ��ʹ��Tim3��ʱ���жϣ�Ƶ��Ϊ��Ƶ�Ĳ����ʡ��жϺ�ͨ��Dac1��Dac2�����Ƶ�Ĳ���ֵ��
�ļ��ṹ��
ÿһ֡��
{
	ͼ��ѹ������:9600 bytes,
	��Ƶ��������:3200 bytes,
	[
		{
			ͨ����Ļ����: 4 bytes,
			ͨ����Ļѹ������:ͨ����Ļ���� bytes
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	LED_Init();					//��ʼ��LED
	usmart_dev.init(84);		//��ʼ��USMART
 	LCD_Init();					//LCD��ʼ��

 	KEY_Init();					//������ʼ��
	W25QXX_Init();				//��ʼ��W25Q128
	

	
	printf("W25Q128 Init Finished\n");
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ��
	printf("Mem Init Finished\n");

 	POINT_COLOR=RED;//��������Ϊ��ɫ

	LCD_ShowString(30,130,200,16,16,"Use USMART for test");
 	while(SD_Init()){ //��ⲻ��SD��
		LCD_ShowString(30,150,200,16,16,"SD Card Error!");
		delay_ms(500);
		LCD_ShowString(30,150,200,16,16,"Please Check! ");
		delay_ms(500);
		LED0=!LED0;//DS0��˸
	}
 	exfuns_init();							//Ϊfatfs��ر��������ڴ�
	f_mount(fs[0],"0:",1); 					//����SD��
 	res=f_mount(fs[1],"1:",1); 				//����FLASH
	if(res==0X0D){ //FLASH����,FAT�ļ�ϵͳ����,���¸�ʽ��FLASH
	
		LCD_ShowString(30,150,200,16,16,"Flash Disk Formatting...");	//��ʽ��FLASH
		res=f_mkfs("1:",1,4096);//��ʽ��FLASH,1,�̷�;1,����Ҫ������,8������Ϊ1����
		if(res==0){
			f_setlabel((const TCHAR *)"1:mcudev");	//����Flash���̵�����Ϊ��mcudev
			LCD_ShowString(30,150,200,16,16,"Flash Disk Format Finish");	//��ʽ�����
		}else
			LCD_ShowString(30,150,200,16,16,"Flash Disk Format Error ");	//��ʽ��ʧ��
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

			SysTick->LOAD=(u32)(PERIOD_PER_FRAME-1)*FAC_MS;//ʱ�����(SysTick->LOADΪ24bit)
			SysTick->VAL =0x00;           //��ռ�����
			SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ����
			
			
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
			}while((tTemp&0x01)&&!(tTemp&(1<<16)));//�ȴ�ʱ�䵽��
			SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
			SysTick->VAL =0X00;       //��ռ�����
			//delay_ms(50);
		}
		end:
		
		SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
		SysTick->VAL =0X00;       //��ռ�����
		myfree(SRAMIN,imgBuf);
		musicBufDeinit();
		f_close(&fp);
		LCD_Clear(BLACK);
		while(KEY_Scan(0)!=KEY0_PRES) delay_ms(100);
	}

}