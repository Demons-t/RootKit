#include "File.h"

// �����ڴ�
VOID* alloc(ULONG uSize)
{
	return ExAllocatePoolWithTag(PagedPool, uSize, 'abcd');
}

// ���·���ռ�
void* reAlloc(void* src, ULONG uSize)
{
	if (!src)
		return NULL;
	
	void* data = alloc(uSize);
	RtlCopyMemory(data, src, uSize);
	free(data);
	return data;
}

// �ͷ��ڴ�ռ�
void free(void* data)
{
	if (data)
		ExFreePoolWithTag(data, 'abcd');
}

// �����ļ�
NTSTATUS createFile(wchar_t* wFilePath, ULONG access, ULONG share, ULONG openModel, BOOLEAN isDir, HANDLE* hFile)
{
	NTSTATUS status = STATUS_SUCCESS;

	IO_STATUS_BLOCK StatusBlock = { 0 };
	ULONG           ulShareAccess = share;
	ULONG ulCreateOpt = FILE_SYNCHRONOUS_IO_NONALERT;

	UNICODE_STRING path;
	RtlInitUnicodeString(&path, wFilePath);

	// 1. ��ʼ��OBJECT_ATTRIBUTES������
	OBJECT_ATTRIBUTES objAttrib = { 0 };
	ULONG ulAttributes = OBJ_CASE_INSENSITIVE/*�����ִ�Сд*/ | OBJ_KERNEL_HANDLE/*�ں˾��*/;
	InitializeObjectAttributes(&objAttrib,    // ���س�ʼ����ϵĽṹ��
		&path,      // �ļ���������
		ulAttributes,  // ��������
		NULL, NULL);   // һ��ΪNULL

// 2. �����ļ�����
	ulCreateOpt |= isDir ? FILE_DIRECTORY_FILE : FILE_NON_DIRECTORY_FILE;

	status = ZwCreateFile(
		hFile,					// �����ļ����
		access,					// �ļ���������
		&objAttrib,				// OBJECT_ATTRIBUTES
		&StatusBlock,			// ���ܺ����Ĳ������
		0,						// ��ʼ�ļ���С
		FILE_ATTRIBUTE_NORMAL,	// �½��ļ�������
		ulShareAccess,			// �ļ�����ʽ
		openModel,				// �ļ�������򿪲������򴴽�
		ulCreateOpt,			// �򿪲����ĸ��ӱ�־λ
		NULL,					// ��չ������
		0);						// ��չ����������

	return status;
}

// ��ȡ�ļ���С
NTSTATUS getFileSize(HANDLE hFile, ULONG64* size)
{
	IO_STATUS_BLOCK ioStatus = { 0 };
	FILE_STANDARD_INFORMATION file = { 0 };
	NTSTATUS status;
	status = ZwQueryInformationFile(
		hFile,						// �ļ����
		&ioStatus,					// ���״̬
		&file,						// �����ļ���Ϣ�Ļ�����
		sizeof(file),				// ���������ֽ���
		FileStandardInformation);	// Ҫ��ȡ����Ϣ����
	if (STATUS_SUCCESS == status)
	{
		// �����ļ��ֽ���
		*size = file.EndOfFile.QuadPart;
	}

	return status;
}

// ��ȡ�ļ�����
NTSTATUS readFile(HANDLE hFile, ULONG offsetLow, ULONG offsetHig, ULONG sizeToRead, PVOID pBuff, ULONG* read)
{
	NTSTATUS status;
	IO_STATUS_BLOCK ioStatus = { 0 };
	LARGE_INTEGER offset;

	// ����Ҫ��ȡ���ļ�ƫ��
	offset.HighPart = offsetHig;
	offset.LowPart = offsetLow;

	status = ZwReadFile(
		hFile,			// �ļ����
		NULL,			// �¼����������첽IO
		NULL,			// APC �����֪ͨ���̣������첽IO
		NULL,			// ���֪ͨ���̵ĸ��Ӳ���
		&ioStatus,		// IO ״̬
		pBuff,			// �����ļ����ݵĻ�����
		sizeToRead,		// Ҫ��ȡ���ֽ���
		&offset,		// Ҫ��ȡ���ļ�λ��
		NULL);
	if (status == STATUS_SUCCESS)
		*read = ioStatus.Information;

	return status;
}

