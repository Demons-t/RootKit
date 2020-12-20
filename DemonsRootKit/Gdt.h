#pragma once
#include "Data.h"
#include <Windef.h>

// ±éÀú GDT
ULONG GetGdtInfo(PVOID pOutputBuff, ULONG uBuffSize);
