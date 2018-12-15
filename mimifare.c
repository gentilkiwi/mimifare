#include "mimifare.h"

const BYTE defaultACL[] = {0xff, 0x07, 0x80, 0x00};
BYTE Read_Block_buffer[MIFARE_CLASSIC_BLOCK_SIZE];
const BYTE defaultUID[MIFARE_CLASSIC_UID_SIZE] = {0x42, 0x42, 0x42, 0x42};
MIFARE_CLASSIC_STD_CARD curCard;

void LCD_DisplayASCIIMessageOnNewLine(const UINT8 *LcdMessageToDisplay)
{
	UINT8 ucLcdRowPosition, ucLcdColumnPosition;
	LCD_GetCursor(&ucLcdRowPosition, &ucLcdColumnPosition);
	LCD_SetCursor(ucLcdRowPosition + 1, 3);
	if(LcdMessageToDisplay)
		LCD_DisplayASCIIMessage(LcdMessageToDisplay);
}

/*
void LCD_DisplayASCIIMessageAt(UINT8 ucLcdRowPosition, UINT8 ucLcdColumnPosition, const UINT8 *LcdMessageToDisplay)
{
	LCD_SetCursor(ucLcdRowPosition, ucLcdColumnPosition);
	LCD_DisplayASCIIMessage(LcdMessageToDisplay);
}
*/

BOOLEAN getYesNo()
{
	KeyInputEnumType key;
	do
	{
		key = getKeyPressed();
	} while((key != Key_PowerKeyInput) && (key != Key_ClearKeyInput));
	return (key == Key_PowerKeyInput);
}

void Dump_SectorKeysState(PMIFARE_CLASSIC_STD_SECTOR sector)
{
	char sep;
	if(sector->b3.keyA.isKey && sector->b3.keyB.isKey)
		sep = '*';
	else if(sector->b3.keyA.isKey && !sector->b3.keyB.isKey)
		sep = 'A';
	else if(!sector->b3.keyA.isKey && sector->b3.keyB.isKey)
		sep = 'B';
	else sep = 'x';
	LCD_Display_ASCIIChar(sep, TRUE);
}


BOOLEAN isBlockEmpty(PMIFARE_CLASSIC_STD_BLOCK block)
{
	BOOLEAN result = block->isRead;
	BYTE i;
	for(i = 0; (i < MIFARE_CLASSIC_BLOCK_SIZE) && result; i++)
		result &= !block->data[i];
	return result;
}

BOOLEAN isDangerousAccessBits(PMIFARE_ACCESS_BITS bits, BOOLEAN fixIt)
{
	BOOLEAN isDangerous;
	isDangerous = !bits->c3_3 || (bits->c1_3 && bits->c2_3);
	if(isDangerous && fixIt)
	{
		if(bits->c1_3 && !bits->c2_3)
		{
			bits->c1_3 = 0;
			bits->c2_3 = 1;
		}
		else bits->c2_3 = 0;
		bits->c3_3 = 1;

		bits->not_c1_3 = !bits->c1_3;
		bits->not_c2_3 = !bits->c2_3;
		bits->not_c3_3 = !bits->c3_3;
	}
	return isDangerous;
}


void InitDefaultCard()
{
	BYTE s;
	PMIFARE_CLASSIC_STD_SECTOR curSec;
	memset(&curCard, 0x00, sizeof(MIFARE_CLASSIC_STD_CARD));
	curCard.SAK = 0x08;
	curCard.ATQA[0] = 0x04;
	//curCard.ATQA[1] = 0x00;
	memcpy(curCard.uid, defaultUID, sizeof(curCard.uid));
	memcpy(curCard.sectors[0].b0.data, curCard.uid, sizeof(curCard.uid));
	curCard.sectors[0].b0.data[4] = curCard.sectors[0].b0.data[0] ^ curCard.sectors[0].b0.data[1] ^ curCard.sectors[0].b0.data[2] ^ curCard.sectors[0].b0.data[3];
	curCard.sectors[0].b0.data[5] = curCard.SAK;
	curCard.sectors[0].b0.data[6] = curCard.ATQA[0];
	//curCard.sectors[0].b0.data[7] = curCard.ATQA[1];
	for(s = 0; s < MIFARE_CLASSIC_SECTORS; s++)
	{
		curSec = &curCard.sectors[s];
		curSec->b0.isRead = curSec->b1.isRead = curSec->b2.isRead = curSec->b3.isRead = TRUE;
		curSec->b3.keyA.isKey = MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_A;
		curSec->b3.keyB.isKey = MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_B;
		memcpy(curSec->b3.keyA.key, MIMIFARE_BLANK_KEY, sizeof(curSec->b3.keyA.key));
		memcpy(curSec->b3.keyB.key, MIMIFARE_BLANK_KEY, sizeof(curSec->b3.keyA.key));
		memcpy(&curSec->b3.bits, defaultACL, sizeof(curSec->b3.bits));
	}
}