// д���ļ�����
NTSTATUS writeFile(HANDLE hFile, ULONG offsetLow, ULONG offsetHig, ULONG sizeToWrite, PVOID pBuff, ULONG* write)
{
	NTSTATUS status;
	IO_STATUS_BLOCK ioStatus = { 0 };
	LARGE_INTEGER offset;

	// ����Ҫд����ļ�ƫ��
	offset.HighPart = offsetHig;
	offset.LowPart = offsetLow;

	status = ZwWriteFile(
		hFile,			// �ļ����
		NULL,			// �¼����������첽IO
		NULL,			// APC �����֪ͨ���̣������첽IO
		NULL,			// ���֪ͨ���̵ĸ��Ӳ���
		&ioStatus,		// IO ״̬
		pBuff,			// д�뵽�ļ��еĻ�����
		sizeToWrite,		// д����ֽ���
		&offset,		// д�뵽���ļ�ƫ��
		NULL);
	if (status == STATUS_SUCCESS)
		*write = ioStatus.Information;

	return status;
}

// �����ļ�
NTSTATUS copyFile(wchar_t* srcPath, wchar_t* destPath)
{
	HANDLE hSrc = (HANDLE)-1;
	HANDLE hDest = (HANDLE)-1;
	NTSTATUS status = STATUS_SUCCESS;
	ULONG64 srcSize = 0;
	ULONG size = 0;
	char* pBuff = NULL;

	__try
	{
		// ��Դ�ļ�
		status = createFile(
			srcPath,
			GENERIC_READ,
			FILE_SHARE_READ,
			FILE_OPEN_IF,
			FALSE,
			&hSrc);
		if (STATUS_SUCCESS != status)
			__leave;

		// ��ȡԴ�ļ���С
		if (STATUS_SUCCESS != getFileSize(hSrc, &srcSize))
			__leave;

		// �����ڴ�ռ䱣��Դ�ļ�������
		pBuff = ExAllocatePoolWithTag(PagedPool, (ULONG)srcSize, 'abcd');
		if (pBuff == NULL)
			__leave;

		// ��ȡԴ�ļ������ݵ��ڴ���
		status = readFile(hSrc, 0, 0, (ULONG)srcSize, pBuff, &size);
		if (STATUS_SUCCESS != status || size != (ULONG)srcSize)
			__leave;

		// ��Ŀ���ļ�
		status = createFile(
			destPath,
			GENERIC_WRITE,
			FILE_SHARE_READ,
			FILE_CREATE,
			FALSE,
			&hDest);
		if (STATUS_SUCCESS != status)
			__leave;

		// ��Դ�ļ�������д�뵽Ŀ���ļ�
		status = writeFile(hDest, 0, 0, (ULONG)srcSize, pBuff, &size);
		if (STATUS_SUCCESS != status || size != srcSize)
			__leave;
	}
	__finally
	{
		// �ر�Դ�ļ�
		if (hSrc != (HANDLE)-1)
			ZwClose(hSrc);

		// �ر�Ŀ���ļ�
		if (hDest != (HANDLE)-1)
			ZwClose(hDest);

		// �ͷŻ�����
		if (pBuff)
			ExFreePoolWithTag(pBuff, 'abcd');
	}

	return status;
}

// �����ļ�
NTSTATUS moveFile(wchar_t* srcPath, wchar_t* destPath)
{
	NTSTATUS status = STATUS_SUCCESS;

	// ����һ���ļ�
	status = copyFile(srcPath, destPath);

	// ��������ɹ���ɾ��Դ�ļ�
	if (status == STATUS_SUCCESS)
		status = removeFile(srcPath);

	return status;
}

