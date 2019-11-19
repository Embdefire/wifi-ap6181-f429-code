#include "wifi_base_config.h"
//#include "./systick/bsp_SysTick.h"
#include "./camera/bsp_ov5640.h"
#include "./delay/core_delay.h"
#include "./camera/ov5640_reg.h"
#include "Debug.h"
#include "wifi_base_config.h"
#include "camera_data_queue.h"

#define ov5640_SCCB_log(M, ...) custom_log("ov5640_SCCB", M, ##__VA_ARGS__)


uint32_t frame_counter = 0;
uint32_t line_counter = 0;
uint32_t vs_counter = 0;
uint32_t err_counter = 0;

uint32_t dcmi_start_counter = 0;
uint32_t dma_start_counter = 0;
uint32_t dma_complete_counter = 0;

//uint32_t img_real_len=0;

uint32_t  XferTransferNumber=0;
uint32_t 	XferCount = 0;
uint32_t 	XferSize = 0;
uint32_t 	pBuffPtr = 0;
uint8_t 	DCMI_State;
uint32_t	DMA2_Stream1_State;

#define  TIMEOUT  2

ImageFormat_TypeDef ImageFormat;

/*Һ�����ķֱ��ʣ����������ַƫ��*/
uint16_t lcd_width=800, lcd_height=480;

/*����ͷ�ɼ�ͼ��Ĵ�С*/
uint16_t img_width=800, img_height=480;
// uint16_t img_width=640, img_height=480;
//uint16_t img_width=320, img_height=240;


/**
  * @brief  ��ʼ����������ͷʹ�õ�GPIO(I2C/DCMI)
  * @param  None
  * @retval None
  */
