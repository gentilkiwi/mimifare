#ifndef KEY_HELPERS_H
#define KEY_HELPERS_H
#include "globals.h"
#include "acr_pn532.h"

typedef const BYTE* (* PMIFARE_HELPER) (BYTE nbSector, BYTE type);

typedef struct _DEFAULT_KEYS_WITH_HELPER {
	const BYTE key[MIFARE_CLASSIC_KEY_SIZE];
	const BYTE type; // not used;
	const char *descr;
	PMIFARE_HELPER helper;
} DEFAULT_KEYS_WITH_HELPER, *PDEFAULT_KEYS_WITH_HELPER;

const DEFAULT_KEYS_WITH_HELPER defaultKeysWithHelper[17];
#define MIMIFARE_BLANK_KEY defaultKeysWithHelper[(sizeof(defaultKeysWithHelper) / sizeof(DEFAULT_KEYS_WITH_HELPER)) - 6].key

const BYTE *MIFARE_HELPER_Intratone(BYTE nbSector, BYTE type);
const BYTE *MIFARE_HELPER_URMET(BYTE nbSector, BYTE type);
const BYTE *MIFARE_HELPER_Noralsy(BYTE nbSector, BYTE type);
const BYTE *MIFARE_HELPER_PROAC(BYTE nbSector, BYTE type);
const BYTE *MIFARE_HELPER_CDVI(BYTE nbSector, BYTE type);
const BYTE *MIFARE_HELPER_Comelit(BYTE nbSector, BYTE type);
const BYTE *MIFARE_HELPER_Vigik1(BYTE nbSector, BYTE type);

#endif
