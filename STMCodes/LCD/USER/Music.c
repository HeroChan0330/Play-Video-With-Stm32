#include "Music.h"

__IO short*audioBuf=0;
__IO u32 audioBufLen=0;
__IO u32 audioBufPtr=0;

//Frequency of Tim3 84MHZ
void dacMusicTimInit(u32 sampleRate){

    u32 arr=84000000/sampleRate-1;
    u32 psc=0;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟

    TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 

    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//初始化TIM3

    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
    TIM_Cmd(TIM3,ENABLE); //使能定时器3

    NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
}

void musicDacInit(){

	GPIO_InitTypeDef  GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitType;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);//使能DAC时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化

	DAC_InitType.DAC_Trigger=DAC_Trigger_None;	//不使用触发功能 TEN1=0
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//不使用波形发生
	DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//屏蔽、幅值设置
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;	//DAC1输出缓存关闭 BOFF1=1
	DAC_Init(DAC_Channel_1,&DAC_InitType);	 //初始化DAC通道1
	DAC_Init(DAC_Channel_2,&DAC_InitType);	 //初始化DAC通道2

	DAC_Cmd(DAC_Channel_1, ENABLE);  //使能DAC通道1
	DAC_Cmd(DAC_Channel_2, ENABLE);  //使能DAC通道1

	DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12位右对齐数据格式设置DAC值
	DAC_SetChannel2Data(DAC_Align_12b_R, 0);  //12位右对齐数据格式设置DAC值

}

//定时器3中断服务函数
void TIM3_IRQHandler(void){
	u32 tmp;
	if(!audioBuf||audioBufPtr>=audioBufLen) {
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		return;
	}
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) {//溢出中断
		/*
		if(audioBuf[audioBufPtr]>0){
			GPIOB->BSRRH = GPIO_Pin_7;
			// TIM_SetCompare1(TIM4,audioBuf[audioBufPtr]>>5);
			TIM4->CCR1 = audioBuf[audioBufPtr]>>5;
		}
		else{
			GPIOB->BSRRL = GPIO_Pin_7;
			// TIM_SetCompare1(TIM4,(32768+audioBuf[audioBufPtr])>>5);
			TIM4->CCR1 = (32768+audioBuf[audioBufPtr])>>5;
		}
		*/
		if(audioBuf[audioBufPtr]>0){
			//GPIOB->BSRRH = GPIO_Pin_7;
			//pwmOut=audioBuf[i]>>5;
			//DAC_SetChannel1Data(DAC_Align_12b_R,audioBuf[audioBufPtr]>>3);
			//DAC_SetChannel2Data(DAC_Align_12b_R,0);
			tmp = (uint32_t)(DAC_BASE+0x00000008 + DAC_Align_12b_R);
			*(__IO uint32_t *) tmp = audioBuf[audioBufPtr]>>3;
			
			tmp = (uint32_t)(DAC_BASE+0x00000014 + DAC_Align_12b_R);
			*(__IO uint32_t *) tmp = 0;
		}
		else{
			//GPIOB->BSRRL = GPIO_Pin_7;
			//pwmOut=(32768+audioBuf[i])>>5;
			//DAC_SetChannel1Data(DAC_Align_12b_R,0);
			//DAC_SetChannel2Data(DAC_Align_12b_R,(-audioBuf[audioBufPtr])>>3);
			tmp = (uint32_t)(DAC_BASE+0x00000008 + DAC_Align_12b_R);
			*(__IO uint32_t *) tmp = 0;
			
			tmp = (uint32_t)(DAC_BASE+0x00000014 + DAC_Align_12b_R);
			*(__IO uint32_t *) tmp = (-audioBuf[audioBufPtr])>>3;
		}
		audioBufPtr++;
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}


void musicGPIOConfigure(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//使能PORTB时钟	
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_Initstructure.GPIO_OType=GPIO_OType_PP;
	GPIO_Initstructure.GPIO_Pin=GPIO_Pin_7;
	GPIO_Initstructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_Initstructure);              //初始化PB6
}


void musicIOInit(){
	//pwmOutputInit();
	//musicGPIOConfigure();
	musicDacInit();
}

void musicBufInit(u32 len){
	audioBuf=(short*)mymalloc(SRAMIN,len);
}
void musicBufDeinit(){
	myfree(SRAMIN,audioBuf);
}
u8 readMusicFromFile(FIL*fp,u32 len){
	u32 res=f_read(fp,audioBuf,len,&audioBufLen)==FR_OK;
	if(res){
		audioBufPtr=0;
		return 1;
	}else{
		audioBufPtr=0;
		audioBufLen=0;
	}
	return 0;
}