void OV5640_HW_Init(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  I2C_InitTypeDef  I2C_InitStruct;

	  /***DCMI��������***/
	  /* ʹ��DCMIʱ�� */
	  RCC_AHB1PeriphClockCmd(DCMI_PWDN_GPIO_CLK|DCMI_RST_GPIO_CLK|DCMI_VSYNC_GPIO_CLK | DCMI_HSYNC_GPIO_CLK | DCMI_PIXCLK_GPIO_CLK|
			  DCMI_D0_GPIO_CLK| DCMI_D1_GPIO_CLK| DCMI_D2_GPIO_CLK| DCMI_D3_GPIO_CLK|
			  DCMI_D4_GPIO_CLK| DCMI_D5_GPIO_CLK| DCMI_D6_GPIO_CLK| DCMI_D7_GPIO_CLK, ENABLE);

	  /*����/ͬ���ź���*/
	  GPIO_InitStructure.GPIO_Pin = DCMI_VSYNC_GPIO_PIN;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	  GPIO_Init(DCMI_VSYNC_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_VSYNC_GPIO_PORT, DCMI_VSYNC_PINSOURCE, DCMI_VSYNC_AF);

	  GPIO_InitStructure.GPIO_Pin = DCMI_HSYNC_GPIO_PIN ;
	  GPIO_Init(DCMI_HSYNC_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_HSYNC_GPIO_PORT, DCMI_HSYNC_PINSOURCE, DCMI_HSYNC_AF);

	  GPIO_InitStructure.GPIO_Pin = DCMI_PIXCLK_GPIO_PIN ;
	  GPIO_Init(DCMI_PIXCLK_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_PIXCLK_GPIO_PORT, DCMI_PIXCLK_PINSOURCE, DCMI_PIXCLK_AF);
    
		/*�����ź�*/
	  GPIO_InitStructure.GPIO_Pin = DCMI_D0_GPIO_PIN ;
	  GPIO_Init(DCMI_D0_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_D0_GPIO_PORT, DCMI_D0_PINSOURCE, DCMI_D0_AF);

	  GPIO_InitStructure.GPIO_Pin = DCMI_D1_GPIO_PIN ;
	  GPIO_Init(DCMI_D1_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_D1_GPIO_PORT, DCMI_D1_PINSOURCE, DCMI_D1_AF);

	  GPIO_InitStructure.GPIO_Pin = DCMI_D2_GPIO_PIN ;
	  GPIO_Init(DCMI_D2_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_D2_GPIO_PORT, DCMI_D2_PINSOURCE, DCMI_D2_AF);

	  GPIO_InitStructure.GPIO_Pin = DCMI_D3_GPIO_PIN ;
	  GPIO_Init(DCMI_D3_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_D3_GPIO_PORT, DCMI_D3_PINSOURCE, DCMI_D3_AF);

	  GPIO_InitStructure.GPIO_Pin = DCMI_D4_GPIO_PIN ;
	  GPIO_Init(DCMI_D4_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_D4_GPIO_PORT, DCMI_D4_PINSOURCE, DCMI_D4_AF);

	  GPIO_InitStructure.GPIO_Pin = DCMI_D5_GPIO_PIN ;
	  GPIO_Init(DCMI_D5_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_D5_GPIO_PORT, DCMI_D5_PINSOURCE, DCMI_D5_AF);

	  GPIO_InitStructure.GPIO_Pin = DCMI_D6_GPIO_PIN ;
	  GPIO_Init(DCMI_D6_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_D6_GPIO_PORT, DCMI_D6_PINSOURCE, DCMI_D6_AF);

	  GPIO_InitStructure.GPIO_Pin = DCMI_D7_GPIO_PIN ;
	  GPIO_Init(DCMI_D7_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(DCMI_D7_GPIO_PORT, DCMI_D7_PINSOURCE, DCMI_D7_AF);



      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
      GPIO_InitStructure.GPIO_Pin = DCMI_PWDN_GPIO_PIN ;
      GPIO_Init(DCMI_PWDN_GPIO_PORT, &GPIO_InitStructure);
      GPIO_InitStructure.GPIO_Pin = DCMI_RST_GPIO_PIN ;
      GPIO_Init(DCMI_RST_GPIO_PORT, &GPIO_InitStructure);
      /*PWDN���ţ��ߵ�ƽ�رյ�Դ���͵�ƽ����*/
     
      GPIO_ResetBits(DCMI_RST_GPIO_PORT,DCMI_RST_GPIO_PIN);
      GPIO_SetBits(DCMI_PWDN_GPIO_PORT,DCMI_PWDN_GPIO_PIN);
       
      Delay(10);
      
      GPIO_ResetBits(DCMI_PWDN_GPIO_PORT,DCMI_PWDN_GPIO_PIN);
      
      Delay(10);
      
      GPIO_SetBits(DCMI_RST_GPIO_PORT,DCMI_RST_GPIO_PIN);
      
	  /****** ����I2C��ʹ��I2C������ͷ��SCCB�ӿ�ͨѶ*****/
	 /* ʹ��I2Cʱ�� */
	  RCC_APB1PeriphClockCmd(CAMERA_I2C_CLK, ENABLE);
	  /* ʹ��I2Cʹ�õ�GPIOʱ�� */
	  RCC_AHB1PeriphClockCmd(CAMERA_I2C_SCL_GPIO_CLK|CAMERA_I2C_SDA_GPIO_CLK, ENABLE);
	  /* ��������Դ */
	  GPIO_PinAFConfig(CAMERA_I2C_SCL_GPIO_PORT, CAMERA_I2C_SCL_SOURCE, CAMERA_I2C_SCL_AF);
	  GPIO_PinAFConfig(CAMERA_I2C_SDA_GPIO_PORT, CAMERA_I2C_SDA_SOURCE, CAMERA_I2C_SDA_AF);

	  /* ��ʼ��GPIO */
	  GPIO_InitStructure.GPIO_Pin = CAMERA_I2C_SCL_PIN ;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_Init(CAMERA_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);
	  GPIO_PinAFConfig(CAMERA_I2C_SCL_GPIO_PORT, CAMERA_I2C_SCL_SOURCE, CAMERA_I2C_SCL_AF);

	  GPIO_InitStructure.GPIO_Pin = CAMERA_I2C_SDA_PIN ;
	  GPIO_Init(CAMERA_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);

	  /*��ʼ��I2Cģʽ */
	  I2C_DeInit(CAMERA_I2C); 

	  I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	  I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	  I2C_InitStruct.I2C_OwnAddress1 = 0xFE;
	  I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	  I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	  I2C_InitStruct.I2C_ClockSpeed = 400000;

	  /* д������ */
	  I2C_Init(CAMERA_I2C, &I2C_InitStruct);
		
		 /* ʹ��I2C */
	  I2C_Cmd(CAMERA_I2C, ENABLE);
      
      Delay(50);
}

/**
  * @brief  Resets the OV5640 camera.
  * @param  None
  * @retval None
  */
void OV5640_Reset(void)
{
	/*OV5640�����λ*/
  OV5640_WriteReg(0x3008, 0x80);
}

/**
  * @brief  ��ȡ����ͷ��ID.
  * @param  OV5640ID: �洢ID�Ľṹ��
  * @retval None
  */
void OV5640_ReadID(OV5640_IDTypeDef *OV5640ID)
{

	/*��ȡ�Ĵ�оƬID*/
  OV5640ID->PIDH = OV5640_ReadReg(OV5640_SENSOR_PIDH);
  OV5640ID->PIDL = OV5640_ReadReg(OV5640_SENSOR_PIDL);
}



/**
  * @brief  ���� DCMI/DMA �Բ�������ͷ����
  * @param  None
  * @retval None
  */
void OV5640_Init(void) 
{
  DCMI_InitTypeDef DCMI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /*** ����DCMI�ӿ� ***/
  /* ʹ��DCMIʱ�� */
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);

  /* DCMI ����*/ 
  DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_Continuous;
  DCMI_InitStructure.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;
  DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Rising;
  DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_High;
  DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_Low;
  DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;
  DCMI_InitStructure.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;
  DCMI_Init(&DCMI_InitStructure); 	
	
	//��ʼ���䣬�Ӻ��濪ʼһ����ɨ��������ʵ�����ݷ�ת
	//dma_memory ��16λ����Ϊ��λ�� dma_bufsize��32λ����Ϊ��λ(�����ظ���/2)
  OV5640_DMA_Config( ((uint32_t)0xD0000000)+(lcd_height-1)*(lcd_width)*2,img_width*2/4); 	

	/* �����ж� */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* �����ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn ;//DMA�������ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,ENABLE); 	
	
 	/* ����֡�жϣ����յ�֡ͬ���źžͽ����ж� */
	NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn ;	//֡�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	DCMI_ITConfig (DCMI_IT_FRAME,ENABLE);	

}


