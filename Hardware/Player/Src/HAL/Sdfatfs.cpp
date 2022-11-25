/****************************************************************************
* @File name: Sdfatfs.cpp
* @Author: sunpeng
* @Version: v0.0.3
* @Date: 2022.11.22
* @Description: SD card and file system initialization functions are provided,
* and necessary functions such as read and write functions are provided
****************************************************************************/
/*SD fatfs Header*/
#include "Sdfatfs.h"
/*Physical layer pins*/
#include "../config.h"


/*====fatfs v=====*/
FATFS HAL::fs;
FIL   HAL::f;
FRESULT res;
const TCHAR* SD_PATH = (const TCHAR* )"0:";
/*SD obj but use in intern */
SD_Manager HAL::SD;
static SD_Manager::SD_Error status;

/*=============================================================sdfatfs API==========================================================*/

/**
 * @brief SDFatfs Init
 * 
 */
void HAL::SDfatfs_Init()
{
	/*SD Init*/
	status = SD.SD_Init();
	while(status)
	{
#if (PRINT_INFO)
 		LOG("->SD Load Failed-Error Code : %d\r\n",status);
		delay_ms(500);   
#endif
	}
#if (PRINT_INFO)
    LOG("->SD Load success\r\n");
#endif
	/*fatfs Init*/
	res = f_mount(&fs,(const TCHAR*)SD_PATH,1);
	while(res != FR_OK)
	{
#if (PRINT_INFO)
		LOG("->Fatfs System Load Failed-Error Code : %d\r\n",res);
		delay_ms(500); 
#endif
	}
#if (PRINT_INFO)
    LOG("->Fatfs Load success\r\n");
#endif	
	/*jpg decode init*/
    piclib_init();
#if (PRINT_INFO)
    LOG("->DecodeJPG Load success\r\n");
#endif	
}




/*============================================================sdfatfs API==============================================================*/





/*=================================SD��ز���:ʹ��SD_Manager����SD����ʼ���ṩ�ļ�ϵͳ��дPAI==========================================*/




__align(4) uint8_t SDIO_DATA_BUFFER[512];

extern "C"
{
    /*�ر������ж�(���ǲ�����fault��NMI�ж�)*/
    __asm void _INTX_DISABLE(void)
    {
        CPSID   I
        BX      LR	  
    }
    /*���������ж�*/
    __asm void _INTX_ENABLE(void)
    {
        CPSIE   I
        BX      LR  
    }

}


SD_Manager::SD_Manager()
{
    /*SD��ʼ����ز���*/
    CardType=SDIO_STD_CAPACITY_SD_CARD_V1_1;		
    RCA=0;					
    DeviceMode=SD_DMA_MODE;		   				
    StopCondition=0; 								
    TransferError=SD_OK;					
    TransferEnd=0;		

}


/*SDIO reg deinit*/
void SD_Manager::SDIO_Register_Deinit()
{

	SDIO->POWER=0x00000000;
	SDIO->CLKCR=0x00000000;
	SDIO->ARG=0x00000000;
	SDIO->CMD=0x00000000;
	SDIO->DTIMER=0x00000000;
	SDIO->DLEN=0x00000000;
	SDIO->DCTRL=0x00000000;
	SDIO->ICR=0x00C007FF;
	SDIO->MASK=0x00000000;	 
}


/*SD��ʼ��*/
SD_Manager::SD_Error SD_Manager::SD_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	SD_Error errorstatus=SD_OK;	 
  	uint8_t clkdiv=0;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);//ʹ��GPIOC,GPIOD DMA2ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);//SDIOʱ��ʹ��
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SDIO, ENABLE);//SDIO��λ
	pinMode(SDIO_D0,OUTPUT_AF);
    pinMode(SDIO_D1,OUTPUT_AF);
    pinMode(SDIO_D2,OUTPUT_AF);
    pinMode(SDIO_D3,OUTPUT_AF);
    pinMode(SDIO_CLK,OUTPUT_AF);
    pinMode(SDIO_CMD,OUTPUT_AF);
	 //���Ÿ���ӳ������
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_SDIO); //PC8,AF12
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_SDIO);
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SDIO);
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SDIO);
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SDIO);	
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_SDIO);	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SDIO, DISABLE);//SDIO������λ
		
 	//SDIO����Ĵ�������ΪĬ��ֵ 			   
	SDIO_Register_Deinit();
	
    NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	
   	errorstatus=SD_PowerON();			//SD���ϵ�
 	if(errorstatus==SD_OK)errorstatus=SD_InitializeCards();			//��ʼ��SD��														  
  	if(errorstatus==SD_OK)errorstatus=SD_GetCardInfo(&SDCardInfo);	//��ȡ����Ϣ
 	if(errorstatus==SD_OK)errorstatus=SD_SelectDeselect((uint32_t)(SDCardInfo.RCA<<16));//ѡ��SD��   
   	if(errorstatus==SD_OK)errorstatus=SD_EnableWideBusOperation(SDIO_BusWide_4b);	//4λ����,�����MMC��,������4λģʽ 
  	if((errorstatus==SD_OK)||(SDIO_MULTIMEDIA_CARD==CardType))
	{  		    
		if(SDCardInfo.CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1||SDCardInfo.CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0)
		{
			clkdiv=SDIO_TRANSFER_CLK_DIV+2;	//V1.1/V2.0�����������48/4=12Mhz
		}else clkdiv=SDIO_TRANSFER_CLK_DIV;	//SDHC�����������������48/2=24Mhz
		SDIO_Clock_Set(clkdiv);	//����ʱ��Ƶ��,SDIOʱ�Ӽ��㹫ʽ:SDIO_CKʱ��=SDIOCLK/[clkdiv+2];����,SDIOCLK�̶�Ϊ48Mhz 
		errorstatus=SD_SetDeviceMode(SD_WORKMODE);//���ù���ģʽΪ��ѯģʽ
 	}
	return errorstatus;		  
}



/* 
SDIOʱ�ӳ�ʼ������
clkdiv:ʱ�ӷ�Ƶϵ��
CKʱ��=SDIOCLK/[clkdiv+2];(SDIOCLKʱ�ӹ̶�Ϊ48Mhz) 
*/
void SD_Manager::SDIO_Clock_Set(uint8_t clkdiv)
{
	uint32_t tmpreg=SDIO->CLKCR; 
  	tmpreg&=0XFFFFFF00; 
 	tmpreg|=clkdiv;   
	SDIO->CLKCR=tmpreg;
} 


