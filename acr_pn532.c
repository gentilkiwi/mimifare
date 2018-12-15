#include "acr_pn532.h"

UINT16 RAW_SendAndRecv_Size;
BYTE RAW_SendAndRecv_Buffer[ACR_APDU_RECV_MAX];

BOOLEAN RAW_SendAndRecvPN532(const BYTE *pbData, const UINT16 cbData)
{
	BOOLEAN status = FALSE;
	if(pbData && cbData && (cbData <= ACR_APDU_SEND_MAX))
	{
		if((cbData > 7) && (pbData[5] == PN532_Host2PN532) && !(pbData[6] & 1)) // we have a valid pseudo APDU for PN532 command
		{
			if(RAW_SendAndRecv(pbData, cbData))
			{
				if((RAW_SendAndRecv_Size >= 4) && (RAW_SendAndRecv_Size < ACR_APDU_RECV_MAX)) // PN532_PN5322Host & cmd+1 then SW1 & SW2 at end
				{
					status = (RAW_SendAndRecv_Buffer[RAW_SendAndRecv_Size - 2] == 0x90) && (RAW_SendAndRecv_Buffer[RAW_SendAndRecv_Size - 1] == 0x00) &&
						(RAW_SendAndRecv_Buffer[0] == PN532_PN5322Host) && (RAW_SendAndRecv_Buffer[1] == (pbData[6] + 1));
					if(status)
						RAW_SendAndRecv_Size -= 2;
				}
			}
		}
	}
	return status;
}

const BYTE ACR_PN532_InListPassiveTarget_Mifare[9] = {ACR_PSEUDO_APDU2PN532(ACR_PN532_InListPassiveTarget_Mifare, PN532_CMD_InListPassiveTarget), 0x01, 0x00}; // Max 1 Tg, 14443-A
BOOLEAN ACR_PN532_GetMifareCard(BYTE UID[MIFARE_CLASSIC_UID_SIZE], BYTE ATQA[2], BYTE *SAK)
{
	BOOLEAN status = FALSE;
	if(RAW_SendAndRecvPN532(ACR_PN532_InListPassiveTarget_Mifare, sizeof(ACR_PN532_InListPassiveTarget_Mifare)))
	{
		if((RAW_SendAndRecv_Size >= (3 + 1 + 2 + 1 + 1 + 4)) && (RAW_SendAndRecv_Buffer[2] == ACR_PN532_InListPassiveTarget_Mifare[7]))
		{
			if((RAW_SendAndRecv_Buffer[4] == 0x00) && (RAW_SendAndRecv_Buffer[5] == 0x04) && ((RAW_SendAndRecv_Buffer[6] == 0x08) || (RAW_SendAndRecv_Buffer[6] == 0x88)))
			{
				status = (RAW_SendAndRecv_Buffer[7] == MIFARE_CLASSIC_UID_SIZE);
				if(status)
				{
					if(UID)
						memcpy(UID, RAW_SendAndRecv_Buffer + 8, MIFARE_CLASSIC_UID_SIZE);
					if(ATQA)
						memcpy(ATQA, RAW_SendAndRecv_Buffer + 4, 2);
					if(SAK)
						*SAK = RAW_SendAndRecv_Buffer[6];
				}
			}
		}
	}
	return status;
}

const BYTE ACR_PN532_InRelease_Mifare[8] = {ACR_PSEUDO_APDU2PN532(ACR_PN532_InRelease_Mifare, PN532_CMD_InRelease), 0x01}; // Tg = 1
BOOLEAN ACR_PN532_ReleaseMifareCard()
{
	BOOLEAN status = FALSE;
	if(RAW_SendAndRecvPN532(ACR_PN532_InRelease_Mifare, sizeof(ACR_PN532_InRelease_Mifare)))
		status = (RAW_SendAndRecv_Size == 3) && !RAW_SendAndRecv_Buffer[2];
	return status;
}

const BYTE
	ACR_PN532_SetTxRx00[13] =		{ACR_PSEUDO_APDU2PN532(ACR_PN532_SetTxRx00, PN532_CMD_WriteRegister), PN532_REG_CIU_TxMode, 0x00, PN532_REG_CIU_RxMode, 0x00},
	ACR_PN532_SetTxRx80[13] =		{ACR_PSEUDO_APDU2PN532(ACR_PN532_SetTxRx80, PN532_CMD_WriteRegister), PN532_REG_CIU_TxMode, 0x80, PN532_REG_CIU_RxMode, 0x80},
	ACR_PN532_HaltCRC[11] =			{ACR_PSEUDO_APDU2PN532(ACR_PN532_HaltCRC, PN532_CMD_InCommunicateThru), MIFARE_CLASSIC_CMD_Halt, 0x57, 0xcd},
	ACR_PN532_Set_BitFraming7[10] =	{ACR_PSEUDO_APDU2PN532(ACR_PN532_Set_BitFraming7, PN532_CMD_WriteRegister), PN532_REG_CIU_BitFraming, 7},
	ACR_PN532_Set_BitFraming0[10] =	{ACR_PSEUDO_APDU2PN532(ACR_PN532_Set_BitFraming0, PN532_CMD_WriteRegister), PN532_REG_CIU_BitFraming, 0},
	ACR_PN532_SendPersUID[8] =		{ACR_PSEUDO_APDU2PN532(ACR_PN532_SendPersUID, PN532_CMD_InCommunicateThru), MIFARE_CLASSIC_CMD_PERSONALIZE_UID_USAGE},
	ACR_PN532_SendSMT[8] =			{ACR_PSEUDO_APDU2PN532(ACR_PN532_SendSMT, PN532_CMD_InCommunicateThru), MIFARE_CLASSIC_CMD_SET_MOD_TYPE};
