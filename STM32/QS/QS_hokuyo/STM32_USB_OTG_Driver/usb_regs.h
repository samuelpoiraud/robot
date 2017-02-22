/**
  ******************************************************************************
  * @file    usb_regs.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    22-July-2011
  * @brief   hardware registers
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
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_OTG_REGS_H__
#define __USB_OTG_REGS_H__

/* Includes ------------------------------------------------------------------*/
#include "../usb_conf.h"


/** @addtogroup USB_OTG_DRIVER
  * @{
  */
  
/** @defgroup USB_REGS
  * @brief This file is the 
  * @{
  */ 


/** @defgroup USB_REGS_Exported_Defines
  * @{
  */ 

#define USB_OTG_HS_BASE_ADDR                 0x40040000
#define USB_OTG_FS_BASE_ADDR                 0x50000000

#define USB_OTG_CORE_GLOBAL_REGS_OFFSET      0x000
#define USB_OTG_DEV_GLOBAL_REG_OFFSET        0x800
#define USB_OTG_DEV_IN_EP_REG_OFFSET         0x900
#define USB_OTG_EP_REG_OFFSET                0x20
#define USB_OTG_DEV_OUT_EP_REG_OFFSET        0xB00
#define USB_OTG_HOST_GLOBAL_REG_OFFSET       0x400
#define USB_OTG_HOST_PORT_REGS_OFFSET        0x440
#define USB_OTG_HOST_CHAN_REGS_OFFSET        0x500
#define USB_OTG_CHAN_REGS_OFFSET             0x20
#define USB_OTG_PCGCCTL_OFFSET               0xE00
#define USB_OTG_DATA_FIFO_OFFSET             0x1000
#define USB_OTG_DATA_FIFO_SIZE               0x1000


#define USB_OTG_MAX_TX_FIFOS                 15

#define USB_OTG_HS_MAX_PACKET_SIZE           512
#define USB_OTG_FS_MAX_PACKET_SIZE           64
#define USB_OTG_MAX_EP0_SIZE                 64
/**
  * @}
  */ 

/** @defgroup USB_REGS_Exported_Types
  * @{
  */ 

/** @defgroup __USB_OTG_Core_register
  * @{
  */
typedef struct _USB_OTG_GREGS  //000h
{
  __IO Uint32 GOTGCTL;      /* USB_OTG Control and Status Register    000h*/
  __IO Uint32 GOTGINT;      /* USB_OTG Interrupt Register             004h*/
  __IO Uint32 GAHBCFG;      /* Core AHB Configuration Register    008h*/
  __IO Uint32 GUSBCFG;      /* Core USB Configuration Register    00Ch*/
  __IO Uint32 GRSTCTL;      /* Core Reset Register                010h*/
  __IO Uint32 GINTSTS;      /* Core Interrupt Register            014h*/
  __IO Uint32 GINTMSK;      /* Core Interrupt Mask Register       018h*/
  __IO Uint32 GRXSTSR;      /* Receive Sts Q Read Register        01Ch*/
  __IO Uint32 GRXSTSP;      /* Receive Sts Q Read & POP Register  020h*/
  __IO Uint32 GRXFSIZ;      /* Receive FIFO Size Register         024h*/
  __IO Uint32 DIEPTXF0_HNPTXFSIZ;   /* EP0 / Non Periodic Tx FIFO Size Register 028h*/
  __IO Uint32 HNPTXSTS;     /* Non Periodic Tx FIFO/Queue Sts reg 02Ch*/
  __IO Uint32 GI2CCTL;      /* I2C Access Register                030h*/
  Uint32 Reserved34;  /* PHY Vendor Control Register        034h*/
  __IO Uint32 GCCFG;        /* General Purpose IO Register        038h*/
  __IO Uint32 CID;          /* User ID Register                   03Ch*/
  Uint32  Reserved40[48];   /* Reserved                      040h-0FFh*/
  __IO Uint32 HPTXFSIZ; /* Host Periodic Tx FIFO Size Reg     100h*/
  __IO Uint32 DIEPTXF[USB_OTG_MAX_TX_FIFOS];/* dev Periodic Transmit FIFO */
}
USB_OTG_GREGS;
/**
  * @}
  */