BOOLEAN Read_Card()
{
	BOOLEAN status = FALSE;
	BYTE s, i;
	PMIFARE_CLASSIC_STD_SECTOR curSec;
	PMIFARE_HELPER helper = NULL, tmpHelper;
	char ReadTitle[12];

	LCD_DrawTitleBox("Read");
	if(initReaderAndCard())
	{
		memset(&curCard, 0, sizeof(MIFARE_CLASSIC_STD_CARD));
		status = ACR_PN532_GetMifareCard(curCard.uid, curCard.ATQA, &curCard.SAK);
		if(status)
		{
			sprintf(ReadTitle, "%02X%02X%02X%02X/%02X", curCard.uid[3], curCard.uid[2], curCard.uid[1], curCard.uid[0], curCard.SAK);
			LCD_DrawTitleBox(ReadTitle);
			LCD_SetCursor(1, 2 * 6 + 3);
			for(s = 0; s < MIFARE_CLASSIC_SECTORS; s++)
			{
				curSec = &curCard.sectors[s];
				LCD_Display_ASCIIChar('.', FALSE);
				if(s)
				{
					if(helper)
						Read_Sector_with_Keys(s, curSec, curCard.uid, NULL, helper);
					if(curCard.sectors[s - 1].b3.keyA.isKey)
						Read_Sector_with_Keys(s, curSec, curCard.uid, curCard.sectors[s - 1].b3.keyA.key, NULL);
					if(curCard.sectors[s - 1].b3.keyB.isKey)
						Read_Sector_with_Keys(s, curSec, curCard.uid, curCard.sectors[s - 1].b3.keyB.key, NULL);
				}
				for(i = 0; (!(curSec->b3.keyA.isKey && curSec->b3.keyB.isKey)) && (i < (sizeof(defaultKeysWithHelper) / sizeof(DEFAULT_KEYS_WITH_HELPER))); i++)
				{
					if(Read_Sector_with_Keys(s, curSec, curCard.uid, defaultKeysWithHelper[i].key, NULL))
					{
						tmpHelper = defaultKeysWithHelper[i].helper;
						if(tmpHelper)
						{
							Read_Sector_with_Keys(s, curSec, curCard.uid, NULL, tmpHelper);
							if(!helper)
								helper = tmpHelper;
						}
					}
				}
				Dump_SectorKeysState(curSec);
			}
		}
		ACR_PN532_ReleaseMifareCard();
		finishReaderAndCard();
	}
	return status;
}

BOOLEAN Read_Sector_with_Keys(BYTE nbSector, PMIFARE_CLASSIC_STD_SECTOR pSector, const BYTE uid[MIFARE_CLASSIC_UID_SIZE], const BYTE key[MIFARE_CLASSIC_KEY_SIZE], PMIFARE_HELPER helper)
{
	BOOLEAN status = FALSE;
	if(pSector)
	{
		if(!pSector->b3.keyA.isKey)
		{
			if(helper)
				key = helper(nbSector, MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_A);
			status = Read_Sector_with_Key(nbSector, pSector, &pSector->b3.keyA, MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_A, key, uid);
		}
		if(!pSector->b3.keyB.isKey)
		{
			if(helper)
				key = helper(nbSector, MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_B);
			status |= Read_Sector_with_Key(nbSector, pSector, &pSector->b3.keyB, MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_B, key, uid);
		}
	}
	return status;
}

BOOLEAN Read_Sector_with_Key(BYTE nbSector, PMIFARE_CLASSIC_STD_SECTOR pSector, PMIFARE_CLASSIC_STD_KEY pKey, BYTE type, const BYTE key[MIFARE_CLASSIC_KEY_SIZE], const BYTE uid[MIFARE_CLASSIC_UID_SIZE])
{
	BOOLEAN status = FALSE;
	if(pSector && pKey && !pKey->isKey && key)
	{
		if(ACR_PN532_Auth_Block(type, nbSector * 4, key, uid))
		{
			pKey->isKey = type;
			memcpy(pKey->key, key, MIFARE_CLASSIC_KEY_SIZE);
			Read_Block_Std(nbSector, 0, &pSector->b0);
			Read_Block_Std(nbSector, 1, &pSector->b1);
			Read_Block_Std(nbSector, 2, &pSector->b2);
			Read_Block_Sec(nbSector, &pSector->b3);
		}
		else ACR_PN532_GetMifareCard(NULL, NULL, NULL);
		status = pKey->isKey;
	}
	return status;
}

