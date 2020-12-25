#include "Registry.h"

// ����ע���
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

	// ��ʼ���ַ���
	RtlInitUnicodeString(&uRegUnicodeString, pRegisterName);
	InitializeObjectAttributes(&objAttributes, &uRegUnicodeString, OBJ_CASE_INSENSITIVE, NULL, NULL);

	// ��ע���
	status = ZwOpenKey(&hRegister, KEY_ALL_ACCESS, &objAttributes);
	
	// ��ѯ VALUE �Ĵ�С
	ZwQueryKey(hRegister, KeyFullInformation, NULL, 0, &uSize);
	pKeyInfo = (PKEY_FULL_INFORMATION)ExAllocatePoolWithTag(PagedPool, uSize, 'abcd');
	ZwQueryKey(hRegister, KeyFullInformation, pKeyInfo, uSize, &uSize);
	uCount = pKeyInfo->SubKeys + pKeyInfo->Values;

	// �жϴ������Ĵ�С�Ƿ���ȷ
	if (uCount * sizeof(REGISTRY) > uBuffSize)
		return uCount * sizeof(REGISTRY);

	// �������������
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

		// ��ѯ���� VALUE �Ĵ�С
		ZwEnumerateValueKey(hRegister, i, KeyValueFullInformation, NULL, 0, &uSize);
		pKeyValueFull = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePoolWithTag(PagedPool, uSize, '1234');

		// ��ѯ���� VALUE ������
		ZwEnumerateValueKey(hRegister, i, KeyValueFullInformation, pKeyValueFull, uSize, &uSize);

		// ��ȡֵ������
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

// ��������
ULONG NewReg(PVOID pRegisterName)
{
	OBJECT_ATTRIBUTES objectAttributes;
	UNICODE_STRING uKeyName;
	NTSTATUS status;
	HANDLE hRegister;
	RtlInitUnicodeString(&uKeyName, pRegisterName);
	
	// ��ʼ��������Ϣ
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

// ɾ��ע����
ULONG DeleteRegKey(PVOID pRegisterName)
{
	OBJECT_ATTRIBUTES objectAttributes;
	UNICODE_STRING uKeyName;
	NTSTATUS status;
	HANDLE hRegister;
	RtlInitUnicodeString(&uKeyName, pRegisterName);
	
	// ��ʼ��ע����·��
	InitializeObjectAttributes(
		&objectAttributes,
		&uKeyName,
		OBJ_CASE_INSENSITIVE,
		NULL, NULL);

	// ��ע�����ȡע������·��
	status = ZwOpenKey(&hRegister, KEY_ALL_ACCESS, &objectAttributes);
	if (NT_SUCCESS(status))
	{
		// ɾ��ע����
		status = ZwDeleteKey(hRegister);
		ZwClose(hRegister);
	}

	return status;
}
