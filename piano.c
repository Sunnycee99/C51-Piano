#include <reg52.h>
#include <math.h>

#define GPIO_KEY P2
#define time 1
#define uchar unsigned char
#define uint unsigned int

void judge();
void keyscan();
void rgbplay(uchar a);
	
sbit buzzer = P1^7;
sbit red = P3^5;
sbit green = P3^6;
sbit blue = P3^7;
char KeyValue=-1, //键值
		function = 0,   //当前模式
		function_flag = 0,  //是否已改变音调
		play_flag = 0,  //是否播放音乐
		play_list = 0,  //播放的第几首歌
		tune = 0,  //音调
		delay_time = 0,  //节拍次数
		record_flag = 0,  //录音标志位，0-停止，1-开始
		record_play_flag = 0,  //录音播放标志位
		flag = 0;  //录音播放是否暂停过

uint tune_index = 0;//第几个音调(用于播放歌曲)

char	record_long = 0,  //总共的录音音阶数
			one_tune_long = 0;  //录音单个音阶的持续时间

char record_tune[40];  //每个音阶
char tune_long[40];  //每个音阶持续时间
/************************************音乐*******************************************/
uchar code music1[] = {
1,2,3,2,2,1,6,1,1,1,2,1,5,1,2,5,2,3,1,3,1,6,2,1,5,2,1,3,2,1,5,2,1,2,2,4,2,2,3,3,2,1,7,1,2,6,1,2,5,1,3,6,1,1,1,2,2,2,2,2,3,1,2,1,2,2,6,1,1,5,1,1,6,1,1,1,2,1,5,1,4,3,2,3,5,2,1,7,1,2,2,2,2,6,1,1,1,2,1,5,1,4,3,1,1,5,1,2,3,1,2,5,1,1,6,1,1,7,1,1,2,2,1,6,1,4,5,1,1,6,1,1,1,2,3,2,2,1,5,2,2,3,2,2,2,2,2,3,2,1,2,2,1,1,2,2,6,1,1,5,1,1,3,1,2,3,1,4,1,2,4,6,1,3,1,2,1,6,1,1,5,1,1,3,1,1,5,1,1,6,1,1,1,2,1,5,1,4,0,0,0
};//《梁祝》，音阶、音调、节拍
uchar code music2[] = {
	6,2,3,5,2,1,3,2,2,5,2,2,6,2,1,6,2,2,5,2,1,3,2,4,6,1,3,1,2,1,2,2,2,5,2,1,3,2,1,2,2,4,
6,2,3,2,3,1,2,3,2,1,3,1,7,2,1,6,2,3,5,2,1,3,2,4,2,2,1,5,2,1,5,2,1,3,2,1,2,2,1,3,2,1,1,2,1,7,1,1,6,1,4,
1,2,3,6,1,1,6,2,1,1,2,2,2,2,2,5,
2,1,3,2,1,2,2,4,5,2,3,3,2,1,3,2,
2,5,2,2,6,2,4,2,3,3,2,3,1,1,3,2,
2,3,2,6,2,1,1,3,1,5,2,1,6,2,1,3,
2,4,2,2,1,5,2,1,5,2,1,3,2,1,2,2,
1,3,2,1,1,2,1,7,1,1,6,1,4,
0, 0, 0};//《青花瓷》
code unsigned char music3[]={
6, 2, 3, 5, 2, 1, 3, 2, 2, 5, 2, 2, 1, 3, 2, 6, 2, 1, 5, 2, 1,
6, 2, 4, 3, 2, 2, 5, 2, 1, 6, 2, 1, 5, 2, 2, 3, 2, 2, 1, 2, 1,
6, 1, 1, 5, 2, 1, 3, 2, 1, 2, 2, 4, 2, 2, 3, 3, 2, 1, 5, 2, 2,
5, 2, 1, 6, 2, 1, 3, 2, 2, 2, 2, 2, 1, 2, 4, 5, 2, 3, 3, 2, 1,
2, 2, 1, 1, 2, 1, 6, 1, 1, 1, 2, 1, 5, 1, 6, 0, 0, 0};  //《世上只有妈妈好》
//code unsigned char music3[]={
//         5,1,1, 5,1,1, 6,1,2, 5,1,2, 1,2,2, 7,1,4,
//         5,1,1, 5,1,1, 6,1,2, 5,1,2, 2,2,2, 1,2,4,
//         5,1,1, 5,1,1, 5,2,2, 3,2,2, 1,2,2, 7,1,2, 6,1,2,
//         4,2,1, 4,2,1, 3,2,2, 1,2,2, 2,2,2, 1,2,4, 0, 0, 0};  //《生日快乐歌》