BOOLEAN Read_Block_Std(BYTE nbSector, BYTE nbBlock, PMIFARE_CLASSIC_STD_BLOCK pBlock)
{
	BOOLEAN status = FALSE;
	if(pBlock && !pBlock->isRead)
	{
		pBlock->isRead = ACR_PN532_MIFARE_Read(nbSector * MIFARE_CLASSIC_BLOCKS_PER_SECTOR + nbBlock, pBlock->data);
		status = pBlock->isRead;
	}
	return status;
}

BOOLEAN Read_Block_Sec(BYTE nbSector, PMIFARE_CLASSIC_SEC_BLOCK pBlock)
{
	BYTE status = FALSE;
	if(pBlock && !pBlock->isRead)
	{
		pBlock->isRead = ACR_PN532_MIFARE_Read(nbSector * MIFARE_CLASSIC_BLOCKS_PER_SECTOR + 3, Read_Block_buffer);
		memcpy(&pBlock->bits, Read_Block_buffer + MIFARE_CLASSIC_KEY_SIZE, sizeof(pBlock->bits));
		status = pBlock->isRead;
	}
	return status;
}

//const MIFARE_CLASSIC_STD_ACL stdACl[8] = {
//	/* 0 0 0 */ {MIFARE_CLASSIC_ACL_KEY_AB, MIFARE_CLASSIC_ACL_KEY_AB, MIFARE_CLASSIC_ACL_KEY_AB, MIFARE_CLASSIC_ACL_KEY_AB},
//	/* 0 0 1 */ {MIFARE_CLASSIC_ACL_KEY_AB, MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_KEY_AB},
//	/* 0 1 0 */ {MIFARE_CLASSIC_ACL_KEY_AB, MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_NONE},
//	/* 0 1 1 */ {MIFARE_CLASSIC_ACL_KEY_B, MIFARE_CLASSIC_ACL_KEY_B, MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_NONE},
//	/* 1 0 0 */ {MIFARE_CLASSIC_ACL_KEY_AB, MIFARE_CLASSIC_ACL_KEY_B, MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_NONE},
//	/* 1 0 1 */ {MIFARE_CLASSIC_ACL_KEY_B, MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_NONE},
//	/* 1 1 0 */ {MIFARE_CLASSIC_ACL_KEY_AB, MIFARE_CLASSIC_ACL_KEY_B, MIFARE_CLASSIC_ACL_KEY_B, MIFARE_CLASSIC_ACL_KEY_AB},
//	/* 1 1 1 */ {MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_NONE},
//};
//
//const MIFARE_CLASSIC_SEC_BLOCK_ACL secBlockAcl[8] = {
//	/* 0 0 0 */ {{MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_KEY_A}, {MIFARE_CLASSIC_ACL_KEY_A, MIFARE_CLASSIC_ACL_NONE}, {MIFARE_CLASSIC_ACL_KEY_A, MIFARE_CLASSIC_ACL_KEY_A}},
//	/* 0 0 1 */ {{MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_KEY_A}, {MIFARE_CLASSIC_ACL_KEY_A, MIFARE_CLASSIC_ACL_KEY_A}, {MIFARE_CLASSIC_ACL_KEY_A, MIFARE_CLASSIC_ACL_KEY_A}},
//	/* 0 1 0 */ {{MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_NONE}, {MIFARE_CLASSIC_ACL_KEY_A, MIFARE_CLASSIC_ACL_NONE}, {MIFARE_CLASSIC_ACL_KEY_A, MIFARE_CLASSIC_ACL_NONE}},
//	/* 0 1 1 */ {{MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_KEY_B}, {MIFARE_CLASSIC_ACL_KEY_AB, MIFARE_CLASSIC_ACL_KEY_B}, {MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_KEY_B}},
//	/* 1 0 0 */ {{MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_KEY_B}, {MIFARE_CLASSIC_ACL_KEY_AB, MIFARE_CLASSIC_ACL_NONE}, {MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_KEY_B}},
//	/* 1 0 1 */ {{MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_NONE}, {MIFARE_CLASSIC_ACL_KEY_AB, MIFARE_CLASSIC_ACL_KEY_B}, {MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_NONE}},
//	/* 1 1 0 */ {{MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_NONE}, {MIFARE_CLASSIC_ACL_KEY_AB, MIFARE_CLASSIC_ACL_NONE}, {MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_NONE}},
//	/* 1 1 1 */ {{MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_NONE}, {MIFARE_CLASSIC_ACL_KEY_AB, MIFARE_CLASSIC_ACL_NONE}, {MIFARE_CLASSIC_ACL_NONE, MIFARE_CLASSIC_ACL_NONE}},
//};
//
//
//void AbToACL(PMIFARE_ACCESS_BITS bits, PMIFARE_CLASSIC_STD_SECTOR_ACL acl)
//{
//	acl->b0 = stdACl[(bits->c1_0 << 2) | (bits->c2_0 << 1) | bits->c3_0];
//	acl->b1 = stdACl[(bits->c1_1 << 2) | (bits->c2_1 << 1) | bits->c3_1];
//	acl->b2 = stdACl[(bits->c1_2 << 2) | (bits->c2_2 << 1) | bits->c3_2];
//	acl->b3 = secBlockAcl[(bits->c1_3 << 2) | (bits->c2_3 << 1) | bits->c3_3];
//}
//
//
//BOOLEAN GetWritePath(const BYTE uid[4])
//{
//	BYTE s;
//	PMIFARE_CLASSIC_STD_SECTOR_ACL curSec;
//	BOOLEAN isAuth = FALSE;
//
//	memset(&curAcl, 0x00, sizeof(MIFARE_CLASSIC_STD_CARD_ACL));
//	for(s = 0; s < MIFARE_CLASSIC_SECTORS; s++)
//	{
//		curSec = &curAcl.sectors[s];
//		isAuth = ACR_PN532_Auth_Block(MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_A, s * MIFARE_CLASSIC_BLOCKS_PER_SECTOR + 3, MIMIFARE_BLANK_KEY, uid);
//		if(isAuth)
//		{
//			if(ACR_PN532_MIFARE_Read(s * MIFARE_CLASSIC_BLOCKS_PER_SECTOR + 3, Read_Block_buffer))
//			{
//				AbToACL((PMIFARE_ACCESS_BITS) (Read_Block_buffer + 6), curSec);
//				printf("B0: %02x/%02x - B1: %02x/%02x - B2: %02x/%02x\n", curSec->b0.keyToRead, curSec->b0.keyToWrite, curSec->b1.keyToRead, curSec->b1.keyToWrite, curSec->b2.keyToRead, curSec->b2.keyToWrite);
//				printf("KA: %02x/%02x - SB: %02x/%02x - KB: %02x/%02x\n", curSec->b3.keyA.keyToRead, curSec->b3.keyA.keyToWrite, curSec->b3.accessBits.keyToRead, curSec->b3.accessBits.keyToWrite, curSec->b3.keyB.keyToRead, curSec->b3.keyB.keyToWrite);
//			}
//		}
//	}
//	return FALSE;
//}