/** @defgroup __device_Registers
  * @{
  */
typedef struct _USB_OTG_DREGS // 800h
{
  __IO Uint32 DCFG;         /* dev Configuration Register   800h*/
  __IO Uint32 DCTL;         /* dev Control Register         804h*/
  __IO Uint32 DSTS;         /* dev Status Register (RO)     808h*/
  Uint32 Reserved0C;           /* Reserved                     80Ch*/
  __IO Uint32 DIEPMSK;   /* dev IN Endpoint Mask         810h*/
  __IO Uint32 DOEPMSK;  /* dev OUT Endpoint Mask        814h*/
  __IO Uint32 DAINT;     /* dev All Endpoints Itr Reg    818h*/
  __IO Uint32 DAINTMSK; /* dev All Endpoints Itr Mask   81Ch*/
  Uint32  Reserved20;          /* Reserved                     820h*/
  Uint32 Reserved9;       /* Reserved                     824h*/
  __IO Uint32 DVBUSDIS;    /* dev VBUS discharge Register  828h*/
  __IO Uint32 DVBUSPULSE;  /* dev VBUS Pulse Register      82Ch*/
  __IO Uint32 DTHRCTL;     /* dev thr                      830h*/
  __IO Uint32 DIEPEMPMSK; /* dev empty msk             834h*/
  __IO Uint32 DEACHINT;    /* dedicated EP interrupt       838h*/
  __IO Uint32 DEACHMSK;    /* dedicated EP msk             83Ch*/  
  Uint32 Reserved40;      /* dedicated EP mask           840h*/
  __IO Uint32 DINEP1MSK;  /* dedicated EP mask           844h*/
  Uint32  Reserved44[15];      /* Reserved                 844-87Ch*/
  __IO Uint32 DOUTEP1MSK; /* dedicated EP msk            884h*/   
}
USB_OTG_DREGS;
/**
  * @}
  */


/** @defgroup __IN_Endpoint-Specific_Register
  * @{
  */
typedef struct _USB_OTG_INEPREGS
{
  __IO Uint32 DIEPCTL; /* dev IN Endpoint Control Reg 900h + (ep_num * 20h) + 00h*/
  Uint32 Reserved04;             /* Reserved                       900h + (ep_num * 20h) + 04h*/
  __IO Uint32 DIEPINT; /* dev IN Endpoint Itr Reg     900h + (ep_num * 20h) + 08h*/
  Uint32 Reserved0C;             /* Reserved                       900h + (ep_num * 20h) + 0Ch*/
  __IO Uint32 DIEPTSIZ; /* IN Endpoint Txfer Size   900h + (ep_num * 20h) + 10h*/
  __IO Uint32 DIEPDMA; /* IN Endpoint DMA Address Reg    900h + (ep_num * 20h) + 14h*/
  __IO Uint32 DTXFSTS;/*IN Endpoint Tx FIFO Status Reg 900h + (ep_num * 20h) + 18h*/
  Uint32 Reserved18;             /* Reserved  900h+(ep_num*20h)+1Ch-900h+ (ep_num * 20h) + 1Ch*/
}
USB_OTG_INEPREGS;
/**
  * @}
  */


/** @defgroup __OUT_Endpoint-Specific_Registers
  * @{
  */
typedef struct _USB_OTG_OUTEPREGS
{
  __IO Uint32 DOEPCTL;       /* dev OUT Endpoint Control Reg  B00h + (ep_num * 20h) + 00h*/
  __IO Uint32 DOUTEPFRM;   /* dev OUT Endpoint Frame number B00h + (ep_num * 20h) + 04h*/
  __IO Uint32 DOEPINT;              /* dev OUT Endpoint Itr Reg      B00h + (ep_num * 20h) + 08h*/
  Uint32 Reserved0C;                    /* Reserved                         B00h + (ep_num * 20h) + 0Ch*/
  __IO Uint32 DOEPTSIZ; /* dev OUT Endpoint Txfer Size   B00h + (ep_num * 20h) + 10h*/
  __IO Uint32 DOEPDMA;              /* dev OUT Endpoint DMA Address  B00h + (ep_num * 20h) + 14h*/
  Uint32 Reserved18[2];                 /* Reserved B00h + (ep_num * 20h) + 18h - B00h + (ep_num * 20h) + 1Ch*/
}
USB_OTG_OUTEPREGS;
/**
  * @}
  */


