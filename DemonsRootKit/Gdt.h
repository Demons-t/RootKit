#pragma once
#include "Data.h"
#include <Windef.h>

// ���� GDT
ULONG GetGdtInfo(PVOID pOutputBuff, ULONG uBuffSize);
