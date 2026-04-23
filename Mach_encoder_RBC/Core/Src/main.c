/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int16_t last = 0;
int32_t pos = 0;
volatile int32_t rpm = 0;
int speed = 900;
volatile int32_t cnt = 0;

typedef struct {
  volatile int8_t sts_tren;  // doc tu ngat EXTI (PB11)
  volatile int8_t sts_duoi;  // doc tu ngat EXTI (PB10)
  int8_t control;            // 0: dung, 1: len, 2: xuong
} Taygap;

Taygap tay = {0, 0, 0};

int tuan=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//ID CHIP ////////////////////////////////////////////////////////////////////////////////////////////////////////
int id=7;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t rx_Data[8];
uint8_t RxData[8];
CAN_RxHeaderTypeDef RxHeader;
volatile int Setpoint = 0;
int MAX_SPEED = 900;
uint8_t l = 0;

//CAN///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {
        if (RxHeader.StdId == 0x123) {
        	Setpoint = (RxData[6]-7.5)*(RxData[id-1]-100);
        	l=1-l;
					if(l)HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
					if(Setpoint>=MAX_SPEED)Setpoint=MAX_SPEED;
					if(Setpoint<=-MAX_SPEED)Setpoint=-MAX_SPEED;
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void quay_thuan(int spd) {
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, spd);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
}

void quay_nghich(int spd) {
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, spd);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
}

void dung() {
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
}

void go_home() {
	if (tay.sts_duoi != 1) {
		quay_nghich(speed);
	} else {
		dung();
	}
}

void Tay_control() {
	switch (tay.control) {
		case 1:  // len
			if (tay.sts_tren == 1) {
				dung();
			} else {
				quay_thuan(speed);
			}
			break;
		case 2:  // xuong
			if (tay.sts_duoi == 1) {
				dung();
			} else {
				quay_nghich(speed);
			}
			break;
		default:  // dung
			dung();
			break;
	}
}

// Ngat TIM3: doc encoder
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3) {
		int32_t raw = (int32_t)__HAL_TIM_GET_COUNTER(&htim4);
		int16_t delta = (int16_t)(raw - last);
		last = (int16_t)raw;
		pos += delta;
		cnt = raw;
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  // CAN Filter Config (giong PID_Board)
//  CAN_FilterTypeDef sFilterConfig;
//  sFilterConfig.FilterBank = 0;
//  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
//  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
//  sFilterConfig.FilterIdHigh = 0x123 << 5;
//  sFilterConfig.FilterIdLow = 0x0000;
//  sFilterConfig.FilterMaskIdHigh = 0xFFE0;
//  sFilterConfig.FilterMaskIdLow = 0x0000;
//  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
//  sFilterConfig.FilterActivation = ENABLE;
//  if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK) {
//	  Error_Handler();
//  }
//
//  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
//  if (HAL_CAN_Start(&hcan) != HAL_OK) {
//	  Error_Handler();
//  }
//
//  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
//  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//  go_home();  // ve vi tri ban dau truoc khi bat dau

  while (1)
  {
	  // Tay_control();  // dieu khien tay gap theo control

    quay_thuan(speed);
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    HAL_Delay(3000);
//    tuan++;
    quay_nghich(speed);
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    HAL_Delay(3000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

//Ngat EXTI: doc cong tac hanh trinh tren va duoi
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == PIN11_Pin) {  // PB11 - cong tac tren
		tay.sts_tren = HAL_GPIO_ReadPin(PIN11_GPIO_Port, PIN11_Pin);
		if (tay.sts_tren == 1) {
			dung();  // dung motor ngay khi cham cong tac tren
		}
	}
	if (GPIO_Pin == PIN10_Pin) {  // PB10 - cong tac duoi
		tay.sts_duoi = HAL_GPIO_ReadPin(PIN10_GPIO_Port, PIN10_Pin);
		if (tay.sts_duoi == 1) {
			dung();  // dung motor ngay khi cham cong tac duoi
		}
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
