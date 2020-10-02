/**
 * \file
 *
 * \brief Demo user application
 * 
 * This Demo user application send the data periodically for every 10 sec
 * with a payload of 4 Bytes. Two Bands are given in the application (EU and IND).
 * The MACRO section and the STATIC VARIABLES section can be configured as per
 * the user configurations.
 *
 * \Default Default configurations in the Demo user application
 * Band				 - IND
 * Activation Method - OTAA
 * Transmission Type - UNCONFIRMED
 * Device Class		 - CLASS A
 * Data Rate		 - DR3
 * FPORT			 - 1
 * Payload Size		 - 4 Bytes
 * Uplink interval	 - 10 Sec
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

/****************************** INCLUDES **************************************/
#include "asf.h"
#include "radio_driver_hal.h"
#include "radio_interface.h"
#include "lorawan.h"
#include "sys.h"
#include "system_init.h"
#include "sio2host.h"
#include "extint.h"
#include "sw_timer.h"
#include "conf_sio2host.h"
#include "system_task_manager.h"
#include "sal.h"

/******************************** MACROS ***************************************/
#define BAND_CHOICE						  2		// For EU_BAND = 1, IND_BAND = 2
#define DEMO_APP_TIMEOUT				  10000 // Delay time in milliseconds for sending uplink packet

/* Activation method constants */
#define OVER_THE_AIR_ACTIVATION           LORAWAN_OTAA
#define ACTIVATION_BY_PERSONALIZATION     LORAWAN_ABP

/* Message Type constants */
#define UNCONFIRMED                       LORAWAN_UNCNF
#define CONFIRMED                         LORAWAN_CNF

/* Enable one of the activation methods */
#define DEMO_APP_ACTIVATION_TYPE               OVER_THE_AIR_ACTIVATION
//#define DEMO_APP_ACTIVATION_TYPE               ACTIVATION_BY_PERSONALIZATION

/* Select the Type of Transmission - Confirmed(CNF) / Unconfirmed(UNCNF) */
#define DEMO_APP_TRANSMISSION_TYPE              UNCONFIRMED
//#define DEMO_APP_TRANSMISSION_TYPE            CONFIRMED

/* FPORT Value (1-255) */
#define DEMO_APP_FPORT                           1

/* Device Class - Class of the device (CLASS_A/CLASS_C) */
#define DEMO_APP_ENDDEVICE_CLASS                 CLASS_A
//#define DEMO_APP_ENDDEVICE_CLASS                 CLASS_C

#define PA_ENABLE 1
/************************** STATIC VARIABLES ***********************************/
static uint8_t demoAppTimerId;
static uint8_t sendData[5];
static uint8_t sendDataLen;
static bool pktRxd = false;
static LorawanSendReq_t lorawanSendReq;

static uint8_t dataRate = DR3;     //Keep the required datarate

/*ABP Join Parameters */
static uint32_t devAddr	   = 0x270000DC;
static uint8_t nwksKey[16] = {0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34, 0x56, 0x78, 0x90, 0x12};
static uint8_t appsKey[16] = {0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34, 0x56, 0x78, 0x90, 0x21};

/* OTAA join parameters */
static uint8_t devEui[8]   = {0x1C, 0xA5, 0x34, 0x13, 0xD9, 0xF1, 0x2A, 0x97};
static uint8_t appEui[8]   = {0x1C, 0xA5, 0x34, 0x13, 0xD9, 0xF1, 0x2A, 0x34};
static uint8_t appKey[16]  = {0x18, 0xC2, 0xC6, 0x77, 0x3E, 0xC9, 0x53, 0xFB, 0x7A, 0x3A, 0x04, 0x98, 0xED, 0x8A, 0x6C, 0x25};

static const char* bandStrings[] =
{
	"No Band",
	#if (EU_BAND == 1)
	"EU868",
	#endif
	#if (IND_BAND == 1)
	"IND865"
	#endif
};

static uint8_t bandTable[] =
{
	0xFF,
	#if (EU_BAND == 1)
	ISM_EU868,
	#endif
	#if (IND_BAND == 1)
	ISM_IND865
	#endif
};

/************************** EXTERN VARIABLES ***********************************/

/************************** FUNCTION PROTOTYPES ********************************/

/*********************************************************************//*
 \brief    Demo Application Initialization
 ************************************************************************/
