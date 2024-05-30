#include "Utils.h"
#include <filesystem>
#include <fstream>

Baker::Baker()
{
    hDevice = INVALID_HANDLE_VALUE;
    pid = NULL;
}

Baker::~Baker()
{
    printf("Baker's destructor called!\n");
    printf("Press any key to exit...\n");
    if (hDevice != INVALID_HANDLE_VALUE) {
        CloseHandle(hDevice);
    }
}

Baker* Baker::__instance = nullptr;

Baker* Baker::GetInstance()
{
    if (__instance == nullptr) {
        __instance = new Baker();
    }

    return __instance;
}

bool Baker::ChangePrivilege(const std::string& name)
{
    HANDLE hToken = NULL;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        return false;
    }

    LUID luid {};

    if (!LookupPrivilegeValueA(nullptr, name.c_str(), &luid)) {
        return false;
    }

    TOKEN_PRIVILEGES state {};

    state.PrivilegeCount = 1;
    state.Privileges[0].Luid = luid;
    state.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(hToken, FALSE, &state, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr)) {
        return false;
    }

    CloseHandle(hToken);

    return true;
}

std::wstring Baker::ExtractFilePath(const WCHAR* path)
{
    std::wstring wstrPath(path);

    // Find the last backslash or forward slash separator
    std::wstring::size_type pos = wstrPath.rfind(L'\\');
    if (pos == std::wstring::npos) {
        pos = wstrPath.rfind(L'/');
    }

    // Extract the path if a separator is found
    std::wstring extractedPath;
    if (pos != std::wstring::npos) {
        extractedPath = wstrPath.substr(0, pos + 1);
    }

    return extractedPath;
}

DWORD64 Baker::GetPEProcess()
{
    DWORD64 res;
    DWORD x;
    if (DeviceIoControl(hDevice, IOCTL_CE_GETPEPROCESS, &pid, 4, &res, 8, &x, nullptr)) {
        return res;
    }

    throw std::runtime_error("Error GetPEProcess");
}

