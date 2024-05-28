#pragma once
#include <Windows.h>
#include <stdio.h>
#include <string>

constexpr DWORD FILE_RW_ACCESS = FILE_READ_ACCESS | FILE_WRITE_ACCESS;
constexpr DWORD IOCTL_UNKNOWN_BASE = FILE_DEVICE_UNKNOWN;
constexpr DWORD IOCTL_CE_READMEMORY = (IOCTL_UNKNOWN_BASE << 16) | (0x0800 << 2) | (METHOD_BUFFERED) | (FILE_RW_ACCESS << 14);
constexpr DWORD IOCTL_CE_GETPEPROCESS = (IOCTL_UNKNOWN_BASE << 16) | (0x0805 << 2) | (METHOD_BUFFERED) | (FILE_RW_ACCESS << 14);

class Baker {
public:
    Baker();
    ~Baker();
    bool InitializeDevice();
    bool GetProcessId();
    bool IsWindowActive();
    DWORD64 GetModuleBase();
    bool ReadProcessMemory(LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead);

    template <typename T>
    T Read(DWORD64 addr);

private:
    HANDLE hDevice;
    DWORD pid;
    static Baker* __instance;
    std::wstring ExtractFilePath(const WCHAR* path);
    DWORD64 GetPEProcess();
    bool ReadProcessMemory64(DWORD64 lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead);
    bool ReadProcessMemory64_Internal(DWORD64 lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead);

    struct __Request {
        UINT64 processid;
        UINT64 startaddress;
        WORD bytestoread;
    };

public:
    static Baker* GetInstance();
    static bool ChangePrivilege(const std::string& name);
};

/// Utilities used for realing process memory
namespace Mem {

/// Reads a DWORD64 at the specified memory location
DWORD64 ReadDWORD(DWORD64 addr);

/// Reads an arbitrary struct at the specified memory location
void Read(DWORD64 addr, void* structure, int size);

/// Reads a DWORD64 at the specified location in a given buffer
DWORD64 ReadDWORDFromBuffer(void* buff, int position);
};

namespace Character {
std::string ToLower(std::string str);
std::string RandomString(const int len);
std::string Format(const char* c, const char* args...);
bool ContainsOnlyASCII(const char* buff, int maxSize);
}