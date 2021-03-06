/*******************************************************
 Author:					Manfred Langemann
 mailto:					Manfred.Langemann �t t-online.de
 Begin of project:			04.01.2008
 Latest version generated:	04.01.2008
 Filename:					TWI_Slave.h
 Description:    			TWI slave functions include file

 ********************************************************/
#ifndef _TWIS
#define _TWIS

#define	TWIS_ReadBytes		0x60
#define	TWIS_WriteBytes		0xA8

/****************************************************************************
  Public Function definitions
****************************************************************************/
uint8_t	TWIS_Init (uint8_t Address, uint32_t Bitrate);
void	TWIS_Stop (void);
void	TWIS_Write (uint8_t byte);
uint8_t	TWIS_ReadAck (void);
uint8_t	TWIS_ReadNack (void);
uint8_t	TWIS_ResonseRequired (uint8_t *TWI_ResonseType);

#endif