static void demo_app_init(void);

/*********************************************************************//*
 \brief      Set device type function
 \param[in]  End device Class type - CLASS A/CLASS C
 \return     LORAWAN_SUCCESS, if successfully set the join parameters
             LORAWAN_INVALID_PARAMETER, otherwise
 ************************************************************************/
StackRetStatus_t set_device_type(EdClass_t ed_class);

/*********************************************************************//*
 \brief      Set join parameters function
 \param[in]  activation type - notifies the activation type (OTAA/ABP)
 \return     LORAWAN_SUCCESS, if successfully set the join parameters
             LORAWAN_INVALID_PARAMETER, otherwise
 ************************************************************************/
static StackRetStatus_t demo_set_join_parameters(ActivationType_t activation_type);

/*********************************************************************//*
 \brief    Certification Timer Callback
 ************************************************************************/
static void demo_app_timer_callback(uint8_t param);

/*********************************************************************//**
\brief Callback function for the ending of Bidirectional communication of
       Application data
 *************************************************************************/
static void demo_appdata_callback(void *appHandle, appCbParams_t *appdata);

/*********************************************************************//*
\brief Callback function for the ending of Activation procedure
 ************************************************************************/
static void demo_joindata_callback(StackRetStatus_t status);

/*********************************************************************//*
 \brief      Function to Print array of characters
 \param[in]  *array  - Pointer of the array to be printed
 \param[in]   length - Length of the array
 ************************************************************************/
static void print_array (uint8_t *array, uint8_t length);

/*********************************************************************//*
 \brief      Function to Print stack return status
 \param[in]  status - Status from the stack
 ************************************************************************/
static void print_stack_status(StackRetStatus_t status);

/*********************************************************************//*
 \brief    Application Task Handler
 ************************************************************************/
SYSTEM_TaskStatus_t APP_TaskHandler(void);

/***************************** FUNCTIONS ***************************************/

/*********************************************************************//**
\brief    Initialization the Demo application
*************************************************************************/
void demo_app_init(void)
{
    StackRetStatus_t status;

    status = SwTimerCreate(&demoAppTimerId);

    if(status!=LORAWAN_SUCCESS)
    {
        printf("\r\nUnable to start demo application timer. Pls check");
        while(1);
    }

    /* Initialize the LORAWAN Stack */
    LORAWAN_Init(demo_appdata_callback, demo_joindata_callback);

    LORAWAN_Reset(bandTable[BAND_CHOICE]);
	
	printf("\n\n\r*******************************************************\n\r");
    printf("\n\rMicrochip LoRaWAN Stack %s\r\n",STACK_VER);
	printf("\r\nInit - Successful\r\n");
	
	/* Set the device type */
	status = set_device_type(DEMO_APP_ENDDEVICE_CLASS);
	if (LORAWAN_SUCCESS != status)
	{
		printf("\nUnsupported Device Type\n\r");
	}
	
    /* Set the join parameters */
    status = demo_set_join_parameters(DEMO_APP_ACTIVATION_TYPE);
    if (LORAWAN_SUCCESS != status)
    {
        printf("\nJoin parameters initialization failed\n\r");
    }
	
	uint8_t pa_boost = PA_ENABLE;
	RADIO_SetAttr(PABOOST,&pa_boost);
	
	uint8_t pa;
	RADIO_GetAttr(PABOOST,&pa);
	printf("\nPABOOST is configured to %d\n", pa);
    /* Send Join request */
    status = LORAWAN_Join(DEMO_APP_ACTIVATION_TYPE);
    if (status == LORAWAN_SUCCESS)
    {
        printf("\nJoin Request Sent for %s\n\r",bandStrings[BAND_CHOICE]);
    }
    else
    {
        print_stack_status(status);
    }
}

/*********************************************************************//*
 \brief      Function to Print stack return status
 \param[in]  status - Status from the stack
 ************************************************************************/
