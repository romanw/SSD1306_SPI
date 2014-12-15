#include "SSD1306_SPI.h"
#include <SPI.h>

SSD1306_SPI::SSD1306_SPI()
{
}

void SSD1306_SPI::begin(bool invert)
{
	this->begin(invert, 0x7f);
}

void SSD1306_SPI::begin(bool invert, uint8_t contrast)
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

size_t SSD1306_SPI::write(uint8_t data)
{
	// Non-ASCII characters are not supported.
	if (data < 0x20 || data > 0x7F) return 0;

	uint8_t buf[6];
	memcpy_P(buf, ASCII[data - 0x20], 5);
	buf[5] = 0x00;
	this->writeDisplay(SSD1306_DATA, buf, 6);
	this->advanceXY(6);
	return 1;
}

void SSD1306_SPI::clear()
{
	// set column start and end address
	this->writeDisplay(SSD1306_COMMAND, 0x21);  // set column start and end address
	this->writeDisplay(SSD1306_COMMAND, 0x00);	// start at 0
	this->writeDisplay(SSD1306_COMMAND, 0x7f);	// end at 127
	// set row start and end address
	this->writeDisplay(SSD1306_COMMAND, 0x22);	// set column start and end address
	this->writeDisplay(SSD1306_COMMAND, 0x00);	// start at 0
	this->writeDisplay(SSD1306_COMMAND, 0x07);	// end at 7

	for (uint16_t i = 0; i < BUF_LEN; i++)
		this->writeDisplay(SSD1306_DATA, 0x00);
	//SSD1306_PORT = (SSD1306_PORT & ~PINS_CE_DC) | SSD1306_DATA;
	//for (uint16_t i = BUF_LEN; i > 0; i--) SPI.transfer(0x00);
	//SSD1306_PORT |= PIN_CE;

	// set page addressing mode
	//this->writeDisplay(SSD1306_COMMAND, 0x20);
	//this->writeDisplay(SSD1306_COMMAND, 0x02);

	this->gotoXY(0, 0);
}

uint8_t SSD1306_SPI::gotoXY(uint8_t x, uint8_t y) 
{	
	if (x >= SSD1306_X_PIXELS || y >= SSD1306_ROWS) return SSD1306_ERROR;
	//SSD1306_PORT = (SSD1306_PORT & ~PINS_CE_DC) | SSD1306_COMMAND;
	//SPI.transfer(0x80 | x);
	//SPI.transfer(0x40 | y);
	//SSD1306_PORT |= PIN_CE;
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

	this->m_Column = x;
	this->m_Line = y;
	return SSD1306_SUCCESS;
}

uint8_t SSD1306_SPI::writeBitmap(const uint8_t *bitmap, uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	//if (x >= SSD1306_X_PIXELS || y >= SSD1306_ROWS) return;
	////height = (this->m_Line + height > SSD1306_Y_PIXELS / 8) ? ((SSD1306_Y_PIXELS / 8) - this->m_Line) : height;
	////width = (this->m_Column + width > SSD1306_X_PIXELS) ? (SSD1306_X_PIXELS - this->m_Column) : width;
	//this->gotoXY(x, y);
	//for (uint8_t y = 0; y < height; y++)
	//{
	//	for (uint8_t x = 0; x < width; x++)
	//		this->writeDisplay(SSD1306_DATA, bitmap[x + (y * width)]);
	//	this->gotoXY(this->m_Column, this->m_Line + 1);
	//}

	//this->advanceXY(width);
	
	if (this->gotoXY(x, y) == SSD1306_ERROR) return SSD1306_ERROR;	
	const uint8_t *maxY = bitmap + height * width;	

	for (const uint8_t *y = bitmap; y < maxY; y += width)
	{
		//for (uint8_t x = 0; x < width; x++, y++)
		//	this->writeDisplay(SSD1306_DATA, *y);
		
		this->writeDisplay(SSD1306_DATA, y , width);
		this->gotoXY(this->m_Column, this->m_Line + 1);
	}

	this->advanceXY(width);
}

void SSD1306_SPI::advanceXY(uint8_t columns)
{
	this->m_Column += columns;
	if (this->m_Column >= SSD1306_X_PIXELS)
	{
		this->m_Column %= SSD1306_X_PIXELS;
		this->m_Line++;
		this->m_Line %= SSD1306_ROWS;
		//this->gotoXY(this->m_Column, m_Line);
	}
}

void SSD1306_SPI::writeDisplay(uint8_t dataOrCommand, const uint8_t *data, uint16_t count)
{
	SSD1306_PORT = (SSD1306_PORT & ~PINS_CE_DC) | dataOrCommand;
	//for (uint16_t i = 0; i < count; i++)
	//	SPI.transfer(data[i]);
    for (uint16_t i = count; i > 0; i--)
		SPI.transfer(data[count-i]);
	SSD1306_PORT |= PIN_CE;
}

void SSD1306_SPI::writeDisplay(uint8_t dataOrCommand, uint8_t data)
{
	SSD1306_PORT = (SSD1306_PORT & ~PINS_CE_DC) | dataOrCommand;
	SPI.transfer(data);
	SSD1306_PORT |= PIN_CE;
}

