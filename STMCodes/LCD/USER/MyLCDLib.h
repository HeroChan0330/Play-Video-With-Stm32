#ifndef _MY_LCD_LIB_H_
#define _MY_LCD_LIB_H_

#include <stdint.h>
#include "lcd.h"

// //16色位图or单色位图
// typedef enum _bitmapBitsType{
//     BITMAP_BITS_TYPE_2,
//     BITMAP_BITS_TYPE_16
// }BitmapBitsType;

// typedef struct _bitmap{
//     uint16_t width;
//     uint16_t height;
//     BitmapBitsType type;
//     uint16_t*bits;
// }Bitmap;
// Bitmap*createBitmap(int x,int y){

// }
// void drawBitmap(Bitmap bitmap,int x,int y,uint16_t foreColor=0xffff);

void drawBitmap_16bit(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t*ptr);
//attention:width为真实宽度的1/8（即横向8个像素二值化变成一个字节）
void drawBitmap_1bit(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t*ptr,uint16_t foreColor,uint16_t backColor);

void bitmap1BitTo16Bits(u8*src,u16*dest,u32 srcLen,u16 foreColor,u16 backColor);


void copyCaption(u16*back,u8*caption,u16 y,u16 cWidth,u16 cHeight,u16 bWidth,u16 cColor);
#endif