#ifndef ACR_PN532_H
#define ACR_PN532_H
#include "globals.h"

#define ACR_PSEUDO_APDU(var)						0xff, 0x00, 0x00, 0x00, (sizeof(var) - 5)
#define ACR_PSEUDO_APDU2PN532(var, command)			ACR_PSEUDO_APDU(var), PN532_Host2PN532, command

#define ACR_APDU_SEND_MAX							(4 + 1 + 1 + 0xff)
#define ACR_APDU_RECV_MAX							(0x100 + 2)

#define PN532_Host2PN532							0xd4
#define PN532_PN5322Host							0xd5

#define PN532_CMD_WriteRegister						0x08
#define PN532_CMD_InListPassiveTarget				0x4a
#define PN532_CMD_InDataExchange					0x40
#define PN532_CMD_InCommunicateThru					0x42
#define PN532_CMD_InRelease							0x52

#define PN532_REG_CIU_TxMode						0x63, 0x02
#define PN532_REG_CIU_RxMode						0x63, 0x03
#define PN532_REG_CIU_Control						0x63, 0x3c
#define PN532_REG_CIU_BitFraming					0x63, 0x3d

#define IEC14443_CMD_Halt							0x50, 0x00

#define MIFARE_CLASSIC_KEY_SIZE						6
#define MIFARE_CLASSIC_SECTORS						16
#define MIFARE_CLASSIC_BLOCKS_PER_SECTOR			4
#define MIFARE_CLASSIC_BLOCK_SIZE					16
#define MIFARE_CLASSIC_UID_SIZE						4	// ok, I know about 7 or 10 too...

#define MIFARE_CLASSIC_ACL_NONE						0x00
#define MIFARE_CLASSIC_ACL_KEY_A					0x01
#define MIFARE_CLASSIC_ACL_KEY_B					0x02
#define MIFARE_CLASSIC_ACL_KEY_AB					MIFARE_CLASSIC_ACL_KEY_A | MIFARE_CLASSIC_ACL_KEY_B

#define MIFARE_CLASSIC_CMD_Halt						IEC14443_CMD_Halt
#define MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_A		0x60
#define MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_B		0x61
#define MIFARE_CLASSIC_CMD_PERSONALIZE_UID_USAGE	0x40
#define MIFARE_CLASSIC_CMD_SET_MOD_TYPE				0x43
#define MIFARE_CLASSIC_CMD_READ						0x30
#define MIFARE_CLASSIC_CMD_WRITE					0xa0

typedef struct _MIFARE_ACCESS_BITS {
	unsigned not_c1_0: 1;
	unsigned not_c1_1: 1;
	unsigned not_c1_2: 1;
	unsigned not_c1_3: 1;
	unsigned not_c2_0: 1;
	unsigned not_c2_1: 1;
	unsigned not_c2_2: 1;
	unsigned not_c2_3: 1;
	unsigned not_c3_0: 1;
	unsigned not_c3_1: 1;
	unsigned not_c3_2: 1;
	unsigned not_c3_3: 1;
	unsigned c1_0: 1;
	unsigned c1_1: 1;
	unsigned c1_2: 1;
	unsigned c1_3: 1;
	unsigned c2_0: 1;
	unsigned c2_1: 1;
	unsigned c2_2: 1;
	unsigned c2_3: 1;
	unsigned c3_0: 1;
	unsigned c3_1: 1;
	unsigned c3_2: 1;
	unsigned c3_3: 1;

	unsigned data: 8;
} MIFARE_ACCESS_BITS, *PMIFARE_ACCESS_BITS;

UINT16 RAW_SendAndRecv_Size;
BYTE RAW_SendAndRecv_Buffer[ACR_APDU_RECV_MAX];
BOOLEAN RAW_SendAndRecv(const BYTE *pbData, const UINT16 cbData);
BOOLEAN RAW_SendAndRecvPN532(const BYTE *pbData, const UINT16 cbData);

BOOLEAN ACR_PN532_GetMifareCard(BYTE UID[MIFARE_CLASSIC_UID_SIZE], BYTE ATQA[2], BYTE *SAK);
BOOLEAN ACR_PN532_ReleaseMifareCard();
BOOLEAN ACR_PN532_Unlock_MyChinaCard(); // I could escape this feeling
BOOLEAN ACR_PN532_Auth_Block(const BYTE authKey, const BYTE blockId, const BYTE key[MIFARE_CLASSIC_KEY_SIZE], const BYTE uid[MIFARE_CLASSIC_UID_SIZE]);
BOOLEAN ACR_PN532_MIFARE_Read(const BYTE Block, BYTE Output[MIFARE_CLASSIC_BLOCK_SIZE]);
BOOLEAN ACR_PN532_MIFARE_Write(const BYTE Block, const BYTE Input[MIFARE_CLASSIC_BLOCK_SIZE]);

#endif
