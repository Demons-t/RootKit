#include "DemonsDriverEntry.h"

// ж�غ���
#pragma code_seg("PAGE")
NTSTATUS  DriverUnload(PDRIVER_OBJECT pDriverObject)
{
	// ж�� SysenterHook
	UnInstallHook();

	// ɾ���豸�����ʱ�򣬱�����ɾ������������ɾ���豸����
	UNICODE_STRING SymLinkName = { 0 };
	RtlInitUnicodeString(&SymLinkName, NAME_SYMBOL);
	IoDeleteSymbolicLink(&SymLinkName);

	// ����ͨ����������������ҵ��豸���󣬽���ɾ��
	IoDeleteDevice(pDriverObject->DeviceObject);

	// ж���ں�����
	UnHook();

	// ж�� SSDT HOOK
	// UnInstaSsdtllHook();
	
	return STATUS_SUCCESS;
}

// �����������ں���
#pragma code_seg("INIT")
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	UNREFERENCED_PARAMETER(pRegistryPath);
	
	g_pDriver = pDriverObject;

	// ж�������Ļص�����
	pDriverObject->DriverUnload = DriverUnload;                                                                                

	// �����豸����
	CreateDevice(pDriverObject);

	// ������ǲ����
	SetDispatch(pDriverObject);

	// SSDT HOOK
	// InstallSsdtHook();

	// �����ں�
	KernelReload();

	// ��װ SysenterHook
	InstallHook();

	return STATUS_SUCCESS;
}