//���ϵ�
//��ѯ����SDIO�ӿ��ϵĿ��豸,����ѯ���ѹ������ʱ��
//����ֵ:�������;(0,�޴���)
SD_Manager::SD_Error SD_Manager::SD_PowerON(void)
{
 	uint8_t i=0;
	SD_Error errorstatus=SD_OK;
	uint32_t response=0,count=0,validvoltage=0;
	uint32_t SDType=SD_STD_CAPACITY;
	
        /*��ʼ��ʱ��ʱ�Ӳ��ܴ���400KHz*/ 
    SDIO_InitStructure.SDIO_ClockDiv = SDIO_INIT_CLK_DIV;	/* HCLK = 72MHz, SDIOCLK = 72MHz, SDIO_CK = HCLK/(178 + 2) = 400 KHz */
    SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
    SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;  //��ʹ��bypassģʽ��ֱ����HCLK���з�Ƶ�õ�SDIO_CK
    SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;	// ����ʱ���ر�ʱ�ӵ�Դ
    SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;	 				//1λ������
    SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;//Ӳ����
    SDIO_Init(&SDIO_InitStructure);

    SDIO_SetPowerState(SDIO_PowerState_ON);	//�ϵ�״̬,������ʱ��   
    SDIO->CLKCR|=1<<8;			//SDIOCKʹ��  
    
 	for(i=0;i<74;i++)
	{
 
		SDIO_CmdInitStructure.SDIO_Argument = 0x0;//����CMD0����IDLE STAGEģʽ����.
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_GO_IDLE_STATE; //cmd0
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_No;  //����Ӧ
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;  //��CPSM�ڿ�ʼ��������֮ǰ�ȴ����ݴ�������� 
        SDIO_SendCommand(&SDIO_CmdInitStructure);	  		//д���������Ĵ���
		
		errorstatus=CmdError();
		
		if(errorstatus==SD_OK)break;
 	}
 	if(errorstatus)return errorstatus;//���ش���״̬
        
    SDIO_CmdInitStructure.SDIO_Argument = SD_CHECK_PATTERN;	//����CMD8,����Ӧ,���SD���ӿ�����
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SEND_IF_COND;	//cmd8
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;	 //r7
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;			 //�رյȴ��ж�
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
	
    errorstatus=CmdResp7Error();						//�ȴ�R7��Ӧ
	
 	if(errorstatus==SD_OK) 								//R7��Ӧ����
	{
		CardType=SDIO_STD_CAPACITY_SD_CARD_V2_0;		//SD 2.0��
		SDType=SD_HIGH_CAPACITY;			   			//��������
	}
	  
	SDIO_CmdInitStructure.SDIO_Argument = 0x00;//����CMD55,����Ӧ	
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);		//����CMD55,����Ӧ	 
	
	errorstatus=CmdResp1Error(SD_CMD_APP_CMD); 		 	//�ȴ�R1��Ӧ   
	
	if(errorstatus==SD_OK)//SD2.0/SD 1.1,����ΪMMC��
	{																  
		//SD��,����ACMD41 SD_APP_OP_COND,����Ϊ:0x80100000 
		while((!validvoltage)&&(count<SD_MAX_VOLT_TRIAL))
		{	   										   
		    SDIO_CmdInitStructure.SDIO_Argument = 0x00;//����CMD55,����Ӧ
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;	  //CMD55
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
            SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);			//����CMD55,����Ӧ	 
			
			errorstatus=CmdResp1Error(SD_CMD_APP_CMD); 	 	//�ȴ�R1��Ӧ  
			
 			if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����

            //acmd41�����������֧�ֵĵ�ѹ��Χ��HCSλ��ɣ�HCSλ��һ�����ֿ���SDSc����sdhc
            SDIO_CmdInitStructure.SDIO_Argument = SD_VOLTAGE_WINDOW_SD | SDType;	//����ACMD41,����Ӧ	
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_OP_COND;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  //r3
            SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);
			
			errorstatus=CmdResp3Error(); 					//�ȴ�R3��Ӧ   
			
 			if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ���� 
			response=SDIO->RESP1;;			   				//�õ���Ӧ
			validvoltage=(((response>>31)==1)?1:0);			//�ж�SD���ϵ��Ƿ����
			count++;
		}
		if(count>=SD_MAX_VOLT_TRIAL)
		{
			errorstatus=SD_INVALID_VOLTRANGE;
			return errorstatus;
		}	 
		if(response&=SD_HIGH_CAPACITY)
		{
			CardType=SDIO_HIGH_CAPACITY_SD_CARD;
		}
 	}else//MMC��
	{
		//MMC��,����CMD1 SDIO_SEND_OP_COND,����Ϊ:0x80FF8000 
		while((!validvoltage)&&(count<SD_MAX_VOLT_TRIAL))
		{	   										   				   
			SDIO_CmdInitStructure.SDIO_Argument = SD_VOLTAGE_WINDOW_MMC;//����CMD1,����Ӧ	   
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_OP_COND;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  //r3
            SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);
			
			errorstatus=CmdResp3Error(); 					//�ȴ�R3��Ӧ   
			
 			if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����  
			response=SDIO->RESP1;;			   				//�õ���Ӧ
			validvoltage=(((response>>31)==1)?1:0);
			count++;
		}
		if(count>=SD_MAX_VOLT_TRIAL)
		{
			errorstatus=SD_INVALID_VOLTRANGE;
			return errorstatus;
		}	 			    
		CardType=SDIO_MULTIMEDIA_CARD;	  
  	}  
  	return (errorstatus);		
}

/* 
SD�� Power OFF
����ֵ:�������;(0,�޴���) 
*/
SD_Manager::SD_Error SD_Manager::SD_PowerOFF(void)
{
    SDIO_SetPowerState(SDIO_PowerState_OFF);//SDIO��Դ�ر�,ʱ��ֹͣ	
    return SD_OK;	  
}   

/*
��ʼ�����еĿ�,���ÿ��������״̬
����ֵ:�������
*/
SD_Manager::SD_Error SD_Manager::SD_InitializeCards(void)
{
 	SD_Error errorstatus=SD_OK;
	uint16_t rca = 0x01;
	
    if (SDIO_GetPowerState() == SDIO_PowerState_OFF)	//����Դ״̬,ȷ��Ϊ�ϵ�״̬
    {
        errorstatus = SD_REQUEST_NOT_APPLICABLE;
        return(errorstatus);
    }

 	if(SDIO_SECURE_DIGITAL_IO_CARD!=CardType)			//��SECURE_DIGITAL_IO_CARD
	{
		SDIO_CmdInitStructure.SDIO_Argument = 0x0;//����CMD2,ȡ��CID,����Ӧ
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_ALL_SEND_CID;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);//����CMD2,ȡ��CID,����Ӧ	
		
		errorstatus=CmdResp2Error(); 					//�ȴ�R2��Ӧ 
		
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����		 
		
 		CID_Tab[0]=SDIO->RESP1;
		CID_Tab[1]=SDIO->RESP2;
		CID_Tab[2]=SDIO->RESP3;
		CID_Tab[3]=SDIO->RESP4;
	}
	if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_SECURE_DIGITAL_IO_COMBO_CARD==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))//�жϿ�����
	{
		SDIO_CmdInitStructure.SDIO_Argument = 0x00;//����CMD3,����Ӧ 
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;	//cmd3
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short; //r6
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);	//����CMD3,����Ӧ 
		
		errorstatus=CmdResp6Error(SD_CMD_SET_REL_ADDR,&rca);//�ȴ�R6��Ӧ 
		
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����		    
	}   
    if (SDIO_MULTIMEDIA_CARD==CardType)
    {

		SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)(rca<<16);//����CMD3,����Ӧ 
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;	//cmd3
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short; //r6
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);	//����CMD3,����Ӧ 	
			
        errorstatus=CmdResp2Error(); 					//�ȴ�R2��Ӧ   
			
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����	 
    }
	if (SDIO_SECURE_DIGITAL_IO_CARD!=CardType)			//��SECURE_DIGITAL_IO_CARD
	{
		RCA = rca;
		
        SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)(rca << 16);//����CMD9+��RCA,ȡ��CSD,����Ӧ 
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_CSD;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);
		
		errorstatus=CmdResp2Error(); 					//�ȴ�R2��Ӧ   
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����		    
  		
		CSD_Tab[0]=SDIO->RESP1;
	    CSD_Tab[1]=SDIO->RESP2;
		CSD_Tab[2]=SDIO->RESP3;						
		CSD_Tab[3]=SDIO->RESP4;					    
	}
	return SD_OK;//����ʼ���ɹ�
} 

