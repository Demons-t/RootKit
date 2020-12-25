#pragma once
#include <ntifs.h>

#define NAME_DEVICE L"\\Device\\demons"
#define NAME_SYMBOL L"\\??\\augen"

#define _CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

#define IOCTL_BASE        0x800

#define DEMONS_CODE(code)			\
		CTL_CODE					\
		(							\
			FILE_DEVICE_UNKNOWN	,	\
			IOCTL_BASE + (code),	\
			METHOD_OUT_DIRECT,		\
			0						\
		)

typedef enum _DemonsCtlCOde
{
	GETPID				= DEMONS_CODE(0),
	ENUM_DRIVER			= DEMONS_CODE(1),
	HIDE_DRIVER			= DEMONS_CODE(2),
	PROCESS_FIRST		= DEMONS_CODE(3),
	PROCESS_NEXT		= DEMONS_CODE(4),
	TERMINATE_PROCESS	= DEMONS_CODE(5),
	ENUM_THREAD			= DEMONS_CODE(6),
	ENUM_IDT			= DEMONS_CODE(7),
	ENUM_GDT			= DEMONS_CODE(8),
	ENUM_SSDT			= DEMONS_CODE(9),
	HIDE_PROCESS		= DEMONS_CODE(10),
	FILE_DATA			= DEMONS_CODE(11),
	FILE_FIRST			= DEMONS_CODE(12),
	FILE_NEXT			= DEMONS_CODE(13),
	DELETE_FILE			= DEMONS_CODE(14),
	ENUM_REGISTRY		= DEMONS_CODE(15),
	NEW_REGISTRY		= DEMONS_CODE(16),
	DELETE_REGISTRY_KEY = DEMONS_CODE(17),
	ENUM_MODULE			= DEMONS_CODE(18),
}DemonsCtlCOde;

