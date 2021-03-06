#include "includes.h"
#include "lcd_low.h"
#include "lcd_ui.h"
//#include "timer.h"


#include "lcd_ui_fonts.h"



uint8_t 		lcdui_startH, lcdui_endH, lcdui_startV, lcdui_endV, lcdui_ramAddrOne, lcdui_ramAddrTwo;
int16_t			lcdui_width, lcdui_height, lcdui_cursor_x, lcdui_cursor_y; // Display w/h as modified by current rotation
LCDUI_FONT		*lcdui_current_font;
uint16_t		lcdui_bgcolor = LCDUI_R_G_B(0x00, 0x00, 0x00), lcdui_color = LCDUI_R_G_B(0xFF, 0xFF, 0xFF);

extern LCDUI_FONT		fnt_h18;




void	LCDUI_Init()
{
	lcdui_cursor_x = 0;
	lcdui_cursor_y = 0;
	lcdui_current_font = &fnt_h18;
	lcdui_bgcolor = LCDUI_RGB(0x000000);
	lcdui_color = LCDUI_RGB(0xFFFFFF);
}
//==============================================================================




uint16_t	LCDUI_getcharwidth(char c)
{
	if (c < 32)
		return 0;
	if (c > 126)
		c -= 65;
	c -= 32;
	if (c >= lcdui_current_font->symcount)
		return 0;
	uint16_t res = lcdui_current_font->width[c];
	if (res & 0x8000)
		return lcdui_current_font->width[(res & 0x7FFF)];
	return res;
}
//==============================================================================





char*	LCDUI_getchardata(char c)
{
	if (c < 32)
		return 0;
	if (c > 126)
		c -= 65;
	c -= 32;
	if (c >= lcdui_current_font->symcount)
		return 0;
	uint16_t c1 = lcdui_current_font->width[c];
	if (c1 & 0x8000)
		c = (c1 & 0x7FFF);
	uint16_t ch = lcdui_current_font->height;
	Int32S i = 0, ptr = 0, bits = 0, line_bits = ch;
	for (i = 0; i < c; i++)
	{
		if (lcdui_current_font->width[i] & 0x8000)
			continue;
		bits = lcdui_current_font->width[i] * line_bits;
		ptr += bits >> 3;
		if (bits & 0x07)
			ptr++;
	}

	return &(lcdui_current_font->data[ptr]);
}
//==============================================================================