void print_stack_status(StackRetStatus_t status)
{
    switch(status)
    {
        case LORAWAN_SUCCESS:
             printf("\nlorawan_success\n\r");
        break;

        case LORAWAN_BUSY:
             printf("\nlorawan_state : stack_Busy\n\r");
        break;

        case LORAWAN_NWK_NOT_JOINED:
            printf("\ndevice_not_joined_to_network\n\r");
        break;

        case LORAWAN_INVALID_PARAMETER:
            printf("\ninvalid_parameter\n\r");
        break;

        case LORAWAN_KEYS_NOT_INITIALIZED:
            printf("\nkeys_not_initialized\n\r");
        break;

        case LORAWAN_SILENT_IMMEDIATELY_ACTIVE:
            printf("\nsilent_immediately_active\n\r");
        break;

        case LORAWAN_FCNTR_ERROR_REJOIN_NEEDED:
            printf("\nframecounter_error_rejoin_needed\n\r");
        break;

        case LORAWAN_INVALID_BUFFER_LENGTH:
            printf("\ninvalid_buffer_length\n\r");
        break;

        case LORAWAN_MAC_PAUSED:
            printf("\nMAC_paused\n\r");
        break;

        case LORAWAN_NO_CHANNELS_FOUND:
            printf("\nno_free_channels_found\n\r");
        break;

        case LORAWAN_INVALID_REQUEST:
            printf("\nrequest_invalid\n\r");
        break;
        case LORAWAN_NWK_JOIN_IN_PROGRESS:
            printf("\nprev_join_request_in_progress\n\r");
        break;
        default:
           printf("\nrequest_failed %d\n\r",status);
        break;
    }
}

/*********************************************************************//**
\brief Callback function for the ending of Bidirectional communication of
       Application data
 *************************************************************************/