/* 
�õ�����Ϣ
cardinfo:����Ϣ�洢��
����ֵ:����״̬ 
*/
SD_Manager::SD_Error SD_Manager::SD_GetCardInfo(SD_CardInfo *cardinfo)
{
 	SD_Error errorstatus=SD_OK;
	uint8_t tmp=0;	   
	cardinfo->CardType=(uint8_t)CardType; 				//������
	cardinfo->RCA=(uint16_t)RCA;							//��RCAֵ
	tmp=(uint8_t)((CSD_Tab[0]&0xFF000000)>>24);
	cardinfo->SD_csd.CSDStruct=(tmp&0xC0)>>6;		//CSD�ṹ
	cardinfo->SD_csd.SysSpecVersion=(tmp&0x3C)>>2;	//2.0Э�黹û�����ⲿ��(Ϊ����),Ӧ���Ǻ���Э�鶨���
	cardinfo->SD_csd.Reserved1=tmp&0x03;			//2������λ  
	tmp=(uint8_t)((CSD_Tab[0]&0x00FF0000)>>16);			//��1���ֽ�
	cardinfo->SD_csd.TAAC=tmp;				   		//���ݶ�ʱ��1
	tmp=(uint8_t)((CSD_Tab[0]&0x0000FF00)>>8);	  		//��2���ֽ�
	cardinfo->SD_csd.NSAC=tmp;		  				//���ݶ�ʱ��2
	tmp=(uint8_t)(CSD_Tab[0]&0x000000FF);				//��3���ֽ�
	cardinfo->SD_csd.MaxBusClkFrec=tmp;		  		//�����ٶ�	   
	tmp=(uint8_t)((CSD_Tab[1]&0xFF000000)>>24);			//��4���ֽ�
	cardinfo->SD_csd.CardComdClasses=tmp<<4;    	//��ָ�������λ
	tmp=(uint8_t)((CSD_Tab[1]&0x00FF0000)>>16);	 		//��5���ֽ�
	cardinfo->SD_csd.CardComdClasses|=(tmp&0xF0)>>4;//��ָ�������λ
	cardinfo->SD_csd.RdBlockLen=tmp&0x0F;	    	//����ȡ���ݳ���
	tmp=(uint8_t)((CSD_Tab[1]&0x0000FF00)>>8);			//��6���ֽ�
	cardinfo->SD_csd.PartBlockRead=(tmp&0x80)>>7;	//�����ֿ��
	cardinfo->SD_csd.WrBlockMisalign=(tmp&0x40)>>6;	//д���λ
	cardinfo->SD_csd.RdBlockMisalign=(tmp&0x20)>>5;	//�����λ
	cardinfo->SD_csd.DSRImpl=(tmp&0x10)>>4;
	cardinfo->SD_csd.Reserved2=0; 					//����
 	if((CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1)||(CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0)||(SDIO_MULTIMEDIA_CARD==CardType))//��׼1.1/2.0��/MMC��
	{
		cardinfo->SD_csd.DeviceSize=(tmp&0x03)<<10;	//C_SIZE(12λ)
	 	tmp=(uint8_t)(CSD_Tab[1]&0x000000FF); 			//��7���ֽ�	
		cardinfo->SD_csd.DeviceSize|=(tmp)<<2;
 		tmp=(uint8_t)((CSD_Tab[2]&0xFF000000)>>24);		//��8���ֽ�	
		cardinfo->SD_csd.DeviceSize|=(tmp&0xC0)>>6;
 		cardinfo->SD_csd.MaxRdCurrentVDDMin=(tmp&0x38)>>3;
		cardinfo->SD_csd.MaxRdCurrentVDDMax=(tmp&0x07);
 		tmp=(uint8_t)((CSD_Tab[2]&0x00FF0000)>>16);		//��9���ֽ�	
		cardinfo->SD_csd.MaxWrCurrentVDDMin=(tmp&0xE0)>>5;
		cardinfo->SD_csd.MaxWrCurrentVDDMax=(tmp&0x1C)>>2;
		cardinfo->SD_csd.DeviceSizeMul=(tmp&0x03)<<1;//C_SIZE_MULT
 		tmp=(uint8_t)((CSD_Tab[2]&0x0000FF00)>>8);	  	//��10���ֽ�	
		cardinfo->SD_csd.DeviceSizeMul|=(tmp&0x80)>>7;
 		cardinfo->CardCapacity=(cardinfo->SD_csd.DeviceSize+1);//���㿨����
		cardinfo->CardCapacity*=(1<<(cardinfo->SD_csd.DeviceSizeMul+2));
		cardinfo->CardBlockSize=1<<(cardinfo->SD_csd.RdBlockLen);//���С
		cardinfo->CardCapacity*=cardinfo->CardBlockSize;
	}else if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)	//��������
	{
 		tmp=(uint8_t)(CSD_Tab[1]&0x000000FF); 		//��7���ֽ�	
		cardinfo->SD_csd.DeviceSize=(tmp&0x3F)<<16;//C_SIZE
 		tmp=(uint8_t)((CSD_Tab[2]&0xFF000000)>>24); 	//��8���ֽ�	
 		cardinfo->SD_csd.DeviceSize|=(tmp<<8);
 		tmp=(uint8_t)((CSD_Tab[2]&0x00FF0000)>>16);	//��9���ֽ�	
 		cardinfo->SD_csd.DeviceSize|=(tmp);
 		tmp=(uint8_t)((CSD_Tab[2]&0x0000FF00)>>8); 	//��10���ֽ�	
 		cardinfo->CardCapacity=(long long)(cardinfo->SD_csd.DeviceSize+1)*512*1024;//���㿨����
		cardinfo->CardBlockSize=512; 			//���С�̶�Ϊ512�ֽ�
	}	  
	cardinfo->SD_csd.EraseGrSize=(tmp&0x40)>>6;
	cardinfo->SD_csd.EraseGrMul=(tmp&0x3F)<<1;	   
	tmp=(uint8_t)(CSD_Tab[2]&0x000000FF);			//��11���ֽ�	
	cardinfo->SD_csd.EraseGrMul|=(tmp&0x80)>>7;
	cardinfo->SD_csd.WrProtectGrSize=(tmp&0x7F);
 	tmp=(uint8_t)((CSD_Tab[3]&0xFF000000)>>24);		//��12���ֽ�	
	cardinfo->SD_csd.WrProtectGrEnable=(tmp&0x80)>>7;
	cardinfo->SD_csd.ManDeflECC=(tmp&0x60)>>5;
	cardinfo->SD_csd.WrSpeedFact=(tmp&0x1C)>>2;
	cardinfo->SD_csd.MaxWrBlockLen=(tmp&0x03)<<2;	 
	tmp=(uint8_t)((CSD_Tab[3]&0x00FF0000)>>16);		//��13���ֽ�
	cardinfo->SD_csd.MaxWrBlockLen|=(tmp&0xC0)>>6;
	cardinfo->SD_csd.WriteBlockPaPartial=(tmp&0x20)>>5;
	cardinfo->SD_csd.Reserved3=0;
	cardinfo->SD_csd.ContentProtectAppli=(tmp&0x01);  
	tmp=(uint8_t)((CSD_Tab[3]&0x0000FF00)>>8);		//��14���ֽ�
	cardinfo->SD_csd.FileFormatGrouop=(tmp&0x80)>>7;
	cardinfo->SD_csd.CopyFlag=(tmp&0x40)>>6;
	cardinfo->SD_csd.PermWrProtect=(tmp&0x20)>>5;
	cardinfo->SD_csd.TempWrProtect=(tmp&0x10)>>4;
	cardinfo->SD_csd.FileFormat=(tmp&0x0C)>>2;
	cardinfo->SD_csd.ECC=(tmp&0x03);  
	tmp=(uint8_t)(CSD_Tab[3]&0x000000FF);			//��15���ֽ�
	cardinfo->SD_csd.CSD_CRC=(tmp&0xFE)>>1;
	cardinfo->SD_csd.Reserved4=1;		 
	tmp=(uint8_t)((CID_Tab[0]&0xFF000000)>>24);		//��0���ֽ�
	cardinfo->SD_cid.ManufacturerID=tmp;		    
	tmp=(uint8_t)((CID_Tab[0]&0x00FF0000)>>16);		//��1���ֽ�
	cardinfo->SD_cid.OEM_AppliID=tmp<<8;	  
	tmp=(uint8_t)((CID_Tab[0]&0x000000FF00)>>8);		//��2���ֽ�
	cardinfo->SD_cid.OEM_AppliID|=tmp;	    
	tmp=(uint8_t)(CID_Tab[0]&0x000000FF);			//��3���ֽ�	
	cardinfo->SD_cid.ProdName1=tmp<<24;				  
	tmp=(uint8_t)((CID_Tab[1]&0xFF000000)>>24); 		//��4���ֽ�
	cardinfo->SD_cid.ProdName1|=tmp<<16;	  
	tmp=(uint8_t)((CID_Tab[1]&0x00FF0000)>>16);	   	//��5���ֽ�
	cardinfo->SD_cid.ProdName1|=tmp<<8;		 
	tmp=(uint8_t)((CID_Tab[1]&0x0000FF00)>>8);		//��6���ֽ�
	cardinfo->SD_cid.ProdName1|=tmp;		   
	tmp=(uint8_t)(CID_Tab[1]&0x000000FF);	  		//��7���ֽ�
	cardinfo->SD_cid.ProdName2=tmp;			  
	tmp=(uint8_t)((CID_Tab[2]&0xFF000000)>>24); 		//��8���ֽ�
	cardinfo->SD_cid.ProdRev=tmp;		 
	tmp=(uint8_t)((CID_Tab[2]&0x00FF0000)>>16);		//��9���ֽ�
	cardinfo->SD_cid.ProdSN=tmp<<24;	   
	tmp=(uint8_t)((CID_Tab[2]&0x0000FF00)>>8); 		//��10���ֽ�
	cardinfo->SD_cid.ProdSN|=tmp<<16;	   
	tmp=(uint8_t)(CID_Tab[2]&0x000000FF);   			//��11���ֽ�
	cardinfo->SD_cid.ProdSN|=tmp<<8;		   
	tmp=(uint8_t)((CID_Tab[3]&0xFF000000)>>24); 		//��12���ֽ�
	cardinfo->SD_cid.ProdSN|=tmp;			     
	tmp=(uint8_t)((CID_Tab[3]&0x00FF0000)>>16);	 	//��13���ֽ�
	cardinfo->SD_cid.Reserved1|=(tmp&0xF0)>>4;
	cardinfo->SD_cid.ManufactDate=(tmp&0x0F)<<8;    
	tmp=(uint8_t)((CID_Tab[3]&0x0000FF00)>>8);		//��14���ֽ�
	cardinfo->SD_cid.ManufactDate|=tmp;		 	  
	tmp=(uint8_t)(CID_Tab[3]&0x000000FF);			//��15���ֽ�
	cardinfo->SD_cid.CID_CRC=(tmp&0xFE)>>1;
	cardinfo->SD_cid.Reserved2=1;	 
	return errorstatus;
}

/*
    ����SDIO���߿���(MMC����֧��4bitģʽ)
    wmode:λ��ģʽ.0,1λ���ݿ���;1,4λ���ݿ���;2,8λ���ݿ���
    ����SDIO���߿���(MMC����֧��4bitģʽ)
    @arg SDIO_BusWide_8b: 8-bit data transfer (Only for MMC)
    @arg SDIO_BusWide_4b: 4-bit data transfer
    @arg SDIO_BusWide_1b: 1-bit data transfer (Ĭ��)
    ����ֵ:SD������״̬
 */