void	LCDUI_DrawChar(char c,  uint16_t opt, int16_t x, int16_t y)
{
	if((x > lcdui_width) || (y > lcdui_height) || c < 32)
	{
		return;
	}
	if (x >= 0)
		lcdui_cursor_x = x;
	if (y >= 0)
		lcdui_cursor_y = y;
	if((lcdui_cursor_x > lcdui_width) || (lcdui_cursor_y > lcdui_height))
		return;

	uint16_t	cres[2];
	if (lcdui_current_font->alpha[0])
	{
		cres[0] = lcdui_color;
		cres[1] = lcdui_bgcolor;
	}
	else
	{
		cres[1] = lcdui_color;
		cres[0] = lcdui_bgcolor;
	}
	uint16_t cw = 0, ch = lcdui_current_font->height;
	uint16_t i = 0, ptr = 0, k = 0, y1 = 0;
	cw = LCDUI_text_getcharwidth(c);
	if (cw == 0)
		return;
	char *data = LCDUI_text_getchardata(c);
	if (data == 0)
		return;

	c = data[ptr++];
	uint8_t sh = 0, csh = 0;
	for (i = 0; i < ch; i++)
	{
		y1 = lcdui_cursor_y+i;
		LCDUI_write_reg_beg(lcdui_ramAddrOne);
		LCDUI_write_data_last(lcdui_cursor_x);
		LCDUI_write_reg_beg(lcdui_ramAddrTwo);
		LCDUI_write_data_last(y1);
		LCDUI_write_reg_beg(ILI9225_GRAM_DATA_REG);

		for (k = 0; k < cw; k++)
		{
			csh = (c >> sh);
			if (lcdui_cursor_x+k < lcdui_width)
			{
				LCDUI_write_data_cont(cres[(csh&0x01)]);
			}
			sh ++;
			if (sh == 8)
			{
				c = data[ptr++];
				sh = 0;
/*
				while (c == 0)
				{
				 	ce = 1;
					k += 4;
					if (k >= cw)
					{
						k -= cw;
						i++;
						if (i == ch)
						{
							k = cw;
							break;
						}
					}
					c = lcdui_current_font->data[ptr++];
				}
				if (ce)
				{
					ce = 0;
					LCDUI_write_data_close();
					if (k < cw && i < ch)
					{
						y1 = lcdui_cursor_y+i;
						LCDUI_write_reg_beg(lcdui_ramAddrOne);
						LCDUI_write_data_last(lcdui_cursor_x+k);
						LCDUI_write_reg_beg(lcdui_ramAddrTwo);
						LCDUI_write_data_last(y1);
						LCDUI_write_reg_beg(ILI9225_GRAM_DATA_REG);
					}
				}
*/
			}
		}
		LCDUI_write_data_close();
	}
	if (opt & LCDUI_TEXT_UNDERLINE)
	{
		LCDUI_write_reg_beg(lcdui_ramAddrOne);
		LCDUI_write_data_last(lcdui_cursor_x);
		LCDUI_write_reg_beg(lcdui_ramAddrTwo);
		LCDUI_write_data_last(lcdui_cursor_y+ch-1);
		LCDUI_write_reg_beg(ILI9225_GRAM_DATA_REG);
		for (k = 0; k < cw; k++)
		{
			LCDUI_write_data_cont(lcdui_color);
		}
		LCDUI_write_data_close();
	}

	if (opt & LCDUI_TEXT_OVERLINE)
	{
		LCDUI_write_reg_beg(lcdui_ramAddrOne);
		LCDUI_write_data_last(lcdui_cursor_x);
		LCDUI_write_reg_beg(lcdui_ramAddrTwo);
		LCDUI_write_data_last(lcdui_cursor_y+ch/2);
		LCDUI_write_reg_beg(ILI9225_GRAM_DATA_REG);
		for (k = 0; k < cw; k++)
		{
			LCDUI_write_data_cont(lcdui_color);
		}
		LCDUI_write_data_close();
	}

	lcdui_cursor_x += cw;
}
//==============================================================================





void	LCDUI_DrawCircle_helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername)
{
	int16_t	f     = r;
	int16_t	ddF_x = 1;
	int16_t	ddF_y = -2 * r;
	int16_t	x     = 0;
	int16_t	y     = r;

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		if (cornername & 0x4)
		{
			LCDUI_DrawPixel(x0 + x, y0 + y);
			LCDUI_DrawPixel(x0 + y, y0 + x);
		}
		if (cornername & 0x2)
		{
			LCDUI_DrawPixel(x0 + x, y0 - y);
			LCDUI_DrawPixel(x0 + y, y0 - x);
		}
		if (cornername & 0x8)
		{
			LCDUI_DrawPixel(x0 - y, y0 + x);
			LCDUI_DrawPixel(x0 - x, y0 + y);
		}
		if (cornername & 0x1)
		{
			LCDUI_DrawPixel(x0 - y, y0 - x);
			LCDUI_DrawPixel(x0 - x, y0 - y);
		}
	}
}
//==============================================================================





void	LCDUI_FillCircle_helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta)
{
	int16_t 	f     = r;
	int16_t 	ddF_x = 1;
	int16_t 	ddF_y = -2 * r;
	int16_t 	x     = 0;
	int16_t 	y     = r;

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		if (cornername & 0x1)
		{
			LCDUI_DrawFastVLine(x0+x, y0-y, 2*y+1+delta);
			LCDUI_DrawFastVLine(x0+y, y0-x, 2*x+1+delta);
		}
		if (cornername & 0x2)
		{
			LCDUI_DrawFastVLine(x0-x, y0-y, 2*y+1+delta);
			LCDUI_DrawFastVLine(x0-y, y0-x, 2*x+1+delta);
		}
	}
}
//==============================================================================





void	LCDUI_SetColor(uint16_t color)
{
	lcdui_color = color;
}
//==============================================================================





void	LCDUI_SetBackColor(uint16_t color)
{
	lcdui_bgcolor = color;
}
//==============================================================================





void	LCDUI_SetCursorCoord(int16_t x, int16_t y)
{
	lcdui_cursor_x = x;
	lcdui_cursor_y = y;
}
//==============================================================================