uchar code ledyang[] = {
						0xC0, //"0"
						0xF9, //"1"
						0xA4, //"2"
						0xB0, //"3"
						0x99, //"4"
						0x92, //"5"
						0x82, //"6"
						0xF8, //"7"
						0x80, //"8"
						0x90, //"9"
						0x88, //"A"
						0x83, //"B"
						0xC6, //"C"
						0xA1, //"D"
						0x86, //"E"
						0x8E, //"F"
						0x89, //"H"
						0xC7, //"L"
						0xC8, //"n"
						0xC1, //"u"
						0x8C, //"P"
						0xA3, //"o"
						0xBF, //"-"
                         };//共阴极数码管显示

uint code music_code[] = {63628,63835,64021,64103,64260,64400,64524,
													64580,64684,64777,64820,64898,64968,65030,
													65058,65110,65157,65178,65217,65252,65283};
//对应按键的音调代码

void delay_ms(uint x)//毫秒延时函数
{
	uint y,z;
	for(z=x;z>0;z--)
	{
		for(y=110;y>0;y--);
		{
//			if(record_play_flag==1)
//			{
//				keyscan();
//				if(record_play_flag==0)
//				{
//					flag = 1;
//					break;
//				}
//			}
		}
//		if(function==2&&flag==1)
//			break;
	}
	TR1 = 0;
	flag = 0;
}

/*********************************延长时间，t个半拍******************************/
void tune_delay(unsigned char t) //延时程序，控制发音的时间长度
{
unsigned char t1;
unsigned long t2;
for(t1 = 0; t1 < t; t1++) //双重循环, 共延时t个半拍
	for(t2 = 0; t2 < 8000; t2++)
	{
		keyscan();
		if(play_flag==0)
				break;
	}//延时期间, 可进入T1中断去发音
TR1 = 0; //关闭T1, 停止发音
}

/************************************定时器**************************************/
void init()//定时器初始化
{
	TMOD = 0x11;//选择工作方式1
	EA = 1;//开启总中断
	ET0 = 1;
	ET1 = 1;//开启定时器T1中断
}

void beep()//定时器发声
{
	buzzer = 1;
	TH1 = music_code[tune]/256;
	TL1 = music_code[tune]%256;
	TR1 = 1;//开启定时器T1
}

void T0_time() interrupt 1
{
	TH0=(65536-50000)/256;	//定时50ms
	TL0=(65536-50000)%256;
	one_tune_long++;
}

void T1_time() interrupt 3//产生对应频率的方波,用于蜂鸣器发声
{
	buzzer = !buzzer;
	TH1 = music_code[tune]/256;
	TL1 = music_code[tune]%256;
}
/**************************************************音乐播放函数****************************************/
void playMusic()
{
	while(play_flag==1)
	{
		switch(play_list)
		{
			case 0:tune = music1[tune_index*3]+(music1[tune_index*3+1]-1)*7-1; delay_time = music1[3*tune_index+2];break;
			case 1:tune = music2[tune_index*3]+(music2[tune_index*3+1]-1)*7-1; delay_time = music2[3*tune_index+2];break;
			case 2:tune = music3[tune_index*3]+(music2[tune_index*3+1]-1)*7-1; delay_time = music3[3*tune_index+2];break;
		}
		if(play_list==0)
			P0 = ledyang[1];
		if(play_list==1)
			P0 = 0xB9;   //第二首歌
		if(play_list==2)
			P0 = 0xF1;  //第一首歌
		tune_index++;
		beep();
		tune_delay(delay_time);
		rgbplay(tune%7);
		if(play_list==0&&music1[tune_index*3]==0)
			tune_index = 0;
		if(play_list==1&&music2[tune_index*3]==0)
			tune_index = 0;
		if(play_list==2&&music3[tune_index*3]==0)
			tune_index = 0;	
	}
}

/***************************************************录音播放函数**************************************************/
void playRecord()
{
	while(record_play_flag==1)
	{
		keyscan();
		tune = record_tune[tune_index];
		delay_time = tune_long[tune_index];
		if(tune_index%2==0)
			beep();
		delay_ms(50*delay_time);
		tune_index++;
		if(tune_index>record_long)
			{
				record_play_flag = 0;
				tune_index = 0;
			}
		rgbplay(tune%7);
	}
}