/** @defgroup __Host_Mode_Register_Structures
  * @{
  */
typedef struct _USB_OTG_HREGS
{
  __IO Uint32 HCFG;             /* Host Configuration Register    400h*/
  __IO Uint32 HFIR;      /* Host Frame Interval Register   404h*/
  __IO Uint32 HFNUM;         /* Host Frame Nbr/Frame Remaining 408h*/
  Uint32 Reserved40C;                   /* Reserved                       40Ch*/
  __IO Uint32 HPTXSTS;   /* Host Periodic Tx FIFO/ Queue Status 410h*/
  __IO Uint32 HAINT;   /* Host All Channels Interrupt Register 414h*/
  __IO Uint32 HAINTMSK;   /* Host All Channels Interrupt Mask 418h*/
}
USB_OTG_HREGS;
/**
  * @}
  */


/** @defgroup __Host_Channel_Specific_Registers
  * @{
  */
typedef struct _USB_OTG_HC_REGS
{
  __IO Uint32 HCCHAR;
  __IO Uint32 HCSPLT;
  __IO Uint32 HCINT;
  __IO Uint32 HCGINTMSK;
  __IO Uint32 HCTSIZ;
  __IO Uint32 HCDMA;
  Uint32 Reserved[2];
}
USB_OTG_HC_REGS;
/**
  * @}
  */


/** @defgroup __otg_Core_registers
  * @{
  */
typedef struct USB_OTG_core_regs //000h
{
  USB_OTG_GREGS         *GREGS;
  USB_OTG_DREGS         *DREGS;
  USB_OTG_HREGS         *HREGS;
  USB_OTG_INEPREGS      *INEP_REGS[USB_OTG_MAX_TX_FIFOS];
  USB_OTG_OUTEPREGS     *OUTEP_REGS[USB_OTG_MAX_TX_FIFOS];
  USB_OTG_HC_REGS       *HC_REGS[USB_OTG_MAX_TX_FIFOS];
  __IO Uint32         *HPRT0;
  __IO Uint32         *DFIFO[USB_OTG_MAX_TX_FIFOS];
  __IO Uint32         *PCGCCTL;
}
USB_OTG_CORE_REGS , *PUSB_OTG_CORE_REGS;
typedef union _USB_OTG_OTGCTL_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 sesreqscs :
    1;
Uint32 sesreq :
    1;
Uint32 Reserved2_7 :
    6;
Uint32 hstnegscs :
    1;
Uint32 hnpreq :
    1;
Uint32 hstsethnpen :
    1;
Uint32 devhnpen :
    1;
Uint32 Reserved12_15 :
    4;
Uint32 conidsts :
    1;
Uint32 Reserved17 :
    1;
Uint32 asesvld :
    1;
Uint32 bsesvld :
    1;
Uint32 currmod :
    1;
Uint32 Reserved21_31 :
    11;
  }
  b;
} USB_OTG_OTGCTL_TypeDef ;
typedef union _USB_OTG_GOTGINT_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 Reserved0_1 :
    2;
Uint32 sesenddet :
    1;
Uint32 Reserved3_7 :
    5;
Uint32 sesreqsucstschng :
    1;
Uint32 hstnegsucstschng :
    1;
Uint32 reserver10_16 :
    7;
Uint32 hstnegdet :
    1;
Uint32 adevtoutchng :
    1;
Uint32 debdone :
    1;
Uint32 Reserved31_20 :
    12;
  }
  b;
} USB_OTG_GOTGINT_TypeDef ;
typedef union _USB_OTG_GAHBCFG_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 glblintrmsk :
    1;
Uint32 hburstlen :
    4;
Uint32 dmaenable :
    1;
Uint32 Reserved :
    1;