SD_Manager::SD_Error SD_Manager::SD_EnableWideBusOperation(uint32_t WideMode)
{
  	SD_Error errorstatus=SD_OK;
    if (SDIO_MULTIMEDIA_CARD == CardType)
    {
        errorstatus = SD_UNSUPPORTED_FEATURE;
        return(errorstatus);
    }
 	else if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
	{
		if (SDIO_BusWide_8b == WideMode)   //2.0 sd��֧��8bits
        {
            errorstatus = SD_UNSUPPORTED_FEATURE;
            return(errorstatus);
        }
 		else   
		{
			errorstatus=SDEnWideBus(WideMode);
 			if(SD_OK==errorstatus)
			{
				SDIO->CLKCR&=~(3<<11);		//���֮ǰ��λ������    
				SDIO->CLKCR|=WideMode;//1λ/4λ���߿��� 
				SDIO->CLKCR|=0<<14;			//������Ӳ��������
			}
		}  
	}
	return errorstatus; 
}
/* 
����SD������ģʽ
Mode:
����ֵ:����״̬ 
*/
SD_Manager::SD_Error SD_Manager::SD_SetDeviceMode(uint32_t Mode)
{
	SD_Error errorstatus = SD_OK;
 	if((Mode==SD_DMA_MODE)||(Mode==SD_POLLING_MODE))DeviceMode=Mode;
	else errorstatus=SD_INVALID_PARAMETER;
	return errorstatus;	    
}
/* 
ѡ��
����CMD7,ѡ����Ե�ַ(rca)Ϊaddr�Ŀ�,ȡ��������.���Ϊ0,�򶼲�ѡ��.
addr:����RCA��ַ 
*/
SD_Manager::SD_Error SD_Manager::SD_SelectDeselect(uint32_t addr)
{
    SDIO_CmdInitStructure.SDIO_Argument =  addr;//����CMD7,ѡ��,����Ӧ	
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEL_DESEL_CARD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);//����CMD7,ѡ��,����Ӧ
        
 	return CmdResp1Error(SD_CMD_SEL_DESEL_CARD);	  
}
/*
SD����ȡһ���� 
buf:�����ݻ�����(����4�ֽڶ���!!)
addr:��ȡ��ַ
blksize:���С 
*/
SD_Manager::SD_Error SD_Manager::SD_ReadBlock(uint8_t *buf,long long addr,uint16_t blksize)
{	  
	SD_Error errorstatus=SD_OK;
	uint8_t power;
    uint32_t count=0,*tempbuff=(uint32_t*)buf;//ת��Ϊuint32_tָ�� 
	uint32_t timeout=SDIO_DATATIMEOUT;   
    if(NULL==buf)
            return SD_INVALID_PARAMETER; 
    SDIO->DCTRL=0x0;	//���ݿ��ƼĴ�������(��DMA) 
  
	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//��������
	{
		blksize=512;
		addr>>=9;
	}   
  	SDIO_DataInitStructure.SDIO_DataBlockSize= SDIO_DataBlockSize_1b ;//���DPSM״̬������
	SDIO_DataInitStructure.SDIO_DataLength= 0 ;
	SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
	SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
	SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
	SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);
	
	
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//������
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	
	
		SDIO_CmdInitStructure.SDIO_Argument =  blksize;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);//����CMD16+�������ݳ���Ϊblksize,����Ӧ
		
		errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ 

		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����	
		
	}else return SD_INVALID_PARAMETER;	  	 
	
	SDIO_DataInitStructure.SDIO_DataBlockSize= power<<4 ;//���DPSM״̬������
	SDIO_DataInitStructure.SDIO_DataLength= blksize ;
	SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
	SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
	SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToSDIO;
	SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);
	
	SDIO_CmdInitStructure.SDIO_Argument =  addr;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_READ_SINGLE_BLOCK;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);//����CMD17+��addr��ַ����ȡ����,����Ӧ 
	
	errorstatus=CmdResp1Error(SD_CMD_READ_SINGLE_BLOCK);//�ȴ�R1��Ӧ   
	if(errorstatus!=SD_OK)return errorstatus;   		//��Ӧ����	 
 	if(DeviceMode==SD_POLLING_MODE)						//��ѯģʽ,��ѯ����	 
	{
 		_INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
		while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<10)|(1<<9))))//������/CRC/��ʱ/���(��־)/��ʼλ����
		{
			if(SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)						//����������,��ʾ���ٴ���8����
			{
				for(count=0;count<8;count++)			//ѭ����ȡ����
				{
					*(tempbuff+count)=SDIO->FIFO;
				}
				tempbuff+=8;	 
				timeout=0X7FFFFF; 	//���������ʱ��
			}else 	//������ʱ
			{
				if(timeout==0)return SD_DATA_TIMEOUT;
				timeout--;
			}
		} 
		if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		//���ݳ�ʱ����
		{										   
	 		SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	//������־
			return SD_DATA_TIMEOUT;
	 	}else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	//���ݿ�CRC����
		{
	 		SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		//������־
			return SD_DATA_CRC_FAIL;		   
		}else if(SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET) 	//����fifo�������
		{
	 		SDIO_ClearFlag(SDIO_FLAG_RXOVERR);		//������־
			return SD_RX_OVERRUN;		 
		}else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	//������ʼλ����
		{
	 		SDIO_ClearFlag(SDIO_FLAG_STBITERR);//������־
			return SD_START_BIT_ERR;		 
		}   
		while(SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)	//FIFO����,�����ڿ�������
		{
			*tempbuff=SDIO->FIFO;	//ѭ����ȡ����
			tempbuff++;
		}
		_INTX_ENABLE();//�������ж�
		SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
	 
	}else if(DeviceMode==SD_DMA_MODE)
	{
 		TransferError=SD_OK;
		StopCondition=0;			//�����,����Ҫ����ֹͣ����ָ��
		TransferEnd=0;				//�����������λ�����жϷ�����1
		SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<5)|(1<<9);	//������Ҫ���ж� 
	 	SDIO->DCTRL|=1<<3;		 	//SDIO DMAʹ�� 
 	    SD_DMA_Config((uint32_t*)buf,blksize,DMA_DIR_PeripheralToMemory); 
 		while(((DMA2->LISR&(1<<27))==RESET)&&(TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;//�ȴ�������� 
		if(timeout==0)return SD_DATA_TIMEOUT;//��ʱ
		if(TransferError!=SD_OK)errorstatus=TransferError;  
    }   
 	return errorstatus; 
}
/* 
SD����ȡ����� 
buf:�����ݻ�����
addr:��ȡ��ַ
blksize:���С
nblks:Ҫ��ȡ�Ŀ���
����ֵ:����״̬ 
*/
__align(4) uint32_t *tempbuff;
SD_Manager::SD_Error SD_Manager::SD_ReadMultiBlocks(uint8_t *buf,long long addr,uint16_t blksize,uint32_t nblks)
{
    SD_Error errorstatus=SD_OK;
	uint8_t power;
    uint32_t count=0;
	uint32_t timeout=SDIO_DATATIMEOUT;  
	tempbuff=(uint32_t*)buf;//ת��Ϊuint32_tָ��
	
     SDIO->DCTRL=0x0;		//���ݿ��ƼĴ�������(��DMA)   
	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//��������
	{
		blksize=512;
		addr>>=9;
	}  
	
	SDIO_DataInitStructure.SDIO_DataBlockSize= 0; ;//���DPSM״̬������
	SDIO_DataInitStructure.SDIO_DataLength= 0 ;
	SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
	SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
	SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
	SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);
	
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//������
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	    
		
	    SDIO_CmdInitStructure.SDIO_Argument =  blksize;//����CMD16+�������ݳ���Ϊblksize,����Ӧ 
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);
		
		errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ  
		
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����	 
		
	}else return SD_INVALID_PARAMETER;	  
	
	if(nblks>1)											//����  
	{									    
 	  	if(nblks*blksize>SD_MAX_DATA_LENGTH)return SD_INVALID_PARAMETER;//�ж��Ƿ񳬹������ճ��� 
		
		SDIO_DataInitStructure.SDIO_DataBlockSize= power<<4; ;//nblks*blksize,512���С,����������
		SDIO_DataInitStructure.SDIO_DataLength= nblks*blksize ;
		SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
		SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
		SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToSDIO;
		SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
		SDIO_DataConfig(&SDIO_DataInitStructure);

        SDIO_CmdInitStructure.SDIO_Argument =  addr;//����CMD18+��addr��ַ����ȡ����,����Ӧ 
	    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_READ_MULT_BLOCK;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);	
		
		errorstatus=CmdResp1Error(SD_CMD_READ_MULT_BLOCK);//�ȴ�R1��Ӧ 
		
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����	 
		
 		if(DeviceMode==SD_POLLING_MODE)
		{
			_INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
			while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<8)|(1<<9))))//������/CRC/��ʱ/���(��־)/��ʼλ����
			{
				if(SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)						//����������,��ʾ���ٴ���8����
				{
					for(count=0;count<8;count++)			//ѭ����ȡ����
					{
						*(tempbuff+count)=SDIO->FIFO;
					}
					tempbuff+=8;	 
					timeout=0X7FFFFF; 	//���������ʱ��
				}else 	//������ʱ
				{
					if(timeout==0)return SD_DATA_TIMEOUT;
					timeout--;
				}
			}  
		if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		//���ݳ�ʱ����
		{										   
	 		SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	//������־
			return SD_DATA_TIMEOUT;
	 	}else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	//���ݿ�CRC����
		{
	 		SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		//������־
			return SD_DATA_CRC_FAIL;		   
		}else if(SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET) 	//����fifo�������
		{
	 		SDIO_ClearFlag(SDIO_FLAG_RXOVERR);		//������־
			return SD_RX_OVERRUN;		 
		}else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	//������ʼλ����
		{
	 		SDIO_ClearFlag(SDIO_FLAG_STBITERR);//������־
			return SD_START_BIT_ERR;		 
		}   
	    
		while(SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)	//FIFO����,�����ڿ�������
		{
			*tempbuff=SDIO->FIFO;	//ѭ����ȡ����
			tempbuff++;
		}
	 		if(SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)		//���ս���
			{
				if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
				{				
					SDIO_CmdInitStructure.SDIO_Argument =  0;//����CMD12+��������
				    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
					SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
					SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
					SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
					SDIO_SendCommand(&SDIO_CmdInitStructure);	
					
					errorstatus=CmdResp1Error(SD_CMD_STOP_TRANSMISSION);//�ȴ�R1��Ӧ   
					
					if(errorstatus!=SD_OK)return errorstatus;	 
				}
 			}
			_INTX_ENABLE();//�������ж�
	 		SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
 		}else if(DeviceMode==SD_DMA_MODE)
		{
	   		TransferError=SD_OK;
			StopCondition=1;			//����,��Ҫ����ֹͣ����ָ�� 
			TransferEnd=0;				//�����������λ�����жϷ�����1
			SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<5)|(1<<9);	//������Ҫ���ж� 
		 	SDIO->DCTRL|=1<<3;		 						//SDIO DMAʹ�� 
	 	    SD_DMA_Config((uint32_t*)buf,nblks*blksize,DMA_DIR_PeripheralToMemory); 
	 		while(((DMA2->LISR&(1<<27))==RESET)&&timeout)timeout--;//�ȴ�������� 
			if(timeout==0)return SD_DATA_TIMEOUT;//��ʱ
			while((TransferEnd==0)&&(TransferError==SD_OK)); 
			if(TransferError!=SD_OK)errorstatus=TransferError;  	 
		}		 
  	}
	return errorstatus;
}			    																  
/*
 SD��д1���� 
buf:���ݻ�����
addr:д��ַ
blksize:���С	  
����ֵ:����״̬ 
*/
SD_Manager::SD_Error SD_Manager::SD_WriteBlock(uint8_t *buf,long long addr,  uint16_t blksize)
{
	SD_Error errorstatus = SD_OK;
	
	uint8_t  power=0,cardstate=0;
	
	uint32_t timeout=0,bytestransferred=0;
	
	uint32_t cardstatus=0,count=0,restwords=0;
	
	uint32_t tlen=blksize;						//�ܳ���(�ֽ�)
	
	uint32_t*tempbuff=(uint32_t*)buf;					
	
 	if(buf==NULL)return SD_INVALID_PARAMETER;//��������  
	
    SDIO->DCTRL=0x0;							//���ݿ��ƼĴ�������(��DMA)
	
	SDIO_DataInitStructure.SDIO_DataBlockSize= 0; ;//���DPSM״̬������
	SDIO_DataInitStructure.SDIO_DataLength= 0 ;
	SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
	SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
	SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
	SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);
	
	
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//������
 	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)	//��������
	{
		blksize=512;
		addr>>=9;
	}    
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	
		
		SDIO_CmdInitStructure.SDIO_Argument = blksize;//����CMD16+�������ݳ���Ϊblksize,����Ӧ 	
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);	
		
		errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ  
		
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����	 
		
	}else return SD_INVALID_PARAMETER;	
	
	SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)RCA<<16;//����CMD13,��ѯ����״̬,����Ӧ 	
	SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
	SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
	SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStructure);	

	errorstatus=CmdResp1Error(SD_CMD_SEND_STATUS);		//�ȴ�R1��Ӧ  
	
	if(errorstatus!=SD_OK)return errorstatus;
	cardstatus=SDIO->RESP1;													  
	timeout=SD_DATATIMEOUT;
   	while(((cardstatus&0x00000100)==0)&&(timeout>0)) 	//���READY_FOR_DATAλ�Ƿ���λ
	{
		timeout--;  
		
		SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)RCA<<16;//����CMD13,��ѯ����״̬,����Ӧ
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);	
		
		errorstatus=CmdResp1Error(SD_CMD_SEND_STATUS);	//�ȴ�R1��Ӧ   
		
		if(errorstatus!=SD_OK)return errorstatus;		
		
		cardstatus=SDIO->RESP1;													  
	}
	if(timeout==0)return SD_ERROR;

	SDIO_CmdInitStructure.SDIO_Argument = addr;//����CMD24,д����ָ��,����Ӧ 	
	SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_SINGLE_BLOCK;
	SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
	SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
	SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
	SDIO_SendCommand(&SDIO_CmdInitStructure);	
	
	errorstatus=CmdResp1Error(SD_CMD_WRITE_SINGLE_BLOCK);//�ȴ�R1��Ӧ  
	
	if(errorstatus!=SD_OK)return errorstatus;   	 
	
	StopCondition=0;									//����д,����Ҫ����ֹͣ����ָ�� 

	SDIO_DataInitStructure.SDIO_DataBlockSize= power<<4; ;	//blksize, ����������	
	SDIO_DataInitStructure.SDIO_DataLength= blksize ;
	SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
	SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
	SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
	SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);
	
	
	timeout=SDIO_DATATIMEOUT;
	
	if (DeviceMode == SD_POLLING_MODE)
	{
		_INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
		while(!(SDIO->STA&((1<<10)|(1<<4)|(1<<1)|(1<<3)|(1<<9))))//���ݿ鷢�ͳɹ�/����/CRC/��ʱ/��ʼλ����
		{
			if(SDIO_GetFlagStatus(SDIO_FLAG_TXFIFOHE) != RESET)							//���������,��ʾ���ٴ���8����
			{
				if((tlen-bytestransferred)<SD_HALFFIFOBYTES)//����32�ֽ���
				{
					restwords=((tlen-bytestransferred)%4==0)?((tlen-bytestransferred)/4):((tlen-bytestransferred)/4+1);
					
					for(count=0;count<restwords;count++,tempbuff++,bytestransferred+=4)
					{
						SDIO->FIFO=*tempbuff;
					}
				}else
				{
					for(count=0;count<8;count++)
					{
						SDIO->FIFO=*(tempbuff+count);
					}
					tempbuff+=8;
					bytestransferred+=32;
				}
				timeout=0X3FFFFFFF;	//д�������ʱ��
			}else
			{
				if(timeout==0)return SD_DATA_TIMEOUT;
				timeout--;
			}
		} 
		if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		//���ݳ�ʱ����
		{										   
	 		SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	//������־
			return SD_DATA_TIMEOUT;
	 	}else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	//���ݿ�CRC����
		{
	 		SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		//������־
			return SD_DATA_CRC_FAIL;		   
		}else if(SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET) 	//����fifo�������
		{
	 		SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);		//������־
			return SD_TX_UNDERRUN;		 
		}else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	//������ʼλ����
		{
	 		SDIO_ClearFlag(SDIO_FLAG_STBITERR);//������־
			return SD_START_BIT_ERR;		 
		}   
	      
		_INTX_ENABLE();//�������ж�
		SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��  
	}else if(DeviceMode==SD_DMA_MODE)
	{
   		TransferError=SD_OK;
		StopCondition=0;			//����д,����Ҫ����ֹͣ����ָ�� 
		TransferEnd=0;				//�����������λ�����жϷ�����1
		SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<4)|(1<<9);	//���ò������ݽ�������ж�
		SD_DMA_Config((uint32_t*)buf,blksize,DMA_DIR_MemoryToPeripheral);				//SDIO DMA����
 	 	SDIO->DCTRL|=1<<3;								//SDIO DMAʹ��.  
 		while(((DMA2->LISR&(1<<27))==RESET)&&timeout)timeout--;//�ȴ�������� 
		if(timeout==0)
		{
  			SD_Init();	 					//���³�ʼ��SD��,���Խ��д������������
			return SD_DATA_TIMEOUT;			//��ʱ	 
 		}
		timeout=SDIO_DATATIMEOUT;
		while((TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;
 		if(timeout==0)return SD_DATA_TIMEOUT;			//��ʱ	 
  		if(TransferError!=SD_OK)return TransferError;
 	}  
 	SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
 	errorstatus=IsCardProgramming(&cardstate);
 	while((errorstatus==SD_OK)&&((cardstate==SD_CARD_PROGRAMMING)||(cardstate==SD_CARD_RECEIVING)))
	{
		errorstatus=IsCardProgramming(&cardstate);
	}   
	return errorstatus;
}
/* 
SD��д����� 
buf:���ݻ�����
addr:д��ַ
blksize:���С
nblks:Ҫд��Ŀ���
����ֵ:����״̬	 
*/											   
SD_Manager::SD_Error SD_Manager::SD_WriteMultiBlocks(uint8_t *buf,long long addr,uint16_t blksize,uint32_t nblks)
{
	SD_Error errorstatus = SD_OK;
	uint8_t  power = 0, cardstate = 0;
	uint32_t timeout=0,bytestransferred=0;
	uint32_t count = 0, restwords = 0;
	uint32_t tlen=nblks*blksize;				//�ܳ���(�ֽ�)
	uint32_t *tempbuff = (uint32_t*)buf;  
    if(buf==NULL)return SD_INVALID_PARAMETER; //��������  
    SDIO->DCTRL=0x0;							//���ݿ��ƼĴ�������(��DMA)   
	
	SDIO_DataInitStructure.SDIO_DataBlockSize= 0; ;	//���DPSM״̬������	
	SDIO_DataInitStructure.SDIO_DataLength= 0 ;
	SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
	SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
	SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
	SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);
	
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//������
 	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//��������
	{
		blksize=512;
		addr>>=9;
	}    
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);
		
		SDIO_CmdInitStructure.SDIO_Argument = blksize;	//����CMD16+�������ݳ���Ϊblksize,����Ӧ
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);	
		
		errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ  
		
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����	 
		
	}else return SD_INVALID_PARAMETER;	 
	if(nblks>1)
	{					  
		if(nblks*blksize>SD_MAX_DATA_LENGTH)return SD_INVALID_PARAMETER;   
     	if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
    	{
			//�������
			SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)RCA<<16;		//����ACMD55,����Ӧ 	
			SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
			SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
			SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
			SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
			SDIO_SendCommand(&SDIO_CmdInitStructure);	
				
			errorstatus=CmdResp1Error(SD_CMD_APP_CMD);		//�ȴ�R1��Ӧ 
				
			if(errorstatus!=SD_OK)return errorstatus;				 
				
			SDIO_CmdInitStructure.SDIO_Argument =nblks;		//����CMD23,���ÿ�����,����Ӧ 	 
			SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCK_COUNT;
			SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
			SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
			SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
			SDIO_SendCommand(&SDIO_CmdInitStructure);
			
			errorstatus=CmdResp1Error(SD_CMD_SET_BLOCK_COUNT);//�ȴ�R1��Ӧ 
				
			if(errorstatus!=SD_OK)return errorstatus;		
		    
		} 

		SDIO_CmdInitStructure.SDIO_Argument =addr;	//����CMD25,���дָ��,����Ӧ 	  
		SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_MULT_BLOCK;
		SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
		SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
		SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
		SDIO_SendCommand(&SDIO_CmdInitStructure);	

 		errorstatus=CmdResp1Error(SD_CMD_WRITE_MULT_BLOCK);	//�ȴ�R1��Ӧ   		   
	
		if(errorstatus!=SD_OK)return errorstatus;

        SDIO_DataInitStructure.SDIO_DataBlockSize= power<<4; ;	//blksize, ����������	
		SDIO_DataInitStructure.SDIO_DataLength= nblks*blksize ;
		SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
		SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
		SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
		SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
		SDIO_DataConfig(&SDIO_DataInitStructure);
				
		if(DeviceMode==SD_POLLING_MODE)
	    {
			timeout=SDIO_DATATIMEOUT;
			_INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
			while(!(SDIO->STA&((1<<4)|(1<<1)|(1<<8)|(1<<3)|(1<<9))))//����/CRC/���ݽ���/��ʱ/��ʼλ����
			{
				if(SDIO_GetFlagStatus(SDIO_FLAG_TXFIFOHE) != RESET)							//���������,��ʾ���ٴ���8��(32�ֽ�)
				{	  
					if((tlen-bytestransferred)<SD_HALFFIFOBYTES)//����32�ֽ���
					{
						restwords=((tlen-bytestransferred)%4==0)?((tlen-bytestransferred)/4):((tlen-bytestransferred)/4+1);
						for(count=0;count<restwords;count++,tempbuff++,bytestransferred+=4)
						{
							SDIO->FIFO=*tempbuff;
						}
					}else 										//���������,���Է�������8��(32�ֽ�)����
					{
						for(count=0;count<SD_HALFFIFO;count++)
						{
							SDIO->FIFO=*(tempbuff+count);
						}
						tempbuff+=SD_HALFFIFO;
						bytestransferred+=SD_HALFFIFOBYTES;
					}
					timeout=0X3FFFFFFF;	//д�������ʱ��
				}else
				{
					if(timeout==0)return SD_DATA_TIMEOUT; 
					timeout--;
				}
			} 
		if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		//���ݳ�ʱ����
		{										   
	 		SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	//������־
			return SD_DATA_TIMEOUT;
	 	}else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	//���ݿ�CRC����
		{
	 		SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		//������־
			return SD_DATA_CRC_FAIL;		   
		}else if(SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET) 	//����fifo�������
		{
	 		SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);		//������־
			return SD_TX_UNDERRUN;		 
		}else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	//������ʼλ����
		{
	 		SDIO_ClearFlag(SDIO_FLAG_STBITERR);//������־
			return SD_START_BIT_ERR;		 
		}   
	      										   
			if(SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)		//���ͽ���
			{															 
				if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
				{   
					SDIO_CmdInitStructure.SDIO_Argument =0;//����CMD12+�������� 	  
					SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
					SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
					SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
					SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
					SDIO_SendCommand(&SDIO_CmdInitStructure);	
					
					errorstatus=CmdResp1Error(SD_CMD_STOP_TRANSMISSION);//�ȴ�R1��Ӧ   
					if(errorstatus!=SD_OK)return errorstatus;	 
				}
			}
			_INTX_ENABLE();//�������ж�
	 		SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
	    }else if(DeviceMode==SD_DMA_MODE)
		{
	   	    TransferError=SD_OK;
			StopCondition=1;			//���д,��Ҫ����ֹͣ����ָ�� 
			TransferEnd=0;				//�����������λ�����жϷ�����1
			SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<4)|(1<<9);	//���ò������ݽ�������ж�
			SD_DMA_Config((uint32_t*)buf,nblks*blksize,DMA_DIR_MemoryToPeripheral);		//SDIO DMA����
	 	 	SDIO->DCTRL|=1<<3;								//SDIO DMAʹ��. 
			timeout=SDIO_DATATIMEOUT;
	 		while(((DMA2->LISR&(1<<27))==RESET)&&timeout)timeout--;//�ȴ�������� 
			if(timeout==0)	 								//��ʱ
			{									  
  				SD_Init();	 					//���³�ʼ��SD��,���Խ��д������������
	 			return SD_DATA_TIMEOUT;			//��ʱ	 
	 		}
			timeout=SDIO_DATATIMEOUT;
			while((TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;
	 		if(timeout==0)return SD_DATA_TIMEOUT;			//��ʱ	 
	 		if(TransferError!=SD_OK)return TransferError;	 
		}
  	}
 	SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
 	errorstatus=IsCardProgramming(&cardstate);
 	while((errorstatus==SD_OK)&&((cardstate==SD_CARD_PROGRAMMING)||(cardstate==SD_CARD_RECEIVING)))
	{
		errorstatus=IsCardProgramming(&cardstate);
	}   
	return errorstatus;	   
}

