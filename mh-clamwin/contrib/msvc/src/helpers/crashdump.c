/*
 * Clamav Native Windows Port: Crash Dumper Helper
 *
 * Copyright (c) 2005-2008 Gianluigi Tiesi <sherpya@netfarm.it>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this software; if not, write to the
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <platform.h>
#include <osdeps.h>

#ifdef _MSC_VER
DWORD WINAPI CrashMiniDumpWriteDumpProc(LPVOID lpParam)
{
    EXCEPTION_POINTERS *pExPtrs = (EXCEPTION_POINTERS *) lpParam;
    LONG retval = EXCEPTION_EXECUTE_HANDLER;
    MINIDUMP_EXCEPTION_INFORMATION ExInfo;
    pMiniDumpWriteDumpFunc pMiniDumpWriteDump = NULL;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMapFile = NULL;
    LPBYTE lpMapAddress = NULL;
    BY_HANDLE_FILE_INFORMATION FileInformation;
    HMODULE hDll;
    char name[MAX_PATH] = "", fullpath[MAX_PATH] = "";
    char *lSlash = NULL;
    unsigned int i;

    if (!GetModuleFileNameA(NULL, fullpath, MAX_PATH - 1)) /* Very unlikely */
        strncat(name, "Unknown", MAX_PATH - 1 - strlen(name));
    else
    {
        /* Get executable name */
        char *p = fullpath + strlen(fullpath) - 1;
        while (*p && (*p != '\\')) p--;
        strncpy(name, ++p, MAX_PATH - 1);
    }

    name[MAX_PATH - 1] = 0;

    fprintf(stderr, "*** ClamWinDumper ***\n"
                    "*** %s Crashed\n"
                    "    ExpCode   : 0x%8.8x\n"
                    "    ExpAddress: 0x%p\n",
                    name,
                    pExPtrs->ExceptionRecord->ExceptionCode,
                    pExPtrs->ExceptionRecord->ExceptionAddress);

    /* Try to get dll from executable directory, win2k dbghelp misses symbols */
    lSlash = strrchr(fullpath, '\\');
    if (lSlash)
    {
        strcpy(lSlash + 1, "dbghelp.dll");
        hDll = LoadLibraryA(fullpath);
    }

    /* Load the version provided by the environment */
    if (!hDll) hDll = LoadLibraryA("dbghelp.dll");

    if (!hDll)
    {
        fprintf(stderr, "[ClamWin] Cannot find dbghelp.dll, you cannot produce a crash dump without\n");
        return retval;
    }

    pMiniDumpWriteDump = (pMiniDumpWriteDumpFunc) GetProcAddress(hDll, "MiniDumpWriteDump");
    if (!pMiniDumpWriteDump)
    {
        fprintf(stderr, "[ClamWin] Your dbghelp.dll is too old, put an updated version\n"
                        "in the same directory of the executable\n");
        goto cleanup;
    }

    strncat(name, ".dmp", MAX_PATH - 1 - strlen(name));
    name[MAX_PATH - 1] = 0;
    hFile = CreateFileA(name, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "[ClamWin] Cannot open file for writing (%d)\n", GetLastError());
        goto cleanup;
    }

    ExInfo.ThreadId = GetCurrentThreadId();
    ExInfo.ExceptionPointers = pExPtrs;
    ExInfo.ClientPointers = FALSE;

    if (!pMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MINDUMP_FLAGS, &ExInfo, NULL, NULL))
    {
        fprintf(stderr, "[ClamWin] MiniDumpWriteDump() failed with code %d\n", GetLastError());
        goto cleanup;
    }

#ifndef _WIN64
    /* Now scramble it by xor-ing with 42, to avoid false positives on the dump file */
    hMapFile = CreateFileMappingA(hFile, NULL, PAGE_READWRITE, 0, 0, "libClamAVDumper");

    if (!hMapFile)
    {
        fprintf(stderr, "[ClamWin] CreateFileMappingA() on file failed (%d)\n", GetLastError());
        goto cleanup;
    }

    lpMapAddress = (LPBYTE) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (!lpMapAddress)
    {
        fprintf(stderr, "[ClamWin] MapViewOfFile() on file failed (%d)\n", GetLastError());
        goto cleanup;
    }

    if (!GetFileInformationByHandle(hFile, &FileInformation))
    {
        fprintf(stderr, "[ClamWin] GetFileInformationByHandle() on file failed (%d)\n", GetLastError());
        goto cleanup;
    }

    for (i = 0; i < FileInformation.nFileSizeLow; i++)
        lpMapAddress[i] ^= 42;

    FlushViewOfFile(lpMapAddress, 0);
#endif
    fprintf(stderr, "[ClamWin] Crash Dump saved as %s, please report\n", name);

cleanup:
    if (hMapFile) CloseHandle(hMapFile);
    if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
    FreeLibrary(hDll);
    return retval;
}

LONG __stdcall CrashHandlerExceptionFilter(EXCEPTION_POINTERS *pExPtrs)
{
    /* Spawn a new thread this should improve the dump */
    DWORD tid = 0, res = -1;
    HANDLE cProc = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) CrashMiniDumpWriteDumpProc, (LPVOID) pExPtrs, 0, &tid);

    if (!cProc)
    {
        fprintf(stderr, "[ClamWin] W00ps!! Cannot spawn crash dumper thread (%d)\n", GetLastError());
        abort();
    }

    if (WaitForSingleObject(cProc, INFINITE) == WAIT_OBJECT_0)
        GetExitCodeThread(cProc, &res);

    CloseHandle(cProc);
    fprintf(stderr, "[ClamWin] Crash Dumper Thread Done (%d)\n", res);
    return EXCEPTION_EXECUTE_HANDLER;
}

#endif /* _MSC_VER */
