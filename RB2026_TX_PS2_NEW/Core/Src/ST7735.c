#include <ST7735.h>


int16_t _width;       ///< Display width as modified by current rotation
int16_t _height;      ///< Display height as modified by current rotation
int16_t cursor_x;     ///< x location to start print()ing text
int16_t cursor_y;     ///< y location to start print()ing text
uint8_t rotation;     ///< Display rotation (0 thru 3)
uint8_t _colstart;   ///< Some displays need this changed to offset
uint8_t _rowstart;       ///< Some displays need this changed to offset
uint8_t _xstart;
uint8_t _ystart;

  const uint8_t
  init_cmds1[] = {            // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,  
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 },                 //     16-bit color

#if (defined(ST7735_IS_128X128) || defined(ST7735_IS_160X128))
  init_cmds2[] = {            // Init for 7735R, part 2 (1.44" display)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F },           //     XEND = 127
#endif // ST7735_IS_128X128

#ifdef ST7735_IS_160X80
  init_cmds2[] = {            // Init for 7735S, part 2 (160x80 display)
    3,                        //  3 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x4F,             //     XEND = 79
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F ,            //     XEND = 159
    ST7735_INVON, 0 },        //  3: Invert colors
#endif

  init_cmds3[] = {            // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay

void ST7735_Select()
{
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
}

void ST7735_Unselect()
{
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
}

void ST7735_Reset()
{
    HAL_GPIO_WritePin(RST_PORT, RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(RST_PORT, RST_PIN, GPIO_PIN_SET);
}

  void ST7735_WriteCommand(uint8_t cmd)
  {
    HAL_GPIO_WritePin(RS_PORT, RS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&ST7735_SPI_PORT, &cmd, sizeof(cmd), HAL_MAX_DELAY);
}

void ST7735_WriteData(uint8_t* buff, size_t buff_size)
{
    HAL_GPIO_WritePin(RS_PORT, RS_PIN, GPIO_PIN_SET);
    HAL_SPI_Transmit(&ST7735_SPI_PORT, buff, buff_size, HAL_MAX_DELAY);
}

void DisplayInit(const uint8_t *addr)
{
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;
    while(numCommands--) {
        uint8_t cmd = *addr++;
        ST7735_WriteCommand(cmd);

        numArgs = *addr++;
        // If high bit set, delay follows args
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;
        if(numArgs) {
            ST7735_WriteData((uint8_t*)addr, numArgs);
            addr += numArgs;
        }

        if(ms) {
            ms = *addr++;
            if(ms == 255) ms = 500;
            HAL_Delay(ms);
        }
    }
}

void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    // column address set
    ST7735_WriteCommand(ST7735_CASET);
    uint8_t data[] = { 0x00, x0 + _xstart, 0x00, x1 + _xstart };
    ST7735_WriteData(data, sizeof(data));

    // row address set
    ST7735_WriteCommand(ST7735_RASET);
    data[1] = y0 + _ystart;
    data[3] = y1 + _ystart;
    ST7735_WriteData(data, sizeof(data));

    // write to RAM
    ST7735_WriteCommand(ST7735_RAMWR);
}

void ST7735_Init(uint8_t rotation)
{
    ST7735_Select();
    ST7735_Reset();
    DisplayInit(init_cmds1);
    DisplayInit(init_cmds2);
    DisplayInit(init_cmds3);
#if ST7735_IS_160X80
    _colstart = 24;
    _rowstart = 0;
 /*****  IF Doesn't work, remove the code below (before #elif) *****/
    uint8_t data = 0xC0;
    ST7735_Select();
    ST7735_WriteCommand(ST7735_MADCTL);
    ST7735_WriteData(&data,1);
    ST7735_Unselect();

#elif ST7735_IS_128X128
    _colstart = 2;
    _rowstart = 3;
#else
    _colstart = 0;
    _rowstart = 0;
#endif
    ST7735_SetRotation (rotation);
    ST7735_Unselect();

}

void ST7735_SetRotation(uint8_t m)
{

  uint8_t madctl = 0;

  rotation = m % 4; // can't be higher than 3

  switch (rotation)
  {
  case 0:
#if ST7735_IS_160X80
	  madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_BGR;
#else
      madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_RGB;
      _height = ST7735_HEIGHT;
      _width = ST7735_WIDTH;
      _xstart = _colstart;
      _ystart = _rowstart;
#endif
    break;
  case 1:
#if ST7735_IS_160X80
	  madctl = ST7735_MADCTL_MY | ST7735_MADCTL_MV | ST7735_MADCTL_BGR;
#else
      madctl = ST7735_MADCTL_MY | ST7735_MADCTL_MV | ST7735_MADCTL_RGB;
      _width = ST7735_HEIGHT;
      _height = ST7735_WIDTH;
    _ystart = _colstart;
    _xstart = _rowstart;
#endif
    break;
  case 2:
#if ST7735_IS_160X80
	  madctl = ST7735_MADCTL_BGR;
#else
      madctl = ST7735_MADCTL_RGB;
      _height = ST7735_HEIGHT;
      _width = ST7735_WIDTH;
    _xstart = _colstart;
    _ystart = _rowstart;
#endif
    break;
  case 3:
#if ST7735_IS_160X80
	  madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MV | ST7735_MADCTL_BGR;
#else
      madctl = ST7735_MADCTL_MX | ST7735_MADCTL_MV | ST7735_MADCTL_RGB;
      _width = ST7735_HEIGHT;
      _height = ST7735_WIDTH;
    _ystart = _colstart;
    _xstart = _rowstart;
#endif
    break;
  }
  ST7735_Select();
  ST7735_WriteCommand(ST7735_MADCTL);
  ST7735_WriteData(&madctl,1);
  ST7735_Unselect();
}

void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if((x >= _width) || (y >= _height))
        return;

    ST7735_Select();

    ST7735_SetAddressWindow(x, y, x+1, y+1);
    uint8_t data[] = { color >> 8, color & 0xFF };
    ST7735_WriteData(data, sizeof(data));

    ST7735_Unselect();
}

void ST7735_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) {
    uint32_t i, b, j;

    ST7735_SetAddressWindow(x, y, x+font.width-1, y+font.height-1);

    for(i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                uint8_t data[] = { color >> 8, color & 0xFF };
                ST7735_WriteData(data, sizeof(data));
            } else {
                uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                ST7735_WriteData(data, sizeof(data));
            }
        }
    }
}