BOOLEAN Write_Card()
{
	BOOLEAN status = FALSE, isUnlocked = FALSE, isSuccessWrite = TRUE;
	BYTE uid[4], s;
	PMIFARE_CLASSIC_STD_SECTOR curSec;
	LCD_DrawTitleBox("Write");
	LCD_SetCursor(2, 2 * 6);
	if(initReaderAndCard())
	{
		status = ACR_PN532_GetMifareCard(uid, NULL, NULL);
		if(status)
		{
			isUnlocked = ACR_PN532_Unlock_MyChinaCard();
			if(!isUnlocked)
			{
				ACR_PN532_ReleaseMifareCard();
				ACR_PN532_GetMifareCard(uid, NULL, NULL);
				//GetWritePath(uid);
			}

			for(s = 0; (s < MIFARE_CLASSIC_SECTORS) && isSuccessWrite; s++)
			{
				curSec = &curCard.sectors[s];
				if(!(s % 4))
					LCD_SetCursor((s / 4) + 2, 1 * 6);
				LCD_Display_ASCIIChar(SB2CHAR(s), TRUE);
				LCD_Display_ASCIIChar('|', TRUE);

				isSuccessWrite = FALSE;
				if(isUnlocked || ACR_PN532_Auth_Block(MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_A, s * MIFARE_CLASSIC_BLOCKS_PER_SECTOR, MIMIFARE_BLANK_KEY, s ? curCard.uid : uid))
					isSuccessWrite = Write_Sector(s, curSec, !isUnlocked); //
				if(((s + 1) % 4))
					LCD_Display_ASCIIChar(' ', TRUE);
			}
			LCD_SetCursor(4 + 2, 1 * 6);
			ACR_PN532_ReleaseMifareCard();
		}
		finishReaderAndCard();
	}
	return status;
}

