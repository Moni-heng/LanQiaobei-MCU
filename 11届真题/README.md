# 复盘
## 1
在写温度读取或者电压读取的时候
if(time_slow_all%100==0) vol=Ad_Read(0x41);
一定注意控制时间的多少，在允许的时间范围之内。
另外不能写成
if(time_slow_all%100) vol=Ad_Read(0x41);
负责读取时间太快，影响其他操作。