/**
  * @brief  ���� DCMI/DMA �Բ�������ͷ����
	* @param  DMA_Memory0BaseAddr:���δ����Ŀ���׵�ַ
  * @param DMA_BufferSize�����δ����������(��λΪ��,��4�ֽ�)
  */
void OV5640_DMA_Config(uint32_t DMA_Memory0BaseAddr,uint16_t DMA_BufferSize)
{

  DMA_InitTypeDef  DMA_InitStructure;
  
  /* ����DMA��DCMI�л�ȡ����*/
  /* ʹ��DMA*/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  
  DMA_Cmd(DMA2_Stream1,DISABLE);
  while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}	

  DMA_InitStructure.DMA_Channel = DMA_Channel_1;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = DCMI_DR_ADDRESS;	//DCMI���ݼĴ�����ַ
  DMA_InitStructure.DMA_Memory0BaseAddr = DMA_Memory0BaseAddr; //DMA�����Ŀ�ĵ�ַ(����Ĳ���)	
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize =DMA_BufferSize; 						//��������ݴ�С(����Ĳ���)
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			//�Ĵ�����ַ����
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;								//ѭ��ģʽ
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC8;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

  /*DMA�ж����� */
  DMA_Init(DMA2_Stream1, &DMA_InitStructure);
  
  DMA_Cmd(DMA2_Stream1,ENABLE);
	while(DMA_GetCmdStatus(DMA2_Stream1) != ENABLE){}
}


/**
  * @brief  Sets the DMA Transfer parameter.
  * @param  hdma:       pointer to a DMA_HandleTypeDef structure that contains
  *                     the configuration information for the specified DMA Stream.
  * @param  SrcAddress: The source memory Buffer address
  * @param  DstAddress: The destination memory Buffer address
  * @param  DataLength: The length of data to be transferred from source to destination
  * @retval HAL status
  */
static void DMA_SetConfig(uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{
//  /* Clear DBM bit */
//  DMA2_Stream1->CR &= (uint32_t)(~DMA_SxCR_DBM);
	
  /* Configure DMA Stream data length */
  DMA2_Stream1->NDTR = DataLength;

  /* Peripheral to Memory */
    /* Configure DMA Stream source address */
  DMA2_Stream1->PAR = SrcAddress;
    
    /* Configure DMA Stream destination address */
  DMA2_Stream1->M0AR = DstAddress;
}

int32_t HAL_DCMI_Start_DMA(uint32_t pData, uint32_t Length)
{  
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    /* Initialize the second memory address */
    uint32_t SecondMemAddress = 0;

	DMA_ITConfig(DMA2_Stream1, DMA_IT_TC | DMA_IT_TE | DMA_IT_HT,DISABLE);
	DMA_ClearFlag(DMA2_Stream1, DMA_FLAG_TCIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_HTIF1);

	DMA_Cmd(DMA2_Stream1, DISABLE);
	
	DMA_StructInit(&DMA_InitStructure);
	DMA_InitStructure.DMA_Channel = DMA_Channel_1;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
 	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;   
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;//DMA_FIFOMode_Disable
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream1, &DMA_InitStructure);

//	DMA_ITConfig(DMA2_Stream1, /*DMA_IT_TC|*/DMA_IT_TE/*|DMA_IT_HT*/, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;//DMA2_Stream1_IRQn_Priority  TBD----- by Ken
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

//  /* Process Locked */
//  __HAL_LOCK(hdcmi);

//  /* Lock the DCMI peripheral state */
//  hdcmi->State = HAL_DCMI_STATE_BUSY;

//  /* Check the parameters */
//  assert_param(IS_DCMI_CAPTURE_MODE(DCMI_Mode));

//  /* Configure the DCMI Mode */
//  hdcmi->Instance->CR &= ~(DCMI_CR_CM);
//  hdcmi->Instance->CR |=  (uint32_t)(DCMI_Mode);

  /* Set the DMA memory0 conversion complete callback */
//  hdcmi->DMA_Handle->XferCpltCallback = DCMI_DMAConvCplt;

//  /* Set the DMA error callback */
//  hdcmi->DMA_Handle->XferErrorCallback = DCMI_DMAError;

  if(Length/4 <= 0xFFFF)
  {
    /* Enable the DMA Stream */
     /* Disable the peripheral */
		DCMI_CaptureCmd(DISABLE);

  /* Configure the source, destination address and the data length */
  DMA_SetConfig((uint32_t)&DCMI->DR, (uint32_t)pData, Length/4);

  /* Enable all interrupts */
	DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);
		
  }
  else /* DCMI_DOUBLE_BUFFER Mode */
  {
    /* Set the DMA memory1 conversion complete callback */
//    hdcmi->DMA_Handle->XferM1CpltCallback = DCMI_DMAConvCplt; 

    /* Initialize transfer parameters */
    XferCount = 1;
    XferSize = Length/4;
    pBuffPtr = pData;

    /* Get the number of buffer */
    while(XferSize > 0xFFFF)
    {
      XferSize  = XferSize/2;
      XferCount = XferCount*2;
    }
		DMA_DoubleBufferModeCmd(DMA2_Stream1,ENABLE);//˫����ģʽ��??
    /* Update DCMI counter  and transfer number*/
    XferCount = (XferCount - 2);
    XferTransferNumber = XferCount;

    /* Update second memory address */
    SecondMemAddress = (uint32_t)(pData + (4*XferSize));

    /* Start DMA multi buffer transfer */
    DCMI_DMA_MultiBufferStart_IT((uint32_t)&DCMI->DR, (uint32_t)pData, SecondMemAddress,XferSize);
  }
	
