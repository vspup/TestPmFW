/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "power-module.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#include "PWM_Handler.h"
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#include "uart-escape.h"
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */
void pm_process_rx_message(size_t ind, uint8_t* message_p, size_t length, uint32_t time_stamp);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_CRC_Init(void);
/* USER CODE BEGIN PFP */
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define NUM_CMD    38
#define UNK_CMD    NUM_CMD + 1
#define R_CMD      13
#define SH_CMD     14


enum
{
   SET_A1 = 1,
   SET_A2,
   SET_A3,
   SET_B1,
   SET_B2,
   SET_B3,
   ON_A1,
   ON_A2,
   ON_A3,
   ON_B1,
   ON_B2,
   ON_B3,
   OFF_A1,
   OFF_A2,
   OFF_A3,
   OFF_B1,
   OFF_B2,
   OFF_B3,
   DIS_M1,
   DIS_M2,
   EN_M1,
   EN_M2,
   G_PARAM1,
   G_PARAM2,
   SW1_ON,
   SW2_ON,
   SW1_OFF,
   SW2_OFF,
   ON1,
   ON2,
   OFF1,
   OFF2,
   SET1_I,
   SET1_T,
   SET2_I,
   SET2_T,
   RESTART1,
   RESTART2
};
const char listCMD[NUM_CMD][10] =
		{ "pwmA1", "pwmA2", "pwmA3", "pwmB1","pwmB2","pwmB3",
		  "onA1", "onA2", "onA3", "onB1", "onB2", "onB3",
		  "offA1", "offA2", "offA3", "offB1", "offB2", "offB3",
		  "disM1", "disM2","EnM1", "EnM2", "getParam1", "getParam2", "sw1On", "sw2On",
		  "sw1Off", "sw2Off","on1", "on2", "off1", "off2", "set1curr", "set1time","set2curr", "set2time", "restart1", "restart2" };
typedef struct
{
	char rxByte;
	char rxBuff[256];
	uint8_t rxCntBuff;
} t_UART_Data;

t_UART_Data UART_Data;
uint8_t rxByte;
uint8_t numCMD;
int i;

#define MAX_LEN  10
uint32_t getVal = 0;

uint8_t rxByteM1;
uint8_t rxByteM2;

typedef struct
{
	char rxByte;
	char rxBuff[256];
	uint8_t rxCntBuff;
	uint8_t timeoutCnt;
	uint8_t getPacket;
} t_M_UART_Data;

t_M_UART_Data  M_UART_Data1;
t_M_UART_Data  M_UART_Data2;

uint8_t rxDataBuf[250];
uint8_t rxDataBufRd[250];
uint8_t getPk1 = false;

uint8_t rxDataBuf2[250];
uint8_t rxDataBufRd2[250];
uint8_t getPk2 = false;

struct fuse_rd_data_s
{
	bool  on_off_state;
	bool  on_protection_state;
	bool  enabled;
	float protection_current;
	float protection_time;
	float module_current;
	bool  data_valid;
	uint32_t id_uC;
};

struct pm_rx_error_counters_s {
	volatile uint32_t ringbuf_overflow;
	volatile uint32_t decoder_overflow;
	volatile uint32_t decoder_invalid_ch;
	volatile uint32_t msg_too_short;
	volatile uint32_t msg_crc;
	volatile uint32_t msg_format;
	volatile uint32_t msg_type_unknown;
};