extern "C"{

    //SDIO�жϷ�����		  
    void SDIO_IRQHandler(void) 
    {		
        HAL::SD.SD_ProcessIRQSrc();//��������SDIO����ж�
    }	    
}
 																    
/*
 SDIO�жϴ�������
����SDIO��������еĸ����ж�����
����ֵ:������� 
*/
SD_Manager::SD_Error SD_Manager::SD_ProcessIRQSrc(void)
{
	if(SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)//��������ж�
	{	 
		if (StopCondition==1)
		{  
			SDIO_CmdInitStructure.SDIO_Argument =0;//����CMD12+�������� 	  
			SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
			SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
			SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
			SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
			SDIO_SendCommand(&SDIO_CmdInitStructure);	
					
			TransferError=CmdResp1Error(SD_CMD_STOP_TRANSMISSION);
		}else TransferError = SD_OK;	
 		SDIO->ICR|=1<<8;//�������жϱ��
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
 		TransferEnd = 1;
		return(TransferError);
	}
 	if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)//����CRC����
	{
		SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		//������־
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
	    TransferError = SD_DATA_CRC_FAIL;
	    return(SD_DATA_CRC_FAIL);
	}
 	if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)//���ݳ�ʱ����
	{
		SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);  			//���жϱ�־
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
	    TransferError = SD_DATA_TIMEOUT;
	    return(SD_DATA_TIMEOUT);
	}
  	if(SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)//FIFO�������
	{
		SDIO_ClearFlag(SDIO_FLAG_RXOVERR);  			//���жϱ�־
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
	    TransferError = SD_RX_OVERRUN;
	    return(SD_RX_OVERRUN);
	}
   	if(SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET)//FIFO�������
	{
		SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);  			//���жϱ�־
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
	    TransferError = SD_TX_UNDERRUN;
	    return(SD_TX_UNDERRUN);
	}
	if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)//��ʼλ����
	{
		SDIO_ClearFlag(SDIO_FLAG_STBITERR);  		//���жϱ�־
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
	    TransferError = SD_START_BIT_ERR;
	    return(SD_START_BIT_ERR);
	}
	return(SD_OK);
}
  