void demo_appdata_callback(void *appHandle, appCbParams_t *appdata)
{
    StackRetStatus_t status = LORAWAN_INVALID_REQUEST;

    if (LORAWAN_EVT_RX_DATA_AVAILABLE == appdata->evt)
    {
		uint8_t *pData = appdata->param.rxData.pData;
		uint8_t dataLength = appdata->param.rxData.dataLength;
        status = appdata->param.rxData.status;
		uint32_t devAddress = appdata->param.rxData.devAddr;
		
        switch(status)
        {
            case LORAWAN_SUCCESS:
            {
				pktRxd = true;
                //Successful transmission
                if((dataLength > 0U) && (NULL != pData))
                {

	                printf("*** Received DL Data ***\n\r");
	                printf("\nFrame Received at port %d\n\r",pData[0]);
	                printf("\nFrame Length - %d\n\r",dataLength);
	                printf("\nAddress - 0x%lx\n\r", devAddress);
	                printf ("\nPayload: ");
	                for (uint8_t i =0; i<dataLength - 1; i++)
	                {
		                printf("%x",pData[i+1]);
	                }
	                printf("\r\n*************************\r\n");
				}
				else
				{
					printf("Received ACK for Confirmed data\r\n");
				}
            }
            break;
            case LORAWAN_RADIO_NO_DATA:
            {
                printf("\n\rRADIO_NO_DATA \n\r");
            }
            break;
            case LORAWAN_RADIO_DATA_SIZE:
                printf("\n\rRADIO_DATA_SIZE \n\r");
            break;
            case LORAWAN_RADIO_INVALID_REQ:
                printf("\n\rRADIO_INVALID_REQ \n\r");
            break;
            case LORAWAN_RADIO_BUSY:
                printf("\n\rRADIO_BUSY \n\r");
            break;
            case LORAWAN_RADIO_OUT_OF_RANGE:
                printf("\n\rRADIO_OUT_OF_RANGE \n\r");
            break;
            case LORAWAN_RADIO_UNSUPPORTED_ATTR:
                printf("\n\rRADIO_UNSUPPORTED_ATTR \n\r");
            break;
            case LORAWAN_RADIO_CHANNEL_BUSY:
                printf("\n\rRADIO_CHANNEL_BUSY \n\r");
            break;
            case LORAWAN_NWK_NOT_JOINED:
                printf("\n\rNWK_NOT_JOINED \n\r");
            break;
            case LORAWAN_INVALID_PARAMETER:
                printf("\n\rINVALID_PARAMETER \n\r");
            break;
            case LORAWAN_KEYS_NOT_INITIALIZED:
                printf("\n\rKEYS_NOT_INITIALIZED \n\r");
            break;
            case LORAWAN_SILENT_IMMEDIATELY_ACTIVE:
                printf("\n\rSILENT_IMMEDIATELY_ACTIVE\n\r");
            break;
            case LORAWAN_FCNTR_ERROR_REJOIN_NEEDED:
                printf("\n\rFCNTR_ERROR_REJOIN_NEEDED \n\r");
            break;
            case LORAWAN_INVALID_BUFFER_LENGTH:
                printf("\n\rINVALID_BUFFER_LENGTH \n\r");
            break;
            case LORAWAN_MAC_PAUSED :
                printf("\n\rMAC_PAUSED  \n\r");
            break;
            case LORAWAN_NO_CHANNELS_FOUND:
                printf("\n\rNO_CHANNELS_FOUND \n\r");
            break;
            case LORAWAN_BUSY:
                printf("\n\rBUSY\n\r");
            break;
            case LORAWAN_NO_ACK:
                printf("\n\rNO_ACK \n\r");
            break;
            case LORAWAN_NWK_JOIN_IN_PROGRESS:
                printf("\n\rALREADY JOINING IS IN PROGRESS \n\r");
            break;
            case LORAWAN_RESOURCE_UNAVAILABLE:
                printf("\n\rRESOURCE_UNAVAILABLE \n\r");
            break;
            case LORAWAN_INVALID_REQUEST:
                printf("\n\rINVALID_REQUEST \n\r");
            break;
            case LORAWAN_FCNTR_ERROR:
                printf("\n\rFCNTR_ERROR \n\r");
            break;
            case LORAWAN_MIC_ERROR:
                printf("\n\rMIC_ERROR \n\r");
            break;
            case LORAWAN_INVALID_MTYPE:
                printf("\n\rINVALID_MTYPE \n\r");
            break;
            case LORAWAN_MCAST_HDR_INVALID:
                printf("\n\rMCAST_HDR_INVALID \n\r");
            break;
			case LORAWAN_INVALID_PACKET:
				printf("\n\rINVALID_PACKET \n\r");
			break;
            default:
                printf("UNKNOWN ERROR\n\r");
            break;
        }
    }
    else if(LORAWAN_EVT_TRANSACTION_COMPLETE == appdata->evt)
    {
        switch(status = appdata->param.transCmpl.status)
        {
			if(pktRxd == false)
			{
				case LORAWAN_SUCCESS:
				{
					printf("Transmission Success\r\n");
				}
			}
            break;
            case LORAWAN_RADIO_SUCCESS:
            {
                printf("Transmission Success\r\n");
            }
            break;
            case LORAWAN_RADIO_NO_DATA:
            {
                printf("\n\rRADIO_NO_DATA \n\r");
            }
            break;
            case LORAWAN_RADIO_DATA_SIZE:
                printf("\n\rRADIO_DATA_SIZE \n\r");
            break;
            case LORAWAN_RADIO_INVALID_REQ:
                printf("\n\rRADIO_INVALID_REQ \n\r");
            break;
            case LORAWAN_RADIO_BUSY:
                printf("\n\rRADIO_BUSY \n\r");
            break;
            case LORAWAN_TX_TIMEOUT:
                printf("\nTx Timeout\n\r");
            break;
            case LORAWAN_RADIO_OUT_OF_RANGE:
                printf("\n\rRADIO_OUT_OF_RANGE \n\r");
            break;
            case LORAWAN_RADIO_UNSUPPORTED_ATTR:
                printf("\n\rRADIO_UNSUPPORTED_ATTR \n\r");
            break;
            case LORAWAN_RADIO_CHANNEL_BUSY:
                printf("\n\rRADIO_CHANNEL_BUSY \n\r");
            break;
            case LORAWAN_NWK_NOT_JOINED:
                printf("\n\rNWK_NOT_JOINED \n\r");
            break;
            case LORAWAN_INVALID_PARAMETER:
                printf("\n\rINVALID_PARAMETER \n\r");
            break;
            case LORAWAN_KEYS_NOT_INITIALIZED:
                printf("\n\rKEYS_NOT_INITIALIZED \n\r");
            break;
            case LORAWAN_SILENT_IMMEDIATELY_ACTIVE:
                printf("\n\rSILENT_IMMEDIATELY_ACTIVE\n\r");
            break;
            case LORAWAN_FCNTR_ERROR_REJOIN_NEEDED:
                printf("\n\rFCNTR_ERROR_REJOIN_NEEDED \n\r");
            break;
            case LORAWAN_INVALID_BUFFER_LENGTH:
                printf("\n\rINVALID_BUFFER_LENGTH \n\r");
            break;
            case LORAWAN_MAC_PAUSED :
                printf("\n\rMAC_PAUSED  \n\r");
            break;
            case LORAWAN_NO_CHANNELS_FOUND:
                printf("\n\rNO_CHANNELS_FOUND \n\r");
            break;
            case LORAWAN_BUSY:
                printf("\n\rBUSY\n\r");
            break;
            case LORAWAN_NO_ACK:
                printf("\n\rNO_ACK \n\r");
            break;
            case LORAWAN_NWK_JOIN_IN_PROGRESS:
                printf("\n\rALREADY JOINING IS IN PROGRESS \n\r");
            break;
            case LORAWAN_RESOURCE_UNAVAILABLE:
                printf("\n\rRESOURCE_UNAVAILABLE \n\r");
            break;
            case LORAWAN_INVALID_REQUEST:
                printf("\n\rINVALID_REQUEST \n\r");
            break;
            case LORAWAN_FCNTR_ERROR:
                printf("\n\rFCNTR_ERROR \n\r");
            break;
            case LORAWAN_MIC_ERROR:
                printf("\n\rMIC_ERROR \n\r");
            break;
            case LORAWAN_INVALID_MTYPE:
                printf("\n\rINVALID_MTYPE \n\r");
            break;
            case LORAWAN_MCAST_HDR_INVALID:
                printf("\n\rMCAST_HDR_INVALID \n\r");
            break;
			case LORAWAN_INVALID_PACKET:
				printf("\n\rINVALID_PACKET \n\r");
			break;
            default:
                printf("\n\rUNKNOWN ERROR\n\r");
            break;
        }
        printf("\n\r*************************************************\n\r");
    }
}

