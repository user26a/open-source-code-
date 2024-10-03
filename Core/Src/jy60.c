#include "wit_c_sdk.h"
#include "stdio.h"      //������Ҫ����printf()����
#include <string.h>
#include <jy60.h>

//������#include <usart.h>,��Ϊ������cubemx���ɵ�
//uart��ʼ�����������뱾�ļ��е�static������ͻ������huart2��uart,c�ж���ģ����������ԡ�
#include <usart.h>


//uart1����jy60��uart2����usb-ttl
//���ڵİ汾��uart2����jy60��uart1����usb-ttl!!!





#define ACC_UPDATE		0x01
#define GYRO_UPDATE		0x02
#define ANGLE_UPDATE	0x04
#define MAG_UPDATE		0x08
#define READ_UPDATE		0x80



static char s_cDataUpdate = 0;
static void AutoScanSensor(void);
static void SensorUartSend(uint8_t *p_data, uint32_t uiSize);
static void CopeSensorData(uint32_t uiReg, uint32_t uiRegNum);

static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);///



uint32_t uiBuad = 115200;//uiBuad���ģ����Զ�ɨ�������
uint8_t ucRxData = 0;

	float fAcc[3], fGyro[3], fAngle[3], fYaw;
	int i;



//api������

//uart2���ջص�����
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance==USART2)
  {
      WitSerialDataIn(ucRxData);
      UART_Start_Receive_IT(huart, &ucRxData, 1);
  }
}


void jy60_init(void)
{
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();//���õĲ���uart.c�еĶ���static�͵ĺ���

  	WitInit(WIT_PROTOCOL_NORMAL, 0x50);
	WitSerialWriteRegister(SensorUartSend);
	WitRegisterCallBack(CopeSensorData);

	printf("\r\n********************** wit-motion normal example  ************************\r\n");
AutoScanSensor();
}


void jy60_read(void)
{
    
		if(s_cDataUpdate)
		{
			printf("3");
			for(i = 0; i < 3; i++)
			{
				fAcc[i] = sReg[AX+i] / 32768.0f * 16.0f;
				fGyro[i] = sReg[GX+i] / 32768.0f * 2000.0f;
				fAngle[i] = sReg[Roll+i] / 32768.0f * 180.0f;
			}
			if(s_cDataUpdate & ACC_UPDATE)
			{
				printf("acc:%.3f %.3f %.3f\r\n", fAcc[0], fAcc[1], fAcc[2]);
				s_cDataUpdate &= ~ACC_UPDATE;
			}
			if(s_cDataUpdate & GYRO_UPDATE)
			{
				printf("gyro:%.3f %.3f %.3f\r\n", fGyro[0], fGyro[1], fGyro[2]);
				s_cDataUpdate &= ~GYRO_UPDATE;
			}
			if(s_cDataUpdate & ANGLE_UPDATE)
			{
                fYaw = (float)((unsigned short)sReg[Yaw]) / 32768 * 180.0;
				printf("angle:%.3f %.3f %.3f(%.3f)\r\n", fAngle[0], fAngle[1], fAngle[2], fYaw);
				s_cDataUpdate &= ~ANGLE_UPDATE;
			}
			if(s_cDataUpdate & MAG_UPDATE)
			{
				printf("mag:%d %d %d\r\n", sReg[HX], sReg[HY], sReg[HZ]);
				s_cDataUpdate &= ~MAG_UPDATE;
			}
            s_cDataUpdate = 0;
		}

}



//�ڲ�������
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
  huart1.Init.BaudRate = 115200;//uiBuad
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

    // UART_Start_Receive_IT(&huart1, &ucRxData, 1);
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
  huart2.Init.BaudRate = 9600;//115200//uiBuad
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

    UART_Start_Receive_IT(&huart2, &ucRxData, 1);//�������������Ҫ���þö�û��������Ϊ����û�Ĺ���
  /* USER CODE END USART2_Init 2 */

}



static void SensorUartSend(uint8_t *p_data, uint32_t uiSize)
{
    HAL_UART_Transmit(&huart2, p_data, uiSize, uiSize*4);
}

static void CopeSensorData(uint32_t uiReg, uint32_t uiRegNum)
{
	int i;
    for(i = 0; i < uiRegNum; i++)
    {
        switch(uiReg)
        {
            case AZ:
				s_cDataUpdate |= ACC_UPDATE;
            break;
            case GZ:
				s_cDataUpdate |= GYRO_UPDATE;
            break;
            case HZ:
				s_cDataUpdate |= MAG_UPDATE;
            break;
            case Yaw:
				s_cDataUpdate |= ANGLE_UPDATE;
            break;
            default:
				s_cDataUpdate |= READ_UPDATE;
			break;
        }
		uiReg++;
    }
}


static void AutoScanSensor(void)
{
	const uint32_t c_uiBaud[9] = {4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};
	int i, iRetry;
	
	for(i = 0; i < 9; i++)
	{
        uiBuad = c_uiBaud[i];
        MX_USART2_UART_Init();//
		iRetry = 2;
		do
		{
			s_cDataUpdate = 0;
			WitReadReg(AX, 3);
			HAL_Delay(100);
			if(s_cDataUpdate != 0)
			{
				printf("%d baud find sensor\r\n\r\n", c_uiBaud[i]);
				return ;
			}
			iRetry--;
		}while(iRetry);		
	}
	printf("can not find sensor\r\n");
	printf("please check your connection\r\n");
}




