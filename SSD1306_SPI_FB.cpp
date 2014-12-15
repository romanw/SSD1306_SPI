#include "SSD1306_SPI.h"
#include <SPI.h>

SSD1306_SPI_FB::SSD1306_SPI_FB()
{
}

void SSD1306_SPI_FB::begin(bool invert)
{
	this->begin(invert, 0x7f);
}

void SSD1306_SPI_FB::begin(bool invert, uint8_t contrast)
{
	SSD1306_PORT |= (PIN_DC | PIN_RESET | PIN_CE);
	SSD1306_DDR |= (PIN_DC | PIN_RESET | PIN_CE);
	SPI.begin();
	
	// LCD init section:
	
	//uint8_t invertSetting = invert ? 0x0D : 0x0C;
	// Must reset LCD first!
	SSD1306_PORT &= ~PIN_RESET;
	SSD1306_PORT |= PIN_RESET;

	// from the Application Note section at the bottom of the SDD1306 datasheet
	//= Set MUX Ratio A8h, 3Fh (default/reset value)
	//this->writeDisplay(SSD1306_COMMAND, 0xa8);
	//this->writeDisplay(SSD1306_COMMAND, 0x3f);
	//= Set Display Offset D3h, 00h (default/reset value)
	//this->writeDisplay(SSD1306_COMMAND, 0xd3);
	//this->writeDisplay(SSD1306_COMMAND, 0x00);
	//= Set Display Start Line 40h (default/reset value)
	//this->writeDisplay(SSD1306_COMMAND, 0x40);
	//= Set Segment re-map A0h/A1h (default/reset value = A0)
	this->writeDisplay(SSD1306_COMMAND, 0xa1);
	//= Set COM Output Scan Direction C0h/C8h (default/reset value = C0)
	this->writeDisplay(SSD1306_COMMAND, 0xc8);
	//= Set COM Pins hardware configuration DAh, 02 (default/reset value = DA 12)
	//this->writeDisplay(SSD1306_COMMAND, 0xda);
	//this->writeDisplay(SSD1306_COMMAND, 0x02);
	//= Set Contrast Control 81h, 7Fh (default/reset value)
	//= Note: The chip has 256 contrast steps from 00h to FFh.
	//= The segment output current increases as the contrast step value increases.
	this->writeDisplay(SSD1306_COMMAND, 0x81);
	this->writeDisplay(SSD1306_COMMAND, contrast);
	//= Disable Entire Display On A4h (default/reset value)
	//this->writeDisplay(SSD1306_COMMAND, 0xa4);
	//= Set Normal Display A6h (default/reset value)
	this->writeDisplay(SSD1306_COMMAND, 0xa6 | invert);
	//= Set Osc Frequency D5h, 80h (default/reset value)
	//this->writeDisplay(SSD1306_COMMAND, 0xd5);
	//this->writeDisplay(SSD1306_COMMAND, 0x80);
	//= Enable charge pump regulator 8Dh, 14h
	this->writeDisplay(SSD1306_COMMAND, 0x8d);
	this->writeDisplay(SSD1306_COMMAND, 0x14);
	//= Display On AFh
	this->writeDisplay(SSD1306_COMMAND, 0xaf);

	// set horizontal addressing mode
	this->writeDisplay(SSD1306_COMMAND, 0x20);
	this->writeDisplay(SSD1306_COMMAND, 0x00);

	this->clear();
}

size_t SSD1306_SPI_FB::write(uint8_t data)
{
	// Non-ASCII characters are not supported.
	if (data < 0x20 || data > 0x7F) return 0;
	
	if (this->m_Position + 5 >= BUF_LEN) this->m_Position -= (BUF_LEN - 6);
	memcpy_P(this->m_Buffer + this->m_Position, ASCII[data - 0x20], 5);
	this->m_Buffer[this->m_Position+5] = 0x00;
	this->m_Position += 6;
	if (this->m_Position >= BUF_LEN) this->m_Position -= BUF_LEN;
	//this->m_Position %= BUF_LEN;
	return 1;
}

