#include<key.h>
#include<STC15F2K60S2.H>
#include<seg.h>
#include<led.h>
#include<iic.h>
#include<ds1302.h>
#include<init.h>
#define uchar unsigned char;
unsigned int time_slow_all;
unsigned char key_val,key_down,key_old;
unsigned char seg_pos;
uchar seg_buf[8]={10,10,10,10,10,10,10,10};
uchar seg_point[8]={0,0,0,0,0,0,0,0};
uchar led_buf[8]={0,0,0,0,0,0,0,0};
unsigned char ucRtc[3]={23,59,56};
uchar seg_show_mode;
bit chaoxian_jiaozhun;
bit pinlv_shijian;
unsigned int PF=2000;//超限参数
int BF;//标准参数
unsigned int freq_max;
unsigned int ucRtc_huixian[3];
uchar time_0_2s;
bit light_1;
bit light_2;
int freq;
unsigned int freq_1s;
void key_proc(){
	if(time_slow_all%10) return;
	key_val=key_read();
	key_down=key_val&(key_val^key_old);
	key_old=key_val;
	switch(key_down){
		case 4:
	if(++seg_show_mode==4) {seg_show_mode=0;chaoxian_jiaozhun=0;pinlv_shijian=0;}
		break;
		case 5:
		if(seg_show_mode==1)chaoxian_jiaozhun^=1;	
		if(seg_show_mode==3)pinlv_shijian^=1;
		break;
		case 8:
		if(seg_show_mode==1&&chaoxian_jiaozhun==0){	PF+=1000;
		if(PF==10000) PF=1000;}
		if(seg_show_mode==1&&chaoxian_jiaozhun==1){	BF+=100;
		if(BF==1000) BF=900;}	
		break;
		case 9:
		if(seg_show_mode==1&&chaoxian_jiaozhun==0){	PF-=1000;	
		if(PF==0) PF=9000;}	
		if(seg_show_mode==1&&chaoxian_jiaozhun==1){	BF-=100;
		if(BF==-1000) BF=-900;	}	
			break;
	}

}
void seg_proc(){
		Read_Rtc(ucRtc);
	if(time_slow_all%50) return;
	switch(seg_show_mode){
		case 0://频率界面
			seg_buf[0]=14;
			seg_buf[1]=10;
			seg_buf[2]=10;
		seg_buf[3]=(freq>0)?((freq>9999)?(freq/10000%10):10):10;
		seg_buf[4]=(freq>0)?((freq>999)?(freq/1000%10):10):10;
		seg_buf[5]=(freq>0)?((freq>99)?(freq/100%10):10):10;
		seg_buf[6]=(freq>0)?((freq>9)?(freq/10%10):10):13;
		seg_buf[7]=(freq>0)?((freq%10)):13;
			break;
		case 1://参数界面
			////==========
		if(!chaoxian_jiaozhun){
			//超限参数界面
			seg_buf[0]=11;
			seg_buf[1]=1;
			seg_buf[2]=10;
			seg_buf[3]=10;
			seg_buf[4]=PF/1000%10;
			seg_buf[5]=PF/100%10;
			seg_buf[6]=PF/10%10;
			seg_buf[7]=PF/1%10;
		}
		else{
			//参数校准界面
			seg_buf[0]=11;
			seg_buf[1]=2;
			seg_buf[2]=10;
			seg_buf[3]=10;
			
			seg_buf[4]=(BF>=0)?10:15;
			if(BF!=0){
			seg_buf[5]=(BF>0)?(BF/100%10):(((-1)*BF)/100%10);
			seg_buf[6]=(BF>0)?(BF/10%10):(((-1)*BF)/10%10);
			seg_buf[7]=(BF>0)?(BF/1%10):(((-1)*BF)/1%10);
			}
			else {unsigned char i;for(i=5;i<7;i++){seg_buf[i]=10;}seg_buf[7]=0;}
		}
		/////===========
			break;
			case 2://时间界面
			seg_buf[0]=ucRtc[0]/10%10;
			seg_buf[1]=ucRtc[0]%10;
			seg_buf[2]=15;
			seg_buf[3]=ucRtc[1]/10%10;
			seg_buf[4]=ucRtc[1]%10;
			seg_buf[5]=15;
			seg_buf[6]=ucRtc[2]/10%10;
			seg_buf[7]=ucRtc[2]%10;				
			break;
			case 3://回显界面
			if(!pinlv_shijian){//频率回显
			seg_buf[0]=12;
			seg_buf[1]=14;
			seg_buf[2]=10;
				seg_buf[3]=(freq_max>9999)?(freq_max/10000%10):10;
				seg_buf[4]=(freq_max>999)?(freq_max/1000%10):10;
				seg_buf[5]=(freq_max>99)?(freq_max/100%10):10;
				seg_buf[6]=(freq_max>9)?(freq_max/10%10):10;
				seg_buf[7]=freq_max/1%10;
			}
			else{//时间回显
			seg_buf[0]=12;
			seg_buf[1]=16;
			seg_buf[2]=ucRtc_huixian[0]/10%10;
			seg_buf[3]=ucRtc_huixian[0]%10;
			seg_buf[4]=ucRtc_huixian[1]/10%10;
			seg_buf[5]=ucRtc_huixian[1]%10;
			seg_buf[6]=ucRtc_huixian[2]/10%10;
			seg_buf[7]=ucRtc_huixian[2]%10;
			}
			break;
	}

}
void freq_time(){
	unsigned char k;
	if(freq_max<freq){
	for(k=0;k<3;k++){	ucRtc_huixian[k]=ucRtc[k];}
	freq_max=freq;
	}
}
void DAC_Output(){
	if(freq<0) Da_Write(0);
	else if(freq<500) Da_Write(51);
	else if(freq>PF) Da_Write(255);
	else Da_Write(51.0*((4.0/(PF-500))*(freq-500.0)+1.0));
}
void led_proc(){
	freq_time();
	if(seg_show_mode==0) led_buf[0]=light_1;else led_buf[0]=0;
	if(freq<0) led_buf[1]=1;
	else{
		if(freq<=PF) led_buf[1]=0;
		else led_buf[1]=light_2;}
	DAC_Output();
}

void Timer0_Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TMOD|=0X05;
	TL0 = 0x00;				//设置定时初始值
	TH0 = 0x00;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	EA=1;
	ET0=1;
}

void Timer1_Init(void)		//1毫秒@12.000MHz
{
	AUXR &= 0xBF;			//定时器时钟12T模式
	TMOD &= 0x0F;			//设置定时器模式
	TL1 = 0x18;				//设置定时初始值
	TH1 = 0xFC;				//设置定时初始值
	TF1 = 0;				//清除TF1标志
	TR1 = 1;				//定时器1开始计时
	EA=1;
	ET1=1;
}
void timer()interrupt 3{
		if(++time_slow_all==1000) {time_slow_all=0;
		freq=(TH0<<8|TL0);
		freq+=BF;
		TH0=0;TL0=0;}
		if(++seg_pos==8) seg_pos=0;
		seg_disp(seg_pos,seg_buf[seg_pos],seg_point[seg_pos]);
		led_disp(seg_pos,led_buf[seg_pos]);
		if(seg_show_mode==0||freq>PF)if(++time_0_2s==200){light_1^=1;time_0_2s=0;light_2^=1;} 
}
void main(){
	init();
	Timer0_Init();
	Timer1_Init();
	Set_Rtc(ucRtc);
	while(1){
	key_proc();
		led_proc();
			seg_proc();	
	}


}