struct fuse_rd_data_s fuse_rd_data[2] = {0};
struct Measurements pm_measurements[2];
struct pm_status_flags pm_current_status_flags[2];
static volatile struct pm_rx_error_counters_s pm_rx_error_stats[2] = {0};

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		if(M_UART_Data1.getPacket == false)
		{
			if(M_UART_Data1.rxCntBuff == 0)
			{
				if(rxByteM1 == 'C')
				{
					M_UART_Data1.rxBuff[M_UART_Data1.rxCntBuff] = rxByteM1;
					M_UART_Data1.rxCntBuff ++;
				}
			}
			else
			{
				M_UART_Data1.rxBuff[M_UART_Data1.rxCntBuff] = rxByteM1;
				M_UART_Data1.rxCntBuff ++;
			}
		}
		M_UART_Data1.timeoutCnt =0;
		HAL_UART_Receive_IT(&huart1,  &rxByteM1, 1);
	}
	if (huart->Instance == USART2)
	{
		if(M_UART_Data2.getPacket == false)
		{
			if(M_UART_Data2.rxCntBuff == 0)
			{
				if(rxByteM2 == 'C')
				{
					M_UART_Data2.rxBuff[M_UART_Data2.rxCntBuff] = rxByteM2;
					M_UART_Data2.rxCntBuff ++;
				}
			}
			else
			{
				M_UART_Data1.rxBuff[M_UART_Data2.rxCntBuff] = rxByteM2;
				M_UART_Data1.rxCntBuff ++;
			}
		}
		M_UART_Data2.timeoutCnt =0;
		HAL_UART_Receive_IT(&huart2,  &rxByteM2, 1);
	}
	if (huart->Instance == USART3)
	{
		if (rxByte == '\n')
		{
			numCMD = 0;
			for (i = 0; i < NUM_CMD; i++)
			{
				if (strstr((char*)&UART_Data.rxBuff[0], (char*) &listCMD[i][0])!= 0)
				{
					numCMD = i + 1;

				    for(uint8_t pos_val = 0; pos_val < MAX_LEN; pos_val++)
				    {
				      if(UART_Data.rxBuff[pos_val] == 0x20)
				      {
				    	  pos_val++;
				    	  getVal = atoi((uint8_t*)&UART_Data.rxBuff[pos_val]);
				    	  break;
				      }
				    }
					break;
				}
			}
			if(numCMD == 0)
			{
				numCMD = 0xFF;
			}
			memset((char*) &UART_Data, 0x00, sizeof(UART_Data));
		}
		else
		{
			//if(rxByte != '\r')
			{
			  UART_Data.rxBuff[UART_Data.rxCntBuff] = rxByte;
			  UART_Data.rxCntBuff++;
			}
		}
		HAL_UART_Receive_IT(&huart3,  &rxByte, 1);
	}
}

void CountersHandler (void)
{
	M_UART_Data1.timeoutCnt++;
	M_UART_Data2.timeoutCnt++;
}

#define TIMEOUT_PKT  15
uint8_t rxBuff[128];
uint8_t rxBuffHandl[128];
uint8_t rxCount;

#define UART_TX_BUFFER_LEN 64
uint8_t uart_tx_buffer[UART_TX_BUFFER_LEN];
#define UART_RX_BUFFER_LEN 64
uint8_t uart_rx_buffers[2][UART_RX_BUFFER_LEN];
volatile uint8_t* uart_receiving_buffer = uart_rx_buffers[0];
volatile uint8_t* uart_processing_buffer = uart_rx_buffers[1];
size_t uart_rx_buffer_i = 0;
volatile size_t uart_processing_buffer_length = 0;

volatile bool uart_processing_buffer_full = false;
volatile bool uart_rx_ongoing = false;
#define INVERT(V) ((uint8_t)(~(V)))

bool switch_state[2] = {false, false};
bool fuse_state[2] = {false, false};
bool save_param[2] = {false, false};


