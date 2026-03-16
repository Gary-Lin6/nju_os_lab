#include "main.h"

// Simple delay function
void delay() {
	volatile int i;
	for (i = 0; i < 100000; i++);
}

// Convert BCD to string
int bcdToStr(char bcd, char *buf) {
	// TODO: 把bcd转换为string并存入buf中
	
	return 2;
}

// Main program: display RTC time
int main(void) {
	char rtc_time_str[8] = "00:00:00";
	int row = 12;
	int col = (80 - 8) / 2;
	char hint_str[] = "Current Time:";
	displayStr(row - 1, (80 - 12) / 2, hint_str, 12);
	
	while (1) {
		// TODO: 实现展示时间的功能
		
		delay();
	}
	return 0;
}