Uint32 nptxfemplvl_txfemplvl :
    1;
Uint32 ptxfemplvl :
    1;
Uint32 Reserved9_31 :
    23;
  }
  b;
} USB_OTG_GAHBCFG_TypeDef ;
typedef union _USB_OTG_GUSBCFG_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 toutcal :
    3;
Uint32 phyif :
    1;
Uint32 ulpi_utmi_sel :
    1;
Uint32 fsintf :
    1;
Uint32 physel :
    1;
Uint32 ddrsel :
    1;
Uint32 srpcap :
    1;
Uint32 hnpcap :
    1;
Uint32 usbtrdtim :
    4;
Uint32 nptxfrwnden :
    1;
Uint32 phylpwrclksel :
    1;
Uint32 otgutmifssel :
    1;
Uint32 ulpi_fsls :
    1;
Uint32 ulpi_auto_res :
    1;
Uint32 ulpi_clk_sus_m :
    1;
Uint32 ulpi_ext_vbus_drv :
    1;
Uint32 ulpi_int_vbus_indicator :
    1;
Uint32 term_sel_dl_pulse :
    1;
Uint32 Reserved :
    6;
Uint32 force_host :
    1;
Uint32 force_dev :
    1;
Uint32 corrupt_tx :
    1;
  }
  b;
} USB_OTG_GUSBCFG_TypeDef ;
typedef union _USB_OTG_GRSTCTL_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 csftrst :
    1;
Uint32 hsftrst :
    1;
Uint32 hstfrm :
    1;
Uint32 intknqflsh :
    1;
Uint32 rxfflsh :
    1;
Uint32 txfflsh :
    1;
Uint32 txfnum :
    5;
Uint32 Reserved11_29 :
    19;
Uint32 dmareq :
    1;
Uint32 ahbidle :
    1;
  }
  b;
} USB_OTG_GRSTCTL_TypeDef ;
typedef union _USB_OTG_GINTMSK_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 Reserved0 :
    1;
Uint32 modemismatch :
    1;
Uint32 otgintr :
    1;
Uint32 sofintr :
    1;
Uint32 rxstsqlvl :
    1;
Uint32 nptxfempty :
    1;
Uint32 ginnakeff :
    1;
Uint32 goutnakeff :
    1;
Uint32 Reserved8 :
    1;
Uint32 i2cintr :
    1;
Uint32 erlysuspend :
    1;
Uint32 usbsuspend :
    1;
Uint32 usbreset :
    1;
Uint32 enumdone :
    1;
Uint32 isooutdrop :
    1;
Uint32 eopframe :
    1;
Uint32 Reserved16 :
    1;
Uint32 epmismatch :
    1;
Uint32 inepintr :
    1;
Uint32 outepintr :
    1;
Uint32 incomplisoin :
    1;
Uint32 incomplisoout :
    1;
Uint32 Reserved22_23 :
    2;
Uint32 portintr :
    1;
Uint32 hcintr :
    1;
Uint32 ptxfempty :
    1;
Uint32 Reserved27 :
    1;
Uint32 conidstschng :
    1;
Uint32 disconnect :
    1;
Uint32 sessreqintr :
    1;
Uint32 wkupintr :
    1;
  }
  b;
} USB_OTG_GINTMSK_TypeDef ;
typedef union _USB_OTG_GINTSTS_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 curmode :
    1;
Uint32 modemismatch :
    1;
Uint32 otgintr :
    1;
Uint32 sofintr :
    1;
Uint32 rxstsqlvl :
    1;
Uint32 nptxfempty :
    1;
Uint32 ginnakeff :
    1;
Uint32 goutnakeff :
    1;
Uint32 Reserved8 :
    1;
Uint32 i2cintr :
    1;
Uint32 erlysuspend :
    1;
Uint32 usbsuspend :
    1;
Uint32 usbreset :
    1;
Uint32 enumdone :
    1;
Uint32 isooutdrop :
    1;
Uint32 eopframe :
    1;
Uint32 intimerrx :
    1;
Uint32 epmismatch :
    1;
Uint32 inepint:
    1;
Uint32 outepintr :
    1;
