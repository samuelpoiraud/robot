/**
  ******************************************************************************
  * @file    usbh_msc_core.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    22-July-2011
  * @brief   This file implements the MSC class driver functions
  *          ===================================================================
  *                                MSC Class  Description
  *          ===================================================================
  *           This module manages the MSC class V1.0 following the "Universal
  *           Serial Bus Mass Storage Class (MSC) Bulk-Only Transport (BOT) Version 1.0
  *           Sep. 31, 1999".
  *           This driver implements the following aspects of the specification:
  *             - Bulk-Only Transport protocol
  *             - Subclass : SCSI transparent command set (ref. SCSI Primary Commands - 3 (SPC-3))
  *
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
*/

#include "usbh_cdc_core.h"
#include "../../Core/usbh_core.h"
#include "../../../QS/QS_all.h"

#define LOG_PREFIX "usbh_cdc: "
#define LOG_COMPONENT OUTPUTLOG_COMPONENT_USBH_CDC
#include "../../../QS/QS_outputlog.h"

#define USBH_CDC_ERROR_RETRY_LIMIT 10

__ALIGN_BEGIN CDC_Machine_TypeDef         CDC_Machine __ALIGN_END;

static CDC_uart_interface_t interface_functions;
static Uint32 requested_baudrate = 0;	//0: pas de demande, > 0: demande de changer le baudrate à cette valeur

static USBH_Status USBH_CDC_InterfaceInit(USB_OTG_CORE_HANDLE *pdev, void *phost);
static void USBH_CDC_InterfaceDeInit(USB_OTG_CORE_HANDLE *pdev, void *phost);
static USBH_Status USBH_CDC_ClassRequest(USB_OTG_CORE_HANDLE *pdev, void *phost);
static USBH_Status USBH_SET_LINE_CODING(USB_OTG_CORE_HANDLE *pdev, USBH_HOST *phost, Uint8 *buffer);


//Utilise les fonctions dans interface_functions pour savoir quoi envoyer / recevoir
static USBH_Status USBH_CDC_Handle_extern(USB_OTG_CORE_HANDLE *pdev, void *phost);



USBH_Class_cb_TypeDef  USBH_CDC_cb =
{
	USBH_CDC_InterfaceInit,
	USBH_CDC_InterfaceDeInit,
	USBH_CDC_ClassRequest,
	USBH_CDC_Handle_extern
};

void USBH_CDC_ErrorHandle(Uint8 status);


void USB_CDC_set_interface(CDC_uart_interface_t _interface_functions) {
	interface_functions = _interface_functions;
}

void USB_CDC_set_baudrate(Uint32 baudrate) {
	requested_baudrate = baudrate;
}

/**
  * @brief  USBH_MSC_InterfaceInit
  *         Interface initialization for MSC class.
  * @param  pdev: Selected device
  * @param  hdev: Selected device property
  * @retval USBH_Status : Status of class request handled.
  */
