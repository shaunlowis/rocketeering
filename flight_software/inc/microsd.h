/**
  ******************************************************************************
  * @file microsd.h
  * @brief This file contains functions prototypes.
  * @author STMicroelectronics - MCD Application Team
  * @version V1.1.1
  * @date 06/05/2009
  ******************************************************************************
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  * @image html logo.bmp
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MICROSD_H
#define __MICROSD_H

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

/* Private define ------------------------------------------------------------*/
/* Block Size */
#define BLOCK_SIZE ((u16)512)

/* Dummy byte */
#define DUMMY ((u8)0xFF)

/* MicroSD activation port & pin */
#define MSD_CS_PORT (GPIOE)
#define MSD_CS_PIN  (GPIO_PIN_0)

/* MSD detection on its memory slot */
#define MICROSD_PRESENT     ((u8)1)
#define MICROSD_NOT_PRESENT ((u8)0)

/* Start Data tokens  */
/* Tokens (necessary because at nop/idle (and CS active) only 0xff is on the data/command line) */
#define MSD_START_DATA_SINGLE_BLOCK_READ ((u8)0xFE)  /* Data token start byte, Start Single Block Read */
#define MSD_START_DATA_MULTIPLE_BLOCK_READ  ((u8)0xFE)  /* Data token start byte, Start Multiple Block Read */
#define MSD_START_DATA_SINGLE_BLOCK_WRITE ((u8)0xFE) /* Data token start byte, Start Single Block Write */
#define MSD_START_DATA_MULTIPLE_BLOCK_WRITE ((u8)0xFD)  /* Data token start byte, Start Multiple Block Write */
#define MSD_STOP_DATA_MULTIPLE_BLOCK_WRITE ((u8)0xFD)  /* Data toke stop byte, Stop Multiple Block Write */

/* MSD functions return */
#define MSD_SUCCESS       ((u8)0x00)
#define MSD_FAIL          ((u8)0xFF)

/* MSD reponses and error flags */
#define MSD_RESPONSE_NO_ERROR      ((u8)0x00)
#define MSD_IN_IDLE_STATE          ((u8)0x01)
#define MSD_ERASE_RESET            ((u8)0x02)
#define MSD_ILLEGAL_COMMAND        ((u8)0x04)
#define MSD_COM_CRC_ERROR          ((u8)0x08)
#define MSD_ERASE_SEQUENCE_ERROR   ((u8)0x10)
#define MSD_ADDRESS_ERROR          ((u8)0x20)
#define MSD_PARAMETER_ERROR        ((u8)0x40)
#define MSD_RESPONSE_FAILURE       ((u8)0xFF)

/* Data response error */
#define MSD_DATA_OK                ((u8)0x05)
#define MSD_DATA_CRC_ERROR         ((u8)0x0B)
#define MSD_DATA_WRITE_ERROR       ((u8)0x0D)
#define MSD_DATA_OTHER_ERROR       ((u8)0xFF)

/* Commands: CMDxx = CMD-number | 0x40 */
#define MSD_GO_IDLE_STATE          ((u8)0)   /* CMD0=0x40 */
#define MSD_SEND_OP_COND           ((u8)1)   /* CMD1=0x41 */
#define MSD_SEND_CSD               ((u8)9)   /* CMD9=0x49 */
#define MSD_SEND_CID               ((u8)10)  /* CMD10=0x4A */
#define MSD_STOP_TRANSMISSION      ((u8)12)  /* CMD12=0x4C */
#define MSD_SEND_STATUS            ((u8)13)  /* CMD13=0x4D */
#define MSD_SET_BLOCKLEN           ((u8)16)  /* CMD16=0x50 */
#define MSD_READ_SINGLE_BLOCK      ((u8)17)  /* CMD17=0x51 */
#define MSD_READ_MULTIPLE_BLOCK    ((u8)18)  /* CMD18=0x52 */
#define MSD_SET_BLOCK_COUNT        ((u8)23)  /* CMD23=0x57 */
#define MSD_WRITE_BLOCK            ((u8)24)  /* CMD24=0x58 */
#define MSD_WRITE_MULTIPLE_BLOCK   ((u8)25)  /* CMD25=0x59 */
#define MSD_PROGRAM_CSD            ((u8)27)  /* CMD27=0x5B */
#define MSD_SET_WRITE_PROT         ((u8)28)  /* CMD28=0x5C */
#define MSD_CLR_WRITE_PROT         ((u8)29)  /* CMD29=0x5D */
#define MSD_SEND_WRITE_PROT        ((u8)30)  /* CMD30=0x5E */
#define MSD_TAG_SECTOR_START       ((u8)32)  /* CMD32=0x60 */
#define MSD_TAG_SECTOR_END         ((u8)33)  /* CMD33=0x61 */
#define MSD_UNTAG_SECTOR           ((u8)34)  /* CMD34=0x62 */
#define MSD_TAG_ERASE_GROUP_START  ((u8)35)  /* CMD35=0x63 */
#define MSD_TAG_ERASE_GROUP_END    ((u8)36)  /* CMD36=0x64 */
#define MSD_UNTAG_ERASE_GROUP      ((u8)37)  /* CMD37=0x65 */
#define MSD_ERASE                  ((u8)38)  /* CMD38=0x66 */
#define MSD_READ_OCR               ((u8)39)  /* CMD39=0x67 */
#define MSD_CRC_ON_OFF             ((u8)40)  /* CMD40=0x68 */


/* Exported functions ------------------------------------------------------- */

/*----- High layer function -----*/
u8 MSD_Init(void);
u8 MSD_WriteBlock(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
u8 MSD_ReadBlock(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
u8 MSD_WriteBuffer(u8* pBuffer, u32 WriteAddr, u32 NumByteToWrite);
u8 MSD_ReadBuffer(u8* pBuffer, u32 ReadAddr, u32 NumByteToRead);

u8 MSD_ReadBytes(u8 *pBuffer, u32 ReadAddr, u32 offset, u16 NumByteToRead);
/*----- Medium layer function -----*/
void MSD_SendCmd(u8 Cmd, u32 Arg, u8 Crc);
u8 MSD_GetResponse(u8 Response);
u8 MSD_GetDataResponse(void);
u8 MSD_GoIdleState(void);
void MSD_ChipSelect(FunctionalState NewState);
u8 MSD_Detect(void);

/*----- Low layer function -----*/
u8 MSD_WriteByte(u8 byte);
u8 MSD_ReadByte(void);

#endif /* __MSD_H */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