Uint32 incomplisoin :
    1;
Uint32 incomplisoout :
    1;
Uint32 Reserved22_23 :
    2;
Uint32 portintr :
    1;
Uint32 hcintr :
    1;
Uint32 ptxfempty :
    1;
Uint32 Reserved27 :
    1;
Uint32 conidstschng :
    1;
Uint32 disconnect :
    1;
Uint32 sessreqintr :
    1;
Uint32 wkupintr :
    1;
  }
  b;
} USB_OTG_GINTSTS_TypeDef ;
typedef union _USB_OTG_DRXSTS_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 epnum :
    4;
Uint32 bcnt :
    11;
Uint32 dpid :
    2;
Uint32 pktsts :
    4;
Uint32 fn :
    4;
Uint32 Reserved :
    7;
  }
  b;
} USB_OTG_DRXSTS_TypeDef ;
typedef union _USB_OTG_GRXSTS_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 chnum :
    4;
Uint32 bcnt :
    11;
Uint32 dpid :
    2;
Uint32 pktsts :
    4;
Uint32 Reserved :
    11;
  }
  b;
} USB_OTG_GRXFSTS_TypeDef ;
typedef union _USB_OTG_FSIZ_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 startaddr :
    16;
Uint32 depth :
    16;
  }
  b;
} USB_OTG_FSIZ_TypeDef ;
typedef union _USB_OTG_HNPTXSTS_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 nptxfspcavail :
    16;
Uint32 nptxqspcavail :
    8;
Uint32 nptxqtop_terminate :
    1;
Uint32 nptxqtop_timer :
    2;
Uint32 nptxqtop :
    2;
Uint32 chnum :
    2;    
Uint32 Reserved :
    1;
  }
  b;
} USB_OTG_HNPTXSTS_TypeDef ;
typedef union _USB_OTG_DTXFSTSn_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 txfspcavail :
    16;
Uint32 Reserved :
    16;
  }
  b;
} USB_OTG_DTXFSTSn_TypeDef ;
typedef union _USB_OTG_GI2CCTL_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 rwdata :
    8;
Uint32 regaddr :
    8;
Uint32 addr :
    7;
Uint32 i2cen :
    1;
Uint32 ack :
    1;
Uint32 i2csuspctl :
    1;
Uint32 i2cdevaddr :
    2;
Uint32 dat_se0:
    1;
Uint32 Reserved :
    1;
Uint32 rw :
    1;
Uint32 bsydne :
    1;
  }
  b;
} USB_OTG_GI2CCTL_TypeDef ;
typedef union _USB_OTG_GCCFG_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 Reserved_in :
    16;
Uint32 pwdn :
    1;
Uint32 i2cifen :
    1;
Uint32 vbussensingA :
    1;
Uint32 vbussensingB :
    1;
Uint32 sofouten :
    1;
Uint32 disablevbussensing :
    1;
Uint32 Reserved_out :
    10;
  }
  b;
} USB_OTG_GCCFG_TypeDef ;

typedef union _USB_OTG_DCFG_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 devspd :
    2;
Uint32 nzstsouthshk :
    1;
Uint32 Reserved3 :
    1;
Uint32 devaddr :
    7;
Uint32 perfrint :
    2;
Uint32 Reserved13_17 :
    5;
Uint32 epmscnt :
    4;
  }
  b;
} USB_OTG_DCFG_TypeDef ;
typedef union _USB_OTG_DCTL_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 rmtwkupsig :
    1;
Uint32 sftdiscon :
    1;
Uint32 gnpinnaksts :
    1;
Uint32 goutnaksts :
    1;
Uint32 tstctl :
    3;
Uint32 sgnpinnak :
    1;
Uint32 cgnpinnak :
    1;
Uint32 sgoutnak :
    1;
Uint32 cgoutnak :
    1;
Uint32 Reserved :
    21;
  }
  b;
} USB_OTG_DCTL_TypeDef ;
typedef union _USB_OTG_DSTS_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 suspsts :
    1;
Uint32 enumspd :
    2;
Uint32 errticerr :
    1;