void	LCDUI_SetFont(uint8_t newfont)
{

	switch (newfont)
	{
		case LCDUI_FONT_1B12:
			lcdui_current_font = &font_1b12h;
			break;

		case LCDUI_FONT_1B18:
			lcdui_current_font = &font_1b18h;
			break;

		case LCDUI_FONT_1B22DIGITS:
			lcdui_current_font = &font_1b22h_digits;
			break;

		case LCDUI_FONT_1B62SPLASH:
			lcdui_current_font = &font_1b62h_splash;
			break;

		case LCDUI_FONT_1B22SYMBOLS:
			lcdui_current_font = &font_1b22h_symbols;
			break;

	}
}
//==============================================================================





uint32_t	LCDUI_GetTextWidth(const char *str)
{
	uint32_t i = 0, res = 0;
	char c;
	while(c = str[i])
	{
		if (c > 31)
			res += LCDUI_text_getcharwidth(c);
		i++;
	}
	return res;
}
//==============================================================================





uint32_t	LCDUI_GetCurrentFontHeight()
{
	return lcdui_current_font->height;
}
//==============================================================================





uint16_t	LCDUI_GetScreenWidth()
{
	return lcdui_width;
}
//==============================================================================





uint16_t	LCDUI_GetScreenHeight()
{
	return lcdui_height;
}
//==============================================================================





uint16_t	LCDUI_GetCurrentCursorX()
{
	return lcdui_cursor_x;
}
//==============================================================================





uint16_t	LCDUI_GetCurrentCursorY()
{
	return lcdui_cursor_y;
}
//==============================================================================





void	LCDUI_Clear()
{
	uint16_t treg = 0;

	LCDUI_write_reg(ILI9225_RAM_ADDR_SET1, 0);
	LCDUI_write_reg(ILI9225_RAM_ADDR_SET2, 0);

	treg = TFTSPI->CR2 & (~SPI_DataSize_16b);
	TFTSPI->CR2 = treg;
	SPI_Cmd(TFTSPI, ENABLE);
	LCDUI_RS_LOW();
	LCDUI_CS_ON();
	*(uint8_t*)&TFTSPI->DR = ILI9225_GRAM_DATA_REG;
	while ((TFTSPI->SR & SPI_I2S_FLAG_TXE) == 0);
	while ((TFTSPI->SR & SPI_I2S_FLAG_BSY) > 0);
	LCDUI_RS_HIGH();
	SPI_Cmd(TFTSPI, DISABLE);
	treg = TFTSPI->CR2 | SPI_DataSize_16b;
	TFTSPI->CR2 = treg;
	SPI_Cmd(TFTSPI, ENABLE);
	for (uint16_t i = 0; i < lcdui_height; i++)
	{
		for (uint16_t j = 0; j < lcdui_width; j++)
		{
/*
			if (lcdui_bggradient)
				TFTSPI->DR = gradback1[i];
			else
*/
				TFTSPI->DR =  lcdui_bgcolor;
			while(!(TFTSPI->SR & SPI_I2S_FLAG_TXE));
		}

	}
	while ((TFTSPI->SR & SPI_I2S_FLAG_TXE) == 0);
	while ((TFTSPI->SR & SPI_I2S_FLAG_BSY) > 0);
	LCDUI_CS_OFF();
	SPI_Cmd(TFTSPI, DISABLE);
	TFTSPI->CR2 = treg;
	lcdui_cursor_x = 0;
	lcdui_cursor_y = 0;
}
//==============================================================================





void	LCDUI_DrawPixel(uint16_t x1, uint16_t y1)
{
	if((x1 >= lcdui_width) || (y1 >= lcdui_height))
		return;
	LCDUI_write_reg(lcdui_ramAddrOne, x1);
	LCDUI_write_reg(lcdui_ramAddrTwo, y1);
	LCDUI_write_reg(ILI9225_GRAM_DATA_REG, lcdui_color);
}
//==============================================================================





