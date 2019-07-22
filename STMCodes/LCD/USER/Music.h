#ifndef _MMUSIC_H_
#define _MMUSIC_H_
#include "sys.h"
#include "malloc.h" 
#include "ff.h"  
#include "stm32f4xx_dac.h"
extern __IO short*audioBuf;
extern __IO u32 audioBufLen;
extern __IO u32 audioBufPtr;

void dacMusicTimInit();
void musicIOInit();
void musicBufInit(u32 len);
void musicBufDeinit();
u8 readMusicFromFile(FIL*fp,u32 len);


#endif