/* 
���CMD0��ִ��״̬
����ֵ:sd�������� 
*/
SD_Manager::SD_Error SD_Manager::CmdError(void)
{
	SD_Error errorstatus = SD_OK;
	uint32_t timeout=SDIO_CMD0TIMEOUT;	   
	while(timeout--)
	{
		if(SDIO_GetFlagStatus(SDIO_FLAG_CMDSENT) != RESET)break;	//�����ѷ���(������Ӧ)	 
	}	    
	if(timeout==0)return SD_CMD_RSP_TIMEOUT;  
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
	return errorstatus;
}	 
/*
 ���R7��Ӧ�Ĵ���״̬
����ֵ:sd�������� 
*/
SD_Manager::SD_Error SD_Manager::CmdResp7Error(void)
{
	SD_Error errorstatus=SD_OK;
	uint32_t status;
	uint32_t timeout=SDIO_CMD0TIMEOUT;
 	while(timeout--)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)	
	}
 	if((timeout==0)||(status&(1<<2)))	//��Ӧ��ʱ
	{																				    
		errorstatus=SD_CMD_RSP_TIMEOUT;	//��ǰ������2.0���ݿ�,���߲�֧���趨�ĵ�ѹ��Χ
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT); 			//���������Ӧ��ʱ��־
		return errorstatus;
	}	 
	if(status&1<<6)						//�ɹ����յ���Ӧ
	{								   
		errorstatus=SD_OK;
		SDIO_ClearFlag(SDIO_FLAG_CMDREND); 				//�����Ӧ��־
 	}
	return errorstatus;
}	   
/* 
���R1��Ӧ�Ĵ���״̬
cmd:��ǰ����
����ֵ:sd�������� 
*/
SD_Manager::SD_Error SD_Manager::CmdResp1Error(uint8_t cmd)
{	  
   	uint32_t status; 
	while(1)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)
	} 
	if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)					//��Ӧ��ʱ
	{																				    
 		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT); 				//���������Ӧ��ʱ��־
		return SD_CMD_RSP_TIMEOUT;
	}	
 	if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)					//CRC����
	{																				    
 		SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL); 				//�����־
		return SD_CMD_CRC_FAIL;
	}		
	if(SDIO->RESPCMD!=cmd)return SD_ILLEGAL_CMD;//���ƥ�� 
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
	return (SD_Error)(SDIO->RESP1&SD_OCR_ERRORBITS);//���ؿ���Ӧ
}
/* 
���R3��Ӧ�Ĵ���״̬
����ֵ:����״̬ 
*/
SD_Manager::SD_Error SD_Manager::CmdResp3Error(void)
{
	uint32_t status;						 
 	while(1)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)	
	}
 	if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)					//��Ӧ��ʱ
	{											 
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);			//���������Ӧ��ʱ��־
		return SD_CMD_RSP_TIMEOUT;
	}	 
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
 	return SD_OK;								  
}
/* 
���R2��Ӧ�Ĵ���״̬
����ֵ:����״̬ 
*/
SD_Manager::SD_Error SD_Manager::CmdResp2Error(void)
{
	SD_Error errorstatus=SD_OK;
	uint32_t status;
	uint32_t timeout=SDIO_CMD0TIMEOUT;
 	while(timeout--)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)	
	}
  	if((timeout==0)||(status&(1<<2)))	//��Ӧ��ʱ
	{																				    
		errorstatus=SD_CMD_RSP_TIMEOUT; 
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT); 		//���������Ӧ��ʱ��־
		return errorstatus;
	}	 
	if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)						//CRC����
	{								   
		errorstatus=SD_CMD_CRC_FAIL;
		SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);		//�����Ӧ��־
 	}
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
 	return errorstatus;								    		 
} 
/* 
���R6��Ӧ�Ĵ���״̬
cmd:֮ǰ���͵�����
prca:�����ص�RCA��ַ
����ֵ:����״̬ 
*/
SD_Manager::SD_Error SD_Manager::CmdResp6Error(uint8_t cmd,uint16_t*prca)
{
	SD_Error errorstatus=SD_OK;
	uint32_t status;					    
	uint32_t rspr1;
 	while(1)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)	
	}
	if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)					//��Ӧ��ʱ
	{																				    
 		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);			//���������Ӧ��ʱ��־
		return SD_CMD_RSP_TIMEOUT;
	}	 	 
	if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)						//CRC����
	{								   
		SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);					//�����Ӧ��־
 		return SD_CMD_CRC_FAIL;
	}
	if(SDIO->RESPCMD!=cmd)				//�ж��Ƿ���Ӧcmd����
	{
 		return SD_ILLEGAL_CMD; 		
	}	    
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
	rspr1=SDIO->RESP1;					//�õ���Ӧ 	 
	if(SD_ALLZERO==(rspr1&(SD_R6_GENERAL_UNKNOWN_ERROR|SD_R6_ILLEGAL_CMD|SD_R6_COM_CRC_FAILED)))
	{
		*prca=(uint16_t)(rspr1>>16);			//����16λ�õ�,rca
		return errorstatus;
	}
   	if(rspr1&SD_R6_GENERAL_UNKNOWN_ERROR)return SD_GENERAL_UNKNOWN_ERROR;
   	if(rspr1&SD_R6_ILLEGAL_CMD)return SD_ILLEGAL_CMD;
   	if(rspr1&SD_R6_COM_CRC_FAILED)return SD_COM_CRC_FAILED;
	return errorstatus;
}

