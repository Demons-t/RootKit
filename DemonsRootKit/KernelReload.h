#pragma once
#include <ntifs.h>
#include <ntimage.h>

// ���ļ�
HANDLE KernelCreateFile(
	IN PUNICODE_STRING pstrFile, // �ļ�·����������
	IN BOOLEAN bIsDir);   // �Ƿ�Ϊ�ļ���

// ��ȡ�ļ���С
ULONG64 KernelGetFileSize(IN HANDLE hfile);

// ��ȡ�ļ�
ULONG64 KernelReadFile(
	IN  HANDLE         hfile,    // �ļ����
	IN  PLARGE_INTEGER Offset,   // �����￪ʼ��ȡ
	IN  ULONG          ulLength, // ��ȡ�����ֽ�
	OUT PVOID          pBuffer);  // �������ݵĻ���

// �����ڴ�����
void* SearchMemory(char* buf, int BufLenth, char* Mem, int MaxLenth);

// �ر�ҳ����
void OffProtected();

// ����ҳ����
void OnProtected();


// ͨ�����ƻ�ȡģ���ַ
ULONG32 MyGetModuleHandle(PUNICODE_STRING pModuleName);


// ��ȡ�ں�ģ�鵽�ڴ���
void ReadKernelToBuf(PWCHAR pPath, PUCHAR* pBuf);


// չ���ں�PE�ļ�
void ZKKernel(PUCHAR* pZkBUf, PUCHAR buf);


// �޸����ں��ض�λ
void FixReloc(PUCHAR ZkBuf, PUCHAR OldBase);


// �޸���SSDT��
void FixSSDT(PUCHAR pZKBuf, PUCHAR OldBase);


// ��ȡKiFastCallEntry����
ULONG GetKiFastCallEntry();


// hook�й��˺���
ULONG   FilterFun(ULONG SSdtBase, PULONG OldFun, ULONG Id);


// hookKiFastCallEntry����
void OnHookKiFastCallEntry();


// �ں����� ��ʼ
void KernelReload();


// ж���ں˹���
void UnHook();