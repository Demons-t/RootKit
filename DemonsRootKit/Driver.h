#pragma once
#include "Data.h"

// 清理空间
VOID FreePool(PDRIVER pTemp, PDRIVER pDriverHead, ULONG uCount);

// 遍历驱动
ULONG EnumDriver(PVOID pOutBuff, ULONG uCoutSize, PDRIVER_OBJECT pDriverObject);

// 隐藏驱动
UINT8 HideDriver(UNICODE_STRING uDriverName, PDRIVER_OBJECT pDriverObject);