/*
 SDIOʹ�ܿ�����ģʽ
enx:0,��ʹ��;1,ʹ��;
����ֵ:����״̬
 */
SD_Manager::SD_Error SD_Manager::SDEnWideBus(uint8_t enx)
{
	SD_Error errorstatus = SD_OK;
 	uint32_t scr[2]={0,0};
	uint8_t arg=0X00;
	if(enx)arg=0X02;
	else arg=0X00;
 	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//SD������LOCKED״̬		    
 	errorstatus=FindSCR(RCA,scr);						//�õ�SCR�Ĵ�������
 	if(errorstatus!=SD_OK)return errorstatus;
	if((scr[1]&SD_WIDE_BUS_SUPPORT)!=SD_ALLZERO)		//֧�ֿ�����
	{
		SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16;//����CMD55+RCA,����Ӧ	
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);
		
	 	errorstatus=CmdResp1Error(SD_CMD_APP_CMD);
		
	 	if(errorstatus!=SD_OK)return errorstatus; 
		
		SDIO_CmdInitStructure.SDIO_Argument = arg;//����ACMD6,����Ӧ,����:10,4λ;00,1λ.	
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_SD_SET_BUSWIDTH;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);
			
        errorstatus=CmdResp1Error(SD_CMD_APP_SD_SET_BUSWIDTH);
		
		return errorstatus;
	}else return SD_REQUEST_NOT_APPLICABLE;				//��֧�ֿ��������� 	 
}												   
/* 
��鿨�Ƿ�����ִ��д����
pstatus:��ǰ״̬.
����ֵ:������� 
*/
SD_Manager::SD_Error SD_Manager::IsCardProgramming(uint8_t *pstatus)
{
 	volatile uint32_t respR1 = 0, status = 0;  
    
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16; //����Ե�ַ����
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;//����CMD13 	
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);	
 	
	status=SDIO->STA;
	
	while(!(status&((1<<0)|(1<<6)|(1<<2))))status=SDIO->STA;//�ȴ��������
   	if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)			//CRC���ʧ��
	{  
	  SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);	//���������
		return SD_CMD_CRC_FAIL;
	}
   	if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)			//���ʱ 
	{
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);			//���������
		return SD_CMD_RSP_TIMEOUT;
	}
 	if(SDIO->RESPCMD!=SD_CMD_SEND_STATUS)return SD_ILLEGAL_CMD;
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
	respR1=SDIO->RESP1;
	*pstatus=(uint8_t)((respR1>>9)&0x0000000F);
	return SD_OK;
}
/* 
��ȡ��ǰ��״̬
pcardstatus:��״̬
����ֵ:�������
 */