BOOLEAN Write_Sector(const BYTE sectorId, const PMIFARE_CLASSIC_STD_SECTOR sector, BOOLEAN fixDangerousACL)
{
	BOOLEAN is0, is1 = FALSE, is2 = FALSE, is3 = FALSE;
	is0 = Write_Block_Std(sectorId * MIFARE_CLASSIC_BLOCKS_PER_SECTOR + 0, &sector->b0);
	if(is0)
	{
		is1 = Write_Block_Std(sectorId * MIFARE_CLASSIC_BLOCKS_PER_SECTOR + 1, &sector->b1);
		if(is1)
		{
			is2 = Write_Block_Std(sectorId * MIFARE_CLASSIC_BLOCKS_PER_SECTOR + 2, &sector->b2);
			if(is2)
				is3 = Write_Block_Sec(sectorId * MIFARE_CLASSIC_BLOCKS_PER_SECTOR + 3, &sector->b3, fixDangerousACL);
		}
	}
	LCD_Display_ASCIIChar(is3 ? '#' : 'x', TRUE);
	LCD_Display_ASCIIChar((is0 && is1 && is2) ? 'd' : 'x', TRUE);
	return (is0 && is1 && is2 && is3);
}

BOOLEAN Write_Block_Std(const BYTE blockId, const PMIFARE_CLASSIC_STD_BLOCK block)
{
	BOOLEAN status = FALSE;
	if(block->isRead)
		status = ACR_PN532_MIFARE_Write(blockId, block->data);
	return status;
}


BOOLEAN Write_Block_Sec(const BYTE blockId, const PMIFARE_CLASSIC_SEC_BLOCK block, BOOLEAN fixDangerousACL)
{
	BOOLEAN status = FALSE;
	MIFARE_ACCESS_BITS bits;
	const BYTE *data;

	if(block->keyA.isKey || block->isRead || block->keyB.isKey)
	{
		if(block->keyA.isKey)
			memcpy(Read_Block_buffer, block->keyA.key, MIFARE_CLASSIC_KEY_SIZE);
		else memset(Read_Block_buffer, 0xff, MIFARE_CLASSIC_KEY_SIZE);
		
		if(block->isRead)
		{
			if(fixDangerousACL)
			{
				bits = block->bits;
				isDangerousAccessBits(&bits, TRUE);
				data = (BYTE *) &bits;
			}
			else data = (BYTE *) &block->bits;
		}
		else data = defaultACL;
		memcpy(Read_Block_buffer + MIFARE_CLASSIC_KEY_SIZE, data, 4);

		if(block->keyB.isKey)
			memcpy(Read_Block_buffer + MIFARE_CLASSIC_KEY_SIZE + 4, block->keyB.key, MIFARE_CLASSIC_KEY_SIZE);
		else memset(Read_Block_buffer + MIFARE_CLASSIC_KEY_SIZE + 4, 0xff, MIFARE_CLASSIC_KEY_SIZE);
		status = ACR_PN532_MIFARE_Write(blockId, Read_Block_buffer);
	}
	return status;
}


void Dump_Card(PMIFARE_CLASSIC_STD_CARD card)
{
	BYTE s;
	PMIFARE_CLASSIC_STD_SECTOR curSec;
	BOOLEAN isCurSecDangerousACL, isDangerousACL = FALSE;

	if(card)
	{
		for(s = 0; s < MIFARE_CLASSIC_SECTORS; s++)
		{
			curSec = &card->sectors[s];
			if(curSec->b3.isRead)
			{
				isCurSecDangerousACL = isDangerousAccessBits(&curSec->b3.bits, FALSE);
				isDangerousACL |= isCurSecDangerousACL;
			}
			else isCurSecDangerousACL = FALSE;

			if(!(s % 4))
				LCD_SetCursor((s / 4) + 2, 1 * 6);
			LCD_Display_ASCIIChar(SB2CHAR(s), TRUE);
			LCD_Display_ASCIIChar(isCurSecDangerousACL ? '!' : '|', TRUE);
			Dump_Sector(curSec);
			if(((s + 1) % 4))
				LCD_Display_ASCIIChar(' ', TRUE);
		}
		if(card->sectors[0].b0.isRead)
		{
			LCD_DisplayASCIIMessageOnNewLine("SAK:");
			LCD_Display_ASCIIChar((card->SAK == card->sectors[0].b0.data[5]) ? '*' : '!', TRUE);
			LCD_DisplayASCIIMessage(" ACL:");
			LCD_Display_ASCIIChar(isDangerousACL ? '!' : '*', TRUE);
		}
	}
}

