/*
 * PS2_STM32.c
 *
 *  Created on: Apr 13, 2025
 *      Author: htbt2
 */


#include "PS2_F103.h"

Data *data;
TIM_HandleTypeDef *PS2_TIMER;

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) + out_min;
}
void delay_us(uint16_t us)
{
	__HAL_TIM_SET_COUNTER(PS2_TIMER, 0);
	while (__HAL_TIM_GET_COUNTER(PS2_TIMER) < us);
}

uint8_t PS2_SendByte(uint8_t cmd)
{
	uint8_t data = 0x00;
	uint16_t buffer = 0x01;
	for (buffer = 0x01; buffer < 0x0100; buffer <<= 1) {
		if (buffer & cmd)
		{
			HAL_GPIO_WritePin(MOSI_GPIO_Port, MOSI_Pin, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(MOSI_GPIO_Port, MOSI_Pin, GPIO_PIN_RESET);
		}
		HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, GPIO_PIN_SET);
		delay_us(8);
		HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, GPIO_PIN_RESET);
		delay_us(8);
		HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, GPIO_PIN_SET);
		if (HAL_GPIO_ReadPin(MISO_GPIO_Port, MISO_Pin))
		{
			data |= buffer;
		}
	}
	delay_us(8);
	return data;
}

void PS2_Cmd(uint8_t *transmit, uint8_t *receive, uint8_t length)
{
	HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_RESET);
	delay_us(8);
	for (uint8_t i = 0; i < length; i++)
	{
		receive[i] = PS2_SendByte(transmit[i]);
	}
	HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_SET);
	delay_us(8);
}

void PS2_Poll()
{
	uint8_t transmit[5] = { 0x01, 0x42 };
	uint8_t receive[5] = { 0x00 };
	PS2_Cmd(transmit, receive, 5);
}

void PS2_EnterConfig()
{
	uint8_t transmit[5] = { 0x01, 0x43, 0x00, 0x01 };
	uint8_t receive[5] = { 0x00 };
	PS2_Cmd(transmit, receive, 5);
}

void PS2_AnalogMode()
{
	uint8_t transmit[9] = { 0x01, 0x44, 0x00, 0x01, 0x03 };
	uint8_t receive[9] = { 0x00 };
	PS2_Cmd(transmit, receive, 9);
}

void PS2_ExitConfig()
{
	uint8_t transmit[9] = { 0x01, 0x43, 0x00, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A };
	uint8_t receive[9] = { 0x00 };
	PS2_Cmd(transmit, receive, 9);
}

void PS2_Init(TIM_HandleTypeDef *timer, Data *data_tx)
{
	PS2_TIMER = timer, data = data_tx;
	HAL_TIM_Base_Start(PS2_TIMER);
	PS2_Poll();
	PS2_EnterConfig();
	PS2_AnalogMode();
	PS2_Poll();
	PS2_ExitConfig();
}

void PS2_Update()
{
	uint8_t transmit[9] = { 0x01, 0x42 };
	uint8_t receive[9] = { 0x00 };
	PS2_Cmd(transmit, receive, 9);

	// Luôn set address = 255 để tx[0] không bao giờ bị rác
	data -> address = 255;

	// Kiểm tra response hợp lệ (0x41 = digital, 0x73 = analog)
	if (receive[1] != 0x41 && receive[1] != 0x73) return;

	// Đọc button (có cả ở digital mode 0x41 và analog mode 0x73)
	uint8_t buttonStatus[2];
	buttonStatus[0] = ~receive[3];
	buttonStatus[1] = ~receive[4];
	data -> button[0] = buttonStatus[0];
	data -> button[1] = buttonStatus[1];

	// Nếu đang ở digital mode → không có dữ liệu analog, set joystick về center
	if (receive[1] != 0x73) {
		data -> data[0] = 100;  // Center (50+50) khi 2 joy ở giữa
		data -> data[1] = 100;
		return;
	}

	// Analog mode: đọc cả 2 joystick, cộng lại (range 0~200, center=100)
	uint8_t Joystick[2];
	uint8_t sum = map(receive[8], 0, 255, 0, 100) + map(receive[6], 0, 255, 0, 100);
	// Joystick[0] = (sum >= 4) ? (sum - 4) : 0;  // Bù offset calibration
	Joystick[0] = sum;
	Joystick[1] = map(receive[7], 0, 255, 0, 100) + map(receive[5], 0, 255, 0, 100);

	data -> data2[0] =  map(receive[6], 0, 255, 0, 100);
	data -> data2[1] =  map(receive[5], 0, 255, 0, 100);
	data -> data[0] = Joystick[0];
	data -> data[1] = Joystick[1];
}