void ReceiveHandler (void)
{
    int cnt =0;

	 if(getPk1 == true)// if(M_UART_Data1.timeoutCnt > TIMEOUT_PKT)
	 {
		  //M_UART_Data1.getPacket = true;
		  for(; cnt < 250; cnt++ )
		  {
		     if(rxDataBufRd[cnt] == UART_ESCAPE_SOT)
		     {
		    	break;
		     }
		     //cnt++;
		  }
		  while(1)
		  {
				 rxCount = 0;
				 uint8_t packetReady = 0;

				 memset((uint8_t*)&rxBuff[0], 0x00, sizeof(rxBuff));
				 for(; cnt < 250; cnt ++ )
				 {
					 switch(rxDataBufRd[cnt])
					 {
					   case UART_ESCAPE_SOT: break;
					   case UART_ESCAPE_CHAR:
							{
								cnt++;
								rxBuff[rxCount] = INVERT(rxDataBufRd[cnt]);
								rxCount++;
							}
					   break;
					   case UART_ESCAPE_EOT:  packetReady= 1; break;
					   default:
						   rxBuff[rxCount] = rxDataBufRd[cnt];
						   rxCount++;
					   break;
					 }

					 if(packetReady)
					 {
						 pm_process_rx_message(0, (uint8_t*)&rxBuff[0], rxCount, 0xFFFFFFFF);
						 break;
					 }
				 }
				 cnt++;
				 if(rxDataBufRd[cnt] != UART_ESCAPE_SOT)
				 {
					   break;
				 }

			}

		    memset((uint8_t*)&rxDataBufRd[0], 0x00, sizeof(rxDataBufRd));
		    //M_UART_Data1.rxCntBuff = 0;
		    //M_UART_Data1.getPacket = 0;
		    getPk1 = false;
	 }

	 cnt =0;

	 if(getPk2 == true)// if(M_UART_Data1.timeoutCnt > TIMEOUT_PKT)
	 {
		  //M_UART_Data1.getPacket = true;
		  for(; cnt < 250; cnt++ )
		  {
		     if(rxDataBufRd2[cnt] == UART_ESCAPE_SOT)
		     {
		    	break;
		     }
		     //cnt++;
		  }
		  while(1)
		  {
				 rxCount = 0;
				 uint8_t packetReady = 0;

				 memset((uint8_t*)&rxBuff[0], 0x00, sizeof(rxBuff));
				 for(; cnt < 250; cnt ++ )
				 {
					 switch(rxDataBufRd2[cnt])
					 {
					   case UART_ESCAPE_SOT: break;
					   case UART_ESCAPE_CHAR:
							{
								cnt++;
								rxBuff[rxCount] = INVERT(rxDataBufRd2[cnt]);
								rxCount++;
							}
					   break;
					   case UART_ESCAPE_EOT:  packetReady= 1; break;
					   default:
						   rxBuff[rxCount] = rxDataBufRd2[cnt];
						   rxCount++;
					   break;
					 }

					 if(packetReady)
					 {
						 pm_process_rx_message(1, (uint8_t*)&rxBuff[0], rxCount, 0xFFFFFFFF);
						 break;
					 }
				 }
				 cnt++;
				 if(rxDataBufRd2[cnt] != UART_ESCAPE_SOT)
				 {
					   break;
				 }

			}

		    memset((uint8_t*)&rxDataBufRd2[0], 0x00, sizeof(rxDataBufRd2));
		    //M_UART_Data1.rxCntBuff = 0;
		    //M_UART_Data1.getPacket = 0;
		    getPk2 = false;
	 }


}

void SendRsp( char *str)
{
	HAL_UART_Transmit(&huart3, (uint8_t*)str, strlen((char*)str), 100);
	HAL_UART_Transmit(&huart3, (uint8_t*)"\n", 2, 100);
}

void PrintParam (uint8_t ind)
{
	char tempStr[250] = {0x00};
	sprintf(tempStr, " Param%d\n V_HB1 %.2f\n V_OUTA %.2f\n V_OUTB %.2f\n NTC_A %.2f\n NTC_B %.2f\n T_UC %.2f\n fON_OFF_State %d\n ON_PrState %d\n fEnabled: %d\n fProtectionCurrent: %.0f\n fProtectionTime: %.0f\n moduleCurrent: %.2f\n",
			ind +1,
			pm_measurements[ind].V_HB_1,
			pm_measurements[ind].V_OUT_A,
	        pm_measurements[ind].V_OUT_B,
	        pm_measurements[ind].T_NTC_A,
	        pm_measurements[ind].T_NTC_B,
		    pm_measurements[ind].T_UC,
            pm_measurements[ind].fuseON_OFF_State,
	        pm_measurements[ind].fuseON_ProtectionState,
	        pm_measurements[ind].fuseEnabled,
	        pm_measurements[ind].fuseProtectionCurrent,
	        pm_measurements[ind].fuseProtectionTime,
        	pm_measurements[ind].moduleCurrent);
	SendRsp(tempStr);
}