void ST7735_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor) {
    ST7735_Select();

    while(*str) {
        if(x + font.width >= _width) {
            x = 0;
            y += font.height;
            if(y + font.height >= _height) {
                break;
            }

            if(*str == ' ') {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        ST7735_WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }

    ST7735_Unselect();
}

void ST7735_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if((x >= _width) || (y >= _height)) return;
    if((x + w - 1) >= _width) w = _width - x;
    if((y + h - 1) >= _height) h = _height - y;

    ST7735_Select();
    ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);

    uint8_t data[] = { color >> 8, color & 0xFF };
    HAL_GPIO_WritePin(RS_PORT, RS_PIN, GPIO_PIN_SET);
    for(y = h; y > 0; y--) {
        for(x = w; x > 0; x--) {
            HAL_SPI_Transmit(&ST7735_SPI_PORT, data, sizeof(data), HAL_MAX_DELAY);
        }
    }

    ST7735_Unselect();
}

void ST7735_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
    if((x >= _width) || (y >= _height)) return;
    if((x + w - 1) >= _width) return;
    if((y + h - 1) >= _height) return;

    ST7735_Select();
    ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);
    ST7735_WriteData((uint8_t*)data, sizeof(uint16_t)*w*h);
    ST7735_Unselect();
}

void ST7735_InvertColors(bool invert) {
    ST7735_Select();
    ST7735_WriteCommand(invert ? ST7735_INVON : ST7735_INVOFF);
    ST7735_Unselect();
}

void ST7735_DrawBitmap(uint16_t x, uint16_t y, const uint16_t *bitmap)
{
    uint8_t i, j;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            if (bitmap[i] & (1 << (15 - j))) {  // Kiểm tra bit đúng cách
                ST7735_DrawPixel(x + j, y + i, WHITE);
            }
        }
    }
}
void ST7735_DrawBitmapblack(uint16_t x, uint16_t y, const uint16_t *bitmap)
{
    uint8_t i, j;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            if (bitmap[i] & (1 << (15 - j))) {  // Kiểm tra bit đúng cách
                ST7735_DrawPixel(x + j, y + i, BLACK);
            }
        }
    }
}
void ST7735_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
    int dx = (x1 - x0), sx = (x0 < x1) ? 1 : -1;
    int dy = -(y1 - y0), sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy, e2;  // error value e_xy

    while (1) {
        ST7735_DrawPixel(x0, y0, color); // Vẽ pixel tại (x0, y0)
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }  // Di chuyển theo trục x
        if (e2 <= dx) { err += dx; y0 += sy; }  // Di chuyển theo trục y
    }
}