//	DMA_Cmd(DMA2_Stream1, ENABLE);
	
   /* Enable the Peripheral */
		DCMI_CaptureCmd(ENABLE);

  /* Return function status */
  return kNoErr;
}

/**
  * @brief  Starts the multi_buffer DMA Transfer with interrupt enabled.
  * @param  hdma:       pointer to a DMA_HandleTypeDef structure that contains
  *                     the configuration information for the specified DMA Stream.  
  * @param  SrcAddress: The source memory Buffer address
  * @param  DstAddress: The destination memory Buffer address
  * @param  SecondMemAddress: The second memory Buffer address in case of multi buffer Transfer  
  * @param  DataLength: The length of data to be transferred from source to destination
  * @retval HAL status
  */
int32_t DCMI_DMA_MultiBufferStart_IT(uint32_t SrcAddress, uint32_t DstAddress, uint32_t SecondMemAddress, uint32_t DataLength)
{

  /* Current memory buffer used is Memory 0 */
  if((DMA2_Stream1->CR & DMA_SxCR_CT) == 0)
  {
    DMA2_Stream1_State = HAL_DMA_STATE_BUSY_MEM0;
  }
  /* Current memory buffer used is Memory 1 */
  else if((DMA2_Stream1->CR & DMA_SxCR_CT) != 0)
  {
    DMA2_Stream1_State = HAL_DMA_STATE_BUSY_MEM1;
  }
	
	//���㣬ǿ������ʹ�õ�ַ0��ʼ������??
	DMA2_Stream1->CR &= ~(DMA_SxCR_CT);

  /* Check the parameters */
  assert_param(IS_DMA_BUFFER_SIZE(DataLength));

  /* Disable the peripheral */
  DMA_Cmd(DMA2_Stream1, DISABLE);  

  /* Enable the Double buffer mode */
  DMA2_Stream1->CR |= (uint32_t)DMA_SxCR_DBM;

  /* Configure DMA Stream destination address */
  DMA2_Stream1->M1AR = SecondMemAddress;

  /* Configure the source, destination address and the data length */
  DMA_SetConfig(SrcAddress, DstAddress, DataLength); 
	
  /* Enable the transfer complete interrupt */
  /* Enable the Half transfer interrupt */
  /* Enable the transfer Error interrupt */
  /* Enable the fifo Error interrupt */
  /* Enable the direct mode Error interrupt */
	DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);
  /* Enable the peripheral */
  DMA_Cmd(DMA2_Stream1, ENABLE);

  return kNoErr; 
}


/**
* @brief  ��ʼ����ͼ�����������ȡģʽ��ͼƬ��ԴԴ���ϵķ���img_send_thread�̣߳��ȴ�����
* @param
* @param
*/
void start_capture_img()
{
	DCMI_CaptureCmd(ENABLE);
}

