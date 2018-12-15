#ifndef ACR89_H
#define ACR89_H

/* CardManager */
#define SCARD_DATA_BLOCK_MAX_SIZE				262	// Application should prepare a receive buffer at this size

#define CVCC_0_VOLT		0
#define CVCC_1_8_VOLT	1
#define CVCC_3_VOLT		2
#define CVCC_5_VOLT		3

#define SLOT_NO_ERROR							0x81
#define SLOTERROR_UNKNOWN						0x82
#define SLOTERROR_CMD_NOT_SUPPORTED				0x00
#define SLOTERROR_BAD_LENTGH					0x01
#define SLOTERROR_BAD_SLOT						0x05
#define SLOTERROR_BAD_POWERSELECT				0x07
#define SLOTERROR_BAD_PROTOCOLNUM				0x07
#define SLOTERROR_BAD_CLOCKCOMMAND				0x07
#define SLOTERROR_BAD_ABRFU_3B					0x07
#define SLOTERROR_BAD_ABRFU_2B					0x08
#define SLOTERROR_BAD_LEVELPARAMETER			0x08
#define SLOTERROR_BAD_FIDI						0x0A
#define SLOTERROR_BAD_T01CONVCHECKSUM			0x0B
#define SLOTERROR_BAD_GUARDTIME					0x0C
#define SLOTERROR_BAD_WAITINGINTEGER			0x0D
#define SLOTERROR_BAD_CLOCKSTOP					0x0E
#define SLOTERROR_BAD_IFSC						0x0F
#define SLOTERROR_BAD_NAD						0x10
#define SLOTERROR_PIN_CANCELLED					0xEF
#define SLOTERROR_CMD_SLOT_BUSY					0xE0
#define SLOTERROR_PIN_TIMEOUT					0xF0
#define SLOTERROR_WRONG_CARD_TYPE				0xF1
#define SLOTERROR_BUSY_WITH_AUTO_SEQUENCE		0xF2
#define SLOTERROR_DEACTIVATED_PROTOCOL			0xF3
#define SLOTERROR_PROCEDURE_BYTE_CONFLICT		0xF4
#define SLOTERROR_ICC_CLASS_NOT_SUPPORTED		0xF5
#define SLOTERROR_ICC_PROTOCOL_NOT_SUPPORTED	0xF6
#define SLOTERROR_BAD_ATR_TCK					0xF7
#define SLOTERROR_BAD_ATR_TS					0xF8
#define SLOTERROR_HW_ERROR						0xFB
#define SLOTERROR_XFR_OVERRUN					0xFC
#define SLOTERROR_XFR_PARITY_ERROR				0xFD
#define SLOTERROR_ICC_MUTE						0xFE
#define SLOTERROR_CMD_ABORTED					0xFF

#define SLOT_CARDIN                 0xC1
#define SLOT_CARDOUT                0xC0

#define SLOT_PICC	0
#define SLOT_ICC1	1
#define SLOT_ICC2	2
#define SLOT_SAM1	3
#define SLOT_SAM2	4
#define SLOT_SAM3	5

#define SCARD_MSG_UNKNOWN						0x00
#define SCARD_MSG_CARDINSERTED					0x01
#define SCARD_MSG_CARDREMOVED					0x02

typedef struct _SCARD_MSG_TYPE {
	UINT8	ucMessage:5;
	UINT8	ucData:3;
} SCARD_MSG_TYPE, *PSCARD_MSG_TYPE;

BOOLEAN SCard_Manager_Msg_Receive(SCARD_MSG_TYPE *pMsgBuffer, portTickType TimeOut);
void SCard_Manager_SelectCard(UINT8 ucCard);
UINT8 SCard_Manager_CardOn(BOOLEAN bAutoVoltage, UINT8 ucVoltage, UCHAR *pucReceiveBuffer, UINT16 *pusReceiveSize);
BOOLEAN SCard_Manager_CardOff(void);
UINT8 SCard_Manager_SendBlock(UCHAR *pucCmdBlockBuffer, UCHAR *pucResBlockBuffer, UINT16 *pusBufferSize);

/* Battery */
UINT32 Battery_GetMilliVolt(void);
UINT8 Battery_GetPercent(void);
BOOLEAN Battery_WaitChargeStateChangeMsg(portTickType TimeOut, BOOLEAN *pbChargeState);


/* Buzzer */
typedef struct _Buzzer_Script {
	UINT8	BuzzerTime: 7;
	UINT8	BuzzerOn: 1;
} Buzzer_Script, Buzzer_ScriptDataType, *PBuzzer_Script;

BOOLEAN Buzzer_Msg_SendScript(Buzzer_ScriptDataType const* const Script);
BOOLEAN Buzzer_Msg_IsPlaying(void);

/* KeyPad */
typedef enum _KeyStatus {
	Key_release			= 0,
	Key_PressDown		= 1,
	Key_shortPress		= 2,
	Key_longPress		= 3
} KeyStatus, KeyStatusEnumType, *PKeyStatus;