// ɾ���ļ�
NTSTATUS removeFile(wchar_t* filePath)
{
	UNICODE_STRING path;
	RtlInitUnicodeString(&path, filePath);

	// ��ʼ�� OBJECT_ATTRIBUTES ������
	OBJECT_ATTRIBUTES objAttrib = { 0 };
	ULONG             ulAttributes = OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
	InitializeObjectAttributes(
		&objAttrib,    // ���س�ʼ����ϵĽṹ��
		&path,		  // �ļ���������
		ulAttributes,  // ��������
		NULL,          // ��Ŀ¼(һ��ΪNULL)
		NULL);         // ��ȫ����(һ��ΪNULL)
		
	// ɾ��ָ���ļ� / �ļ���
	return ZwDeleteFile(&objAttrib);
}

// �г�һ��Ŀ¼�µ��ļ����ļ���
NTSTATUS listDirGet(wchar_t* dir, FILE_BOTH_DIR_INFORMATION** fileInfo, ULONG maxFileCount)
{
	NTSTATUS status = STATUS_SUCCESS;
	IO_STATUS_BLOCK ioStatus = { 0 };
	HANDLE hDir = (HANDLE)-1;
	VOID* pBuff = NULL;

	__try
	{
		// ��Ŀ¼
		status = createFile(
			dir,
			GENERIC_READ,
			FILE_SHARE_READ,
			FILE_OPEN_IF,
			TRUE,
			&hDir);
		if (STATUS_SUCCESS != status)
			__leave;

		// ���������һ���ļ���Ϣ���������ֽ��� = �ṹ���С + �ļ�����С
		ULONG signalFileInforSize = sizeof(FILE_BOTH_DIR_INFORMATION) + 267 * 2;

		// ������ܿռ��ֽ���
		ULONG uTotalSize = signalFileInforSize * maxFileCount;

		// �����ڴ�ռ�
		pBuff = ExAllocatePoolWithTag(PagedPool, uTotalSize, 'abcd');
		if (pBuff == NULL)
			__leave;

		// ��һ�ε��ã���ȡ���軺�����ֽ���
		status = ZwQueryDirectoryFile(
			hDir,							// Ŀ¼���
			NULL,							// �¼�����
			NULL,							// ���֪ͨ����
			NULL,							// ���֪ͨ���̸��Ӳ���
			&ioStatus,						// IO״̬
			pBuff,							// ������ļ���Ϣ
			uTotalSize,						// �ļ���Ϣ���������ֽ���
			FileBothDirectoryInformation,	// ��ȡ��Ϣ������
			FALSE,							// �Ƿ�ֻ��ȡ��һ��
			0,
			TRUE);							//�Ƿ�����ɨ��Ŀ¼

		// ���滺�����������׵�ַ
		if (status == STATUS_SUCCESS)
			*fileInfo = (FILE_BOTH_DIR_INFORMATION*)pBuff;
	}
	__finally
	{
		if (hDir != (HANDLE)-1)
			ZwClose(hDir);

		if (status != STATUS_SUCCESS && pBuff != NULL)
			ExFreePoolWithTag(pBuff, 'abcd');
	}

	return status;
}