Uint32 Reserved4_7:
    4;
Uint32 soffn :
    14;
Uint32 Reserved22_31 :
    10;
  }
  b;
} USB_OTG_DSTS_TypeDef ;
typedef union _USB_OTG_DIEPINTn_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 xfercompl :
    1;
Uint32 epdisabled :
    1;
Uint32 ahberr :
    1;
Uint32 timeout :
    1;
Uint32 intktxfemp :
    1;
Uint32 intknepmis :
    1;
Uint32 inepnakeff :
    1;
Uint32 emptyintr :
    1;
Uint32 txfifoundrn :
    1;
Uint32 Reserved08_31 :
    23;
  }
  b;
} USB_OTG_DIEPINTn_TypeDef ;
typedef union _USB_OTG_DIEPINTn_TypeDef   USB_OTG_DIEPMSK_TypeDef ;
typedef union _USB_OTG_DOEPINTn_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 xfercompl :
    1;
Uint32 epdisabled :
    1;
Uint32 ahberr :
    1;
Uint32 setup :
    1;
Uint32 Reserved04_31 :
    28;
  }
  b;
} USB_OTG_DOEPINTn_TypeDef ;
typedef union _USB_OTG_DOEPINTn_TypeDef   USB_OTG_DOEPMSK_TypeDef ;

typedef union _USB_OTG_DAINT_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 in :
    16;
Uint32 out :
    16;
  }
  ep;
} USB_OTG_DAINT_TypeDef ;

typedef union _USB_OTG_DTHRCTL_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 non_iso_thr_en :
    1;
Uint32 iso_thr_en :
    1;
Uint32 tx_thr_len :
    9;
Uint32 Reserved11_15 :
    5;
Uint32 rx_thr_en :
    1;
Uint32 rx_thr_len :
    9;
Uint32 Reserved26_31 :
    6;
  }
  b;
} USB_OTG_DTHRCTL_TypeDef ;
typedef union _USB_OTG_DEPCTL_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 mps :
    11;
Uint32 reserved :
    4;
Uint32 usbactep :
    1;
Uint32 dpid :
    1;
Uint32 naksts :
    1;
Uint32 eptype :
    2;
Uint32 snp :
    1;
Uint32 stall :
    1;
Uint32 txfnum :
    4;
Uint32 cnak :
    1;
Uint32 snak :
    1;
Uint32 setd0pid :
    1;
Uint32 setd1pid :
    1;
Uint32 epdis :
    1;
Uint32 epena :
    1;
  }
  b;
} USB_OTG_DEPCTL_TypeDef ;
typedef union _USB_OTG_DEPXFRSIZ_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 xfersize :
    19;
Uint32 pktcnt :
    10;
Uint32 mc :
    2;
Uint32 Reserved :
    1;
  }
  b;
} USB_OTG_DEPXFRSIZ_TypeDef ;
typedef union _USB_OTG_DEP0XFRSIZ_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 xfersize :
    7;
Uint32 Reserved7_18 :
    12;
Uint32 pktcnt :
    2;
Uint32 Reserved20_28 :
    9;
Uint32 supcnt :
    2;
    Uint32 Reserved31;
  }
  b;
} USB_OTG_DEP0XFRSIZ_TypeDef ;
typedef union _USB_OTG_HCFG_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 fslspclksel :
    2;
Uint32 fslssupp :
    1;
  }
  b;
} USB_OTG_HCFG_TypeDef ;
typedef union _USB_OTG_HFRMINTRVL_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 frint :
    16;
Uint32 Reserved :
    16;
  }
  b;
} USB_OTG_HFRMINTRVL_TypeDef ;

typedef union _USB_OTG_HFNUM_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 frnum :
    16;
Uint32 frrem :
    16;
  }
  b;
} USB_OTG_HFNUM_TypeDef ;
typedef union _USB_OTG_HPTXSTS_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 ptxfspcavail :
    16;
Uint32 ptxqspcavail :
    8;
Uint32 ptxqtop_terminate :
    1;
Uint32 ptxqtop_timer :
    2;
Uint32 ptxqtop :
    2;
Uint32 chnum :
    2;
