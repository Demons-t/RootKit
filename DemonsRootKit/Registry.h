#pragma once
#include "Data.h"

// ����ע���
ULONG EnumRegister(PVOID pOutputBuff, ULONG uBuffSize, PVOID pRegisterName);

// ��������
ULONG NewReg(PVOID pRegisterName);

// ɾ��ע����
ULONG DeleteRegKey(PVOID pRegisterName);