/*********************************************************************//*
\brief Callback function for the ending of Activation procedure
 ************************************************************************/
static void demo_joindata_callback(StackRetStatus_t status)
{
    StackRetStatus_t stackRetStatus;
    /* This is called every time the join process is finished */
    if(LORAWAN_SUCCESS == status)
    {
        printf("\nJoining Successful\n\r");

        /*Start the Timer to send data periodically*/
        stackRetStatus = SwTimerStart(demoAppTimerId, MS_TO_US(DEMO_APP_TIMEOUT), SW_TIMEOUT_RELATIVE, (void*)demo_app_timer_callback, NULL);
        if(stackRetStatus != LORAWAN_SUCCESS)
        {
            printf("ERROR : Unable to start Demo Application Timer\r\n");
        }
    }
    else
    {
        stackRetStatus = LORAWAN_Join(DEMO_APP_ACTIVATION_TYPE);
        if (stackRetStatus == LORAWAN_SUCCESS)
        {
            printf("\nJoin Request Sent for %s\n\r",bandStrings[BAND_CHOICE]);
        }
        else
        {
            print_stack_status(stackRetStatus);
        }
    }
    printf("\n\r*******************************************************\n\r");
}

/*********************************************************************//*
 \brief      Function to Initialize the device type
 \param[in]  ed_class - notifies the device class (CLASS_A/CLASS_B/CLASS_C)
 \return     LORAWAN_SUCCESS, if successfully set the device class
             LORAWAN_INVALID_PARAMETER, otherwise
 ************************************************************************/
StackRetStatus_t set_device_type(EdClass_t ed_class)
{
    StackRetStatus_t status = LORAWAN_SUCCESS;

    status = LORAWAN_SetAttr(EDCLASS, &ed_class);

    return status;
}

/*********************************************************************//*
 \brief      Set join parameters function
 \param[in]  activation type - notifies the activation type (OTAA/ABP)
 \return     LORAWAN_SUCCESS, if successfully set the join parameters
             LORAWAN_INVALID_PARAMETER, otherwise
 ************************************************************************/