BOOLEAN ACR_PN532_Unlock_MyChinaCard() // I could escape this feeling
{
	BOOLEAN statusUID = FALSE, statusSMT = FALSE;
	if(RAW_SendAndRecvPN532(ACR_PN532_SetTxRx00, sizeof(ACR_PN532_SetTxRx00)))
	{
		if(RAW_SendAndRecvPN532(ACR_PN532_HaltCRC, sizeof(ACR_PN532_HaltCRC)))
		{
			if((RAW_SendAndRecv_Size == 3) && (RAW_SendAndRecv_Buffer[2] == 0x01))
			{
				if(RAW_SendAndRecvPN532(ACR_PN532_Set_BitFraming7, sizeof(ACR_PN532_Set_BitFraming7)))
					if(RAW_SendAndRecvPN532(ACR_PN532_SendPersUID, sizeof(ACR_PN532_SendPersUID)))
						statusUID = (RAW_SendAndRecv_Size >= 4) && !RAW_SendAndRecv_Buffer[2];
				if(RAW_SendAndRecvPN532(ACR_PN532_Set_BitFraming0, sizeof(ACR_PN532_Set_BitFraming0)))
					if(RAW_SendAndRecvPN532(ACR_PN532_SendSMT, sizeof(ACR_PN532_SendSMT)))
						statusSMT = (RAW_SendAndRecv_Size >= 4) && !RAW_SendAndRecv_Buffer[2];
			}
		}
		RAW_SendAndRecvPN532(ACR_PN532_SetTxRx80, sizeof(ACR_PN532_SetTxRx80));
	}
	return (statusUID && statusSMT);
}

BYTE ACR_PN532_AuthBlock[20] = {ACR_PSEUDO_APDU2PN532(ACR_PN532_AuthBlock, PN532_CMD_InDataExchange), 0x01/*Tg*/, 0x00/*authKey*/, 0x00/*blockId*/}; // key[MIFARE_CLASSIC_KEY_SIZE] + uid[MIFARE_CLASSIC_UID_SIZE]
BOOLEAN ACR_PN532_Auth_Block(const BYTE authKey, const BYTE blockId, const BYTE key[MIFARE_CLASSIC_KEY_SIZE], const BYTE uid[MIFARE_CLASSIC_UID_SIZE])
{
	BOOLEAN status = FALSE;
	ACR_PN532_AuthBlock[8] = authKey;
	ACR_PN532_AuthBlock[9] = blockId;
	memcpy(ACR_PN532_AuthBlock + sizeof(ACR_PN532_AuthBlock) - (MIFARE_CLASSIC_KEY_SIZE + MIFARE_CLASSIC_UID_SIZE), key, MIFARE_CLASSIC_KEY_SIZE);
	memcpy(ACR_PN532_AuthBlock + sizeof(ACR_PN532_AuthBlock) - MIFARE_CLASSIC_UID_SIZE, uid, MIFARE_CLASSIC_UID_SIZE);
	if(RAW_SendAndRecvPN532(ACR_PN532_AuthBlock, sizeof(ACR_PN532_AuthBlock)))
		status = (RAW_SendAndRecv_Size == 3) && !RAW_SendAndRecv_Buffer[2];
	return status;
}

BYTE ACR_PN532_MIFARE_READ[10] = {ACR_PSEUDO_APDU2PN532(ACR_PN532_MIFARE_READ, PN532_CMD_InDataExchange), 0x01, MIFARE_CLASSIC_CMD_READ, 0x00}; // Tg = 1, Block
BOOLEAN ACR_PN532_MIFARE_Read(const BYTE Block, BYTE Output[MIFARE_CLASSIC_BLOCK_SIZE])
{
	BOOLEAN status = FALSE;
	ACR_PN532_MIFARE_READ[9] = Block;
	if(RAW_SendAndRecvPN532(ACR_PN532_MIFARE_READ, sizeof(ACR_PN532_MIFARE_READ)))
	{
		status = (RAW_SendAndRecv_Size == (MIFARE_CLASSIC_BLOCK_SIZE + 3)) && !RAW_SendAndRecv_Buffer[2];
		if(status)
			memcpy(Output, RAW_SendAndRecv_Buffer + 3, MIFARE_CLASSIC_BLOCK_SIZE);
	}
	return status;
}

BYTE ACR_PN532_MIFARE_WRITE[10 + MIFARE_CLASSIC_BLOCK_SIZE] = {ACR_PSEUDO_APDU2PN532(ACR_PN532_MIFARE_WRITE, PN532_CMD_InDataExchange), 0x01, MIFARE_CLASSIC_CMD_WRITE, 0x00}; // Tg = 1, Block
BOOLEAN ACR_PN532_MIFARE_Write(const BYTE Block, const BYTE Input[MIFARE_CLASSIC_BLOCK_SIZE])
{
	BOOLEAN status = FALSE;
	ACR_PN532_MIFARE_WRITE[9] = Block;
	memcpy(ACR_PN532_MIFARE_WRITE + sizeof(ACR_PN532_MIFARE_WRITE) - MIFARE_CLASSIC_BLOCK_SIZE, Input, MIFARE_CLASSIC_BLOCK_SIZE);
	if(RAW_SendAndRecvPN532(ACR_PN532_MIFARE_WRITE, sizeof(ACR_PN532_MIFARE_WRITE)))
		status = (RAW_SendAndRecv_Size == 3) && !RAW_SendAndRecv_Buffer[2];
	return status;
}
