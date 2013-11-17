// Generated by IDAStruct_To_CHeader.py
#pragma once
#include <windows.h>
#include "RwV3D.h"


#pragma pack(push, 1)
struct CMarker	// sizeof = 0x28
{
	DWORD Color;
	DWORD EntityHandle;
	RwV3D Pos;
	WORD _Index;
	BYTE _undefined_0;
	BYTE _undefined_1;
	DWORD _SphereRadius;
	WORD Size;
	BYTE _undefined_2;
	BYTE _undefined_3;
	DWORD field_20;
	BYTE Icon;
	BYTE _Flag;
	BYTE _RadarMode_or_EntityType;
	BYTE field_27;
};
#pragma pack(pop)

static_assert(sizeof(CMarker) == 0x28, "Incorrect struct size: CMarker");