uint8_t getPWM_Val( uint32_t *getPWM_Val)
{

  if(getVal > 100)
  {
	  SendRsp("Value must be 0..100 ");
	  return 0;
  }

   *getPWM_Val = getVal;
   return 1;
}

uint8_t getFuse_I( uint32_t *getFuseI_Val)
{

  if(getVal > 50)
  {
	  SendRsp("Value must be 0..50 ");
	  return 0;
  }

   *getFuseI_Val = getVal;
   return 1;
}

uint8_t getFuse_T( uint32_t *getFuseI_Val)
{

  if(getVal > 1000)
  {
	  SendRsp("Value must be 0..1000 ");
	  return 0;
  }

   *getFuseI_Val = getVal;
   return 1;
}

uint32_t setVal = 0;
bool savePrtCurr[2] = {false};
bool saveTime[2] = {false};
bool restartFuse[2] = {false};
float protetionCurr [2] = {0};
float protectionTime[2] = {0};


void CMD_Handler(void)
{
	if(numCMD == 0)
	{
		return;
	}
    char tempStr[128]= {0};
	switch(numCMD)
	{
	   case SET_A1:
		   if( getPWM_Val(&setVal))
		   {
			   Dis_M1();
			   Dis_M2();
			   HAL_Delay(1);
			   Off_A2 ();
			   Off_A3 ();
			   Set_A3_PWM(0);
			   Set_A2_PWM(0);
			   Set_A1_PWM(setVal);
			   sprintf(tempStr,"Set PWM A1 to %d \n", setVal );
			   SendRsp(tempStr);
			   On_A1 ();
			   HAL_Delay(1);
			   En_M1();
			   En_M2();
		   }
	   break;

	   case	SET_A2:
		   if( getPWM_Val(&setVal))
		   {
			   Dis_M1();
			   Dis_M2();
			   HAL_Delay(1);
			   Off_A1 ();
			   Off_A3 ();
			   Set_A3_PWM(0);
			   Set_A1_PWM(0);
		  	   Set_A2_PWM(setVal);
		  	   sprintf(tempStr, "Set PWM A2 to %d \n", setVal );
		  	   SendRsp(tempStr);
		  	   On_A2 ();
		  	   HAL_Delay(1);
		  	   En_M1();
		  	   En_M2();

		   }
	   break;

	   case SET_A3:
		   if( getPWM_Val(&setVal))
		   {
			    Dis_M1();
			    Dis_M2();
			  	HAL_Delay(1);
			    Off_A1 ();
			    Off_A2 ();
			    Set_A1_PWM(0);
			    Set_A2_PWM(0);
		     	Set_A3_PWM(setVal);
		   		sprintf(tempStr, "Set PWM A3 to %d \n", setVal );
		   		SendRsp(tempStr);
		   		On_A3 ();
		   		HAL_Delay(1);
                En_M1();
		   		En_M2();
		   }
	   break;
	   case	SET_B1:
		   if( getPWM_Val(&setVal))
		    {
			    Dis_M1();
			    Dis_M2();
			  	HAL_Delay(1);

			    Off_B2 ();
			    Off_B3 ();
			    Set_B2_PWM(0);
			    Set_B3_PWM(0);
			    Set_B1_PWM(setVal);
		   		sprintf(tempStr, "Set PWM B1 to %d \n", setVal );
		   		SendRsp(tempStr);
		   		On_B1 ();
		   		HAL_Delay(1);
                En_M1();
		   		En_M2();
		    }
	   break;
	   case	SET_B2:
		   if( getPWM_Val(&setVal))
		    {
			    Dis_M1();
			  	Dis_M2();
			  	HAL_Delay(1);
			    Off_B1 ();
			    Off_B3 ();
			    Set_B1_PWM(0);
			  	Set_B3_PWM(0);
		   		Set_B2_PWM(setVal);
		   		sprintf(tempStr, "Set PWM B2 to %d \n", setVal );
		   		SendRsp(tempStr);
		   		On_B2 ();
		   		HAL_Delay(1);
                En_M1();
		   		En_M2();
		    }
	   break;
	   case	SET_B3:
		   if( getPWM_Val(&setVal))
		    {
			    Dis_M1();
			  	Dis_M2();
			  	HAL_Delay(1);
			    Off_B1 ();
			    Off_B2 ();
			    Set_B1_PWM(0);
			  	Set_B2_PWM(0);
		   		Set_B3_PWM(setVal);
		   		sprintf(tempStr, "Set PWM B3 to %d \n", setVal );
		   		SendRsp(tempStr);
		   		On_B3 ();
		   		HAL_Delay(1);
                En_M1();
		   		En_M2();
		    }
	   break;
	   case	ON_A1:

		    On_A1();
		    SendRsp("ON PWM A1");
	   break;
	   case	ON_A2:
		   On_A2();
		   SendRsp("ON PWM A2");
	   break;
	   case	ON_A3:

		   On_A3();
		   SendRsp("ON PWM A3");
	   break;
	   case	ON_B1:
		   On_B1();
		   SendRsp("ON PWM B1");

	   break;
	   case	ON_B2:
	   		On_B2();
	   		SendRsp("ON PWM B2");

	   break;
	   case	ON_B3:
	   	   	On_B3();
	   	   	SendRsp("ON PWM B3");
	   break;
	   case	OFF_A1:
		   Off_A1();
		   SendRsp("OFF A1");
	   break;
	   case	OFF_A2:
		   Off_A2();
		   SendRsp("OFF A2");
	   break;
	   case	OFF_A3:
		   Off_A3();
		   SendRsp("OFF A3");
	   break;
	   case	OFF_B1:
		    Off_B1();
		    SendRsp("OFF B1");
	   break;
	   case	OFF_B2:
	   		Off_B2();
	   		SendRsp("OFF B2");
	   break;
	   case	OFF_B3:
	   	   	Off_B3();
	   	   	SendRsp("OFF B3");
	   break;
	   case	DIS_M1:
		   Dis_M1();
		   SendRsp("DIS M1");
	   break;
	   case DIS_M2:
		   Dis_M1();
		   SendRsp("DIS M2");
	   break;
	   case EN_M1:
		   En_M1();
		   SendRsp("EN M1");
	   break;
	   case	EN_M2:
		   En_M2();
		   SendRsp("EN M2");
	   break;
	   case G_PARAM1:
		    PrintParam(0);
	   break;
	   case G_PARAM2:
		    PrintParam(1);
	   break;
	   case  SW1_ON:
		   switch_state[0] = true;
		   SendRsp("SW1 is ON");
	   break;
	   case SW2_ON:
		   switch_state[1] = true;
		   SendRsp("SW2 is ON");
	   break;
	   case SW1_OFF:
		   switch_state[0] = false;
		   SendRsp("SW1 is OFF");
	   break;
	   case SW2_OFF:
		   switch_state[1] = false;
		   SendRsp("SW1 is OFF");
	   break;
	   case ON1:
		       fuse_state[0] = true;
		       save_param[0] = true;
	  		   SendRsp("FUSE 1 is ON");
	  	   break;
	   case ON2:
		       fuse_state[1] = true;
		   	   save_param[1] = true;
	  		   SendRsp("FUSE 2 is ON");
	  	   break;
	   case OFF1:
		       fuse_state[0] = false;
		   	   save_param[0] = true;
	  		   SendRsp("FUSE 1 is OFF");
	  	   break;
	   case OFF2:
		       fuse_state[1] = false;
		   	   save_param[1] = true;
	  		   SendRsp("FUSE 2 is OFF");
	   break;

	   case SET1_I:
		        if( getFuse_I(&setVal))
		        {
		        	protetionCurr[0] = setVal;
		        	savePrtCurr[0] = true;;
		        	sprintf(tempStr, "Set Protection Current M1  to %d \n",  setVal);
		        	SendRsp(tempStr);
		        }
	   break;
	   case SET1_T:
		        if(getFuse_T(&setVal))
		        {
		        	protectionTime[0] =  setVal;
                    saveTime[0] = true;
                    sprintf(tempStr, "Set Protection Time M1  to %d \n",  setVal);
                    SendRsp(tempStr);
		        }
	   break;
	   case SET2_I:
		          if( getFuse_I(&setVal))
				  {
				     protetionCurr[1] = setVal;
				     savePrtCurr[1] = true;
				     sprintf(tempStr, "Set Protection Current M2  to %d \n",  setVal);
                     SendRsp(tempStr);
				  }
	   break;
	   case SET2_T:
		           if(getFuse_T(&setVal))
		   		   {
		   		      protectionTime[1] =  setVal;
		              saveTime[1] = true;
		              sprintf(tempStr, "Set Protection Time M2  to %d \n",  setVal);
		              SendRsp(tempStr);
		   		   }

	   break;
	   case RESTART1:
             restartFuse[0] = true;
             SendRsp("Restart fuse M1");
	   break;
	   case RESTART2:
		     restartFuse[1] = true;
		     SendRsp("Restart fuse M2");
	   break;
	   default:     SendRsp("Wrong CMD"); break;
	}

	numCMD = 0;
}

