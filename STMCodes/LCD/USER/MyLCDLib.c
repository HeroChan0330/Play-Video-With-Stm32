#include "MyLCDLib.h"

void drawBitmap_16bit(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t*ptr){
    LCD_Color_Fill(x,y,x+width-1,y+height-1,ptr);
}

void drawBitmap_1bit(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t*ptr,uint16_t foreColor,uint16_t backColor){
	u16 i,j,idx;
    u8 mask=0x01;

	// width=ex-sx+1; 			//得到填充的宽度
	// height=ey-sy+1;			//高度
 	for(j=0;j<height;j++)
	{
 		LCD_SetCursor(x,y+j);   	//设置光标位置 
		LCD_WriteRAM_Prepare();     //开始写入GRAM
		for(i=0;i<width;i++){
            mask=0x80;
            for(idx=0;idx<8;idx++){
                if(mask&ptr[j*width+i]){
                    LCD->LCD_RAM=foreColor;//写入数据
                }else{
									LCD->LCD_RAM=backColor;//写入数据
								}
								mask>>=1;
            }
        }
	}		  
}

void bitmap1BitTo16Bits(u8*src,u16*dest,u32 srcLen,u16 foreColor,u16 backColor){
	u32 i=0,j=0,idx;
    u8 mask=0x01;

	// width=ex-sx+1; 			//得到填充的宽度
	// height=ey-sy+1;			//高度
	for(i=0;i<srcLen;i++){
		mask=0x80;
		for(idx=0;idx<8;idx++){
			if(mask&src[i]){
				dest[j++]=foreColor;
			}else{
				dest[j++]=backColor;
			}
			mask>>=1;
		}
	}
}

void copyCaption(u16*back,u8*caption,u16 y,u16 cWidth,u16 cHeight,u16 bWidth,u16 cColor){
	u16 x,i,j,idx;
	u8 mask=0x01;
	x=(bWidth-cWidth*8)/2;
	for(j=0;j<cHeight;j++){
		for(i=0;i<cWidth;i++){
			mask=0x80;
			for(idx=0;idx<8;idx++){
				if(mask&caption[i+j*cWidth]){
					back[(y+j)*bWidth+x+i*8+idx]=cColor;
				}else{
					
				}
				mask>>=1;
			}
		}
	}
}
