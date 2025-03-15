#include<STC15F2K60S2.H>
#include<key.H>
#include<seg.H>
#include<iic.H>
#include<led.H>
#include<intrins.H>

unsigned int time_slow_all;
unsigned char key_val,key_down,key_old;
unsigned char seg_pos;
unsigned char seg_buf[8]={13,13,13,13,13,13,13,13};
unsigned char point_buf[8]={0,0,0,0,0,0,0,0};	
unsigned char led_buf[8]={0,0,0,0,0,0,0,0};	
float vol;
float vol_disp=3.0;//电压参数
unsigned char vol_save;//电压保存值
unsigned char seg_show_mode;
unsigned char count;//计数
bit flag;//函数下降标志位
unsigned int time_5s;
bit flag_5s;
bit light;
unsigned char error;

void error_proc(){
	unsigned char i;
	for(i=5;i<17;i++){
	if(key_down==i) error++;
	}
	if(key_down==1) error=0;
	if(error==0) led_buf[2]=0;
	if(error>=3) led_buf[2]=1;
}

void key_proc(){
	if(time_slow_all%10) return;
	key_val=key_read();
	key_down=key_val&(key_val^key_old);
	key_old=key_val;
	switch(key_down){
			case 1://S12作用，切换
			if(++seg_show_mode==3)seg_show_mode=0;
			if(seg_show_mode==2){
			vol_save=(unsigned char)(vol_disp*10.0);
			EEPROM_Write(&vol_save,0,1);}
					break;
			case 2://S13作用，尽在计数界面清零
			if(seg_show_mode==2){error=0;count=0;}
			else error++;
			break;
		case 4://减函数--S17
			if(seg_show_mode==1){
				error=0;
		vol_disp-=0.5;
		if(vol_disp==(0-0.5)) vol_disp=5;}
			else error++;
			break;
		case 3://加函数--S16
			if(seg_show_mode==1){
				error=0;
		vol_disp+=0.5;
		if(vol_disp==5.5) vol_disp=0;}
		else 	error++;	
			break;
		}
	error_proc();
}

void seg_proc(){
	if(time_slow_all%50) return;

	switch(seg_show_mode){
		case 0://数据界面
			seg_buf[0]=10;
			seg_buf[5]=(unsigned int)(vol*100)/100%10;
			seg_buf[6]=(unsigned int)(vol*100)/10%10;
			seg_buf[7]=(unsigned int)(vol*100)%10;
			point_buf[5]=1;
			break;
			case 1://参数界面
			seg_buf[0]=11;
			seg_buf[5]=(unsigned int)(vol_disp*100)/100%10;
			seg_buf[6]=(unsigned int)(vol_disp*100)/10%10;
			seg_buf[7]=(unsigned int)(vol_disp*100)%10;
			point_buf[5]=1;
			break;
			case 2://计数界面
			seg_buf[0]=12;
			point_buf[5]=0;
			seg_buf[5]=(count>99)?count/100%10:13;
			seg_buf[6]=(count>9)?count/10%10:13;
			seg_buf[7]=count%10;
			break;
	
	}


}
void led_proc(){
	if(time_slow_all%50==0) vol=Ad_Read(0x43)/51.0;
	if(vol>vol_disp) flag=1;
	if(flag)if(vol<vol_disp){count++;flag=0;}
	flag_5s=(vol<vol_disp)?1:0;
	led_buf[1]=(count%2==0)?0:1;
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
void tiemr()interrupt 3{
	if(++time_slow_all==1000) time_slow_all=0;
	if(++seg_pos==8) seg_pos=0;
	seg_disp(seg_pos,seg_buf[seg_pos],point_buf[seg_pos]);
	led_disp(seg_pos,led_buf[seg_pos]);
	if(flag_5s){
		if(++time_5s==5000){
			time_5s=0;
			led_buf[0]=1;
		}
	}
	else {time_5s=0;led_buf[0]=0;}
}
void init(){
			P0=0XFF;
			P2=(P2&0X1F)|0X80;
			P2&=0X1F;
	
			P0=0X00;
			P2=(P2&0X1F)|0XA0;
			P2&=0X1F;
}//关闭led和蜂鸣器等外设

void main(){
	init();
	Timer1_Init();
	EEPROM_Read(&vol_save,0,1);
	vol_disp=vol_save/10.0;
		while(1){
		key_proc();
			seg_proc();
			led_proc();
		}
}