// ��ȡһ��Ŀ¼�µĵ�һ���ļ�
NTSTATUS firstFile(wchar_t* dir, HANDLE* hFind, FILE_BOTH_DIR_INFORMATION* fileInfo, int nSize)
{
	NTSTATUS status = STATUS_SUCCESS;
	IO_STATUS_BLOCK ioStatus = { 0 };

	// ��Ŀ¼
	status = createFile(
		dir,
		GENERIC_READ,
		FILE_SHARE_READ,
		FILE_OPEN_IF,
		TRUE,
		hFind);

	if (STATUS_SUCCESS != status)
		return status;

	// ��һ�ε��ã���ȡ���軺�������ֽ���
	status = ZwQueryDirectoryFile(
		*hFind,							// Ŀ¼���
		NULL,							// �¼�����
		NULL,							// ���֪ͨ����
		NULL,							// ���֪ͨ���̸��Ӳ���
		&ioStatus,						// IO״̬
		fileInfo,						// ������ļ���Ϣ
		nSize,							// �ļ���Ϣ���������ֽ���
		FileBothDirectoryInformation,	// ��ȡ��Ϣ������
		TRUE,							// �Ƿ�ֻ��ȡ��һ��
		0,
		TRUE);							//�Ƿ�����ɨ��Ŀ¼

	return status;
}

// ��ȡһ��Ŀ¼�µ���һ���ļ�
NTSTATUS nextFile(HANDLE hFind, FILE_BOTH_DIR_INFORMATION* fileInfo, int nSize)
{
	IO_STATUS_BLOCK ioStatus = { 0 };

	// ��һ�ε��ã���ȡ���軺�����ֽ���
	return ZwQueryDirectoryFile(
		hFind,							// Ŀ¼���
		NULL,							// �¼�����
		NULL,							// ���֪ͨ����
		NULL,							// ���֪ͨ���̸��Ӳ���
		&ioStatus,						// IO״̬
		fileInfo,						// ������ļ���Ϣ
		nSize,							// �ļ���Ϣ���������ֽ���
		FileBothDirectoryInformation,	// ��ȡ��Ϣ������
		TRUE,							// �Ƿ�ֻ��ȡ��һ��
		0,
		FALSE);							//�Ƿ�����ɨ��Ŀ¼
}

// �ͷſռ�
void listDirFree(FILE_BOTH_DIR_INFORMATION* fileInfo)
{
	ExFreePoolWithTag(fileInfo, 'abcd');
}

// �Զ����ö������
ULONG EnumFileData(PVOID pInputBuff, PVOID pOutputBuff, ULONG outputSize)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING filePath;
	HANDLE hFile = NULL;
	ULONG uComplateSize = 0;

	// ��ʼ���ַ���
	RtlInitUnicodeString(&filePath, (PCWSTR)pInputBuff);

	// ���ļ�
	status = createFile(
		(LPWSTR)pInputBuff,
		GENERIC_READ,
		FILE_SHARE_READ,
		FILE_OPEN_IF,
		FALSE,
		&hFile);
	if (STATUS_SUCCESS != status)
		uComplateSize = 0;

	// ��ȡ�ļ���С
	ULONG64 fileSize = 0;
	status = getFileSize(hFile, &fileSize);
	if (STATUS_SUCCESS != status)
	{
		uComplateSize = 0;
		ZwClose(hFile);
	}

	// �ж�����������Ĵ�С�Ƿ��ܹ�����
	if (pOutputBuff == NULL || outputSize < fileSize)
	{
		// ���������Ϊ NULL ��������������Ĵ�С������
		// ��������ɵ��ֽ����������ֽ���
		// ̽�󲻶Ի��������������ֻ����
		// �������Ĵ�С��������DeviceIoControl
		// ����֮�󣬾��ܵõ������ֽ���
		uComplateSize = (ULONG)fileSize;
		ZwClose(hFile);
	}

	ULONG read = 0;
	readFile(hFile, 0, 0, (ULONG)fileSize, pOutputBuff, &read);
	ZwClose(hFile);

	// ���ö�ȡ�����ֽ���
	// ����ʵ�ʶ�ȡ�����ֽ���
	uComplateSize = read;

	return uComplateSize;
}

