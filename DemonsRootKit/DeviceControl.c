#include "DeviceControl.h"

PDRIVER_OBJECT g_pDriver;
ULONG g_uPid;

// �����豸����
NTSTATUS CreateDevice(PDRIVER_OBJECT pDriverObject)
{
	// ������Ҫ�õ��ı���
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_OBJECT pDeviceObject = NULL;

	// ��ʼ���豸����
	UNICODE_STRING DeviceName = { 0 };
	RtlInitUnicodeString(&DeviceName, NAME_DEVICE);

	// �����豸����ĺ���
	status = IoCreateDevice(
		pDriverObject,			// �����������󣬴�����ᱻ��ӵ����� DeviceObject ������
		0,						// �豸����չ�ռ��С������Ŀռ�ᱻ DeviceExtension ָ��
		&DeviceName,			// �豸���������
		FILE_DEVICE_UNKNOWN,	// һ��˵������һ����Ӳ���޹ص������豸
		0,						// �豸��������Ϣ
		TRUE,					// �Ƿ��ռ / R3�Ƿ�ɷ���
		&pDeviceObject);		// ���������豸���󱻱��浽�ĵط�

	// ͨ�� NT_SUCCESS �жϴ����Ƿ�ɹ�
	if (!NT_SUCCESS(status))
	{
		KdPrint(("�豸���󴴽�ʧ��"));
		IoDeleteDevice(pDeviceObject);
		return status;
	}

	// �豸���������ֻ�ܱ��ں˳��������Ϊ����R3Ӧ��ʶ��
	// ��Ҫ���÷���������
	UNICODE_STRING SysLinkName = { 0 };
	RtlInitUnicodeString(&SysLinkName, NAME_SYMBOL);
	status = IoCreateSymbolicLink(&SysLinkName, &DeviceName);

	// ͨ�� NT_SUCCESS �жϴ����Ƿ�ɹ�
	if (!NT_SUCCESS(status))
	{
		KdPrint(("�豸���󴴽�ʧ��"));
		IoDeleteDevice(pDeviceObject);
		return status;
	}

	return status;
}

