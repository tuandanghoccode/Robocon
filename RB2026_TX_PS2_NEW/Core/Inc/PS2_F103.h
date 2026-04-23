/*
 * PS2_F103.h
 *
 *  Created on: Apr 13, 2025
 *      Author: htbt2
 */

#ifndef INC_PS2_F103_H_
#define INC_PS2_F103_H_

#include "main.h"
#define MOSI_GPIO_Port GPIOB
#define MOSI_Pin GPIO_PIN_3
#define SCK_GPIO_Port GPIOB
#define SCK_Pin	GPIO_PIN_7
#define MISO_GPIO_Port GPIOA
#define MISO_Pin GPIO_PIN_15
#define SS_GPIO_Port	GPIOB
#define SS_Pin	GPIO_PIN_4
typedef struct{
	uint8_t address;
	uint8_t data[2];
	uint8_t button[2];
	uint8_t data2[2];
}Data;

void delay_us(uint16_t);
uint8_t PS2_SendByte(uint8_t);
void PS2_Cmd(uint8_t *, uint8_t *, uint8_t );
void PS2_Poll(void);
void PS2_EnterConfig(void);
void PS2_AnalogMode(void);
void PS2_ExitConfig(void);
void PS2_Init(TIM_HandleTypeDef*, Data*);
void PS2_Update(void);

#endif /* INC_PS2_F103_H_ */