typedef enum _KeyInput {
	Key_noKeyInput		= 0,
	Key_ClearKeyInput	= 1,
	Key_Num0KeyInput	= 2,
	Key_RightKeyInput	= 4,
	Key_Num7KeyInput	= 6,
	Key_Num8KeyInput	= 7,
	Key_Num9KeyInput	= 8,
	Key_LeftKeyInput	= 9,
	Key_Num4KeyInput	= 11,
	Key_Num5KeyInput	= 12,
	Key_Num6KeyInput	= 13,
	Key_DownKeyInput	= 14,
	Key_Num1KeyInput	= 16,
	Key_Num2KeyInput	= 17,
	Key_Num3KeyInput	= 18,
	Key_UpKeyInput		= 19,
	Key_F1KeyInput		= 21,
	Key_F2KeyInput		= 22,
	Key_F3KeyInput		= 23,
	Key_F4KeyInput		= 24,
	Key_PowerKeyInput	= 26
} KeyInput, KeyInputEnumType, *PKeyInput;

typedef struct _Key_Message {
	KeyStatusEnumType   eKeyStatus;
	KeyInputEnumType    eInputKey;
} Key_Message, Key_MessageDataType, *PKey_Message;

BOOLEAN Key_Port_IsAnyKeyDown(void);
BOOLEAN Key_Msg_ReceiveKey(Key_MessageDataType* Key_MsgRecBuffer, portTickType TimeOut);
KeyInputEnumType Key_Msg_GetKeyPressing(void);
void Key_Ctrl_FlushMsgBuffer(void);
void Key_Ctrl_ScanLock(void);
void Key_Ctrl_ScanUnlock(void);
void Key_Ctrl_SetLongPressThreshold(UINT8 Seconds);
UINT16 Key_Tim_GetKeyDownTime(void);

/* EEPROM */
BOOLEAN EEPROM_Write(UINT16 usAddress, const UINT8 *pucData, UINT16 usSize);
BOOLEAN EEPROM_Read(UINT16 usAddress, UINT8 *pucData, UINT16 usSize);

/* EXRTC */
BOOLEAN EXRTC_Write_Ram(UINT16 usAddress, const UINT8 *pucData, UINT16 usSize);
BOOLEAN EXRTC_Read_Ram(UINT16 usAddress, UINT8 *pucData, UINT16 usSize);
BOOLEAN EXRTC_Write_Time(UINT8 ucHour, UINT8 ucMinute, UINT8 ucSecond);
BOOLEAN EXRTC_Write_TimeBCD(UINT8 ucHour, UINT8 ucMinute, UINT8 ucSecond);
BOOLEAN EXRTC_Read_Time(UINT8 *pucHour, UINT8 *pucMinute, UINT8 *pucSecond);
BOOLEAN EXRTC_Write_Date(UINT8 ucYear, UINT8 ucMonth, UINT8 ucWeekday, UINT8 ucDay);
BOOLEAN EXRTC_Write_DateBCD(UINT8 ucYear, UINT8 ucMonth, UINT8 ucWeekday, UINT8 ucDay );
BOOLEAN EXRTC_Read_Date(UINT8 *pucYear, UINT8 *pucMonth, UINT8 *pucWeekday, UINT8 *pucDay);

/* LCD */
#define LCD_MAX_ROW		(8 - 1)
#define LCD_MAX_COLUMN	(128 - 1)

#define LCD_WIDTH		(LCD_MAX_COLUMN + 1)
#define LCD_HEIGHT		8

#define ENG_FONT_WIDTH	6
#define ENG_FONT_HEIGHT	8

#define INDEX_FULL		0
#define INDEX_ROW		1
#define INDEX_COLUMN	2

void LCD_Init_Display(void);
BOOLEAN LCD_SetCursor(UINT8 ucLcdRowPosition, UINT8 ucLcdColumnPosition);
void LCD_GetCursor(UINT8 *pucLcdRowPosition, UINT8 *pucLcdColumnPosition);
void LCD_Display_ASCIIChar(UINT8 ucLcdCharacterToDisplay, BOOLEAN bSetNextPosCur);
void LCD_DisplayASCIIMessage(const UINT8 *LcdMessageToDisplay);
void LCD_ClearDisplay(UINT8 index, UINT8 ucNumber);
void LCD_SetContrast(UINT8 contrast_level);
void LCD_SetBacklight(BOOLEAN bTurnOn);
void LCD_Display_Cursor(void);
void LCD_Clear_Cursor(void);
void LCD_Display_Page(UINT8 *pucBitmap);
void LCD_DisplayGraphic(UINT8 ucLcdRowNumber, UINT8 ucLcdColumnNumber, const UINT8 *pucBitMap );
void LCD_DisplayOn(BOOLEAN bTurnOn);
void LCD_DisplayDecimal(UINT32 ulDecimal);
void LCD_DisplayHex(UINT8 ucDisplay);
void LCD_DisplayHexN(const UINT8 *pucDisplay, UINT8 Number);
void LCD_DisplayFloat(UINT32 ulDecimal, UINT8 Exp);
void LCD_DrawTitleBox(const UINT8 *TitleMessage);