// Ĭ�ϵ���Ϣ��ǲ����
NTSTATUS DefaultDispatch(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);

	// ������Ϣ�Ĵ���״̬
	pIrp->IoStatus.Status = STATUS_SUCCESS;

	// ������Ϣ��������ݳ���
	pIrp->IoStatus.Information = 0;

	// ֪ͨ�����Ѿ���ɣ���ɺ���ߵ�ǰ�� IRQL
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS  Create(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	KdPrint(("������������\n"));
	// ����IRP���״̬
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS  Close(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	KdPrint(("�������ر���\n"));
	// ����IRP���״̬
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DeviceIoControlDispatch(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	NTSTATUS ntStatus = STATUS_SUCCESS;
	ULONG ulComplateSize = 0;

	// ��ȡ���� IO ջ
	PIO_STACK_LOCATION pStack = IoGetCurrentIrpStackLocation(pIrp);

	// �õ����뻺�����ֽ���
	ULONG uCinSize = pStack->Parameters.DeviceIoControl.InputBufferLength;

	// �õ�����������ֽ���
	ULONG uCoutSize = pStack->Parameters.DeviceIoControl.OutputBufferLength;

	// ��ȡ֪ͨ��
	ULONG uCode = pStack->Parameters.DeviceIoControl.IoControlCode;

	// ��ȡ���뻺����
	PVOID pCinBuff = pIrp->AssociatedIrp.SystemBuffer;

	// ��ȡ���������
	// ���������ָ����`METHOD_OUT_DIRECT`, ��ô�������������MDL����ʽ����
	// ����ڵ���DeviceIoContral�����������������NULLʱ,`MdlAddress` Ҳ��ΪNULL 
	PVOID pCoutBuff = NULL;
	if (pIrp->MdlAddress && METHOD_FROM_CTL_CODE(uCode) & METHOD_OUT_DIRECT)
	{
		pCoutBuff = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, 0);
	}

	// ���ݿ����������в���
	switch (uCode)
	{
	case GETPID:
	{
		ULONG uPID;
		uPID = *(ULONG*)pCinBuff;
		g_uPid = uPID;
		g_Pid = uPID;
		KdPrint(("��ȡ��PID: %d\n", uPID));
		KdPrint(("g_Pid = %d\n", g_Pid));
		KdPrint(("g_uPid = %d\n", g_uPid));
		break;
	}
	case ENUM_DRIVER:
	{
		// ��������
		ulComplateSize = EnumDriver(pCoutBuff, uCoutSize, g_pDriver);
		break;
	}
	case HIDE_DRIVER:
	{
		PWCHAR pInputBuf = (PWCHAR)pCinBuff;
		UNICODE_STRING uDriverName = { 0 };
		RtlInitUnicodeString(&uDriverName, pInputBuf);
		UINT8 uRet;

		// ��������
		uRet = HideDriver(uDriverName, g_pDriver);
		break;
	}
	case PROCESS_FIRST:
	case PROCESS_NEXT:
	{
		// ��������
		ulComplateSize = EnumProcess(pCoutBuff, uCoutSize, pCinBuff, uCode);
		break;
	}
	case TERMINATE_PROCESS:
	{
		// ��������
		KillProcess(pCinBuff);
		break;
	 }
	case ENUM_IDT:
	{
		// ���� IDT
		ulComplateSize = GetIdtInfo(pCoutBuff);
		break;
	}
	case ENUM_GDT:
	{
		// ���� GDT
		ulComplateSize = GetGdtInfo(pCoutBuff, uCoutSize);
		break;
	}
	case ENUM_SSDT:
	{
		// ���� ssdt
		ulComplateSize = EnumSsdt(pCoutBuff, uCoutSize);
		break;
	}
	case ENUM_THREAD:
	{
		// �����߳�
		ulComplateSize = EnumThread(pCoutBuff, uCoutSize, pCinBuff);
		break;
	}
	case HIDE_PROCESS:
	{
		// ���ؽ���
		HideProcess(pCinBuff);
		break;
	}
	case FILE_DATA:
	{
		// �����Զ����ļ�����
		ulComplateSize = EnumFileData(pCinBuff, pCoutBuff, uCoutSize);
		break;
	}
	case FILE_FIRST:
	{
		// ��ȡ��һ���ļ�
		ulComplateSize = EnumFirstFile(pCinBuff, pCoutBuff, uCoutSize);
		break;
	}
	case FILE_NEXT:
	{
		// ������뻺������������������ǽṹ�� FILE_INFO,���˳�
		if (uCinSize != sizeof(FILE_INFO) || uCoutSize != uCinSize)
			break;

		// ��ȡ��һ���ļ�
		ulComplateSize = EnumNextFile(pCoutBuff);
		break;
	}
	case DELETE_FILE:
	{
		// ɾ���ļ�
		DeleteFiles(pCinBuff);
		break;
	}
	case ENUM_REGISTRY:
	{
		// ����ע���
		ulComplateSize = EnumRegister(pCoutBuff, uCoutSize, pCinBuff);
		break;
	}
	case NEW_REGISTRY:
	{
		// ��������
		NewReg(pCinBuff);
		break;
	}
	case DELETE_REGISTRY_KEY:
	{
		// ɾ������
		DeleteRegKey(pCinBuff);
		break;
	}
	case ENUM_MODULE:
	{
		// ����ģ��
		ulComplateSize = EnumModule(pCoutBuff, uCoutSize, pCinBuff);
		break;
	}
	default:
		break;
	}

	// ������Ϣ�Ĵ���״̬
	pIrp->IoStatus.Status = ntStatus;

	// ������Ϣ��������ݳ���
	pIrp->IoStatus.Information = ulComplateSize;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return ntStatus;
}

// ������ǲ����
VOID SetDispatch(PDRIVER_OBJECT pDriverObject)
{
	// Ϊ���е� IRP ��Ϣ����Ĭ�ϵ���ǲ����
	for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
		pDriverObject->MajorFunction[i] = DefaultDispatch;

	// �����豸����֮�󣬿���ͨ����������������Ϣ����ǲ����(��Ϣ��Ӧ)
	// ��Ϣ�����豸��������ģ���ͳһ������������д���
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = Create;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = Close;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceIoControlDispatch;
}