/**
* @brief  ֹͣ�ɼ�ͼ��
* @param
* @param
*/
void stop_capture_img()
{
	DCMI_CaptureCmd(DISABLE);
}
void OV5640_JPEGConfig(ImageFormat_TypeDef ImageFormat)
{
    uint32_t i;

    OV5640_Reset();
    mico_thread_msleep(200);
    /* Initialize OV5640 */
    for(i=0; i<(sizeof(OV5640_JPEG_INIT)/4); i++)
    {
        OV5640_WriteReg(OV5640_JPEG_INIT[i][0], OV5640_JPEG_INIT[i][1]);
    }
//    for(i=0; i<(sizeof(OV5640_JPEG_INIT)/4); i++)
//    {
//        OV5640_WriteReg(OV5640_JPEG_INIT[i][0], OV5640_JPEG_INIT[i][1]);
//    }
//    for(i=0; i<(sizeof(OV5640_JPEG_INIT)/4); i++)
//    {
//        OV5640_WriteReg(OV5640_JPEG_INIT[i][0], OV5640_JPEG_INIT[i][1]);
//    }
//    for(i=0; i<(sizeof(OV5640_JPEG_INIT)/4); i++)
//    {
//        OV5640_WriteReg(OV5640_JPEG_INIT[i][0], OV5640_JPEG_INIT[i][1]);
//    }
//    for(i=0; i<(sizeof(OV5640_JPEG_INIT)/4); i++)
//    {
//        OV5640_WriteReg(OV5640_JPEG_INIT[i][0], OV5640_JPEG_INIT[i][1]);
//    }

//    for(i=0; i<(sizeof(OV5640_JPEG_INIT)/4); i++)
//    {
//        sensor_reg[i]=OV5640_ReadReg(OV5640_JPEG_INIT[i][0]); 
//        if(OV5640_JPEG_INIT[i][1] != sensor_reg[i])        
//        ov5640_SCCB_log("sensor_reg[%d]:%x-%x\n",i,OV5640_JPEG_INIT[i][1],sensor_reg[i]);
//    }
    
    mico_thread_msleep(10);

    switch(ImageFormat)
    {
     case JPEG_160x120:
        {
            for(i=0; i<(sizeof(OV5640_160x120_JPEG)/4); i++)
            {
                OV5640_WriteReg(OV5640_160x120_JPEG[i][0], OV5640_160x120_JPEG[i][1]);
            }
            break;
        }
     case JPEG_320x240:
        {
            for(i=0; i<(sizeof(OV5640_320x240_JPEG)/4); i++)
            {
                OV5640_WriteReg(OV5640_320x240_JPEG[i][0], OV5640_320x240_JPEG[i][1]);
            }
            break;
        }
     case JPEG_640x480:
        {
            for(i=0; i<(sizeof(OV5640_640x480_JPEG)/4); i++)
            {
                OV5640_WriteReg(OV5640_640x480_JPEG[i][0], OV5640_640x480_JPEG[i][1]);
            }
            break;            
        }

     case JPEG_800x600:
        {
            for(i=0; i<(sizeof(OV5640_800x600_JPEG)/4); i++)
            {
                OV5640_WriteReg(OV5640_800x600_JPEG[i][0], OV5640_800x600_JPEG[i][1]);
            }
            break;
        }
     case JPEG_1024x768:
        {
            for(i=0; i<(sizeof(OV5640_1024x768_JPEG)/4); i++)
            {
                OV5640_WriteReg(OV5640_1024x768_JPEG[i][0], OV5640_1024x768_JPEG[i][1]);
            }
            break;
        }
		case JPEG_1280x960:
        {
            for(i=0; i<(sizeof(OV5640_1280x960_JPEG)/4); i++)
            {
                    OV5640_WriteReg(OV5640_1280x960_JPEG[i][0], OV5640_1280x960_JPEG[i][1]);
            }
            break;
        }
		case JPEG_1600x1200:
        {
            for(i=0; i<(sizeof(OV5640_1600x1200_JPEG)/4); i++)
            {
                    OV5640_WriteReg(OV5640_1600x1200_JPEG[i][0], OV5640_1600x1200_JPEG[i][1]);
            }
            break;
        }
        case JPEG_2048x1536:
        {
            for(i=0; i<(sizeof(OV5640_2048x1536_JPEG)/4); i++)
            {
                    OV5640_WriteReg(OV5640_2048x1536_JPEG[i][0], OV5640_2048x1536_JPEG[i][1]);
            }
            break;
        }
        case JPEG_2320x1740:
        {
            for(i=0; i<(sizeof(OV5640_2320x1740_JPEG)/4); i++)
            {
                    OV5640_WriteReg(OV5640_2320x1740_JPEG[i][0], OV5640_2320x1740_JPEG[i][1]);
            }
            break;
        }
        case JPEG_2592x1944:
        {
            for(i=0; i<(sizeof(OV5640_2592x1944_JPEG)/4); i++)
            {
                    OV5640_WriteReg(OV5640_2592x1944_JPEG[i][0], OV5640_2592x1944_JPEG[i][1]);
            }
            break;
        }
     default:
        {
            for(i=0; i<(sizeof(OV5640_320x240_JPEG)/4); i++)
            {
                OV5640_WriteReg(OV5640_320x240_JPEG[i][0], OV5640_320x240_JPEG[i][1]);
            }
            break;
        }
    }
}


OV5640_IDTypeDef OV5640_Camera_ID;
int32_t OV5640_SCCB_Init(ImageFormat_TypeDef ImageFormat)
{
	I2C_InitTypeDef  I2C_InitStruct;
	GPIO_InitTypeDef GPIO_InitStructure;
		/****** ����I2C��ʹ��I2C������ͷ��SCCB�ӿ�ͨѶ*****/
		/* ʹ��I2Cʱ�� */
		RCC_APB1PeriphClockCmd(CAMERA_I2C_CLK, ENABLE);
		/* ʹ��I2Cʹ�õ�GPIOʱ�� */
		RCC_AHB1PeriphClockCmd(CAMERA_I2C_SCL_GPIO_CLK|CAMERA_I2C_SDA_GPIO_CLK, ENABLE);
		/* ��������Դ */
		GPIO_PinAFConfig(CAMERA_I2C_SCL_GPIO_PORT, CAMERA_I2C_SCL_SOURCE, CAMERA_I2C_SCL_AF);
		GPIO_PinAFConfig(CAMERA_I2C_SDA_GPIO_PORT, CAMERA_I2C_SDA_SOURCE, CAMERA_I2C_SDA_AF);

		/* ��ʼ��GPIO */
		GPIO_InitStructure.GPIO_Pin = CAMERA_I2C_SCL_PIN ;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(CAMERA_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);
		GPIO_PinAFConfig(CAMERA_I2C_SCL_GPIO_PORT, CAMERA_I2C_SCL_SOURCE, CAMERA_I2C_SCL_AF);

		GPIO_InitStructure.GPIO_Pin = CAMERA_I2C_SDA_PIN ;
		GPIO_Init(CAMERA_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);

		/*��ʼ��I2Cģʽ */
		I2C_DeInit(CAMERA_I2C); 

		I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
		I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
		I2C_InitStruct.I2C_OwnAddress1 = 0xFE;
		I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
		I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
		I2C_InitStruct.I2C_ClockSpeed = 400000;

		/* д������ */
		I2C_Init(CAMERA_I2C, &I2C_InitStruct);

		/* ʹ��I2C */
		I2C_Cmd(CAMERA_I2C, ENABLE);

		Delay(50);
    
		OV5640_Camera_ID.PIDH = OV5640_ReadReg(OV5640_SENSOR_PIDH);
		OV5640_Camera_ID.PIDL = OV5640_ReadReg(OV5640_SENSOR_PIDL);
		CAMERA_DEBUG("%x %x",OV5640_Camera_ID.PIDH ,OV5640_Camera_ID.PIDL);
    
    if (OV5640_Camera_ID.PIDH != 0x56)
        return kGeneralErr;
    
    ov5640_SCCB_log("OV5640_ID:0x%x\n",OV5640_Camera_ID.PIDH);
    
    OV5640_JPEGConfig(ImageFormat);
    ov5640_SCCB_log("OV5640_ImageFormat:%x\n",ImageFormat);
    OV5640_AUTO_FOCUS();
    return kNoErr;
}