static StackRetStatus_t demo_set_join_parameters(ActivationType_t activation_type)
{
    StackRetStatus_t status;

	status = LORAWAN_SetAttr (CURRENT_DATARATE, &dataRate);

    printf("\n********************Join Parameters********************\n\r");

    if(ACTIVATION_BY_PERSONALIZATION == activation_type)
    {
		if (LORAWAN_SUCCESS == status)
		{
			status = LORAWAN_SetAttr (DEV_ADDR, &devAddr);
		}
		
        if (LORAWAN_SUCCESS == status)
        {
            printf("\nDevice Addr - ");
            printf("0x%lx \n\r", devAddr);
            status = LORAWAN_SetAttr (APPS_KEY, appsKey);
        }

        if (LORAWAN_SUCCESS == status)
        {
            printf("\nApplication Session Key - ");
            print_array((uint8_t *)&appsKey, sizeof(appsKey));
            status = LORAWAN_SetAttr (NWKS_KEY, nwksKey);
        }

        if (LORAWAN_SUCCESS == status)
        {
            printf("\nNetwork Session Key - ");
            print_array((uint8_t *)&nwksKey, sizeof(nwksKey));
        }

    }
    else
    {
		if (LORAWAN_SUCCESS == status)
		{
			status = LORAWAN_SetAttr (DEV_EUI, devEui);
		}
		
        if (LORAWAN_SUCCESS == status)
        {
            printf("\nDevice EUI - ");
            print_array((uint8_t *)&devEui, sizeof(devEui));
            status = LORAWAN_SetAttr (APP_EUI, appEui);
        }

        if (LORAWAN_SUCCESS == status)
        {
            printf("\nApplication EUI - ");
            print_array((uint8_t *)&appEui, sizeof(appEui));
            status = LORAWAN_SetAttr (APP_KEY, appKey);
        }

        if (LORAWAN_SUCCESS == status)
        {
            printf("\nApplication Key - ");
            print_array((uint8_t *)&appKey, sizeof(appKey));
        }
    }
    return status;
}

/*********************************************************************//*
 \brief      Function to Print array of characters
 \param[in]  *array  - Pointer of the array to be printed
 \param[in]   length - Length of the array
 ************************************************************************/
static void print_array (uint8_t *array, uint8_t length)
{
    printf("0x");
    for (uint8_t i =0; i < length; i++)
    {
        printf("%02x", *array);
        array++;
    }
    printf("\n\r");
}

/*********************************************************************//*
 \brief    Certification Timer Callback
 ************************************************************************/
static void demo_app_timer_callback(uint8_t param)
{
    StackRetStatus_t status;
    SwTimerStart(demoAppTimerId, MS_TO_US(DEMO_APP_TIMEOUT), SW_TIMEOUT_RELATIVE, (void *)demo_app_timer_callback, NULL);

    lorawanSendReq.buffer = sendData;
    lorawanSendReq.bufferLength = sendDataLen;
    lorawanSendReq.confirmed = DEMO_APP_TRANSMISSION_TYPE;
    lorawanSendReq.port = DEMO_APP_FPORT;
    status = LORAWAN_Send(&lorawanSendReq);

    if (LORAWAN_SUCCESS == status)
    {
		pktRxd = false;
        printf("\nPacket Sent:");
        print_array(lorawanSendReq.buffer,lorawanSendReq.bufferLength);
    }
    else
    {
        print_stack_status(status);
    }
}

/*********************************************************************//*
 \brief      Application Task Handler
 ************************************************************************/

SYSTEM_TaskStatus_t APP_TaskHandler(void)
{
	return SYSTEM_TASK_SUCCESS;
}

int main (void)
{
	/* System Initialization */
	system_init();
	/* Initialize the delay driver */
	delay_init();
	/* Initialize the board target resources */
	board_init();
	
	INTERRUPT_GlobalInterruptEnable();
	/* Initialize the Serial Interface */
	sio2host_init();
	/* Initialize the Radio Hardware */
	HAL_RadioInit();
	/* Initialize the Software Timer Module */
	SystemTimerInit();
	/* Initializes the Security modules */
	SalStatus_t sal_status = SAL_SUCCESS;
	sal_status = SAL_Init();
	
	if (SAL_SUCCESS != sal_status)
	{
		printf("Initialization of Security module is failed\r\n");
		/* Stop Further execution */
		while (1) {
		}
	}
	
	delay_ms(1000);
	
	/* Initialize demo application */
	Stack_Init();
	
	demo_app_init();
	
	/* Application Payload */
	sendData[0] = 0x01;
	sendData[1] = 0x01;
	sendData[2] = 0x01;
	sendData[3] = 0x01;
	sendDataLen = 4;

	while (1)
	{
		SYSTEM_RunTasks();
	}
}
