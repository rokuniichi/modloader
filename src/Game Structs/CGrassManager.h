// Generated by IDAStruct_To_CHeader.py
#pragma once
#include <windows.h>
#include "CGrass.h"


#pragma pack(push, 1)
struct CGrassManager	// sizeof = 0xE18
{
	DWORD numActive;
	CGrass structs[32];
	DWORD type;
	DWORD pObjectList[4];
};
#pragma pack(pop)

static_assert(sizeof(CGrassManager) == 0xE18, "Incorrect struct size: CGrassManager");