void	LCDUI_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r)
{
	int16_t f = r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;
	LCDUI_DrawPixel(x0 - r, y0); // top left
	LCDUI_DrawPixel(x0, y0 + r); // top right
	LCDUI_DrawPixel(x0, y0-  r); // bottom right
	LCDUI_DrawPixel(x0 + r, y0); // bottom left

	while (x<y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		LCDUI_DrawPixel(x0 + x, y0 + y);
		LCDUI_DrawPixel(x0 - x, y0 + y);
		LCDUI_DrawPixel(x0 + x, y0 - y);
		LCDUI_DrawPixel(x0 - x, y0 - y);
		LCDUI_DrawPixel(x0 + y, y0 + x);
		LCDUI_DrawPixel(x0 - y, y0 + x);
		LCDUI_DrawPixel(x0 + y, y0 - x);
		LCDUI_DrawPixel(x0 - y, y0 - x);
	}
}
//==============================================================================





void	LCDUI_DrawFastVLine(int16_t x, int16_t y, int16_t h)
{
	if((x >= lcdui_width) || (y >= lcdui_height))
		return;
	if((y+h) >= lcdui_height)
		h = lcdui_height-y;
	LCDUI_set_window(x, y, x, y+h-1);
	LCDUI_write_reg_beg(ILI9225_GRAM_DATA_REG);
	while (h-- > 0)
		LCDUI_write_data_cont(lcdui_color);
	LCDUI_write_data_last(lcdui_color);
	LCDUI_set_window(0, 0, lcdui_width-1, lcdui_height-1);
}
//==============================================================================





void	LCDUI_DrawFastHLine(int16_t x, int16_t y, int16_t w)
{
	if((x >= lcdui_width) || (y >= lcdui_height))
		return;
	if((x+w) >= lcdui_width)
		w = lcdui_width-x;
	LCDUI_set_window(x, y, x+w, y);
	LCDUI_write_reg_beg(ILI9225_GRAM_DATA_REG);
	while (w-- > 0)
		LCDUI_write_data_cont(lcdui_color);
	LCDUI_write_data_close();
	LCDUI_set_window(0, 0, lcdui_width-1, lcdui_height-1);
}
//==============================================================================





void	LCDUI_FillRect(int16_t x, int16_t y, int16_t w, int16_t h)
{
	if((x >= lcdui_width) || (y >= lcdui_height))
		return;
	if((x + w) >= lcdui_width)
		w = lcdui_width - x;
	if((y + h) >= lcdui_height)
		h = lcdui_height - y;

	LCDUI_set_window(x, y, x+w-1, y+h-1);
	LCDUI_write_reg_beg(ILI9225_GRAM_DATA_REG);
	h *= w;
	for(y = 0; y < h; y++)
	{
		LCDUI_write_data_cont(lcdui_color);
	}
	LCDUI_write_data_close();
	LCDUI_set_window(0, 0, lcdui_width-1, lcdui_height-1);
}
//==============================================================================





void	LCDUI_FillCircle(int16_t x0, int16_t y0, int16_t r)
{
	LCDUI_DrawFastVLine(x0, y0-r, 2*r);
	LCDUI_fill_circle_helper(x0, y0, r, 3, 0);
}
//==============================================================================





