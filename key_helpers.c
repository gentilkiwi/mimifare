#include "key_helpers.h"

const DEFAULT_KEYS_WITH_HELPER defaultKeysWithHelper[17] = {
	{{0x48, 0x45, 0x58, 0x41, 0x43, 0x54}, 0, "Intratone/HEXACT/COGELEC", MIFARE_HELPER_Intratone},
	{{0x88, 0x29, 0xda, 0x9d, 0xaf, 0x76}, 0, "URMET/CAPTIV/STARPROX", NULL},

	{{0x41, 0x4c, 0x41, 0x52, 0x4f, 0x4e}, 0, "Noralsy", MIFARE_HELPER_Noralsy}, // 0 et 1 (le reste... ?) + autre badge aucune clé
	{{0x4a, 0x63, 0x52, 0x68, 0x46, 0x77}, 0, "Comelit/Immotec/GemPlus", MIFARE_HELPER_Comelit},
	{{0x44, 0x41, 0x56, 0x49, 0x44, 0x42}, 0, "CDVI", MIFARE_HELPER_CDVI},
	{{0x02, 0x12, 0x09, 0x19, 0x75, 0x91}, 0, "BTicino", NULL},
	{{0x2e, 0xf7, 0x20, 0xf2, 0xaf, 0x76}, 0, "URMET/CAPTIV ?", MIFARE_HELPER_URMET},
	{{0x41, 0x43, 0x41, 0x4f, 0x52, 0x50}, 0, "PROAC", MIFARE_HELPER_PROAC},
	{{0xa0, 0x00, 0x03, 0x00, 0x0a, 0x88}, 0, "- ? -", NULL},

	{{0x31, 0x4b, 0x49, 0x47, 0x49, 0x56}, 0, "Vigik 1", MIFARE_HELPER_Vigik1},
	{{0x56, 0x4c, 0x50, 0x5f, 0x4d, 0x41}, 0, "Vigik B demo", NULL},

	{{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, 0, "EMPTY", NULL},
	{{0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5}, 0, "NXP MAD A", NULL},
	{{0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5}, 0, "NXP Def B", NULL},
	{{0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7}, 0, "NXP NDef A", NULL},
	{{0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}, 0, "DefA", NULL},
	{{0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb}, 0, "DefB", NULL},
};

const BYTE MIFARE_HELPER_Intratone_DefaultKeys[MIFARE_CLASSIC_SECTORS][MIFARE_CLASSIC_KEY_SIZE] = {
	{0xa2, 0x2a, 0xe1, 0x29, 0xc0, 0x13}, {0x49, 0xfa, 0xe4, 0xe3, 0x84, 0x9f}, {0x38, 0xfc, 0xf3, 0x30, 0x72, 0xe0}, {0x8a, 0xd5, 0x51, 0x7b, 0x4b, 0x18},
	{0x50, 0x93, 0x59, 0xf1, 0x31, 0xb1}, {0x6c, 0x78, 0x92, 0x8e, 0x13, 0x17}, {0xaa, 0x07, 0x20, 0x01, 0x87, 0x38}, {0xa6, 0xca, 0xc2, 0x88, 0x64, 0x12},
	{0x62, 0xd0, 0xc4, 0x24, 0xed, 0x8e}, {0xe6, 0x4a, 0x98, 0x6a, 0x5d, 0x94}, {0x8f, 0xa1, 0xd6, 0x01, 0xd0, 0xa2}, {0x89, 0x34, 0x73, 0x50, 0xbd, 0x36},
	{0x66, 0xd2, 0xb7, 0xdc, 0x39, 0xef}, {0x6b, 0xc1, 0xe1, 0xae, 0x54, 0x7d}, {0x22, 0x72, 0x9a, 0x9b, 0xd4, 0x0f}, {0x48, 0x45, 0x58, 0x41, 0x43, 0x54}, // A & B
};
const BYTE *MIFARE_HELPER_Intratone(BYTE nbSector, BYTE type)
{
	const BYTE *result = NULL;
	if(type == MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_A)
		result = MIFARE_HELPER_Intratone_DefaultKeys[(sizeof(MIFARE_HELPER_Intratone_DefaultKeys) / MIFARE_CLASSIC_KEY_SIZE) - 1];
	else if(nbSector < (sizeof(MIFARE_HELPER_Intratone_DefaultKeys) / MIFARE_CLASSIC_KEY_SIZE))
		result = MIFARE_HELPER_Intratone_DefaultKeys[nbSector];
	return result;
}

const BYTE MIFARE_HELPER_URMET_DefaultKeys[2][MIFARE_CLASSIC_KEY_SIZE] = {{0x2e, 0xf7, 0x20, 0xf2, 0xaf, 0x76}/*URMET A*/, {0xbf, 0x1f, 0x44, 0x24, 0xaf, 0x76}/*URMET B*/};
const BYTE *MIFARE_HELPER_URMET(BYTE nbSector, BYTE type)
{
	const BYTE *result = NULL;
	if(type == MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_A)
		result = MIFARE_HELPER_URMET_DefaultKeys[0];
	else if(type == MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_B)
		result = MIFARE_HELPER_URMET_DefaultKeys[1];
	return result;
}

const BYTE MIFARE_HELPER_Noralsy_DefaultKeys[2][MIFARE_CLASSIC_KEY_SIZE] = {{0x41, 0x4c, 0x41, 0x52, 0x4f, 0x4e}/*Noralsy A*/, {0x42, 0x4c, 0x41, 0x52, 0x4f, 0x4e}/*Noralsy B*/};
const BYTE *MIFARE_HELPER_Noralsy(BYTE nbSector, BYTE type)
{
	const BYTE *result = NULL;
	if(type == MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_A)
		result = MIFARE_HELPER_Noralsy_DefaultKeys[0];
	else if(type == MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_B)
		result = MIFARE_HELPER_Noralsy_DefaultKeys[1];
	return result;
}

const BYTE MIFARE_HELPER_PROAC_DefaultKeys[2][MIFARE_CLASSIC_KEY_SIZE] = {{0x41, 0x43, 0x41, 0x4f, 0x52, 0x50}/*PROAC A*/, {0x42, 0x43, 0x41, 0x4f, 0x52, 0x50}/*PROAC B*/};
const BYTE *MIFARE_HELPER_PROAC(BYTE nbSector, BYTE type)
{
	const BYTE *result = NULL;
	if(type == MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_A)
		result = MIFARE_HELPER_PROAC_DefaultKeys[0];
	else if(type == MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_B)
		result = MIFARE_HELPER_PROAC_DefaultKeys[1];
	return result;
}

const BYTE MIFARE_HELPER_CDVI_A[3][MIFARE_CLASSIC_KEY_SIZE] = {{0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5}/*A(0,f)*/, {0x43, 0x44, 0x56, 0x49, 0x52, 0x43}/*A(1-7)*/ , {0x43, 0x41, 0x43, 0x44, 0x56, 0x49}/*A(8-e)*/};
const BYTE MIFARE_HELPER_CDVI_B[2][MIFARE_CLASSIC_KEY_SIZE] = {{0x44, 0x41, 0x56, 0x49, 0x44, 0x42}/*B(0,8-e)*/, {0x4a, 0x4c, 0x47, 0x4f, 0x52, 0x4d}/*B(1-7,f)*/};
const BYTE *MIFARE_HELPER_CDVI(BYTE nbSector, BYTE type)
{
	const BYTE *result = NULL;
	if(type == MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_A)
	{
		if((nbSector == 0x0) || (nbSector == 0xf))
			result = MIFARE_HELPER_CDVI_A[0];
		else if((nbSector >= 0x1) && (nbSector <= 0x7))
			result = MIFARE_HELPER_CDVI_A[1];
		else if((nbSector >= 0x8) && (nbSector <= 0xe))
			result = MIFARE_HELPER_CDVI_A[2];
	}
	else if(type == MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_B)
	{
		if((nbSector == 0x0) || ((nbSector >= 0x8) && (nbSector <= 0xe)))
			result = MIFARE_HELPER_CDVI_B[0];
		else if((nbSector == 0xf) || ((nbSector >= 0x1) && (nbSector <= 0x7)))
			result = MIFARE_HELPER_CDVI_B[1];
	}
	return result;
}

const BYTE MIFARE_HELPER_Comelit_DefaultKeys[2][MIFARE_CLASSIC_KEY_SIZE] = {{0x4a, 0x63, 0x52, 0x68, 0x46, 0x77}/*JcRhFw*/, {0x53, 0x66, 0x53, 0x64, 0x4c, 0x65}/*SfSdLe*/};
const BYTE *MIFARE_HELPER_Comelit(BYTE nbSector, BYTE type)
{
	const BYTE *result = NULL;
	if(type == MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_A)
		result = MIFARE_HELPER_Comelit_DefaultKeys[0];
	else if(type == MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_B)
		result = MIFARE_HELPER_Comelit_DefaultKeys[1];
	return result;
}

const BYTE MIFARE_HELPER_Vigik1_DefaultKeys[2][MIFARE_CLASSIC_KEY_SIZE] = {{0x31, 0x4b, 0x49, 0x47, 0x49, 0x56}/*Vigik1 A*/, {0xba, 0x5b, 0x89, 0x5d, 0xa1, 0x62}/*Vigik1 B ?*/};
const BYTE *MIFARE_HELPER_Vigik1(BYTE nbSector, BYTE type)
{
	const BYTE *result = NULL;
	if(type == MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_A)
		return MIFARE_HELPER_Vigik1_DefaultKeys[0];
	else if(type == MIFARE_CLASSIC_CMD_AUTHENTICATION_KEY_B)
		return MIFARE_HELPER_Vigik1_DefaultKeys[1];
	return result;
}