void ST7735_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if ((x >= _width) || (y >= _height)) return;
    if ((x + w - 1) >= _width) w = _width - x;
    if ((y + h - 1) >= _height) h = _height - y;

    // Vẽ 4 cạnh của hình chữ nhật
    ST7735_DrawLine(x, y, x + w - 1, y, color);          // Cạnh trên
    ST7735_DrawLine(x, y + h - 1, x + w - 1, y + h - 1, color);  // Cạnh dưới
    ST7735_DrawLine(x, y, x, y + h - 1, color);          // Cạnh trái
    ST7735_DrawLine(x + w - 1, y, x + w - 1, y + h - 1, color);  // Cạnh phải
}

void ST7735_DrawChar(uint8_t x, uint8_t y, char c, uint16_t color, uint16_t bgcolor, uint8_t size) {
    if (c < 32 || c > 126) return; // Kiểm tra ký tự hợp lệ

    for (int i = 0; i < 11; i++) { // Duyệt theo chiều rộng 11 pixel
        uint8_t line = font11x18[c - 32][i]; // Lấy dữ liệu cột
        for (int j = 0; j < 18; j++) { // Duyệt theo chiều cao 18 pixel
            if (line & (1 << j)) { // Kiểm tra từng bit trong cột
                ST7735_DrawPixel(x + i, y + j, color);
            } else {
                ST7735_DrawPixel(x + i, y + j, bgcolor);
            }
        }
    }
}

void ST7735_DrawText(uint8_t x, uint8_t y, char *text, uint16_t color, uint16_t bgcolor) {
    while (*text) {
        ST7735_DrawChar(x, y, *text, color, bgcolor, 1);  // Hiển thị từng ký tự
        x += 8;  // Dịch ngang 6 pixel cho ký tự tiếp theo
        text++;
    }
}

const uint16_t arrow_right_16x16[] = {
    0b0000000000000000,
    0b0000000011000000,
    0b0000000011100000,
    0b0000000001110000,
    0b0000000000111000,
    0b0000000000011100,
    0b0000000000001110,
    0b1111111111111111,
    0b1111111111111111,
    0b0000000000001110,
    0b0000000000011100,
    0b0000000000111000,
    0b0000000001110000,
    0b0000000011100000,
    0b0000000011000000,
    0b0000000000000000
};
const uint16_t arrow_left_16x16[] = {
	0b0000000000000000,
    0b0000001100000000,
    0b0000011100000000,
    0b0000111000000000,
    0b0001110000000000,
    0b0011100000000000,
    0b0111000000000000,
    0b1111111111111111,
    0b1111111111111111,
    0b0111000000000000,
    0b0011100000000000,
    0b0001110000000000,
    0b0000111000000000,
    0b0000011100000000,
    0b0000001100000000,
    0b0000000000000000
};
const uint16_t arrow_up_16x16[] = {
	0b0000000010000000,
    0b0000000111000000,
    0b0000011111110000,
    0b0000111111111000,
    0b0001110111011100,
    0b0011100111001110,
    0b0111000111000111,
    0b0110000111000011,
    0b0000000111000000,
    0b0000000111000000,
    0b0000000111000000,
    0b0000000111000000,
    0b0000000111000000,
    0b0000000111000000,
    0b0000000111000000,
    0b0000000111000000
};
const uint16_t arrow_down_16x16[] = {
    0b0000000111000000,
    0b0000000111000000,
    0b0000000111000000,
    0b0000000111000000,
    0b0000000111000000,
    0b0000000111000000,
    0b0000000111000000,
    0b0000000111000000,
    0b0110000111000011,
    0b0111000111000111,
    0b0011100111001110,
    0b0001110111011100,
    0b0000111111111000,
    0b0000011111110000,
    0b0000000111000000,
    0b0000000010000000
};