void SSD1306_SPI_FB::clear(bool render)
{
	memset(this->m_Buffer, 0x00, sizeof(this->m_Buffer));
	if (render)
		this->renderAll();
	//this->m_Position = 0;
	this->gotoXY(0, 0);
}

uint8_t SSD1306_SPI_FB::gotoXY(uint8_t x, uint8_t y) 
{	
	if (x >= SSD1306_X_PIXELS || y >= SSD1306_ROWS) return SSD1306_ERROR;
	this->writeDisplay(SSD1306_COMMAND, 0x21);  // set column start and end address
	this->writeDisplay(SSD1306_COMMAND, x);		// set column start address
	this->writeDisplay(SSD1306_COMMAND, 0x7f);	// set column end address
	this->writeDisplay(SSD1306_COMMAND, 0x22);	// set column start and end address
	this->writeDisplay(SSD1306_COMMAND, y);		// set row start address
	this->writeDisplay(SSD1306_COMMAND, 0x07);	// set row end address
/*
	this->writeDisplay(SSD1306_COMMAND, 0xb0 | y);		// set row start address
	this->writeDisplay(SSD1306_COMMAND, x & 0x0f);		// set column start address
	this->writeDisplay(SSD1306_COMMAND, 0x10 | (x >> 4));		// set column start address
*/

	this->m_Position = (SSD1306_X_PIXELS * y) + x;
	return SSD1306_SUCCESS;
}

uint8_t SSD1306_SPI_FB::writeBitmap(const uint8_t *bitmap, uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	//if (x >= SSD1306_X_PIXELS || y >= SSD1306_Y_PIXELS) return;
	//this->gotoXY(x, y);
	//uint16_t pos = this->m_Position;
	//for (uint8_t y = 0; y < height; y++)
	//{
	//	memcpy(this->m_Buffer + pos, bitmap + (y*width), width);
	//	pos += SSD1306_X_PIXELS;
	//}
	
	if (this->gotoXY(x, y) == SSD1306_ERROR) return SSD1306_ERROR;

	uint8_t *pos = this->m_Buffer + this->m_Position;
    const uint8_t *maxY = bitmap + height * width;	

	for (const uint8_t *y = (uint8_t*) bitmap; y < maxY; y += width)
	{
		memcpy(pos, y, width);
		pos += SSD1306_X_PIXELS;
	}
	return SSD1306_SUCCESS;
}

void SSD1306_SPI_FB::renderAll()
{	
	this->gotoXY(0, 0);
	this->writeDisplay(SSD1306_DATA, this->m_Buffer, BUF_LEN);
}

uint8_t SSD1306_SPI_FB::renderString(uint8_t x, uint8_t y, uint16_t length)
{
	if (this->gotoXY(x, y) == SSD1306_ERROR) return SSD1306_ERROR;
	length *= 6;
	this->writeDisplay(SSD1306_DATA, this->m_Buffer + this->m_Position, length);
	this->m_Position += length;
	return SSD1306_SUCCESS;
}

void SSD1306_SPI_FB::setPixel(uint8_t x, uint8_t y, uint8_t value)
{
	if (x >= SSD1306_X_PIXELS || y >= SSD1306_Y_PIXELS) return;
	uint8_t bank = y / 8;
	uint8_t bitMask = 1 << (y % 8);
	uint8_t &byte = this->m_Buffer[(SSD1306_X_PIXELS * bank) + x];
	if (value)
		byte |= bitMask;
	else
		byte &= ~bitMask;
}

uint8_t SSD1306_SPI_FB::writeLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	if (x1 == x2 || y1 == y2)
	{
		if (y1 > y2)
			swap(y1, y2);
		if (x1 > x2)
			swap(x1, x2);
		return this->writeRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
	}
	else
		return SSD1306_ERROR;
}

