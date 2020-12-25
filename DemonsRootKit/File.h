#pragma once
#include "Data.h"

#define ListDirNext(Type,fileinfo) ((Type*)((ULONG_PTR)fileinfo + fileinfo->NextEntryOffset))
#define ListDirForEach(FileInfoType,fileInfo, iterator) \
	for (FileInfoType* iterator = fileInfo; \
		iterator->NextEntryOffset != 0;	 \
		iterator = ListDirNext(FileInfoType,iterator))

// �����ڴ�
VOID* alloc(ULONG uSize);

// ���·���ռ�
void* reAlloc(void* src, ULONG uSize);

// �ͷ��ڴ�ռ�
void free(void* data);

// �����ļ�
NTSTATUS createFile(
	wchar_t* wFilePath,
	ULONG access,
	ULONG share,
	ULONG openModel,
	BOOLEAN isDir,
	HANDLE* hFile);

// ��ȡ�ļ���С
NTSTATUS getFileSize(HANDLE hFile, ULONG64* size);

// ��ȡ�ļ�����
NTSTATUS readFile(
	HANDLE hFile,
	ULONG offsetLow,
	ULONG offsetHig,
	ULONG sizeToRead,
	PVOID pBuff,
	ULONG* read);

// д���ļ�����
NTSTATUS writeFile(
	HANDLE hFile,
	ULONG offsetLow,
	ULONG offsetHig,
	ULONG sizeToWrite,
	PVOID pBuff,
	ULONG* write);

// �����ļ�
NTSTATUS copyFile(
	wchar_t* srcPath,
	wchar_t* destPath);

// �ƶ��ļ�
NTSTATUS moveFile(
	wchar_t* srcPath,
	wchar_t* destPath);

// ɾ���ļ�
NTSTATUS removeFile(wchar_t* filePath);

// �г�һ��Ŀ¼�µ��ļ����ļ���
NTSTATUS listDirGet(
	wchar_t* dir,
	FILE_BOTH_DIR_INFORMATION** fileInfo,
	ULONG maxFileCount);

// ��ȡһ��Ŀ¼�µĵ�һ���ļ�
NTSTATUS firstFile(
	wchar_t* dir,
	HANDLE* hFind,
	FILE_BOTH_DIR_INFORMATION* fileInfo,
	int nSize);

// ��ȡһ��Ŀ¼�µ���һ���ļ�
NTSTATUS nextFile(
	HANDLE hFind,
	FILE_BOTH_DIR_INFORMATION* fileInfo,
	int nSize);

// �ͷſռ�
void listDirFree(FILE_BOTH_DIR_INFORMATION* fileInfo);

// �Զ����ö������
ULONG EnumFileData(
	PVOID pInputBuff, 
	PVOID pOutputBuff,
	ULONG outputSize);

// ��ȡ��һ���ļ�
ULONG EnumFirstFile(
	PVOID pInputBuff,
	PVOID pOutputBuff,
	ULONG outputSize);

// ��ȡ��һ���ļ�
ULONG EnumNextFile(PVOID pOutputBuff);

// ɾ���ļ�
BOOLEAN DeleteFiles(PVOID pFileName);