const uint16_t arrow_round_16x16[] = {
	0b0000001111110000,
    0b0000011111111000,
    0b0000111000011100,
    0b0001110000001110,
    0b0011100000000111,
    0b0011000000000011,
    0b0011000000000011,
    0b0011000000000011,
    0b0011000000000011,
    0b1011010000000011,
    0b1111110000000011,
    0b0111100000000011,
    0b0011000000000011,
    0b0000000000000011,
    0b0000000000000011,
    0b0000000000000011
};
const uint16_t arrow_1_16x16[] = {
	0b0000000000000000,
    0b0000111111110000,
    0b0001111111111000,
    0b0011100000011100,
    0b0111000001101110,
    0b1110000011100111,
    0b1100000101100011,
    0b1100000001100011,
    0b1100000001100011,
    0b1100000001100011,
    0b1110000001100111,
    0b0111000000001110,
    0b0011100000011100,
    0b0001111111111000,
    0b0000111111110000,
    0b0000000000000000
};
const uint16_t arrow_clear_16x16[] = {
	0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111
};
const uint16_t arrow_clear_10x28[] = {
	0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111,
    0b1111111111111111
};
const uint16_t logo_16x16[256] = {
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0021,
    0x8C09,
    0xF710,
    0xF70E,
    0xEECE,
    0xEECE,
    0xEEEF,
    0x8BE7,
    0x1082,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0xB4E8,
    0xF792,
    0xCE11,
    0xCDF0,
    0xCDCF,
    0xD60F,
    0xE6F1,
    0xE6B1,
    0xE6CE,
    0x4A26,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0020,
    0xB507,
    0xEF12,
    0xB52F,
    0xCD86,
    0x8387,
    0x396B,
    0x39AB,
    0x62CB,
    0xB54D,
    0xD672,
    0xFF6F,
    0x39C5,
    0x0000,
    0x0000,
    0x0000,
    0xB50B,
    0xE6D1,
    0xB52E,
    0x72E7,
    0xEE40,
    0xFEA0,
    0xA446,
    0x93C7,
    0x6289,
    0x290B,
    0xB50B,
    0xDEB2,
    0xB529,
    0x1082,
    0x0000,
    0x4A26,
    0xF731,
    0xCDEE,
    0x41AB,
    0xA446,
    0xFF80,
    0xB485,
    0x294D,
    0x41CC,
    0x8BA7,
    0x8BA8,
    0x292B,
    0xBD6D,
    0xFF92,
    0x7B88,
    0x0000,
    0x8C08,
    0xF732,
    0x8BEC,
    0x398B,
    0xB4A5,
    0xEE61,
    0xC543,
    0x18AE,
    0x292D,
    0x108E,
    0x8388,
    0x6AA9,
    0x6AEB,
    0xE6D1,
    0xBD6B,
    0x0862,
    0xE690,
    0xFFD1,
    0x524C,
    0x8368,
    0x41CC,
    0x398C,
    0xE622,
    0x93E7,
    0x18CE,
    0x314D,
    0x294D,
    0x9BE7,
    0x41AB,
    0xE6D0,
    0xFFB1,
    0x0861,
    0xEEF0,
    0xFFB1,
    0x41AB,
    0x93E7,
    0x5A4A,
    0x398C,
    0x522B,
    0xF661,
    0x7309,
    0x20EE,
    0x210D,
    0x8BA8,
    0x41CA,
    0xDE70,
    0xFF8F,
    0x1061,
    0xE6AF,
    0xE6B0,
    0x49EB,
    0x8BA7,
    0xBCE4,
    0xE601,
    0xD583,
    0xFEE0,
    0x93E7,
    0x20EE,
    0x290D,
    0x8BA7,
    0x49EA,
    0xBD8F,
    0xEEEE,
    0x0862,
    0xDE70,
    0xDE4F,
    0x526B,
    0x8367,
    0x4A0B,
    0x8BA7,
    0xF680,
    0x7328,
    0x18AE,
    0x20EE,
    0x290D,
    0x93E7,
    0x49EB,
    0xC5AE,
    0xF730,
    0x0000,
    0x8C08,
    0xF732,
    0x83AC,
    0x6289,
    0xEE41,
    0xC504,
    0xDDE2,
    0xA446,
    0xACA5,
    0xA465,
    0xCDA3,
    0x7B28,
    0x6ACB,
    0xEEF1,
    0xBD8B,
    0x0000,
    0x4A26,
    0xF731,
    0xC5CF,
    0x49EB,
    0x6AE9,
    0xC524,
    0x6AC9,
    0x9C06,
    0x7329,
    0xC544,
    0xA486,
    0x398B,
    0xBD6D,
    0xFF72,
    0x7B88,
    0x0000,
    0x0000,
    0xB50B,
    0xFF72,
    0xC58D,
    0x398B,
    0x41CB,
    0x8BA7,
    0x8B88,
    0x8BA7,
    0x524A,
    0x290C,
    0xA4AB,
    0xDE92,
    0xBD69,
    0x1082,
    0x0000,
    0x0000,
    0x0000,
    0xB508,
    0xF733,
    0xCDEE,
    0x7B6B,
    0x49EB,
    0x41CB,
    0x41CB,
    0x6AEB,
    0xBD8D,
    0xC5F1,
    0xFF4F,
    0x39C5,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0xB529,
    0xEF52,
    0xCE10,
    0xCDEF,
    0xC5AF,
    0xD650,
    0xD650,
    0xF731,
    0xE6CE,
    0x4A26,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0001,
    0x8C2A,
    0xEF10,
    0xEECE,
    0xEEAE,
    0xF70E,
    0xEEEF,
    0x8BE7,
    0x1082,
    0x0000,
    0x0000,
    0x0000
};
