#include "main.h"
#include "port.h"
#include "..\lib\Lcd4.h"
#include "..\lib\Rtc_Ds1307.h"
#include "..\lib\Soft_I2C.h"
#include "..\lib\Delay.h"

unsigned char * code days[] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};

bit alarm = 0;

void display_real_time(unsigned char second, unsigned char minute, unsigned char hour, unsigned char day, unsigned char date, unsigned char month, unsigned char year)
{
	unsigned char old_second;
	if(old_second != second) // do IC chay nhanh qua, man se giat, cau lenh nay va cau lenh ngay duoi, dung de neu lam mach thuc te se khong bi giat, do co 1 thoi gian cho
	{
		old_second = second; //
		Lcd_Chr(1, 5, hour/10+0x30); //xuat ky tu ra LCD nen phai +48 theo ma ASCII, o day dung con tro, cac so sau in ngay sau day
		Lcd_Chr_Cp(hour%10+0x30);
		Lcd_Chr_Cp(':'); //???
		Lcd_Chr_Cp(minute/10+0x30); //tuong tu
		Lcd_Chr_Cp(minute%10+0x30);
		Lcd_Chr_Cp(':'); //???
		Lcd_Chr_Cp(second/10+0x30); //tuong tu
		Lcd_Chr_Cp(second%10+0x30);
		
		Lcd_Out(2, 2, days[day - 1]); // do mang tu 0-7, con ngay tu 1 den 8, doi cho con tro, cac so sau in ngay sau day
		Lcd_Chr_Cp(' ');
		Lcd_Chr_Cp(date/10+0x30); //xuat ngay trong thang
		Lcd_Chr_Cp(date%10+0x30);
		Lcd_Out_Cp("/");
		Lcd_Chr_Cp(month/10+0x30); //xuat thang
		Lcd_Chr_Cp(month%10+0x30);
		Lcd_Out_Cp("/20");
		Lcd_Chr_Cp(year/10+0x30); //xuat nam
		Lcd_Chr_Cp(year%10+0x30);
	}
}

void setupalarm_hour(unsigned char *set_up_hour, unsigned char *set_up_minute)
{
	Lcd_Out(1, 2, "Alarm's Hour:");
	Lcd_Cmd(0xC1);
	Lcd_Chr_Cp(set_up_hour[0]/10+0x30); 
	Lcd_Chr_Cp(set_up_hour[0]%10+0x30);
	Lcd_Chr_Cp(':');
	Lcd_Chr_Cp(set_up_minute[0]/10+0x30); 
	Lcd_Chr_Cp(set_up_minute[0]%10+0x30);
	Lcd_Out_Cp("          ");
	if(BTN_UP == 0)
	{
		Delay_ms(500);
		set_up_hour[0]++;
		if(set_up_hour[0] > 0x18)	
		{
			set_up_hour[0] = 0; //do chi hen gio trong ngay nen neu nhieu hon 24 gio thi se quay ve 0
		}
	}
	if(BTN_DOWN == 0)
	{
		Delay_ms(500);
		if(set_up_hour[0] > 0x00)
		{
			set_up_hour[0]--; //do chi hen gio trong ngay nen neu nho hon 0 gio thi se khong giam duoc nua
		}
	}
}

void setupalarm_minute(unsigned char *set_up_hour, unsigned char *set_up_minute)
{
	//co the clean lai de LCD khong phai viet nhieu
	Lcd_Out(1, 2, "Alarm's Min:    ");
	Lcd_Cmd(0xC1); // chuyen xuong dong 2
	Lcd_Chr_Cp(set_up_hour[0]/10+0x30); 
	Lcd_Chr_Cp(set_up_hour[0]%10+0x30);
	Lcd_Chr_Cp(':');
	Lcd_Chr_Cp(set_up_minute[0]/10+0x30); 
	Lcd_Chr_Cp(set_up_minute[0]%10+0x30);
	Lcd_Out_Cp("          ");
	if(BTN_UP == 0)
	{
		Delay_ms(500);
		set_up_minute[0]++;
		if(set_up_minute[0] > 0x3B)
		{
			set_up_hour[0]++;
			set_up_minute[0] = 0;
		}
	}
	if(BTN_DOWN == 0)
	{
		Delay_ms(500);
		if(set_up_hour[0] > 0x00)
		{
			if(set_up_minute[0] > 0x00)
			{
				set_up_minute[0]--;
			}
			else	//neu phut < 0, thi se tru gio di 1, phut bang 0
			{
				set_up_minute[0] = 59;
				set_up_hour[0]--;
			}
		}
		else
		{
			if(set_up_minute[0] > 0x00)
			{
				set_up_minute[0]--; //phut > 0 thi se tru nhu binh thuong
			}
		}
	}
}

void mode_alarm(unsigned char *remind_times)
{
	Lcd_Out(1, 2, "Alarm Clock:    "); //in ra o hang 1 cot 2
	Lcd_Cmd(0xC1); //chuyen con tro ve dau hang thu 2
	if(BTN_UP == 0 || BTN_DOWN == 0)
	{
		Delay_ms(500);
		alarm = !alarm;
	}
	if(alarm == 1)
	{
		Lcd_Out_Cp("ON             ");
		remind_times[0] = 1;
	}
	else
	{
		Lcd_Out_Cp("OFF            ");
	}
}

void alarming()
{
	Lcd_Out(1, 2, "Press 'Next' to ");	//in ra o hang 1 cot 2
	Lcd_Cmd(0xC1);
	Lcd_Out_Cp("shut me up!pls!");
	P1_2 = 0;		//bat chuong
}

void main()
{
	unsigned char mode = 0, set_up_hour = 21, set_up_minute = 4;
	unsigned char second, minute, hour, day, date, month, year, mode_clock, remind_times = 0;
	
	Soft_I2c_Init();
	Ds1307_Init();
	Lcd_Init();
	while(1) 
	{
		Ds1307_Read_Time(&hour, &minute, &second, &mode_clock);
		Ds1307_Read_Date(&day, &date, &month, &year);  //chi hien thi duoc tu 2000 den 2099
		if(alarm == 1 && remind_times == 1) //dieu kien de xuat hien bao thuc 
		{
			if(set_up_hour == hour && set_up_minute == minute) //neu gio hien tai trung gio hen
			{
				while(BTN_NEXT == 1) //neu khong an nut, chuong se keu het 1 phut
				{
					alarming();
				}
				remind_times = 0; //kiem tra xem da reo hay chua
				P1_2 = 1; //ngat chuong
				mode = 0;	//quay ve man hinh chinh
				Lcd_Cmd(0x01);	//xoa man hinh
				Delay_ms(500);
			}
		}
		if(BTN_NEXT == 0)
		{
			Delay_ms(500);
			mode++;
		}
		if(mode == 0)
		{
			display_real_time(second, minute, hour, day, date, month, year);
		}
		if(mode == 1)
		{
			setupalarm_hour(&set_up_hour, &set_up_minute);
		}
		if(mode == 2)
		{
			setupalarm_minute(&set_up_hour, &set_up_minute);
		}
		if(mode == 3)
		{
			mode_alarm(&remind_times);
		}
		if(mode == 4)
		{
			Lcd_Cmd(0x01); //xoa man hinh
			mode = 0;	//quay ve man hinh chinh
			Delay_ms(20);
		}
		
	}
}