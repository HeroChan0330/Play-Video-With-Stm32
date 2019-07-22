#include "opencv2/opencv.hpp"

#include <iostream>
#include <fstream>
#include <stdint.h>

using namespace cv;
using namespace std;


__attribute__((packed)) struct WavHead{
	int8_t RIFF[4];    //头部分那个RIFF
	int32_t filesize;//存的是后面所有文件大小
	int8_t WAVE[4];
	int8_t FMT[4];

	int32_t size1;//存的是fmt保存的大小，包含这之后，data前面几个，共16个
	int16_t fmttag;//1
	int16_t channel;//1
	int32_t samplespersec;//每秒采样数，用的是11025
	int32_t bytepersec;
	int16_t blockalign;
	int16_t bitpersamples;
	int8_t DATA[4];
	int32_t datasize;//剩下文件大小，也就是声音采样是大小，因为是一秒钟的内容，那么就是11025了。
};
// --------------------- 
// 作者：cosmos~yu 
// 来源：CSDN 
// 原文：https://blog.csdn.net/u012236671/article/details/86690684 
// 版权声明：本文为博主原创文章，转载请附上博文链接！


void resize(Mat&img){
	resize(img,img,Size(426,240));
	img=img(Rect(53,0,320,240));
	// cout<<img.rows<<endl;
}

int main(){
	fstream wavFs,outputFs;
	wavFs.open("CXK.wav");
	outputFs.open("output.dat",ios::out);
	WavHead wavInfo;
	//首先把wav文件的信息读出
	wavFs.read((char*)&wavInfo,sizeof(WavHead));
	cout<<"size of WavHead:"<<sizeof(WavHead)<<endl;
	cout<<"bit rate:"<<wavInfo.bytepersec*8<<endl;
	cout<<"bit depth:"<<wavInfo.bitpersamples<<endl;
	cout<<"data size:"<<wavInfo.datasize<<endl;
	//设计播放1秒10帧
	int wavBufferSize=wavInfo.bytepersec/10;
	uint8_t* wavBuffer=new uint8_t[wavBufferSize];

	//读取视频
	VideoCapture cap("CXK.mp4");
	if(cap.isOpened()){
		cout<<"success to open"<<endl;
	}
	Mat frame,frame_16bits;
	int index=0;
	uint8_t*imgBuffer=new uint8_t[320*240*2];
	uint16_t*imgBufferPtr=(uint16_t*)imgBuffer;
	while (cap.read(frame)){
		resize(frame);
		cvtColor(frame,frame_16bits,COLOR_BGR2BGR565);
		imshow("display",frame);
		index++;
		cap.set(CAP_PROP_POS_MSEC ,100*index);
		for(int j=0;j<240;j++){
			for(int i=0;i<320;i++){
				imgBufferPtr[j*320+i]=frame_16bits.at<uint16_t>(j,i);
			}
		}
		int pos=wavFs.tellg();
		cout<<"pos:"<<pos<<endl;
		memset(wavBuffer,0,wavBufferSize);
		wavFs.read((char*)wavBuffer,wavBufferSize);
		outputFs.write((char*)wavBuffer,wavBufferSize);
		outputFs.write((char*)imgBuffer,320*240*2);
		waitKey(1);
	}
	wavFs.close();
	outputFs.close();
}