Uint32 ptxqtop_odd :
    1;
  }
  b;
} USB_OTG_HPTXSTS_TypeDef ;
typedef union _USB_OTG_HPRT0_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 prtconnsts :
    1;
Uint32 prtconndet :
    1;
Uint32 prtena :
    1;
Uint32 prtenchng :
    1;
Uint32 prtovrcurract :
    1;
Uint32 prtovrcurrchng :
    1;
Uint32 prtres :
    1;
Uint32 prtsusp :
    1;
Uint32 prtrst :
    1;
Uint32 Reserved9 :
    1;
Uint32 prtlnsts :
    2;
Uint32 prtpwr :
    1;
Uint32 prttstctl :
    4;
Uint32 prtspd :
    2;
Uint32 Reserved19_31 :
    13;
  }
  b;
} USB_OTG_HPRT0_TypeDef ;
typedef union _USB_OTG_HAINT_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 chint :
    16;
Uint32 Reserved :
    16;
  }
  b;
} USB_OTG_HAINT_TypeDef ;
typedef union _USB_OTG_HAINTMSK_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 chint :
    16;
Uint32 Reserved :
    16;
  }
  b;
} USB_OTG_HAINTMSK_TypeDef ;
typedef union _USB_OTG_HCCHAR_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 mps :
    11;
Uint32 epnum :
    4;
Uint32 epdir :
    1;
Uint32 Reserved :
    1;
Uint32 lspddev :
    1;
Uint32 eptype :
    2;
Uint32 multicnt :
    2;
Uint32 devaddr :
    7;
Uint32 oddfrm :
    1;
Uint32 chdis :
    1;
Uint32 chen :
    1;
  }
  b;
} USB_OTG_HCCHAR_TypeDef ;
typedef union _USB_OTG_HCSPLT_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 prtaddr :
    7;
Uint32 hubaddr :
    7;
Uint32 xactpos :
    2;
Uint32 compsplt :
    1;
Uint32 Reserved :
    14;
Uint32 spltena :
    1;
  }
  b;
} USB_OTG_HCSPLT_TypeDef ;
typedef union _USB_OTG_HCINTn_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 xfercompl :
    1;
Uint32 chhltd :
    1;
Uint32 ahberr :
    1;
Uint32 stall :
    1;
Uint32 nak :
    1;
Uint32 ack :
    1;
Uint32 nyet :
    1;
Uint32 xacterr :
    1;
Uint32 bblerr :
    1;
Uint32 frmovrun :
    1;
Uint32 datatglerr :
    1;
Uint32 Reserved :
    21;
  }
  b;
} USB_OTG_HCINTn_TypeDef ;
typedef union _USB_OTG_HCTSIZn_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 xfersize :
    19;
Uint32 pktcnt :
    10;
Uint32 pid :
    2;
Uint32 dopng :
    1;
  }
  b;
} USB_OTG_HCTSIZn_TypeDef ;
typedef union _USB_OTG_HCGINTMSK_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 xfercompl :
    1;
Uint32 chhltd :
    1;
Uint32 ahberr :
    1;
Uint32 stall :
    1;
Uint32 nak :
    1;
Uint32 ack :
    1;
Uint32 nyet :
    1;
Uint32 xacterr :
    1;
Uint32 bblerr :
    1;
Uint32 frmovrun :
    1;
Uint32 datatglerr :
    1;
Uint32 Reserved :
    21;
  }
  b;
} USB_OTG_HCGINTMSK_TypeDef ;
typedef union _USB_OTG_PCGCCTL_TypeDef 
{
  Uint32 d32;
  struct
  {
Uint32 stoppclk :
    1;
Uint32 gatehclk :
    1;
Uint32 Reserved :
    30;
  }
  b;
} USB_OTG_PCGCCTL_TypeDef ;

/**
  * @}
  */ 


/** @defgroup USB_REGS_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_REGS_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_REGS_Exported_FunctionsPrototype
  * @{
  */ 
/**
  * @}
  */ 


#endif //__USB_OTG_REGS_H__


/**
  * @}
  */ 

/**
  * @}
  */ 
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