/*struct pm_request_messages {
	/// set true to request a 'Measurements' message
	bool measurements;
	/// request only binary fault and status flags
	bool flags;
	// added YS

	bool  fuseRestart;
	bool  fuseParamSave;
	bool  fuseON_OFF;
	bool  fuseEN;
	float fuseProtectionCurrent;
	float fuseProtectionTime;
};*/

#define PM_MAX_UART_MSG_LEN	128
uint8_t tx_buf[PM_MAX_UART_MSG_LEN];
void SendReq (void)
{
	uint8_t msg_len = 0;
	struct pm_request_messages req = {.flags = true, .measurements = false}; // org





	req.fuseRestart = 0;// fuse_set_data[ind].fuseRestart;
	req.fuseParamSave = 0;//fuse_set_data[ind].fuseParamSave;
	req.fuseON_OFF = pm_measurements[0].fuseON_OFF_State; //fuse_set_data[ind].fuseON_OFF;
	req.fuseEN = pm_measurements[0].fuseEnabled;//fuse_set_data[ind].fuseEN;
	req.fuseProtectionCurrent = pm_measurements[0].fuseProtectionCurrent;// fuse_set_data[ind].fuseProtectionCurrent;
	req.fuseProtectionTime = pm_measurements[0].fuseProtectionTime; //fuse_set_data[ind].fuseProtectionTime;

	if(restartFuse[0])
	{
		req.fuseRestart = true;
		restartFuse[0] = false;
	}

	if(save_param[0])
	{
		req.fuseON_OFF = fuse_state[0];
		save_param[0] = false;
		req.fuseParamSave = true;

	}

	if(savePrtCurr[0])
	{
		req.fuseProtectionCurrent = protetionCurr[0];
		savePrtCurr[0] = false;
		req.fuseParamSave = true;
	}
	if(saveTime[0])
	{
		req.fuseProtectionTime = protectionTime[0];
		saveTime[0] = false;
		req.fuseParamSave = true;
	}

	msg_len = pm_request_messages_encode(&req, &tx_buf[0], PM_MAX_UART_MSG_LEN);
	HAL_UART_Transmit(&huart1, (uint8_t*)&tx_buf[0], msg_len, 100);
	if(req.fuseParamSave == true)
	{
		HAL_Delay(500);
	}
	HAL_Delay(50);

	req.fuseRestart = 0;// fuse_set_data[ind].fuseRestart;
	req.fuseParamSave = 0;//fuse_set_data[ind].fuseParamSave;
	req.fuseON_OFF = pm_measurements[1].fuseON_OFF_State; //fuse_set_data[ind].fuseON_OFF;
	req.fuseEN = pm_measurements[1].fuseEnabled;//fuse_set_data[ind].fuseEN;
	req.fuseProtectionCurrent = pm_measurements[1].fuseProtectionCurrent;// fuse_set_data[ind].fuseProtectionCurrent;
	req.fuseProtectionTime = pm_measurements[1].fuseProtectionTime; //fuse_set_data[ind].fuseProtectionTime;

	if(restartFuse[1])
	{
		req.fuseRestart = true;
		restartFuse[1] = false;
	}


	if(save_param[1] == true)
    {
		req.fuseON_OFF = fuse_state[1];
		req.fuseParamSave = true;
		save_param[1] = false;
	}

	if(savePrtCurr[1])
	{
		req.fuseProtectionCurrent = protetionCurr[1];
		savePrtCurr[1] = false;
		req.fuseParamSave = true;
	}
	if(saveTime[1])
	{
		req.fuseProtectionTime = protectionTime[1];
		saveTime[1] = false;
		req.fuseParamSave = true;
	}

	msg_len = pm_request_messages_encode(&req, &tx_buf[0], PM_MAX_UART_MSG_LEN);
    HAL_UART_Transmit(&huart2, (uint8_t*)&tx_buf[0], msg_len, 100);

	HAL_Delay(50);
	if(req.fuseParamSave == true)
	{
		HAL_Delay(500);

	}



	struct pm_control control_data;
	if(switch_state[0])
	{
		control_data.zcd = false;
		control_data.powerswitch = true;
		control_data.clear_fault = false;
	}
	else
	{
		control_data.zcd = false;
		control_data.powerswitch = false;
		control_data.clear_fault = false;
	}


	msg_len = pm_control_encode(&control_data, (uint8_t*)&tx_buf[0], PM_MAX_UART_MSG_LEN);
	HAL_UART_Transmit(&huart1, (uint8_t*)&tx_buf[0], msg_len, 100);
	HAL_Delay(50);
	if(switch_state[1])
	{
		control_data.zcd = false;
		control_data.powerswitch = true;
		control_data.clear_fault = false;
	}
	else
	{
		control_data.zcd = false;
		control_data.powerswitch = false;
		control_data.clear_fault = false;
	}

	msg_len = pm_control_encode(&control_data, (uint8_t*)&tx_buf[0], PM_MAX_UART_MSG_LEN);
	HAL_UART_Transmit(&huart2, (uint8_t*)&tx_buf[0], msg_len, 100);
	HAL_Delay(50);

}