bool Baker::InitializeDevice()
{
    SC_HANDLE hSCManager = OpenSCManager(nullptr, nullptr, GENERIC_READ | GENERIC_WRITE);
    WCHAR* appPath;
    std::wstring applicationPath, dataLocation, driverLocation;
    std::wstring serviceName, sysfile, processEventName, threadEventName;

    try {
        appPath = (WCHAR*)malloc(510);
        if (appPath != NULL) {
            GetModuleFileNameW(0, appPath, 250);
        }
        applicationPath = ExtractFilePath(appPath);
        dataLocation = applicationPath + L"driver64.dat";

        if (!std::filesystem::exists(dataLocation)) {
            serviceName = L"chokevy";
            processEventName = L"DBKProcList60";
            threadEventName = L"DBKThreadList60";
            sysfile = L"chokevy64.sys";
        } else {
            std::wifstream driverData(dataLocation);
            if (driverData.is_open()) {
                std::getline(driverData, serviceName);
                std::getline(driverData, processEventName);
                std::getline(driverData, threadEventName);
                std::getline(driverData, sysfile);
                driverData.close();
            } else {
                printf("Problem while trying open driverData\n");

                free(appPath);

                if (hSCManager != NULL) {
                    CloseServiceHandle(hSCManager);
                }

                return false;
            }
        }

        driverLocation = applicationPath + sysfile;
        if (!std::filesystem::exists(driverLocation)) {
            printf("Chokevy64.sys not found!\n");

            free(appPath);

            if (hSCManager != NULL) {
                CloseServiceHandle(hSCManager);
            }

            return false;
        }
    } catch (const std::exception& ex) {
        printf("Exception: %s\n", ex.what());

        free(appPath);

        if (hSCManager != NULL) {
            CloseServiceHandle(hSCManager);
        }

        return false;
    }

    free(appPath);

    if (hSCManager != NULL) {
        SC_HANDLE hService = OpenServiceW(hSCManager, serviceName.c_str(), SERVICE_ALL_ACCESS);
        if (hService == NULL) {
            hService = CreateServiceW(
                hSCManager, // SCManager database
                serviceName.c_str(), // name of service
                serviceName.c_str(), // name to display
                SERVICE_ALL_ACCESS, // desired access
                SERVICE_KERNEL_DRIVER, // service type
                SERVICE_DEMAND_START, // start type
                SERVICE_ERROR_NORMAL, // error control type
                driverLocation.c_str(), // service's binary
                nullptr, // no load ordering group
                nullptr, // no tag identifier
                nullptr, // no dependencies
                nullptr, // LocalSystem account
                nullptr // no password
            );
        } else {
            // Make sure the service points to the chokevy64 file
            ChangeServiceConfigW(hService,
                SERVICE_KERNEL_DRIVER,
                SERVICE_DEMAND_START,
                SERVICE_ERROR_NORMAL,
                driverLocation.c_str(),
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                serviceName.c_str() // name of service
            );
        }

        HKEY hKey;
        if (hService != NULL) {
            // Setup the configs before starting the driver
            LSTATUS result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (L"SYSTEM\\CurrentControlSet\\Services\\" + serviceName).c_str(), 0, KEY_READ | KEY_WRITE, &hKey);
            if (result != ERROR_SUCCESS) {
                printf("Error open registry\n");
                CloseServiceHandle(hService);
                CloseServiceHandle(hSCManager);

                return false;
            }

            RegSetValueEx(hKey, L"A", 0, REG_SZ, (LPBYTE)(L"\\Device\\" + serviceName).c_str(), (lstrlenW((L"\\Device\\" + serviceName).c_str()) + 1) * sizeof(wchar_t));
            RegSetValueEx(hKey, L"B", 0, REG_SZ, (LPBYTE)(L"\\DosDevices\\" + serviceName).c_str(), (lstrlenW((L"\\DosDevices\\" + serviceName).c_str()) + 1) * sizeof(wchar_t));
            RegSetValueEx(hKey, L"C", 0, REG_SZ, (LPBYTE)(L"\\BaseNamedObjects\\" + processEventName).c_str(), (lstrlenW((L"\\BaseNamedObjects\\" + processEventName).c_str()) + 1) * sizeof(wchar_t));
            RegSetValueEx(hKey, L"D", 0, REG_SZ, (LPBYTE)(L"\\BaseNamedObjects\\" + threadEventName).c_str(), (lstrlenW((L"\\BaseNamedObjects\\" + threadEventName).c_str()) + 1) * sizeof(wchar_t));

            if (!StartService(hService, 0, nullptr)) {
                DWORD errorCode = GetLastError();
                if (errorCode != 1056) {
                    printf("Failed to load Chokevy64.sys!\n");
                    if (errorCode == 577) {
                        printf("[+] Please RUN disable DSE.exe, type yes then hit enter.\n");
                        printf("[+] Keep disable DSE.exe opened then open this shit again.\n");
                        printf("[+] After saw \"Waiting for League process...\" you MUST return to disable DSE.exe window, type yes then enter.\n");
                    }

                    CloseServiceHandle(hService);
                    CloseServiceHandle(hSCManager);
                    RegCloseKey(hKey);

                    return false;
                }

                printf("Baker has started\n");
            }

            CloseServiceHandle(hService);
            hService = NULL;
        } else {
            printf("The service couldn't get opened\n");
            CloseServiceHandle(hSCManager);
            return false;
        }

        hDevice = INVALID_HANDLE_VALUE;
        hDevice = CreateFileW((L"\\\\\.\\" + serviceName).c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,
            0);

        if (hDevice == INVALID_HANDLE_VALUE) {
            printf("Driver couldn't be loaded\n");
            CloseServiceHandle(hSCManager);
            RegCloseKey(hKey);

            return false;
        } else {
            printf("Baker loaded!\n");

            // Successfully initialized, gtfo the init params
            RegDeleteValue(hKey, L"A");
            RegDeleteValue(hKey, L"B");
            RegDeleteValue(hKey, L"C");
            RegDeleteValue(hKey, L"D");

            CloseServiceHandle(hSCManager);
            RegCloseKey(hKey);
        }
    } else {
        printf("hSCManager is NULL\n");
        return false;
    }

    return true;
}

