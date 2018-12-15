#ifndef MIMIFARE_H
#define MIMIFARE_H
#include "globals.h"
#include "key_helpers.h"
#include "acr_pn532.h"

#define SB2CHAR(s) (char) ((s < 10) ? ('0' + s) : (s < 0x10) ? 'a' + (s - 10) : '?')

typedef struct _MIFARE_CLASSIC_STD_ACL {
	BYTE keyToRead;
	BYTE keyToWrite;
	BYTE keyToInc;
	BYTE keyToDecTraRes;
} MIFARE_CLASSIC_STD_ACL, *PMIFARE_CLASSIC_STD_ACL;

typedef struct _MIFARE_CLASSIC_SEC_ACL {
	BYTE keyToRead;
	BYTE keyToWrite;
} MIFARE_CLASSIC_SEC_ACL, *PMIFARE_CLASSIC_SEC_ACL;

typedef struct _MIFARE_CLASSIC_SEC_BLOCK_ACL {
	MIFARE_CLASSIC_SEC_ACL keyA;
	MIFARE_CLASSIC_SEC_ACL accessBits;
	MIFARE_CLASSIC_SEC_ACL keyB;
} MIFARE_CLASSIC_SEC_BLOCK_ACL, *PMIFARE_CLASSIC_SEC_BLOCK_ACL;

typedef struct _MIFARE_CLASSIC_STD_SECTOR_ACL {
	MIFARE_CLASSIC_STD_ACL b0;
	MIFARE_CLASSIC_STD_ACL b1;
	MIFARE_CLASSIC_STD_ACL b2;
	MIFARE_CLASSIC_SEC_BLOCK_ACL b3;
} MIFARE_CLASSIC_STD_SECTOR_ACL, *PMIFARE_CLASSIC_STD_SECTOR_ACL;

typedef struct _MIFARE_CLASSIC_STD_CARD_ACL {
	MIFARE_CLASSIC_STD_SECTOR_ACL sectors[MIFARE_CLASSIC_SECTORS];
} MIFARE_CLASSIC_STD_CARD_ACL, *PMIFARE_CLASSIC_STD_CARD_ACL;


typedef struct _MIFARE_CLASSIC_STD_KEY {
	BOOLEAN isKey; // 0 = no, MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_A or MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_B
	BYTE key[MIFARE_CLASSIC_KEY_SIZE];
} MIFARE_CLASSIC_STD_KEY, *PMIFARE_CLASSIC_STD_KEY;

typedef struct _MIFARE_CLASSIC_STD_BLOCK {
	BOOLEAN isRead;
	BYTE data[MIFARE_CLASSIC_BLOCK_SIZE];
} MIFARE_CLASSIC_STD_BLOCK, *PMIFARE_CLASSIC_STD_BLOCK;

typedef struct _MIFARE_CLASSIC_SEC_BLOCK {
	MIFARE_CLASSIC_STD_KEY keyA;
	BOOLEAN isRead;
	MIFARE_ACCESS_BITS bits;
	MIFARE_CLASSIC_STD_KEY keyB;
} MIFARE_CLASSIC_SEC_BLOCK, *PMIFARE_CLASSIC_SEC_BLOCK;

typedef struct _MIFARE_CLASSIC_STD_SECTOR {
	MIFARE_CLASSIC_STD_BLOCK b0;
	MIFARE_CLASSIC_STD_BLOCK b1;
	MIFARE_CLASSIC_STD_BLOCK b2;
	MIFARE_CLASSIC_SEC_BLOCK b3;
} MIFARE_CLASSIC_STD_SECTOR, *PMIFARE_CLASSIC_STD_SECTOR;

typedef struct _MIFARE_CLASSIC_STD_CARD {
	BYTE ATQA[2];
	BYTE SAK;
	BYTE uid[MIFARE_CLASSIC_UID_SIZE];
	BYTE timestamp[1 + 1 + 1 + 1 + 1 + 1];
	BYTE descr[64];
	MIFARE_CLASSIC_STD_SECTOR sectors[MIFARE_CLASSIC_SECTORS];
} MIFARE_CLASSIC_STD_CARD, *PMIFARE_CLASSIC_STD_CARD;


BOOLEAN Read_Card();
BOOLEAN Read_Sector_with_Keys(BYTE nbSector, PMIFARE_CLASSIC_STD_SECTOR pSector, const BYTE uid[MIFARE_CLASSIC_UID_SIZE], const BYTE key[MIFARE_CLASSIC_KEY_SIZE], PMIFARE_HELPER helper);
BOOLEAN Read_Sector_with_Key(BYTE nbSector, PMIFARE_CLASSIC_STD_SECTOR pSector, PMIFARE_CLASSIC_STD_KEY pKey, BYTE type, const BYTE key[MIFARE_CLASSIC_KEY_SIZE], const BYTE uid[MIFARE_CLASSIC_UID_SIZE]);
BOOLEAN Read_Block_Std(BYTE nbSector, BYTE nbBlock, PMIFARE_CLASSIC_STD_BLOCK pBlock);
BOOLEAN Read_Block_Sec(BYTE nbSector, PMIFARE_CLASSIC_SEC_BLOCK pBlock);

BOOLEAN Write_Card();
BOOLEAN Write_Sector(const BYTE sectorId, const PMIFARE_CLASSIC_STD_SECTOR sector, BOOLEAN fixDangerousACL);
BOOLEAN Write_Block_Std(const BYTE blockId, const PMIFARE_CLASSIC_STD_BLOCK block);
BOOLEAN Write_Block_Sec(const BYTE blockId, const PMIFARE_CLASSIC_SEC_BLOCK block, BOOLEAN fixDangerousACL);

void Dump_Card(PMIFARE_CLASSIC_STD_CARD card);
void Dump_Sector(PMIFARE_CLASSIC_STD_SECTOR sector);
BOOLEAN isBlockEmpty(PMIFARE_CLASSIC_STD_BLOCK block);


BOOLEAN initReaderAndCard();
void finishReaderAndCard();

KeyInputEnumType getKeyPressed();
static portTASK_FUNCTION(mimifare_Main, pvParameters);

#ifdef WIN32
int main(int argc, char * argv[]);
#define LCD_Display_ASCIIChar(c, b)	printf("%c", c)
#define LCD_DisplayASCIIMessage(s)	printf("%s", s)
#define LCD_DrawTitleBox(s)			printf("\n====\n%s\n====\n", s)
#define LCD_DisplayHex(c)			printf("%02X", c)
void LCD_DisplayHexN(const UINT8 *pucDisplay, UINT8 Number);
#define LCD_ClearDisplay(a, b)		printf("\n")
#define LCD_GetCursor(a, b)			{a; b;}
#define LCD_SetCursor(a, b)			printf("\n")
#define LCD_SetBacklight(b)			{}
#define SCard_Manager_SelectCard(a) {}
#else
void Application_Init(void);
void Display_Title(BOOLEAN isWrite);
BOOLEAN getYesNo();
#endif
#endif