// ��ȡ��һ���ļ�
ULONG EnumFirstFile(PVOID pInputBuff, PVOID pOutputBuff, ULONG outputSize)
{
	HANDLE hFind = NULL;
	NTSTATUS status = STATUS_SUCCESS;
	ULONG uComplateSize = 0;

	// ����һ�������������ڱ���������ļ�����
	char tempBuff[sizeof(FILE_BOTH_DIR_INFORMATION) + 267 * 2];
	FILE_BOTH_DIR_INFORMATION* pInfo = (FILE_BOTH_DIR_INFORMATION*)tempBuff;

	// ��ȡ��һ���ļ�
	status = firstFile((wchar_t*)pInputBuff, &hFind, pInfo, sizeof(tempBuff));

	if (status == STATUS_SUCCESS)
	{
		if (pOutputBuff != NULL && outputSize >= sizeof(FILE_INFO))
		{
			PFILE_INFO pFindInfo = (PFILE_INFO)pOutputBuff;
			pFindInfo->hFindHandle = hFind;

			// ������Ϣ
			RtlCopyMemory(pFindInfo->cFileName, pInfo->FileName, pInfo->FileNameLength);
			pFindInfo->cFileName[pInfo->FileNameLength / 2] = 0;
			
			pFindInfo->dwFileAttributes = pInfo->FileAttributes;
			pFindInfo->ftCreationTime = pInfo->CreationTime.QuadPart;
			pFindInfo->ftLastAccessTime = pInfo->LastAccessTime.QuadPart;
			pFindInfo->ftLastWriteTime = pInfo->LastWriteTime.QuadPart;
			pFindInfo->nFileSizeHigh = pInfo->EndOfFile.HighPart;
			pFindInfo->nFileSizeLow = pInfo->EndOfFile.LowPart;

			// ������ɵ��ֽ���
			uComplateSize = sizeof(FILE_INFO);
		}
	}

	return uComplateSize;
}

// ��ȡ��һ���ļ�
ULONG EnumNextFile(PVOID pOutputBuff)
{
	NTSTATUS status = STATUS_SUCCESS;
	ULONG uComplateSize = 0;

	// ��ȡ���������
	PFILE_INFO pFindInfo = (PFILE_INFO)pOutputBuff;

	// ����һ���ṹ�壬���ڱ���������ļ�����
	char tempBuff[sizeof(FILE_BOTH_DIR_INFORMATION) + 267 * 2];
	FILE_BOTH_DIR_INFORMATION* pInfo = (FILE_BOTH_DIR_INFORMATION*)tempBuff;

	// ��ȡ��һ���ļ�
	status = nextFile(pFindInfo->hFindHandle, pInfo, sizeof(tempBuff));

	if (status == STATUS_SUCCESS)
	{
		RtlCopyMemory(pFindInfo->cFileName, pInfo->FileName, pInfo->FileNameLength);
		pFindInfo->cFileName[pInfo->FileNameLength / 2] = 0;

		pFindInfo->dwFileAttributes = pInfo->FileAttributes;
		pFindInfo->ftCreationTime = pInfo->CreationTime.QuadPart;
		pFindInfo->ftLastAccessTime = pInfo->LastAccessTime.QuadPart;
		pFindInfo->ftLastWriteTime = pInfo->LastWriteTime.QuadPart;
		pFindInfo->nFileSizeHigh = pInfo->EndOfFile.HighPart;
		pFindInfo->nFileSizeLow = pInfo->EndOfFile.LowPart;

		// ������ɵ��ֽ���
		uComplateSize = sizeof(FILE_INFO);
	}
	else
	{
		ZwClose(pFindInfo->hFindHandle);
	}

	return uComplateSize;
}

// ɾ���ļ�
BOOLEAN DeleteFiles(PVOID pFileName)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	UNICODE_STRING StrFile;
	RtlInitUnicodeString(&StrFile, pFileName);

	OBJECT_ATTRIBUTES objAttribute = { 0 };
	ULONG ulAttributes = OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;

	InitializeObjectAttributes(&objAttribute, &StrFile, ulAttributes, NULL, NULL);

	// ɾ��ָ���ļ�/�ļ���
	status = ZwDeleteFile(&objAttribute);

	return TRUE;
}