/**
* @brief  ��ʼ������ͷ����ʼ��mcu��dcmi�ӿڣ��Լ�����ͷ
* @param
* @param
*/
int32_t open_camera(uint32_t *BufferSRC, uint32_t BufferSize)
{
	DCMI_InitTypeDef DCMI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);//DMA2
	OV5640_HW_Init();

	DCMI_DeInit();
	DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_Continuous;
	DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;
	DCMI_InitStructure.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;
	DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_Low;
	DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Rising;
	DCMI_InitStructure.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;
	DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_Low;

	DCMI_Init(&DCMI_InitStructure);

	//	DMA_DCMIConfiguration(BufferSRC, BufferSize);
	HAL_DCMI_Start_DMA((uint32_t)BufferSRC, BufferSize);

	DCMI_ITConfig(DCMI_IT_FRAME, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;//DCMI_IRQn_Priority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	if (OV5640_SCCB_Init(JPEG_FORMAT) != kNoErr)
        return  kGeneralErr;

  start_capture_img();
  
	DCMI_JPEGCmd(ENABLE); //for OV5640
  DCMI_Cmd(ENABLE); 
  
	return kNoErr;
}




/**
  * @brief  дһ�ֽ����ݵ�OV5640�Ĵ���
  * @param  Addr: OV5640 �ļĴ�����ַ
  * @param  Data: Ҫд�������
  * @retval ����0��ʾд��������0xFF��ʾ����
  */
uint8_t OV5640_WriteReg(uint16_t Addr, uint8_t Data)
{
	uint32_t timeout = DCMI_TIMEOUT_MAX;
  
	/* Generate the Start Condition */
	I2C_GenerateSTART(CAMERA_I2C, ENABLE);

	/* Test on CAMERA_I2C EV5 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_MODE_SELECT))
	{
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	}
   
	/* Send DCMI selcted device slave Address for write */
	I2C_Send7bitAddress(CAMERA_I2C, OV5640_DEVICE_ADDRESS, I2C_Direction_Transmitter);
 
	/* Test on CAMERA_I2C EV6 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	}
 
	/* Send CAMERA_I2C location address MSB */
	I2C_SendData(CAMERA_I2C, (uint8_t)( (Addr >> 8) & 0xFF) );

	/* Test on CAMERA_I2C EV8 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	}
	//--------------------------------------------------------
	/* Send I2C1 location address LSB */
	I2C_SendData( CAMERA_I2C, (uint8_t)(Addr & 0xFF) );
	/* Test on I2C1 EV8 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	} 
  

	//--------------------------------------------------------

	/* Send Data */
	I2C_SendData(CAMERA_I2C, Data);    

	/* Test on CAMERA_I2C EV8 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	}  
 
	/* Send I2C1 STOP Condition */
	I2C_GenerateSTOP(CAMERA_I2C, ENABLE);
  
	/* If operation is OK, return 0 */
	return 0;
}
/**
  * @brief  ��OV5640�Ĵ����ж�ȡһ���ֽڵ�����
  * @param  Addr: �Ĵ�����ַ
  * @retval ���ض�ȡ�õ�����
  */