static USBH_Status USBH_CDC_InterfaceInit ( USB_OTG_CORE_HANDLE *pdev,
											void *phost)
{
	USBH_HOST *pphost = phost;

	debug_printf("Initializing bulk endpoint\n");

	if((pphost->device_prop.Itf_Desc[0].bInterfaceClass == CDC_CLASS) && \
			(pphost->device_prop.Itf_Desc[0].bInterfaceProtocol == CDC_PROTOCOL))
	{
		if(pphost->device_prop.Ep_Desc[0][0].bEndpointAddress & 0x80)
		{
			CDC_Machine.MSBulkInEp = (pphost->device_prop.Ep_Desc[0][0].bEndpointAddress);
			CDC_Machine.MSBulkInEpSize  = pphost->device_prop.Ep_Desc[0][0].wMaxPacketSize;
		}
		else
		{
			CDC_Machine.MSBulkOutEp = (pphost->device_prop.Ep_Desc[0][0].bEndpointAddress);
			CDC_Machine.MSBulkOutEpSize  = pphost->device_prop.Ep_Desc[0] [0].wMaxPacketSize;
		}

		if(pphost->device_prop.Ep_Desc[0][1].bEndpointAddress & 0x80)
		{
			CDC_Machine.MSBulkInEp = (pphost->device_prop.Ep_Desc[0][1].bEndpointAddress);
			CDC_Machine.MSBulkInEpSize  = pphost->device_prop.Ep_Desc[0][1].wMaxPacketSize;
		}
		else
		{
			CDC_Machine.MSBulkOutEp = (pphost->device_prop.Ep_Desc[0][1].bEndpointAddress);
			CDC_Machine.MSBulkOutEpSize  = pphost->device_prop.Ep_Desc[0][1].wMaxPacketSize;
		}

		CDC_Machine.hc_num_out = USBH_Alloc_Channel(pdev,
													CDC_Machine.MSBulkOutEp);
		CDC_Machine.hc_num_in = USBH_Alloc_Channel(pdev,
												   CDC_Machine.MSBulkInEp);

		// Open the new channels
		USBH_Open_Channel(pdev,
						  CDC_Machine.hc_num_out,
						  pphost->device_prop.address,
						  pphost->device_prop.speed,
						  EP_TYPE_BULK,
						  CDC_Machine.MSBulkOutEpSize);

		USBH_Open_Channel(pdev,
						  CDC_Machine.hc_num_in,
						  pphost->device_prop.address,
						  pphost->device_prop.speed,
						  EP_TYPE_BULK,
						  CDC_Machine.MSBulkInEpSize);

		debug_printf("  DONE\n");
	}

	else
	{
		debug_printf("  FAILED  (wrong device interface, inteface_class: %u, interface_subclass: %u, interface_protocol: %u\n",
					 pphost->device_prop.Itf_Desc[0].bInterfaceClass,
				pphost->device_prop.Itf_Desc[0].bInterfaceSubClass,
				pphost->device_prop.Itf_Desc[0].bInterfaceProtocol);
		pphost->usr_cb->USBH_USR_DeviceNotSupported();
	}

	return USBH_OK ;

}


/**
  * @brief  USBH_MSC_InterfaceDeInit
  *         De-Initialize interface by freeing host channels allocated to interface
  * @param  pdev: Selected device
  * @param  hdev: Selected device property
  * @retval None
  */
void USBH_CDC_InterfaceDeInit ( USB_OTG_CORE_HANDLE *pdev,
								void *phost)
{
	if ( CDC_Machine.hc_num_out)
	{
		USB_OTG_HC_Halt(pdev, CDC_Machine.hc_num_out);
		USBH_Free_Channel  (pdev, CDC_Machine.hc_num_out);
		CDC_Machine.hc_num_out = 0;     /* Reset the Channel as Free */
	}

	if ( CDC_Machine.hc_num_in)
	{
		USB_OTG_HC_Halt(pdev, CDC_Machine.hc_num_in);
		USBH_Free_Channel  (pdev, CDC_Machine.hc_num_in);
		CDC_Machine.hc_num_in = 0;     /* Reset the Channel as Free */
	}
}


typedef struct
{
	Uint32 bitrate;     //bauds
	Uint8  format;      //0: 1 stop, 1: 1.5 stop, 2: 2 stop bits
	Uint8  paritytype;  //Dans l'ordre (de 0 à 4): None, Odd(impair), Even(pair), Mark, Space
	Uint8  databits;    //8 bits le plus souvent
} vcp_line_coding_e;

typedef enum
{
	CDC_STATE_SET_LINE_CODING,
	CDC_STATE_SEND_BM,
	CDC_STATE_RECEIVE_BM,
	CDC_STATE_SEND_GD,
	CDC_STATE_RECEIVE_LRF_DATA,
	CDC_STATE_PROCESS_DATA
}cdc_state_e;
volatile static cdc_state_e cdc_state = CDC_STATE_SET_LINE_CODING;

typedef enum
{
	CDC_STATE_EXT_SET_LINE_CODING,
	CDC_STATE_EXT_WAIT_COMPLETION,
	CDC_STATE_EXT_ERROR_RECOVER
} cdc_handle_ext_state_e;
volatile static cdc_handle_ext_state_e cdc_ext_state = CDC_STATE_EXT_SET_LINE_CODING;

