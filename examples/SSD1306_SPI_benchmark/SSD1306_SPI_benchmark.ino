#include <SPI.h>
#include "SSD1306_SPI.h"

#define USE_FRAME_BUFFER

#ifdef USE_FRAME_BUFFER
SSD1306_SPI_FB oled;
#else
SSD1306_SPI oled;
#endif

void setup(void)
{
	Serial.begin(9600);
	oled.begin();
	oled.print(F("Preparing benchmark"));
#ifdef USE_FRAME_BUFFER
	oled.renderAll();
#endif
	delay(1000);
}

uint8_t bmp[] = {
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 
	0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 
};

void loop(void) 
{
	size_t len;

	oled.clear();
	unsigned long time = micros();
	len = oled.print(F("0123456789_ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz{}[]()*&^%$!#~@;;<>?"));
#ifdef USE_FRAME_BUFFER
	oled.renderString(0, 0, len);
#endif
	time = micros() - time;
	delay(3000);

	oled.clear();
	oled.print(F("The time it took to print 84 chars is:    "));
	oled.print(time);
#ifdef USE_FRAME_BUFFER
	oled.renderAll();
#endif
	delay(3000);

	oled.clear();
	oled.gotoXY(5,3);
	oled.print(F("Test gotoXY"));
#ifdef USE_FRAME_BUFFER
	oled.renderAll();
#endif
	delay(3000);

	oled.clear();
	time = micros();
	oled.writeBitmap(bmp, 10, 2, 25, 3);
#ifdef USE_FRAME_BUFFER
	oled.renderAll();
#endif
	time = micros() - time;
	delay(3000);

	oled.clear();
	oled.print(F("The time it took to draw a 25x3 (25x18) bitmap is: "));
	oled.print(time);
#ifdef USE_FRAME_BUFFER
	oled.renderAll();
#endif
	delay(3000);
	
#ifdef USE_FRAME_BUFFER
	oled.clear();
	time = micros();
	oled.writeRect(5, 5, 50, 40);
	oled.writeLine(75, 3, 75, 35);
	oled.writeLine(60, 10, 60, 40);
	oled.writeLine(10, 47, 60, 47);
	oled.renderAll();
	time = micros() - time;
	delay(3000);

	oled.clear();
	len = oled.print(F("The time it took draw a rect and 3 lines: "));
	len += oled.print(time);
	oled.renderString(0, 0, len);
	delay(3000);

	oled.clear();
	time = micros();
	for (uint8_t row = 0; row < SSD1306_Y_PIXELS; row++)
	{
		for (uint8_t col = 0; col < SSD1306_X_PIXELS; col++)
		{
			uint8_t pixel = (col + row) % 2;
			oled.setPixel(col, row, pixel);
		}
	}
	oled.renderAll();
	time = micros() - time;
	delay(5000);

	oled.clear();
	oled.print(F("The time it took to run setPixel on all 8192 pixels and render it:    "));
	oled.print(time);
	oled.renderAll();
	delay(5000);
#endif
}
