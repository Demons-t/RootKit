#include "Registry.h"

// 遍历注册表
ULONG EnumRegister(PVOID pOutputBuff, ULONG uBuffSize, PVOID pRegisterName)
{
	ULONG uCount = 0;
	ULONG uSize = 0;
	ULONG i = 0;
	NTSTATUS status;
	PKEY_FULL_INFORMATION pKeyInfo;
	OBJECT_ATTRIBUTES objAttributes;
	HANDLE hRegister;
	UNICODE_STRING uRegUnicodeString;
	PREGISTRY pReg = (PREGISTRY)pOutputBuff;

	// 初始化字符串
	RtlInitUnicodeString(&uRegUnicodeString, pRegisterName);
	InitializeObjectAttributes(&objAttributes, &uRegUnicodeString, OBJ_CASE_INSENSITIVE, NULL, NULL);

	// 打开注册表
	status = ZwOpenKey(&hRegister, KEY_ALL_ACCESS, &objAttributes);
	
	// 查询 VALUE 的大小
	ZwQueryKey(hRegister, KeyFullInformation, NULL, 0, &uSize);
	pKeyInfo = (PKEY_FULL_INFORMATION)ExAllocatePoolWithTag(PagedPool, uSize, 'abcd');
	ZwQueryKey(hRegister, KeyFullInformation, pKeyInfo, uSize, &uSize);
	uCount = pKeyInfo->SubKeys + pKeyInfo->Values;

	// 判断传进来的大小是否正确
	if (uCount * sizeof(REGISTRY) > uBuffSize)
		return uCount * sizeof(REGISTRY);

	// 遍历子项的名字
	ULONG uIndex = 0;
	for (i = 0; i < pKeyInfo->SubKeys; i++)
	{
		pReg[uIndex].uType = 0;
		ULONG uSize = 0;
		ZwEnumerateKey(hRegister, i, KeyBasicInformation, NULL, 0, &uSize);
		PKEY_BASIC_INFORMATION pKeyBasic = (PKEY_BASIC_INFORMATION)ExAllocatePoolWithTag(PagedPool, uSize, '1111');
		ZwEnumerateKey(hRegister, i, KeyBasicInformation, pKeyBasic, uSize, &uSize);
		RtlCopyMemory(&pReg[uIndex].cKeyName, pKeyBasic->Name, pKeyBasic->NameLength);
		ExFreePoolWithTag(pKeyBasic, '1111');
		uIndex++;
	}
	for (i = 0; i < pKeyInfo->Values; i++)
	{
		pReg[uIndex].uType = 1;
		PKEY_VALUE_FULL_INFORMATION pKeyValueFull;

		// 查询单个 VALUE 的大小
		ZwEnumerateValueKey(hRegister, i, KeyValueFullInformation, NULL, 0, &uSize);
		pKeyValueFull = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePoolWithTag(PagedPool, uSize, '1234');

		// 查询单个 VALUE 的详情
		ZwEnumerateValueKey(hRegister, i, KeyValueFullInformation, pKeyValueFull, uSize, &uSize);

		// 获取值的名字
		RtlCopyMemory(&pReg[uIndex].cValueName, pKeyValueFull->Name, pKeyValueFull->NameLength);
		pReg[uIndex].uValueType = pKeyValueFull->Type;
		RtlCopyMemory(pReg[uIndex].uValue, pKeyValueFull + pKeyValueFull->DataOffset, pKeyValueFull->DataLength);
		ExFreePoolWithTag(pKeyValueFull, '1234');
		uIndex++;
	}

	ExFreePoolWithTag(pKeyInfo, 'abcd');
	ZwClose(hRegister);

	return uCount * sizeof(REGISTRY);
}

// 创建子项
ULONG NewReg(PVOID pRegisterName)
{
	OBJECT_ATTRIBUTES objectAttributes;
	UNICODE_STRING uKeyName;
	NTSTATUS status;
	HANDLE hRegister;
	RtlInitUnicodeString(&uKeyName, pRegisterName);
	
	// 初始化对象信息
	InitializeObjectAttributes(
		&objectAttributes,
		&uKeyName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL);
	status = ZwCreateKey(
		&hRegister,
		KEY_ALL_ACCESS,
		&objectAttributes,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		NULL);
	if (NT_SUCCESS(status))
		ZwClose(hRegister);

	return status;
}

// 删除注册表键
ULONG DeleteRegKey(PVOID pRegisterName)
{
	OBJECT_ATTRIBUTES objectAttributes;
	UNICODE_STRING uKeyName;
	NTSTATUS status;
	HANDLE hRegister;
	RtlInitUnicodeString(&uKeyName, pRegisterName);
	
	// 初始化注册表的路径
	InitializeObjectAttributes(
		&objectAttributes,
		&uKeyName,
		OBJ_CASE_INSENSITIVE,
		NULL, NULL);

	// 打开注册表，获取注册表键的路径
	status = ZwOpenKey(&hRegister, KEY_ALL_ACCESS, &objectAttributes);
	if (NT_SUCCESS(status))
	{
		// 删除注册表键
		status = ZwDeleteKey(hRegister);
		ZwClose(hRegister);
	}

	return status;
}