bool_e USBH_CDC_is_ready_to_run(void){

	if(cdc_ext_state==CDC_STATE_EXT_WAIT_COMPLETION)
		return TRUE;
	else
		return FALSE;
}

#define NUM_RAW_BYTES	1000

/**
  * @brief  USBH_MSC_ClassRequest
  *         This function will only initialize the MSC state machine
  * @param  pdev: Selected device
  * @param  hdev: Selected device property
  * @retval USBH_Status : Status of class request handled.
  */

static USBH_Status USBH_CDC_ClassRequest(USB_OTG_CORE_HANDLE *pdev, void *phost)
{
	USBH_Status status = USBH_OK ;
	cdc_state = CDC_STATE_SET_LINE_CODING;
	cdc_ext_state = CDC_STATE_EXT_SET_LINE_CODING;

	return status;
}

static USBH_Status USBH_CDC_Handle_extern(USB_OTG_CORE_HANDLE *pdev, void *phost) {
	USBH_Status status = USBH_OK;

#define CDC_BUFFER_SIZE 64

	static Uint8 raw_data[CDC_BUFFER_SIZE];
	static Uint8 host_channel_with_error;
	static bool_e clear_feature_not_supported;
	static bool_e recv_requested = FALSE;


//	static URB_STATE old_in_state = -1;
//	static URB_STATE old_out_state = -1;
//	static cdc_handle_ext_state_e last_state = -1;
//	URB_STATE in_state  = HCD_GetURB_State(pdev, CDC_Machine.hc_num_in);
//	URB_STATE out_state  = HCD_GetURB_State(pdev, CDC_Machine.hc_num_out);
//	if(old_in_state != in_state) {
//		debug_printf("state = %d, ", cdc_ext_state);
//		switch(in_state) {
//			case URB_DONE:
//				debug_printf("in_state = URB_DONE\n");
//				break;

//			case URB_ERROR:
//				debug_printf("in_state = URB_ERROR\n");
//				break;

//			case URB_INPROGRESS:
//				debug_printf("in_state = URB_INPROGRESS\n");
//				break;

//			case URB_NOTREADY:
//				debug_printf("in_state = URB_NOTREADY\n");
//				break;

//			case URB_STALL:
//				debug_printf("in_state = URB_STALL\n");
//				break;
//		}
//		old_in_state = in_state;
//		last_state = cdc_ext_state;
//	} else if(old_out_state != out_state) {
//		debug_printf("state = %d, ", cdc_ext_state);
//		switch(out_state) {
//			case URB_DONE:
//				debug_printf("out_state = URB_DONE\n");
//				break;

//			case URB_ERROR:
//				debug_printf("out_state = URB_ERROR\n");
//				break;

//			case URB_INPROGRESS:
//				debug_printf("out_state = URB_INPROGRESS\n");
//				break;

//			case URB_NOTREADY:
//				debug_printf("out_state = URB_NOTREADY\n");
//				break;

//			case URB_STALL:
//				debug_printf("out_state = URB_STALL\n");
//				break;
//		}
//		old_out_state = out_state;
//		last_state = cdc_ext_state;
//	} else if(last_state != cdc_ext_state) {
//		debug_printf("state = %d\n", cdc_ext_state);
//		last_state = cdc_ext_state;
//	}


	if(HCD_IsDeviceConnected(pdev))
	{
		switch(cdc_ext_state)
		{
			case CDC_STATE_EXT_SET_LINE_CODING: {
				vcp_line_coding_e linecoding_config = {
					19200,	//default baudrate
					0,		//format (1 stop bit)
					0,		//no parity
					8		//8 bits / data
				};
				if(requested_baudrate > 0) {
					linecoding_config.bitrate = requested_baudrate;
				}
				status = USBH_SET_LINE_CODING(pdev, phost, (Uint8*)&linecoding_config);
				if(status != USBH_BUSY)
				{
					requested_baudrate = 0;
					cdc_ext_state = CDC_STATE_EXT_WAIT_COMPLETION;
					//USB_OTG_BSP_mDelay(100);
					debug_printf("USB Baudrate set to %lu\n", linecoding_config.bitrate);
				}
				break;
			}

			case CDC_STATE_EXT_WAIT_COMPLETION: {
				URB_STATE in_state, out_state;
				in_state  = HCD_GetURB_State(pdev, CDC_Machine.hc_num_in);
				out_state = HCD_GetURB_State(pdev, CDC_Machine.hc_num_out);

				if(in_state == URB_ERROR || in_state == URB_STALL) {
					debug_printf("Error on input channel: %d\n", in_state);
					host_channel_with_error = CDC_Machine.hc_num_in;
					cdc_ext_state = CDC_STATE_EXT_ERROR_RECOVER;
					break;
				} else if(out_state == URB_ERROR || out_state == URB_STALL) {
					debug_printf("Error on output channel: %d\n", out_state);
					host_channel_with_error = CDC_Machine.hc_num_out;
					cdc_ext_state = CDC_STATE_EXT_ERROR_RECOVER;
					break;
				}

				if(recv_requested && in_state == URB_DONE) {
					(*interface_functions.put_rx_data)(raw_data, HCD_GetXferCnt(pdev, CDC_Machine.hc_num_in));
					//debug_printf("Received %u bytes\n", HCD_GetXferCnt(pdev, CDC_Machine.hc_num_in));
					recv_requested = FALSE;
				} else if(recv_requested && in_state == URB_DONE) {
					break;
				}

				if(requested_baudrate != 0) {
					cdc_ext_state = CDC_STATE_EXT_SET_LINE_CODING;
					break;
				}

				if(out_state != URB_INPROGRESS && (*interface_functions.is_tx_empty)() == FALSE) {
					Uint16 bytes_to_send;

					bytes_to_send = (*interface_functions.get_tx_data)(raw_data, CDC_BUFFER_SIZE);
					if(bytes_to_send) {
						USBH_BulkSendData(pdev,
										  raw_data,
										  bytes_to_send,
										  CDC_Machine.hc_num_out);
					}
				}

				if(in_state != URB_INPROGRESS && (*interface_functions.rx_available)() > CDC_BUFFER_SIZE) {
					USBH_BulkReceiveData(pdev,
										 raw_data,
										 CDC_BUFFER_SIZE,
										 CDC_Machine.hc_num_in);
					recv_requested = TRUE;
				}


				break;
			}

			//Clear ENDPOINT_HALT
			case CDC_STATE_EXT_ERROR_RECOVER: {
				if(clear_feature_not_supported == TRUE) {
					cdc_ext_state = CDC_STATE_EXT_WAIT_COMPLETION;
					break;
				}

				pdev->host.URB_State[host_channel_with_error] = URB_DONE;

				USBH_Status status = USBH_ClrFeature(pdev, phost, 0x00, host_channel_with_error);
				switch(status) {
					case USBH_OK:
						cdc_ext_state = CDC_STATE_EXT_WAIT_COMPLETION;
						break;

					case USBH_NOT_SUPPORTED:
						cdc_ext_state = CDC_STATE_EXT_WAIT_COMPLETION;
						clear_feature_not_supported = TRUE;
						debug_printf("Can\'t use ClearFeature on channel %u, disabling error recovering\n", host_channel_with_error);
						break;

					case USBH_BUSY:
						break;

					default:
					case USBH_FAIL:
						cdc_ext_state = CDC_STATE_EXT_WAIT_COMPLETION;
						debug_printf("Failed to recover error using ClearFeature on channel %u\n", host_channel_with_error);
						break;
				}
				break;
			}
		}
	}
	return status;
}



static USBH_Status USBH_SET_LINE_CODING(USB_OTG_CORE_HANDLE *pdev,
										USBH_HOST *phost,
										Uint8 *buffer)
{

	phost->Control.setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | \
			USB_REQ_RECIPIENT_INTERFACE;

	phost->Control.setup.b.bRequest = 0x20;
	phost->Control.setup.b.wValue.w = 0;
	phost->Control.setup.b.wIndex.w = 0;
	phost->Control.setup.b.wLength.w = 0x88;

	return USBH_CtlReq(pdev, phost, buffer , 7 );
}