bool Baker::GetProcessId()
{
    HWND wnd = FindWindowW(NULL, L"League of Legends (TM) Client");
    if (wnd == NULL) {
        return false;
    }

    GetWindowThreadProcessId(wnd, &pid);
    if (!pid || pid == 4) {
        return false;
    }

    return true;
}

bool Baker::IsWindowActive()
{
    HWND wnd = GetForegroundWindow();
    if (wnd == NULL) {
        return false;
    }

    DWORD h;
    GetWindowThreadProcessId(wnd, &h);
    return pid == h;
}

bool Baker::ReadProcessMemory64_Internal(DWORD64 lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead)
{
    __Request req;
    DWORD x;
    DWORD64 mempointer;
    DWORD64 bufpointer;
    DWORD toread;

    *lpNumberOfBytesRead = 0;
    mempointer = lpBaseAddress;
    bufpointer = (DWORD64)lpBuffer;

    while (true) {
        if ((mempointer & 0xfff) > 0) {
            toread = 4096 - (mempointer & 0xfff);
            if (toread > (nSize - *lpNumberOfBytesRead)) {
                toread = nSize - *lpNumberOfBytesRead;
            }
        } else {
            if (nSize - *lpNumberOfBytesRead >= 4096) {
                toread = 4096;
            } else {
                toread = nSize - *lpNumberOfBytesRead;
            }
        }

        req.processid = pid;
        req.bytestoread = toread;
        req.startaddress = mempointer;

        if (!DeviceIoControl(hDevice, IOCTL_CE_READMEMORY, &req, sizeof(req), (LPVOID)bufpointer, toread, &x, nullptr)) {
            return false;
        }

        mempointer += toread;
        bufpointer += toread;
        *lpNumberOfBytesRead = *lpNumberOfBytesRead + toread;

        if (*lpNumberOfBytesRead == nSize) {
            return true;
        }
    }
}

DWORD64 Baker::GetModuleBase()
{
    try {
        DWORD64 result = GetPEProcess();
        return Read<DWORD64>(result + 0x520);
    } catch (const std::runtime_error&) {
        return NULL;
    }
}

bool Baker::ReadProcessMemory64(DWORD64 lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead)
{
    return ReadProcessMemory64_Internal(lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);
}

bool Baker::ReadProcessMemory(LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead)
{
    return ReadProcessMemory64((DWORD64)lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);
}

template <typename T>
T Baker::Read(DWORD64 addr)
{
    T res = NULL;
    SIZE_T x = 0;
    if (ReadProcessMemory((LPCVOID)addr, &res, sizeof(T), &x)) {
        return res;
    }

    return NULL;
}

DWORD64 Mem::ReadDWORD(DWORD64 addr)
{
    DWORD64 res = NULL;
    SIZE_T x = 0;
    if (Baker::GetInstance()->ReadProcessMemory((LPCVOID)addr, &res, 8, &x)) {
        return res;
    }

    return NULL;
}

void Mem::Read(DWORD64 addr, void* structure, int size)
{
    SIZE_T x = 0;
    Baker::GetInstance()->ReadProcessMemory((LPCVOID)addr, structure, size, &x);
}

DWORD64 Mem::ReadDWORDFromBuffer(void* buff, int position)
{
    DWORD64 result;
    memcpy(&result, (char*)buff + position, 8);
    return result;
}

bool Character::ContainsOnlyASCII(const char* buff, int maxSize)
{
    for (int i = 0; i < maxSize; ++i) {
        if (buff[i] == 0)
            return true;
        if ((unsigned char)buff[i] > 127)
            return false;
    }

    return true;
}

std::string Character::ToLower(std::string str)
{
    std::string result;
    result.resize(str.size());

    std::transform(str.begin(),
        str.end(),
        result.begin(),
        ::tolower);

    return result;
}

std::string Character::RandomString(const int len)
{
    std::string result;
    static const char alphanum[] = "0123456789"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "!@#$%^&*"
                                   "abcdefghijklmnopqrstuvwxyz";

    srand((unsigned int)time(0));
    result.reserve(len);

    for (int i = 0; i < len; ++i)
        result += alphanum[rand() % (sizeof(alphanum) - 1)];

    return result;
}

std::string Character::Format(const char* c, const char* args...)
{
    char buff[200];
    sprintf_s(buff, c, args);

    return std::string(buff);
}