uint32_t calculate_crc(const uint8_t *msg, size_t len){
	uint32_t crc = HAL_CRC_Calculate(&hcrc, (uint32_t *)msg, len);
	return crc;
}



 void pm_process_rx_message(size_t ind, uint8_t* message_p, size_t length, uint32_t time_stamp)
{
	//printf("%s: found msg with len = %zu\n", __func__, length);
	//pm_print_message(message_p, length);

	// ignore messages that are too short (we have at least 4 bytes of CRC)
	if (length < 4) {
		pm_rx_error_stats[ind].msg_too_short++;
		return;
	}

	enum pm_msg_type msg_type = 0;
	enum INTERFACE_STATUS result;
	// decode the message header
	cw_unpack_context cw_ctx;
	result = pm_decode_start(&msg_type, &cw_ctx, (void*)message_p, length);

	if (result == INTERFACE_STATUS_CRC_INVALID) {
		pm_rx_error_stats[ind].msg_crc++;
	}

	if (result == INTERFACE_STATUS_MSG_MALFORMED) {
		pm_rx_error_stats[ind].msg_format++;
	}

	if (result != INTERFACE_STATUS_OK) {
		//printf("%s: module %d: pm_decode_start error %d\n", __func__, (int)ind+1, (int)result);
		//pm_print_message(message_p, length);
		return;
	}

	switch (msg_type) {
		case PM_MT_MEASUREMENT: {
			struct Measurements meas;
			result = pm_measurement_decode(&meas, &cw_ctx);
			if (result == INTERFACE_STATUS_OK) {
				pm_measurements[ind] = meas;

				fuse_rd_data[ind].data_valid = true;
				fuse_rd_data[ind].on_protection_state = meas.fuseON_ProtectionState;
				fuse_rd_data[ind].enabled = meas.fuseEnabled;
				fuse_rd_data[ind].module_current =  meas.moduleCurrent;
				fuse_rd_data[ind].on_off_state = meas.fuseON_OFF_State;
				fuse_rd_data[ind].protection_current = meas.fuseProtectionCurrent;
				fuse_rd_data[ind].protection_time = meas.fuseProtectionTime;
				fuse_rd_data[ind].id_uC = meas.id_uc;

				/*if(printParam[ind])
				{
					//for( int i = 0; i < 8; i++)
					if(fuse_rd_data[ind].data_valid )
					{
						printf("Fuse %d: EN %d, ON_State %d, ProtectionState %d,  Current %.0f, Time %.0f,  valid %d, read_current %.2f,\n",
										  (ind +1), fuse_rd_data[ind].enabled,
										  fuse_rd_data[ind].on_off_state, fuse_rd_data[ind].on_protection_state,
										  fuse_rd_data[ind].protection_current, fuse_rd_data[ind].protection_time,
										  fuse_rd_data[ind].data_valid, fuse_rd_data[ind].module_current);
						printParam[ind] = false;
					}

				}*/

				//pm_meas_timestamp[ind] = time_stamp;
			} else {
				//printf("%s: module %d: pm_measurement_decode error %d\n", __func__, (int)ind+1, (int)result);
				pm_rx_error_stats[ind].msg_format++;
			}
			break;
		}

		case PM_MT_STATUS_FLAGS: {
			struct pm_status_flags flags;
			result = pm_status_flags_decode(&flags, &cw_ctx);
			if (result == INTERFACE_STATUS_OK) {
				pm_current_status_flags[ind] = flags;
				//pm_status_timestamps[ind] = time_stamp;
			} else {
				//printf("%s: module %d: pm_status_flags_decode error %d\n", __func__, (int)ind+1, (int)result);
				pm_rx_error_stats[ind].msg_format++;
			}
			break;
		}

		default: {
			//printf("%s: module %d: received message with unknown type %d\n", __func__, (int)ind+1, (int)msg_type);
			pm_rx_error_stats[ind].msg_type_unknown++;
			break;
		}
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
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart3, &rxByte, 1);
  HAL_UART_Receive_DMA (&huart1, (uint8_t*)&rxDataBuf[0], 217);
  HAL_UART_Receive_DMA (&huart2, (uint8_t*)&rxDataBuf2[0], 217);
  //HAL_UART_Receive_IT(&huart1, &rxByteM1, 1);
  //HAL_UART_Receive_IT(&huart2, &rxByteM2, 1);

 /* HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  CMD_Handler();
	  SendReq ();
	  ReceiveHandler ();
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 4-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 100;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 25;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.Pulse = 50;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.Pulse = 75;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 4-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 100;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 50;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 250000;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 250000;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, M1_DIS_Pin|M2_DIS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : M1_DIS_Pin M2_DIS_Pin */
  GPIO_InitStruct.Pin = M1_DIS_Pin|M2_DIS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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

#ifdef  USE_FULL_ASSERT
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