/**********************************************************录音函数***********************************************/
void record()
{
	//记录空闲时间
	if(record_long>0)
	{
		tune_long[record_long] = one_tune_long;
		record_tune[record_long] = KeyValue;
		record_long++;
		one_tune_long = 0;
	}
	
	TR0 = 1;
}
	
void keyscan()
{
	GPIO_KEY = 0x0f;
	if(GPIO_KEY!=0x0f)//读取按键是否按下
	{
		delay_ms(10);//延时10ms进行消抖
		if(GPIO_KEY!=0x0f)//再次检测键盘是否按下
		{
			if(function==2&&record_flag)
			{
				TR0 = 0;
				TH0 = (65536-50000)/256;
				TL0 = (65536-50000)%256;
			}
			//测试列
			GPIO_KEY = 0X0F;
			switch(GPIO_KEY)
			{
				case(0X07):	KeyValue = 0;break;
				case(0X0b):	KeyValue = 1;break;
				case(0X0d): KeyValue = 2;break;
				case(0X0e):	KeyValue = 3;break;
			}
			//测试行
			GPIO_KEY = 0XF0;
			switch(GPIO_KEY)
			{
				case(0X70):	KeyValue = KeyValue;break;
				case(0Xb0):	KeyValue = KeyValue+4;break;
				case(0Xd0): KeyValue = KeyValue+8;break;
				case(0Xe0):	KeyValue = KeyValue+12;break;
			}
			if(record_flag)
			{
				record();
			}
			judge();
			switch(function_flag)
			{
				case 0:tune = KeyValue;if(KeyValue>13) tune = 0;break;
				case 1:tune = KeyValue+7;if(KeyValue>13) tune = 0;break;
			}
			if(function!=1)
			{
				beep();
				rgbplay(7);  //RGB 熄灭
			}
			//按键松手检测
			while(GPIO_KEY!=0xf0);
			if(function!=1)
				TR1 = 0;
			if(record_flag&&KeyValue!=14)
			{
				TR0 = 0;
				tune_long[record_long] = one_tune_long;
				record_tune[record_long] = KeyValue;
				record_long++;
				one_tune_long = 0;
				//没按键的时间
				TH0 = (65536-50000)/256;
				TL0 = (65536-50000)%256;
				TR0 = 1;
			}
		}
	}
}

//	RGB显示函数
void rgbplay(uchar a)
{
	switch(a)
	{
		case 0:
			red = 0;green = 1;blue = 1;break;
		case 1:
			red = 0;green = 0;blue = 1;break;
		case 2:
			red = 1;green = 0;blue = 1;break;
		case 3:
			red = 1;green = 0;blue = 0;break;
		case 4:
			red = 1;green = 1;blue = 0;break;
		case 5:
			red = 0;green = 1;blue = 0;break;
		case 6:
			red = 0;green = 0;blue = 0;break;
		case 7:
			red = 1;green = 1;blue = 1;break;
	}
}

//	当前模式判断
void judge()
{
		if(KeyValue==15)
		{
			function = (function+1)%3;
			function_flag = 0;//音调恢复
			play_flag = 0;
			record_play_flag = 0;
			record_long = 0;
			record_flag = 0;
			tune_index = 0;
		}
		else
		{
			if(function==0&&KeyValue==14)
				function_flag = (function_flag+1)%2;//音调升高
			if(function==1)
			{
				switch(KeyValue)
				{
					case 12:play_list = (play_list-1);if(play_list<0) play_list=2;break;
					case 13:play_list = (play_list+1)%3;tune_index = 0;break;  //选歌
					case 14:play_flag = !play_flag;break;  //暂停、播放
				} 
			}
			if(function==2)
			{
				if(KeyValue==14)
					record_flag = !record_flag;
				if(KeyValue==13&&record_flag==0)
					record_play_flag = !record_play_flag;
			}
		}
}
void main()
{
	init();
	while(1)
	{
		keyscan();
//		rgbplay(KeyValue%7);
		P0=ledyang[function];
		if(record_flag)
			P0 = 0x24;
		if(function==1)
		{
			if(play_list==1)
				P0 = 0xB9;
			if(play_list==2)
				P0 = 0xF1;
		}
		if(play_flag==1)
			playMusic();
		if(record_play_flag==1)
			playRecord();
	}
}