uint8_t OV5640_ReadReg(uint16_t Addr)
{
	uint32_t timeout = DCMI_TIMEOUT_MAX;
	uint8_t Data = 0;

	/* Generate the Start Condition */
	I2C_GenerateSTART(CAMERA_I2C, ENABLE);

	/* Test on CAMERA_I2C EV5 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_MODE_SELECT))
	{
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	} 
  
	/* Send DCMI selcted device slave Address for write */
	I2C_Send7bitAddress(CAMERA_I2C, OV5640_DEVICE_ADDRESS, I2C_Direction_Transmitter);
 
	/* Test on I2C1 EV6 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	} 

	/* Send I2C1 location address MSB */
	I2C_SendData( CAMERA_I2C, (uint8_t)((Addr>>8) & 0xFF) );

	/* Test on I2C1 EV8 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	} 
  
	/* Clear AF flag if arised */
	CAMERA_I2C->SR1 |= (uint16_t)0x0400; 
  
//--------------------------------------------------------
	/* Send I2C1 location address LSB */
	I2C_SendData( CAMERA_I2C, (uint8_t)(Addr & 0xFF) );
	/* Test on I2C1 EV8 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	} 
  
	/* Clear AF flag if arised */
	CAMERA_I2C->SR1 |= (uint16_t)0x0400;
	//--------------------------------------------------------

	/* Generate the Start Condition */
	I2C_GenerateSTART(CAMERA_I2C, ENABLE);
  
	/* Test on I2C1 EV6 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_MODE_SELECT))
	{
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	} 
  
	/* Send DCMI selcted device slave Address for write */
	I2C_Send7bitAddress(CAMERA_I2C, OV5640_DEVICE_ADDRESS, I2C_Direction_Receiver);
   
	/* Test on I2C1 EV6 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	}  
 
	/* Prepare an NACK for the next data received */
	I2C_AcknowledgeConfig(CAMERA_I2C, DISABLE);
	
	/* Test on I2C1 EV7 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))
	{
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	}   
    
	/* Prepare Stop after receiving data */
	I2C_GenerateSTOP(CAMERA_I2C, ENABLE);

	/* Receive the Data */
	Data = I2C_ReceiveData(CAMERA_I2C);

	/* return the read data */
	return Data;
}






/**

  * @brief  ���̼�д�뵽OV5640Ƭ��MCU
  * @param  Addr: OV5640 ��MCU����ַ0x8000
  * @param  Data: Ҫд�������
  * @retval ����0��ʾд��������0xFF��ʾ����
  */
uint8_t OV5640_WriteFW(uint8_t *pBuffer ,uint16_t BufferSize)
{
	uint32_t timeout = DCMI_TIMEOUT_MAX;
    uint16_t Addr=0x8000;
    
	/* Generate the Start Condition */
	I2C_GenerateSTART(CAMERA_I2C, ENABLE);

	/* Test on CAMERA_I2C EV5 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_MODE_SELECT))
	{
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	}
   
	/* Send DCMI selcted device slave Address for write */
	I2C_Send7bitAddress(CAMERA_I2C, OV5640_DEVICE_ADDRESS, I2C_Direction_Transmitter);
 
	/* Test on CAMERA_I2C EV6 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	}
 
	/* Send CAMERA_I2C location address MSB */
	I2C_SendData(CAMERA_I2C, (uint8_t)( (Addr >> 8) & 0xFF) );

	/* Test on CAMERA_I2C EV8 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	}
	//--------------------------------------------------------
	/* Send I2C1 location address LSB */
	I2C_SendData( CAMERA_I2C, (uint8_t)(Addr & 0xFF) );
	/* Test on I2C1 EV8 and clear it */
	timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
	while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		/* If the timeout delay is exeeded, exit with error code */
		if ((timeout--) == 0) return 0xFF;
	} 
  
	//--------------------------------------------------------
/* While there is data to be written */
  while(BufferSize--)  
  {
    /* Send Data */
    I2C_SendData(CAMERA_I2C, *pBuffer);  
      
    /* Point to the next byte to be written */
    pBuffer++; 
    /* Test on CAMERA_I2C EV8 and clear it */
    timeout = DCMI_TIMEOUT_MAX; /* Initialize timeout value */
    while(!I2C_CheckEvent(CAMERA_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        /* If the timeout delay is exeeded, exit with error code */
        if ((timeout--) == 0) return 0xFF;
    }  
   }
	/* Send I2C1 STOP Condition */
	I2C_GenerateSTOP(CAMERA_I2C, ENABLE);
  
	/* If operation is OK, return 0 */
	return 0;
}
void DCMI_Start(void)
{
	camera_data *data_p;
	
	/*��ȡд������ָ�룬׼��д��������*/
	data_p = cbWrite(&cam_circular_buff);
	
	if (data_p != NULL)	//���������δ������ʼ��
	{		
		dma_start_counter++;
		
		/*����DMA����*/
		HAL_DCMI_Start_DMA((uint32_t)data_p->head, CAMERA_QUEUE_DATA_LEN);
		DMA_Cmd(DMA2_Stream1, ENABLE);			//���´���
	}
	
	 dcmi_start_counter ++;
		
	DCMI_CaptureCmd(ENABLE);
}