SD_Manager::SD_Error SD_Manager::SD_SendStatus(uint32_t *pcardstatus)
{
	SD_Error errorstatus = SD_OK;
	if(pcardstatus==NULL)
	{
		errorstatus=SD_INVALID_PARAMETER;
		return errorstatus;
	}
	
	SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16;//����CMD13,����Ӧ		 
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);	
	
	errorstatus=CmdResp1Error(SD_CMD_SEND_STATUS);	//��ѯ��Ӧ״̬ 
	if(errorstatus!=SD_OK)return errorstatus;
	*pcardstatus=SDIO->RESP1;//��ȡ��Ӧֵ
	return errorstatus;
} 
/* 
����SD����״̬
����ֵ:SD��״̬ 
*/
SD_Manager::SDCardState SD_Manager::SD_GetState(void)
{
	uint32_t resp1=0;
	if(SD_SendStatus(&resp1)!=SD_OK)return SD_CARD_ERROR;
	else return (SDCardState)((resp1>>9) & 0x0F);
}
/* 
����SD����SCR�Ĵ���ֵ
rca:����Ե�ַ
pscr:���ݻ�����(�洢SCR����)
����ֵ:����״̬	 
*/	   
SD_Manager::SD_Error SD_Manager::FindSCR(uint16_t rca,uint32_t *pscr)
{ 
	uint32_t index = 0; 
	SD_Error errorstatus = SD_OK;
	uint32_t tempscr[2]={0,0};  
	
	SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)8;	 //����CMD16,����Ӧ,����Block SizeΪ8�ֽ�	
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN; //	 cmd16
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  //r1
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
	
 	errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);
	
 	if(errorstatus!=SD_OK)return errorstatus;	 
	
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16; 
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;//����CMD55,����Ӧ 	
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
	
 	errorstatus=CmdResp1Error(SD_CMD_APP_CMD);
 	if(errorstatus!=SD_OK)return errorstatus;
	
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DataLength = 8;  //8���ֽڳ���,blockΪ8�ֽ�,SD����SDIO.
    SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_8b  ;  //���С8byte 
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataConfig(&SDIO_DataInitStructure);		

    SDIO_CmdInitStructure.SDIO_Argument = 0x0;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_SEND_SCR;	//����ACMD51,����Ӧ,����Ϊ0	
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  //r1
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
	
 	errorstatus=CmdResp1Error(SD_CMD_SD_APP_SEND_SCR);
 	if(errorstatus!=SD_OK)return errorstatus;							   
 	while(!(SDIO->STA&(SDIO_FLAG_RXOVERR|SDIO_FLAG_DCRCFAIL|SDIO_FLAG_DTIMEOUT|SDIO_FLAG_DBCKEND|SDIO_FLAG_STBITERR)))
	{ 
		if(SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)//����FIFO���ݿ���
		{
			*(tempscr+index)=SDIO->FIFO;	//��ȡFIFO����
			index++;
			if(index>=2)break;
		}
	}
	if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		//���ݳ�ʱ����
	{										   
		SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	//������־
		return SD_DATA_TIMEOUT;
	}else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	//���ݿ�CRC����
	{
		SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		//������־
		return SD_DATA_CRC_FAIL;		   
	}else if(SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET) 	//����fifo�������
	{
		SDIO_ClearFlag(SDIO_FLAG_RXOVERR);		//������־
		return SD_RX_OVERRUN;		 
	}else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	//������ʼλ����
	{
		SDIO_ClearFlag(SDIO_FLAG_STBITERR);//������־
		return SD_START_BIT_ERR;		 
	}  
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
	//������˳��8λΪ��λ������.   	
	*(pscr+1)=((tempscr[0]&SD_0TO7BITS)<<24)|((tempscr[0]&SD_8TO15BITS)<<8)|((tempscr[0]&SD_16TO23BITS)>>8)|((tempscr[0]&SD_24TO31BITS)>>24);
	*(pscr)=((tempscr[1]&SD_0TO7BITS)<<24)|((tempscr[1]&SD_8TO15BITS)<<8)|((tempscr[1]&SD_16TO23BITS)>>8)|((tempscr[1]&SD_24TO31BITS)>>24);
 	return errorstatus;
}
/* 
�õ�NumberOfBytes��2Ϊ�׵�ָ��.
NumberOfBytes:�ֽ���.
����ֵ:��2Ϊ�׵�ָ��ֵ 
*/
uint8_t SD_Manager::convert_from_bytes_to_power_of_two(uint16_t NumberOfBytes)
{
	uint8_t count=0;
	while(NumberOfBytes!=1)
	{
		NumberOfBytes>>=1;
		count++;
	}
	return count;
} 	 

/* ����SDIO DMA  
mbuf:�洢����ַ
bufsize:����������
dir:����;DMA_DIR_MemoryToPeripheral  �洢��-->SDIO(д����);DMA_DIR_PeripheralToMemory SDIO-->�洢��(������);
 */
void SD_Manager::SD_DMA_Config(uint32_t*mbuf,uint32_t bufsize,uint32_t dir)
{		 
    DMA_InitTypeDef  DMA_InitStructure;
	while (DMA_GetCmdStatus(DMA2_Stream3) != DISABLE){}//�ȴ�DMA������ 
    DMA_DeInit(DMA2_Stream3);//���֮ǰ��stream3�ϵ������жϱ�־
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //ͨ��ѡ��
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SDIO->FIFO;//DMA�����ַ
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)mbuf;//DMA �洢��0��ַ
    DMA_InitStructure.DMA_DIR = dir;//�洢��������ģʽ
    DMA_InitStructure.DMA_BufferSize = 0;//���ݴ����� 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;//�������ݳ���:32λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;//�洢�����ݳ���:32λ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// ʹ����ͨģʽ 
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//������ȼ�
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;   //FIFOʹ��      
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;//ȫFIFO
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;//����ͻ��4�δ���
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;//�洢��ͻ��4�δ���
    DMA_Init(DMA2_Stream3, &DMA_InitStructure);//��ʼ��DMA Stream

	DMA_FlowControllerConfig(DMA2_Stream3,DMA_FlowCtrl_Peripheral);//���������� 
	 
    DMA_Cmd(DMA2_Stream3 ,ENABLE);//����DMA����	 

}   


/* 
��SD��
buf:�����ݻ�����
sector:������ַ
cnt:��������	
����ֵ:����״̬;0,����;����,�������;	
 */			  				 
uint8_t SD_Manager::SD_ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
	uint8_t sta=SD_OK;
	long long lsector=sector;
	uint8_t n;
	lsector<<=9;
	if((uint32_t)buf%4!=0)
	{
	 	for(n=0;n<cnt;n++)
		{
		 	sta=SD_ReadBlock(SDIO_DATA_BUFFER,lsector+512*n,512);//����sector�Ķ�����
			memcpy(buf,SDIO_DATA_BUFFER,512);
			buf+=512;
		} 
	}else
	{
		if(cnt==1)sta=SD_ReadBlock(buf,lsector,512);    	//����sector�Ķ�����
		else sta=SD_ReadMultiBlocks(buf,lsector,512,cnt);//���sector  
	}
	return sta;
}
/* 
дSD��
buf:д���ݻ�����
sector:������ַ
cnt:��������	
����ֵ:����״̬;0,����;����,�������;	
 */
uint8_t SD_Manager::SD_WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
	uint8_t sta=SD_OK;
	uint8_t n;
	long long lsector=sector;
	lsector<<=9;
	if((uint32_t)buf%4!=0)
	{
	 	for(n=0;n<cnt;n++)
		{
			memcpy(SDIO_DATA_BUFFER,buf,512);
		 	sta=SD_WriteBlock(SDIO_DATA_BUFFER,lsector+512*n,512);//����sector��д����
			buf+=512;
		} 
	}else
	{
		if(cnt==1)sta=SD_WriteBlock(buf,lsector,512);    	//����sector��д����
		else sta=SD_WriteMultiBlocks(buf,lsector,512,cnt);	//���sector  
	}
	return sta;
}




