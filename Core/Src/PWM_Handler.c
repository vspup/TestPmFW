#include "PWM_Handler.h"
#include "main.h"

extern  TIM_HandleTypeDef htim1;
extern  TIM_HandleTypeDef htim3;

#define SET_A1_PWM(x)    TIM1->CCR1 = x
#define SET_A2_PWM(x)    TIM1->CCR2 = x
#define SET_A3_PWM(x)    TIM1->CCR3 = x

#define SET_B1_PWM(x)    TIM3->CCR1 = x
#define SET_B2_PWM(x)    TIM3->CCR2 = x
#define SET_B3_PWM(x)    TIM3->CCR3 = x

#define SET_B_PWM(x)     TIM3->CCR1 = x; TIM3->CCR2 = x; TIM3->CCR3 = x;

 void Set_A1_PWM (uint8_t val)
 {
	SET_A1_PWM(val);
	//On_A1 ();
 }

 void Set_A2_PWM (uint8_t val)
 {
	 SET_A2_PWM(val);
	 //On_A2 ();
 }


void Set_A3_PWM (uint8_t val)
{
	SET_A3_PWM(val);
	//On_A3 ();
}

void Set_B1_PWM (uint8_t val)
{
	SET_B1_PWM(val);
	//On_B1 ();
}

void Set_B2_PWM (uint8_t val)
{
	SET_B2_PWM(val);
	//On_B2 ();
}

void Set_B3_PWM (uint8_t val)
{
	SET_B3_PWM(val);
	//On_B3 ();
}

void On_A1 (void)
{
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}


void On_A2 (void)
{
   HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
}

void On_A3 (void)
{
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
}

void On_B1 (void)
{
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

void On_B2 (void)
{
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
}

void On_B3 (void)
{
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
}

void Off_A1 (void)
{
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
}

void Off_A2 (void)
{
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
}

void Off_A3 (void)
{
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
}

void Off_B1 (void)
{
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
}

void Off_B2 (void)
{
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
}

void Off_B3 (void)
{
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
}

void Dis_M1 (void)
{
	 HAL_GPIO_WritePin(GPIOA, M1_DIS_Pin, GPIO_PIN_SET);
}

void Dsi_M2 (void)
{
	 HAL_GPIO_WritePin(GPIOA, M2_DIS_Pin, GPIO_PIN_SET);
}

void En_M1 (void)
{
	HAL_GPIO_WritePin(GPIOA, M1_DIS_Pin, GPIO_PIN_RESET);
}

void En_M2 (void)
{
	HAL_GPIO_WritePin(GPIOA, M1_DIS_Pin, GPIO_PIN_RESET);
}