void Dump_Sector(PMIFARE_CLASSIC_STD_SECTOR sector)
{
	char sep;
	Dump_SectorKeysState(sector);
	if(sector->b0.isRead && sector->b1.isRead && sector->b2.isRead)
	{
		if(isBlockEmpty(&sector->b0) && isBlockEmpty(&sector->b1) && isBlockEmpty(&sector->b2))
			sep = '_';
		else sep = 'd';
	}
	else sep = 'x';
	LCD_Display_ASCIIChar(sep, TRUE);
}

BOOLEAN mimifare_info_getUidAndSakFromB0(BYTE uid[4], BYTE *SAK)
{
	BOOLEAN status = ACR_PN532_MIFARE_Read(0, Read_Block_buffer);
	if(status)
	{
		if(uid)
			memcpy(uid, Read_Block_buffer, sizeof(uid));
		if(SAK)
			*SAK = Read_Block_buffer[5];
	}
	return status;
}

void displayUidAndSak(const BYTE uid[4], const BYTE SAK)
{
	char buffer[14];
	sprintf(buffer, "%02X%02X%02X%02X/%02X", uid[3], uid[2], uid[1], uid[0], SAK);
	LCD_DisplayASCIIMessage(buffer);
}

BOOLEAN mimifare_info_tryWriteDiffSak(const BYTE uid[4])
{
	BOOLEAN result = FALSE, isAuth;
	BYTE readed_SAK = Read_Block_buffer[5], tmpSak;
	Read_Block_buffer[5] = (readed_SAK == 0x08) ? 0x88 : 0x08;
	if(ACR_PN532_MIFARE_Write(0, Read_Block_buffer))
	{
		if(!uid)
			ACR_PN532_ReleaseMifareCard();
		if(ACR_PN532_GetMifareCard(NULL, NULL, &tmpSak))
			result = (tmpSak != Read_Block_buffer[5]);
		Read_Block_buffer[5] = readed_SAK;
		isAuth = uid ? ACR_PN532_Auth_Block(MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_A, 0, MIMIFARE_BLANK_KEY, uid) : ACR_PN532_Unlock_MyChinaCard();
		if(isAuth)
		{
			if(ACR_PN532_MIFARE_Write(0, Read_Block_buffer))
			{
				if(!uid)
					ACR_PN532_ReleaseMifareCard();
				ACR_PN532_GetMifareCard(NULL, NULL, NULL);
			}
		}
	}
	return result;
}

void mimifare_info(BOOLEAN isAggressive)
{
	BOOLEAN unlocked, readed = FALSE, b0writable = FALSE, diffSak = FALSE;
	BYTE sent_UID[4], sent_SAK, readed_UID[4], readed_SAK;

	LCD_DrawTitleBox(isAggressive ? "Info - Aggressive" : "Info");
	if(initReaderAndCard())
	{
		if(ACR_PN532_GetMifareCard(sent_UID, NULL, &sent_SAK))
		{
			LCD_DisplayASCIIMessageOnNewLine(NULL);
			LCD_DisplayASCIIMessageOnNewLine("Sent: ");
			displayUidAndSak(sent_UID, sent_SAK);

			if(ACR_PN532_Auth_Block(MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_A, 0, MIMIFARE_BLANK_KEY, sent_UID))
			{
				readed = mimifare_info_getUidAndSakFromB0(readed_UID, &readed_SAK);
				if(readed)
				{
					diffSak = (sent_SAK != readed_SAK);
					if(isAggressive)
					{
						b0writable = ACR_PN532_MIFARE_Write(0, Read_Block_buffer);
						if(b0writable && !diffSak)
							diffSak = mimifare_info_tryWriteDiffSak(sent_UID);
					}
				}
			}

			ACR_PN532_GetMifareCard(NULL, NULL, NULL);
			unlocked = ACR_PN532_Unlock_MyChinaCard();
			if(unlocked && !diffSak)
			{
				if(!readed)
					readed = mimifare_info_getUidAndSakFromB0(readed_UID, &readed_SAK);
				if(readed)
				{
					diffSak = (sent_SAK != readed_SAK);
					if(isAggressive && !b0writable && !diffSak)
						diffSak = mimifare_info_tryWriteDiffSak(NULL);
				}
			}

			if(readed)
			{
				LCD_DisplayASCIIMessageOnNewLine("B0  : ");
				displayUidAndSak(readed_UID, readed_SAK);
				if(isAggressive)
				{
					LCD_DisplayASCIIMessageOnNewLine("Native B0 Write: ");
					LCD_DisplayASCIIMessage(b0writable ? "YES" : "NO");
				}
			}
			LCD_DisplayASCIIMessageOnNewLine("Perso/Mod:       ");
			LCD_DisplayASCIIMessage(unlocked ? "YES" : "NO");
			
			if(readed && (isAggressive || diffSak))
			{
				LCD_DisplayASCIIMessageOnNewLine("Variable SAK:    ");
				LCD_DisplayASCIIMessage(!diffSak ? "YES" : "NO");
			}
		}
		ACR_PN532_ReleaseMifareCard();
		finishReaderAndCard();
	}
}

