/*
 * ILI9341.h
 * ILI9341 SPI Polling Driver (No DMA)
 * Refactored for STM32F407
 */

#ifndef ILI9341_H_
#define ILI9341_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * INCLUDES                                   *
 ******************************************************************************/
#include "main.h"
#include "stm32f4xx_hal.h"

/******************************************************************************
 * HARDWARE CONFIG                                 *
 ******************************************************************************/
// Mapping GPIO từ main.h sang tên driver
// Đảm bảo MH_RST, MH_DC, MH_CS đã được define trong main.h (do CubeMX sinh ra)
#define ILI9341_RES_GPIO_Port   MH_RST_GPIO_Port
#define ILI9341_RES_Pin         MH_RST_Pin

#define ILI9341_DC_GPIO_Port    MH_DC_GPIO_Port
#define ILI9341_DC_Pin          MH_DC_Pin

#define ILI9341_CS_GPIO_Port    MH_CS_GPIO_Port
#define ILI9341_CS_Pin          MH_CS_Pin

/******************************************************************************
 * DISPLAY CONFIG                                  *
 ******************************************************************************/
#define ILI9341_WIDTH           320
#define ILI9341_HEIGHT          240

/******************************************************************************
 * COLOR DEFINITIONS                               *
 ******************************************************************************/
// Color RGB565 (uint16_t)
#define BLACK       0x0000
#define NAVY        0x000F
#define DARKGREEN   0x03E0
#define DARKCYAN    0x03EF
#define MAROON      0x7800
#define PURPLE      0x780F
#define OLIVE       0x7BE0
#define LIGHTGREY   0xC618
#define DARKGREY    0x7BEF
#define BLUE        0x001F
#define GREEN       0x07E0
#define CYAN        0x07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF
#define ORANGE      0xFD20
#define GREENYELLOW 0xAFE5
#define PINK        0xF81F

/******************************************************************************
 * FUNCTION PROTOTYPES                               *
 ******************************************************************************/

/**
 * @brief  Khởi tạo màn hình ILI9341
 * @param  spi_handle: Con trỏ tới hspi (ví dụ &hspi1)
 */
void ILI9341_Init(SPI_HandleTypeDef *spi_handle);

/**
 * @brief  Vẽ toàn bộ khung hình từ buffer ảnh (RGB565)
 * @param  buffer: Con trỏ dữ liệu ảnh (mảng uint8_t nhưng chứa data 16bit)
 * @param  nbytes: Kích thước buffer tính bằng byte (320*240*2)
 */
void ILI9341_DrawFrame(const uint8_t *buffer, uint32_t nbytes);

/**
 * @brief  Vẽ hình chữ nhật đặc màu
 * @param  color: Màu RGB565 (uint16_t)
 * @param  x, y: Tọa độ góc trái trên
 * @param  w, h: Chiều rộng, chiều cao
 */
void ILI9341_FillRect(uint16_t color, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

/**
 * @brief  Vẽ khung viền hình chữ nhật (rỗng ruột)
 * @param  x, y: Tọa độ góc trái trên
 * @param  w, h: Chiều rộng, chiều cao
 * @param  color: Màu viền
 */
void Draw_BoundingBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

/**
 * @brief  Vẽ một ký tự lên màn hình
 * @param  x, y: Tọa độ
 * @param  c: Ký tự cần vẽ
 * @param  color: Màu chữ
 */
void LCD_DrawChar(int x, int y, char c, uint16_t color);

/**
 * @brief  Viết chuỗi ký tự lên màn hình
 * @param  x, y: Tọa độ bắt đầu
 * @param  str: Chuỗi cần viết
 * @param  color: Màu chữ
 */
void LCD_PrintString(int x, int y, const char *str, uint16_t color);
void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *data);
void ILI9341_DrawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

#ifdef __cplusplus
}
#endif

#endif /* ILI9341_H_ */