/* SFlash */
#define SF_ERASE_4K		0x20
#define SF_ERASE_64K	0xD8

#define SFLASH_MAX_ADDR	0x7FFFF

typedef enum _SFlash_EraseBlock {
	ERASE_4K	= SF_ERASE_4K,
	ERASE_64K	= SF_ERASE_64K
} SFlash_EraseBlock, SFlash_EraseBlockType, *PSFlash_EraseBlock;

void SerialFlash_ReadDataBytes(UINT32 ulAddress, UINT8* pucReceiveBufferPtr, UINT32 ulLength);
BOOLEAN SerialFlash_Erase_Block(SFlash_EraseBlockType BlockType, UINT32 ulAddress);
BOOLEAN SerialFlash_WriteDataBytes(UINT32 ulAddress, const UINT8* pucWriteBufferPtr, UINT32 ulLength);


/* RS232 */
typedef enum _Parity {
	No_Parity   = 0,
	Odd_Parity  = 1,
	Even_Parity = 2
} Parity, ParityEnumType, *PParity;

typedef struct _RS232_Parameter {
	UINT32			Baudrate;
	ParityEnumType	ParityMode;
	BOOLEAN			SevenOrEightDataBit;	// TRUE for 7 bit, FALSE for 8 bit
	BOOLEAN			TwoOrOneStopBit;		// TRUE for 2 stop bit, FALSE for 1 stop bit
} RS232_Parameter, RS232_ParamDataType, *PRS232_Parameter;

BOOLEAN RS232_Config(const RS232_ParamDataType* Param);
BOOLEAN RS232_OpenPort(UINT32* pulHandle);
BOOLEAN RS232_ClosePort(UINT32 ulHandle);
UINT16 RS232_ReceivedDataNumber(UINT32 ulHandle);
BOOLEAN RS232_Receive(UINT32 ulHandle, UINT8* pucRecBuf, UINT16* pusLen, portTickType TimeOut);
BOOLEAN RS232_Send(UINT32 ulHandle, const UINT8* pucSndBuf, UINT16 usLen);

/* IO */
typedef enum _IO_VirtualName {
	IO_LED0Output	= 14,
	IO_LED1Output	= 15,
	IO_LED2Output	= 16,
	IO_LED3Output	= 17,
	IO_LED4Output	= 18,
	IO_LED5Output	= 19,
	IO_LED6Output	= 20,
	IO_LED7Output	= 21
} IO_VirtualName, IO_VirtualNameType, *PIO_VirtualName;

typedef enum _IO_ActiveInactiveState {
	IO_InactiveState	= 0,
	IO_ActiveState		= 1,
	IO_UndefineState	= 2
} IO_ActiveInactiveState, IO_ActiveInactiveStateType, *PIO_ActiveInactiveState;

IO_ActiveInactiveStateType IO_ReadDigitalOutput(IO_VirtualNameType Address);
void IO_WriteDigitalOutput(IO_ActiveInactiveStateType State, IO_VirtualNameType Address);
void IO_WriteLEDOutput(UINT8 ucLED);
UINT8 IO_ReadLEDOutput(void);
void IO_SystemSleep(void);
void IO_SystemShutdown(void);
void IO_SystemRestart(void);
const UCHAR* IO_GetSystemVersion(void);

/* RF */
void RFIF_Sleep(void);
void RFIF_WakeUp(void);

/* Printer */
typedef struct _PRT_PAGE_MODE_PARAM {
	UINT16  HorizontalOrigin_X;
	UINT16  VerticalOrigin_Y;
	UINT16  PrintWidth_X;
	UINT16  PrintHeight_Y;
	UINT8   ucLineSpace;
} PRT_PAGE_MODE_PARAM, *PPRT_PAGE_MODE_PARAM;

BOOLEAN Printer_Open(void);
BOOLEAN Printer_Close(void);
BOOLEAN Printer_Reset(void);
BOOLEAN Printer_FeedPaper(UINT8 Num);
BOOLEAN Printer_SetLineSpaceSM(UINT8 Num);
BOOLEAN Printer_PrintStrSM(const UCHAR *Str);
BOOLEAN Printer_PrintStrPM(const PRT_PAGE_MODE_PARAM *Param, const UCHAR *Str);
BOOLEAN Printer_PrintDataSM(const UINT8 *Data, UINT16 Size);
BOOLEAN Printer_PrintDataPM(const PRT_PAGE_MODE_PARAM *Param, const UINT8 *Data, UINT16 Size);
BOOLEAN Printer_PrintImage(const UINT8 *pucBitMap, UINT16 Width, UINT16 High, BOOLEAN PrtImgMode);

#endif