void SSD1306_SPI_FB::swap(uint8_t &a, uint8_t &b)
{
	uint8_t temp = a;
	a = b;
	b = temp;
}

uint8_t SSD1306_SPI_FB::writeRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, bool fill)
{
	if (x >= SSD1306_X_PIXELS || y >= SSD1306_Y_PIXELS || width == 0 || height == 0) return SSD1306_ERROR;

	// Calculate the bitmasks for the pixels.
	uint8_t bottom = y + height - 1;
	uint8_t bankTop = y / 8;
	uint8_t bankBottom = bottom / 8;
	uint8_t bitMaskTop = 0x01;
	uint8_t bitMaskBottom = 0x80;
	uint8_t bitMaskTopFill = 0xFF;
	uint8_t bitMaskBottomFill = 0xFF;
	bitMaskTop <<= (y % 8);
	bitMaskBottom >>= 7 - (bottom % 8);
	bitMaskTopFill <<= (y % 8);
	bitMaskBottomFill >>= 7 - (bottom % 8);

	// When fill is selected, we'll use the FillMask.
	if (fill)
	{
		bitMaskTop = bitMaskTopFill;
		bitMaskBottom = bitMaskBottomFill;
	}

	// When the rectangle fits in a single bank, we AND the top and bottom masks
	// So that we only fill the required area on the LCD.
	if (bankTop == bankBottom)
	{
		bitMaskTop = fill ? bitMaskTop & bitMaskBottom : bitMaskTop | bitMaskBottom;
		bitMaskTopFill &= bitMaskBottomFill;
	}
	this->gotoXY(x, bankTop);

	// Write the left 'side' of the rectangle on the top bank.
	this->m_Buffer[this->m_Position++] |= bitMaskTopFill;
	// Write a line or a fill.
	for (uint8_t i = 1; i < width-1; i++)
		this->m_Buffer[this->m_Position++] |= bitMaskTop;
	// Write the right 'side' of the rectangle on the top bank.
	if (width > 1)
		this->m_Buffer[this->m_Position++] |= bitMaskTopFill;

	this->m_Position += (SSD1306_X_PIXELS - width);

	// Write a fill across the middle banks or two sides of the rectangle.
	if (bankBottom - bankTop > 1)
	{
			for (uint8_t i = bankTop + 1; i < bankBottom; i++)
			{
				if (fill)
					memset(this->m_Buffer + this->m_Position, 0xFF, width);
				else
				{
					this->m_Buffer[this->m_Position] = 0xFF;
					this->m_Buffer[this->m_Position+width-1] = 0xFF;
				}
				this->m_Position += SSD1306_X_PIXELS;
			}
	}
	// If the rectangle spans across more than one bank,
	// apply the same logic for the bottom as the top.
	if (bankBottom > bankTop)
	{
		this->m_Buffer[this->m_Position++] |= bitMaskBottomFill;
		for (uint8_t i = 1; i < width-1; i++)
			this->m_Buffer[this->m_Position++] |= bitMaskBottom;
		if (width > 1)
			this->m_Buffer[this->m_Position++] |= bitMaskBottomFill;
	}
	return SSD1306_SUCCESS;
}

void SSD1306_SPI_FB::writeDisplay(uint8_t dataOrCommand, const uint8_t *data, uint16_t count)
{
	SSD1306_PORT = (SSD1306_PORT & ~PINS_CE_DC) | dataOrCommand;
	//for (uint16_t i = 0; i < count; i++)
	//	SPI.transfer(data[i]);
    for (uint16_t i = count; i > 0; i--)
		SPI.transfer(data[count-i]);
	SSD1306_PORT |= PIN_CE;
}

void SSD1306_SPI_FB::writeDisplay(uint8_t dataOrCommand, uint8_t data)
{
	SSD1306_PORT = (SSD1306_PORT & ~PINS_CE_DC) | dataOrCommand;
	SPI.transfer(data);
	SSD1306_PORT |= PIN_CE;
}