static portTASK_FUNCTION(mimifare_Main, pvParameters)
{
	BOOLEAN needToPrintMenu = TRUE;
	KeyInputEnumType key;
	LCD_SetBacklight(TRUE);
	SCard_Manager_SelectCard(SLOT_PICC);
	InitDefaultCard();
	do
	{
		if(needToPrintMenu)
		{
			LCD_DrawTitleBox("mimifare classic");
			LCD_DisplayASCIIMessageOnNewLine(NULL);
			LCD_DisplayASCIIMessageOnNewLine("1.Info F1.Aggressive");
			LCD_DisplayASCIIMessageOnNewLine("2.Read      F2.~Loop");
			LCD_DisplayASCIIMessageOnNewLine("3.Write    F3.Format");
			LCD_DisplayASCIIMessageOnNewLine("4.~Database Explorer");
		}
		needToPrintMenu = TRUE;
		key = getKeyPressed();
		switch(key)
		{
		case Key_Num1KeyInput:
		case Key_F1KeyInput:
			mimifare_info(key == Key_F1KeyInput);
			getKeyPressed();
			break;
		case Key_Num2KeyInput:
			if(Read_Card())
			{
				Dump_Card(&curCard);
				LCD_DisplayASCIIMessage("  Write?");
			}
			if(getYesNo())
			{
				Write_Card();
				getKeyPressed();
			}
			break;
		case Key_F2KeyInput:
			break;
		case Key_F3KeyInput:
			InitDefaultCard();
		case Key_Num3KeyInput:
			Write_Card();
			getKeyPressed();
			break;
		case Key_Num4KeyInput:
			break;
#ifdef WIN32
		case Key_Num0KeyInput:
		case Key_ClearKeyInput:
			ExitProcess(0);
			break;
#endif
		default:
			needToPrintMenu = FALSE;
			break;
		}
	} while(TRUE);
}

#ifdef WIN32

int main(int argc, char * argv[])
{
	mimifare_Main(NULL);
	return 0;
}

KeyInputEnumType getKeyPressed()
{
	KeyInputEnumType ret;
	int key = _getch();
	switch(key)
	{
	case 0x03:
	case 0x08:
	case 0x1b:
		ret = Key_ClearKeyInput;
		break;
	case ' ':
	case 0x0d:
		ret = Key_PowerKeyInput;
		break;
	case '0':
		ret = Key_Num0KeyInput;
		break;
	case '1':
		ret = Key_Num1KeyInput;
		break;
	case '2':
		ret = Key_Num2KeyInput;
		break;
	case '3':
		ret = Key_Num3KeyInput;
		break;
	case '4':
		ret = Key_Num4KeyInput;
		break;
	case '5':
		ret = Key_Num5KeyInput;
		break;
	case '6':
		ret = Key_Num6KeyInput;
		break;
	case '7':
		ret = Key_Num7KeyInput;
		break;
	case '8':
		ret = Key_Num8KeyInput;
		break;
	case '9':
		ret = Key_Num9KeyInput;
		break;
	case ';':
		ret = Key_F1KeyInput;
		break;
	case '<':
		ret = Key_F2KeyInput;
		break;
	case '=':
		ret = Key_F3KeyInput;
		break;
	case '>':
		ret = Key_F4KeyInput;
		break;
	case 'K':
		ret = Key_LeftKeyInput;
		break;
	case 'H':
		ret = Key_UpKeyInput;
		break;
	case 'P':
		ret = Key_DownKeyInput;
		break;
	case 'M':
		ret = Key_RightKeyInput;
		break;
	default:
		ret = Key_noKeyInput;
	}
	return ret;
}

