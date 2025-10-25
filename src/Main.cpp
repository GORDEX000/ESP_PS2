#include <Overlay.h>
#include "ESP.h"

int main()
{
    DWORD pid;

    while (true)
    {
        system("cls");
        pid = get_process_id(L"PlanetSide2_x64_BE.exe");
        if (pid)
            break; // found it

        std::cout << "[-] Waiting for PlanetSide2_x64_BE.exe...\n";
        std::this_thread::sleep_for(std::chrono::seconds(1)); // wait 1 second
    }

    std::cout << "[+] Found process! PID: 0x" << std::hex << pid << "\n";

    hProc = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!hProc)
    {
        std::cout << "[-] Failed to create hProc\n";
        return 1;
    }
    std::cout << "[+] hProc created\n";


    gameModuleBase = get_module_base(pid, L"PlanetSide2_x64_BE.exe");
    if (!gameModuleBase)
    {
        std::cout << "[-] Failed to find moduleBase PlanetSide2_x64_BE.exe\n";
        return 1;
    }
    std::cout << "[+] moduelBase: " << std::hex << gameModuleBase << std::endl;

    if (!Vec::InitializeViewMatrixAddress(gameModuleBase))
    {
        CloseHandle(hProc);
        std::cout << "[-] Failed to Initialize ViewMatrix\n";
        return 1;
    }
    std::cout << "[+] Initialized ViewMatrix\n";

    Overlay overlay(L"test");
    overlay.renderLoop(); // ✅ stays here. It loops forever, never returns.

    return 0; // <- this is never actually reached
}