void	LCDUI_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	if (y0 == y1)
	{
		if (x1 > x0)
			LCDUI_DrawFastHLine(x0, y0, x1 - x0);
		else
			if (x1 < x0)
				LCDUI_DrawFastHLine(x1, y0, x0 - x1);
			else
				LCDUI_DrawPixel(x0, y0);
		return;
	}
	else
		if (x0 == x1)
		{
			if (y1 > y0)
				LCDUI_DrawFastVLine(x0, y0, y1 - y0);
			else
				LCDUI_DrawFastVLine(x0, y1, y0 - y1);
			return;
		}
	bool steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep)
	{
//		swap(x0, y0);
		int16_t s = x0;
		x0 = y0;
		y0 = s;
//		swap(x1, y1);
		s = x1;
		x1 = y1;
		y1 = s;
	}
	if (x0 > x1)
	{
//		swap(x0, x1);
		int16_t s = x0;
		x0 = x1;
		x1 = s;
//		swap(y0, y1);
		y0 = y1;
		y1 = s;
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);
	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1)
		ystep = 1;
	else
		ystep = -1;

	int16_t xbegin = x0;
	if (steep)
	{
		for (; x0<=x1; x0++)
		{
			err -= dy;
			if (err < 0)
			{
				int16_t len = x0 - xbegin;
				if (len)
					LCDUI_DrawFastVLine(y0, xbegin, len + 1);
				else
					LCDUI_DrawPixel(y0, x0);
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1)
		LCDUI_DrawFastVLine(y0, xbegin, x0 - xbegin);
	}
	else
	{
		for (; x0<=x1; x0++)
		{
			err -= dy;
			if (err < 0)
			{
				int16_t len = x0 - xbegin;
				if (len)
					LCDUI_DrawFastHLine(xbegin, y0, len + 1);
				else
					LCDUI_DrawPixel(x0, y0);
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1)
			LCDUI_DrawFastHLine(xbegin, y0, x0 - xbegin);
	}
}
//==============================================================================





void	LCDUI_DrawRect(int16_t x, int16_t y, int16_t w, int16_t h)
{
	LCDUI_DrawFastHLine(x, y, w);
	LCDUI_DrawFastHLine(x, y+h, w);
	LCDUI_DrawFastVLine(x, y, h);
	LCDUI_DrawFastVLine(x+w, y, h);
}
//==============================================================================





void	LCDUI_DrawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r)
{
	// smarter version
	LCDUI_DrawFastHLine(x+r, y, w-2*r); // Top
	LCDUI_DrawFastHLine(x+r, y+h, w-2*r); // Bottom
	LCDUI_DrawFastVLine(x, y+r, h-2*r); // Left
	LCDUI_DrawFastVLine(x+w, y+r, h-2*r); // Right
	// draw four corners
	LCDUI_draw_circle_helper(x+r, y+r, r, 1);
	LCDUI_draw_circle_helper(x+w-r, y+r, r, 2);
	LCDUI_draw_circle_helper(x+w-r, y+h-r, r, 4);
	LCDUI_draw_circle_helper(x+r, y+h-r, r, 8);
}
//==============================================================================





void	LCDUI_FillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r)
{
	// smarter version
	LCDUI_FillRect(x+r, y, w-2*r, h);
	// draw four corners
	LCDUI_fill_circle_helper(x+w-r, y+r, r, 1, h-2*r);
	LCDUI_fill_circle_helper(x+r, y+r, r, 2, h-2*r);
}
//==============================================================================





void	LCDUI_DrawText(const char *str, uint16_t opt, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	uint16_t i = 0, oldcolor = lcdui_color, oldbgcolor = lcdui_bgcolor, copt = opt;
	char c;
	if (x1 > lcdui_width-1 || y1 > lcdui_height-1)
		return;
	if (x1 < 0)
		x1 = lcdui_cursor_x;
	if (y1 < 0)
		y1 = lcdui_cursor_y;
	if (x2 < 0)
		x2 = lcdui_width-1;
	if (y2 < 0)
		y2 = lcdui_height-1;
	lcdui_cursor_x = x1;
	lcdui_cursor_y = y1;



	char *cp = (char*)str;
	int16_t sp = 0, cw = 0, sw = 0;

	while(1)
	{
		c = cp[i];
		if (c == 0 || c == ' ')
		{
			sp = i;
			sw = cw;
		}
		cw += LCDUI_text_getcharwidth(c);
		if (c == '\n' || cw > x2-x1 || c == 0)
		{
			if (cw > x2-x1)
				cw = sw;
			if (sp == 0)
				sp = i;
			if (opt & LCDUI_TEXT_ALIGN_RIGHT)
				lcdui_cursor_x = x2 - cw;
			if (opt & LCDUI_TEXT_ALIGN_CENTER)
				lcdui_cursor_x = x1 + (x2 - x1 - cw) / 2;
			for (uint16_t j = 0; j < sp; j++)
			{
				LCDUI_text1b_char(cp[j], copt);
			}
			cw = 0;
			sw = 0;
			cp += sp;
			i = 0;
			sp = 0;
			while(cp[0] == ' ')
				cp++;
			if (c != 0)
			{
				lcdui_cursor_y += lcdui_current_font->height;
				lcdui_cursor_x = x1;
			}
			if (c == 0 || lcdui_cursor_y+lcdui_current_font->height > y2)
				break;
			if (c == '\n')
				i++;;
			continue;
		}
		i++;
		if (c == '.' || c == ',' || c == ':' || c == ';' || c == '!' || c == '?')
		{
//			cw += LCDUI_text_getcharwidth(c);
			sp = i;
			sw = cw;
		}

	}

	lcdui_bgcolor = oldbgcolor;
	lcdui_color = oldcolor;
}
//==============================================================================