SCARDCONTEXT hContext;
SCARDHANDLE hCard;
BOOLEAN initReaderAndCard()
{
	BOOLEAN status = FALSE;
	LONG scStatus;
	DWORD dwActiveProtocol;
	hContext = 0;
	hCard = 0;
	scStatus = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
	if(scStatus == SCARD_S_SUCCESS)
	{
		scStatus = SCardConnect(hContext, "ACS ACR122 0", SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx, &hCard, &dwActiveProtocol); // "ACS ACR89 Dual Reader PICC 0"
		status = (scStatus == SCARD_S_SUCCESS);
		if(!status)
		{
			printf("SCardConnect: 0x%08x\n", scStatus);
			SCardReleaseContext(hContext);
		}
	}
	else printf("SCardEstablishContext: 0x%08x\n", scStatus);
	return status;
}

void finishReaderAndCard()
{
	if(hCard)
	{
		SCardDisconnect(hCard, SCARD_UNPOWER_CARD);
		hCard = 0;
	}
	if(hContext)
	{
		SCardReleaseContext(hContext);
		hContext = 0;
	}
}

BOOLEAN RAW_SendAndRecv(const BYTE *pbData, const UINT16 cbData)
{
	BOOLEAN status = FALSE;
	DWORD rawSize = sizeof(RAW_SendAndRecv_Buffer);
	LONG scStatus;
	RAW_SendAndRecv_Size = sizeof(RAW_SendAndRecv_Buffer);
	scStatus = SCardTransmit(hCard, NULL, pbData, cbData, NULL, RAW_SendAndRecv_Buffer, &rawSize);
	status = (scStatus == SCARD_S_SUCCESS);
	if(status)
		RAW_SendAndRecv_Size = (UINT16) rawSize;
	else
	{
		RAW_SendAndRecv_Size = 0;
		printf("SCardTransmit: 0x%08x\n", scStatus);
	}
	return status;
}

void LCD_DisplayHexN(const UINT8 *pucDisplay, UINT8 Number)
{
	UINT8 i;
	for(i = 0; i < Number; i++)
		LCD_DisplayHex(pucDisplay[i]);
}


#else

void Application_Init(void)
{
    xTaskCreate(mimifare_Main, (signed portCHAR *) "mimifare", 128, NULL, 2, (xTaskHandle *) NULL);
}

KeyInputEnumType getKeyPressed()
{
	Key_MessageDataType keyBuffer; 
	Key_Ctrl_FlushMsgBuffer();
	if(Key_Msg_ReceiveKey(&keyBuffer, portMAX_DELAY) == TRUE)
		if(keyBuffer.eKeyStatus == Key_PressDown)
			return keyBuffer.eInputKey;
	return Key_noKeyInput;
}

BOOLEAN initReaderAndCard()
{
	BOOLEAN status = FALSE;
	UINT8 bSC = SCard_Manager_CardOn(TRUE, CVCC_3_VOLT, RAW_SendAndRecv_Buffer, &RAW_SendAndRecv_Size);
	if(bSC == SLOT_NO_ERROR)
	{
		if(RAW_SendAndRecv_Size == 20)
		{
			if(RAW_SendAndRecv_Buffer[12] == 0x03)
			{
				status = ((RAW_SendAndRecv_Buffer[13] == 0x00) && (RAW_SendAndRecv_Buffer[14] == 0x01)) || ((RAW_SendAndRecv_Buffer[13] == 0xff) && (RAW_SendAndRecv_Buffer[14] == 0x88));
				if(!status)
					LCD_DisplayASCIIMessageOnNewLine("*NOT* MIFARE 1K");
			}
			else LCD_DisplayASCIIMessageOnNewLine("*NOT* ISO1443-A");
		}
		else LCD_DisplayASCIIMessageOnNewLine("*NOT* correct ATR");
	}
	else
	{
		LCD_DisplayASCIIMessageOnNewLine("*NOT* CardOn: ");
		LCD_DisplayHex(bSC);
	}
	return status;
}

void finishReaderAndCard()
{
	SCard_Manager_CardOff();
}

BOOLEAN RAW_SendAndRecv(const BYTE *pbData, const UINT16 cbData)
{
	BYTE status = FALSE;
	RAW_SendAndRecv_Size = cbData;
	UINT8 bSC = SCard_Manager_SendBlock((BYTE *) pbData, RAW_SendAndRecv_Buffer, &RAW_SendAndRecv_Size);
	status = (bSC == SLOT_NO_ERROR);
	if(!status)
	{
		RAW_SendAndRecv_Size = 0;
		LCD_DisplayASCIIMessageOnNewLine("Send error: ");
		LCD_DisplayHex(bSC);
	}			
	return status;
}

#endif
