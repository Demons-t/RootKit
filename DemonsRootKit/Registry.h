#pragma once
#include "Data.h"

// 遍历注册表
ULONG EnumRegister(PVOID pOutputBuff, ULONG uBuffSize, PVOID pRegisterName);

// 创建子项
ULONG NewReg(PVOID pRegisterName);

// 删除注册表键
ULONG DeleteRegKey(PVOID pRegisterName);