typedef struct _LDR_DATA_TABLE_ENTRY {
	struct _LIST_ENTRY InLoadOrderLinks;                                    //0x0
	struct _LIST_ENTRY InMemoryOrderLinks;                                  //0x8
	struct _LIST_ENTRY InInitializationOrderLinks;                          //0x10
	PVOID DllBase;                                                          //0x18
	PVOID EntryPoint;                                                       //0x1c
	ULONG SizeOfImage;                                                      //0x20
	struct _UNICODE_STRING FullDllName;                                     //0x24
	struct _UNICODE_STRING BaseDllName;                                     //0x2c
	// ... ���滹���ֶΣ���Ϊ����Ҫʹ�ã����Բ�������
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

// ��������ԭ�ͺͽṹ��
typedef struct _PEB_LDR_DATA {
	ULONG                   Length;
	BOOLEAN                 Initialized;
	PVOID                   SsHandle;
	LIST_ENTRY              InLoadOrderModuleList;
	LIST_ENTRY              InMemoryOrderModuleList;
	LIST_ENTRY              InInitializationOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

// ����Ľṹ����PEB�����ֶ�.
typedef struct _PEB_SHORT {
	BOOLEAN                 InheritedAddressSpace;
	BOOLEAN                 ReadImageFileExecOptions;
	BOOLEAN                 BeingDebugged;
	BOOLEAN                 Spare;
	HANDLE                  Mutant;
	PVOID                   ImageBaseAddress;
	PPEB_LDR_DATA           Ldr;
}PEB_SHORT, *PPEB_SHORT;

// �����ṹ��
#pragma pack(1)
typedef struct _DRIVER
{
	ULONG uDllBase;					// ��ַ
	ULONG uSize;					// ��С
	char cName[256 * 2];			// ������
	char cFullDllName[256 * 2];		// ·��
	struct _DRIVER* Next;
}DRIVER, *PDRIVER;
#pragma pack()

// ���̽ṹ��
#pragma pack(1)
typedef struct _PROCESS_INFO
{
	HANDLE hProcessHandle;
	CHAR wName[256];				// ����
	unsigned int uPid;				// PID
	unsigned int uPPid;				// ������ID
	WCHAR cFullDllName[256];		// ·��

}PROCESS_INFO, *PPROCESS_INFO;
#pragma pack()

// �ļ��ṹ��
#pragma pack(1)
typedef	 struct _FILE_INFO
{
	HANDLE	hFindHandle;

	ULONG dwFileAttributes;
	ULONG64 ftCreationTime;
	ULONG64 ftLastAccessTime;
	ULONG64 ftLastWriteTime;
	ULONG nFileSizeHigh;
	ULONG nFileSizeLow;
	WCHAR  cFileName[266];
}FILE_INFO, * PFILE_INFO;
#pragma pack()

// idtr�Ĵ���ָ������ṹ��
#pragma pack(1)
typedef struct _IDT_INFO
{
	UINT16 uIdtLimit;		// IDT ��Χ
	UINT16 uLowIdtBase;		// IDT �͵�ַ
	UINT16 uHighIdtBase;	// IDT �߻�ַ
}IDT_INFO, *PIDT_INFO;
#pragma pack()

// IDT �����������Ľṹ��
#pragma pack(1)
typedef struct _IDT_ENTRY
{
	UINT16	uOffsetLow;			// �������͵�ַƫ��
	UINT16	uSelector;			// ��ѡ����
	UINT8	uReserved;			// ����
	UINT8	GateType : 4;		// �ж�����
	UINT8	StorageSegment : 1;	// Ϊ 0 ���ж���
	UINT8	DPL : 2;			// ��Ȩ��
	UINT8	Present : 1;		// δʹ���жϿ���Ϊ0
	UINT16	uOffsetHigh;		// �������ߵ�ַƫ��
}IDT_ENTRY,*PIDT_ENTRY;
#pragma pack()

// gdtr �Ĵ���ָ������ṹ��
#pragma pack(1)
typedef struct _GDT_INFO
{
	UINT16	uGdtLimit;			// GDT ��Χ
	UINT16	uLowGdtBase;		// GDT �ͻ�ַ
	UINT16	uHighGdtBase;		// GDT �߻�ַ
}GDT_INFO, *PGDT_INFO;
#pragma pack()

// ���������Ľṹ
#pragma pack(1)
typedef struct _GDT
{
	UINT64 Limit0_15 : 16;	// ����
	UINT64 base0_23 : 24;	// ��ַ
	UINT64 TYPE : 4;		// ����
	UINT64 S : 1;			// S λ��(Ϊ0����ϵͳ�Σ�1�Ǵ���λ������ݶ�)
	UINT64 DPL : 2;			// DPL ��Ȩ��
	UINT64 P : 1;			// Pλ�� �δ���
	UINT64 Limit16_19 : 4;	// ����
	UINT64 AVL : 1;			// AVL ϵͳ�������λ
	UINT64 O : 1;			// O
	UINT64 D_B : 1;			// D_B Ĭ�ϴ�С
	UINT64 G : 1;			// G ������
	UINT64 Base24_31 : 8;	// Base
}GDT,*PGDT;
#pragma pack()

// ϵͳ��������ṹ��
#pragma pack(1)
typedef struct _ServiceDesriptorEntry
{
	PVOID ServiceTableBase;        // ������ַ
	PULONG ServiceCounterTableBase; // �������ַ
	ULONG NumberOfServices;         // ������ĸ���
	ULONG ParamTableBase;          // �������ַ
}SSDTEntry, * PSSDTEntry;
#pragma pack()

// ssdt
#pragma pack(1)
typedef struct _SSDT
{
	ULONG uAddress;			// ���ػ�ַ
	ULONG uSysCallIndex;	// ���ú�
}SSDT, *PSSDT;
#pragma pack()

//�߳���Ϣ�ṹ��
#pragma pack(1)
typedef struct _THREAD_INFO
{
	ULONG ulTID;			// �߳�ID
	ULONG ulBasePriority;	// ���ȼ�
	ULONG ulStartAddress;	// �̵߳���ʼ��ַ
	ULONG ulStatus;			// ״̬
	struct _THREAD_INFO* Next;	//
}THREAD_INFO, * PTHREAD_INFO;
#pragma pack()

// ע���ṹ��
#pragma pack(1)
typedef struct _REGISTRY
{
	ULONG uType;				// ���ͣ�0�����1��ֵ
	char cKeyName[256 * 2];		// �������
	char cValueName[256 * 2];	// ֵ������
	ULONG uValueType;			// ֵ������
	UCHAR uValue[256];			// ֵ
}REGISTRY, * PREGISTRY;
#pragma pack()

// ģ��ṹ��
#pragma pack(1)
typedef struct _MODULE
{
	char cFullDllName[256 * 2];	// ģ��·��
	ULONG uStartAddress;		// ģ�����ַ
	ULONG uSize;				// ģ���С
	struct _MODULE* Next;
}MODULE, * PMODULE;
#pragma pack()

struct _PEB
{
	UCHAR InheritedAddressSpace;                                            //0x0
	UCHAR ReadImageFileExecOptions;                                         //0x1
	UCHAR BeingDebugged;
	UCHAR BitField;
	void* Mutant;                                                           //0x4
	void* ImageBaseAddress;                                                 //0x8
	struct _PEB_LDR_DATA* Ldr;
};