void DCMI_Stop(void)
{	
	camera_data *data_p;
	
	/*�ر�dma*/
    DMA_Cmd(DMA2_Stream1,DISABLE);
	while(DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}

//    DCMI_CaptureCmd(DISABLE);	
		

	/*��ȡ���ڲ�����дָ��*/	
	data_p = cbWriteUsing(&cam_circular_buff);
		
	/*����dma�������????�������ڼ���jpeg�����ļ�β��ʱ��*/	
	if (data_p != NULL)	
	{
		data_p->img_dma_len =0; //��λ	
		
		if(CAMERA_QUEUE_DATA_LEN>65536*4)	//˫dma buff
		{
			data_p->img_dma_len = (XferSize - DMA_GetCurrDataCounter(DMA2_Stream1))*4; //���һ????
			
			if(dma_complete_counter>=2)
				data_p->img_dma_len += ((dma_complete_counter-1)*XferSize)*4 ;		//	dma_complete_counter??��СΪXferSize�İ�
		}
		else	//��dma buf
			data_p->img_dma_len = (CAMERA_QUEUE_DATA_LEN/4 - DMA_GetCurrDataCounter(DMA2_Stream1))*4;

//		data_p->img_dma_len = img_real_len;

	
	}
	
	/*д�뻺������??*/
	cbWriteFinish(&cam_circular_buff);
}

static void DMA_ChangeMemory(uint32_t Address, HAL_DMA_MemoryTypeDef memory)
{
  if(memory == MEMORY0)
  {
    /* change the memory0 address */
    DMA2_Stream1->M0AR = Address;
  }
  else
  {

    /* change the memory1 address */
    DMA2_Stream1->M1AR = Address;
  }

}

/* Private functions ---------------------------------------------------------*/
/** @defgroup DCMI_Private_Functions DCMI Private Functions
  * @{
  */
  /**
  * @brief  DMA conversion complete callback. 
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
void DCMI_DMAConvCplt(void)
{
  uint32_t tmp = 0;
 
  DCMI_State= 1;
	

  if(XferCount != 0)
  {
    /* Update memory 0 address location */
    tmp = ((DMA2_Stream1->CR) & DMA_SxCR_CT);
    if(((XferCount % 2) == 0) && (tmp != 0))
    {
      tmp = DMA2_Stream1->M0AR;
      DMA_ChangeMemory((tmp + (8*XferSize)), MEMORY0);
      XferCount--;
			dma_complete_counter++;

    }
    /* Update memory 1 address location */
    else if((DMA2_Stream1->CR & DMA_SxCR_CT) == 0)
    {
      tmp = DMA2_Stream1->M1AR;
      DMA_ChangeMemory((tmp + (8*XferSize)), MEMORY1);
      XferCount--;
			dma_complete_counter++;

    }
  }
  /* Update memory 0 address location */
  else if((DMA2_Stream1->CR & DMA_SxCR_CT) != 0)
  {
    DMA2_Stream1->M0AR = pBuffPtr;
  }
  /* Update memory 1 address location */
  else if((DMA2_Stream1->CR & DMA_SxCR_CT) == 0)
  {
    tmp = pBuffPtr;
    DMA2_Stream1->M1AR = (tmp + (4*XferSize));
    XferCount = XferTransferNumber;
  }

  if(DCMI_GetFlagStatus(DCMI_FLAG_FRAMERI) != RESET)
  {
    /* FRAME Callback */
//    HAL_DCMI_FrameEventCallback(hdcmi);
  }
}
void DMA2_Stream1_IRQHandler(void)
{
      /* Transfer Complete Interrupt management ***********************************/
  if (DMA_GetFlagStatus(DMA2_Stream1,DMA_FLAG_TCIF1)==SET)
  {
//    if(DMA_GetFlagStatus(DMA2_Stream1, DMA_IT_TC) != RESET)
    {
      if(((DMA2_Stream1->CR) & (uint32_t)(DMA_SxCR_DBM)) != 0)
      {

        /* Clear the transfer complete flag */
        DMA_ClearFlag(DMA2_Stream1,DMA_FLAG_TCIF1);

        /* Current memory buffer used is Memory 1 */
        if((DMA2_Stream1->CR & DMA_SxCR_CT) == 0)
        {
          /* Transfer complete Callback for memory0 */
            DCMI_DMAConvCplt();
        }
        /* Current memory buffer used is Memory 0 */
        else if((DMA2_Stream1->CR & DMA_SxCR_CT) != 0) 
        {		

          /* Transfer complete Callback for memory0 */
            DCMI_DMAConvCplt();
        }
      }
      /* Disable the transfer complete interrupt if the DMA mode is not CIRCULAR */
      else
      {
        if(((DMA2_Stream1->CR) & (uint32_t)(DMA_SxCR_DBM)) == 0)
        {
          /* Disable the transfer complete interrupt */
          DMA_ITConfig(DMA2_Stream1, DMA_IT_TC,DISABLE);
        }
        /* Clear the transfer complete flag */
        DMA_ClearFlag(DMA2_Stream1,DMA_FLAG_TCIF1);
        /* Update error code */

        /* Change the DMA state */
        DMA2_Stream1_State = HAL_DMA_STATE_READY_MEM0;
 
        /* Transfer complete callback */
        DCMI_DMAConvCplt();
      }
    }
  }
}

void DCMI_IRQHandler(void)
{
	if(DCMI_GetITStatus(DCMI_IT_FRAME) == SET )
	{
			DCMI_ClearITPendingBit(DCMI_IT_FRAME);
			frame_counter ++;

			//1.ֹͣdma����
			DCMI_Stop();

			//2.���ݻ�����ʹ����������Ƿ���dma
			DCMI_Start();			

			/*��λdma�������*/
			dma_complete_counter=0;
	}
}