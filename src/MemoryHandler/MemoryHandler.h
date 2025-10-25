#pragma once
#include "../Includes.h"

template <typename T>
bool read(HANDLE hProc, uintptr_t addr, T& out) {
	SIZE_T bytesRead = 0;
	return ReadProcessMemory(hProc, reinterpret_cast<LPCVOID>(addr), &out, sizeof(T), &bytesRead)
		&& bytesRead == sizeof(T);
}

template <typename T>
bool write(HANDLE hProc, uintptr_t addr, const T& in) {
	SIZE_T bytesWritten = 0;
	return WriteProcessMemory(hProc, reinterpret_cast<LPVOID>(addr), &in, sizeof(T), &bytesWritten)
		&& bytesWritten == sizeof(T);
}

inline static std::vector<unsigned char> ReadBytes(HANDLE hProc, uintptr_t address, size_t size)
{
	std::vector<unsigned char> buffer(size);
	SIZE_T bytesRead = 0;
	if (ReadProcessMemory(hProc, reinterpret_cast<LPCVOID>(address), buffer.data(), size, &bytesRead) && bytesRead == size) {
		return buffer;
	}
	return std::vector<unsigned char>(); // Return empty on failure
}

static DWORD get_process_id(const wchar_t* process_name)
{
	DWORD process_id = 0;

	HANDLE snap_shot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (snap_shot == INVALID_HANDLE_VALUE)
		return process_id;

	PROCESSENTRY32 entry = {};
	entry.dwSize = sizeof(decltype(entry));

	if (Process32FirstW(snap_shot, &entry) == TRUE)
	{
		// Check if the firts handle is the one we want.
		if (_wcsicmp(process_name, entry.szExeFile) == 0)
			process_id = entry.th32ProcessID;
		else
		{
			while (Process32NextW(snap_shot, &entry) == TRUE)
			{
				if (_wcsicmp(process_name, entry.szExeFile) == 0)
				{
					process_id = entry.th32ProcessID;
					break;
				}
			}
		}
	}
	CloseHandle(snap_shot);
	return process_id;
}

static uintptr_t get_module_base(DWORD pid, const wchar_t* module_name)
{
	uintptr_t module_base = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	if (snapshot == INVALID_HANDLE_VALUE)
		return 0;

	MODULEENTRY32W entry{};
	entry.dwSize = sizeof(entry);

	if (Module32FirstW(snapshot, &entry))
	{
		do
		{
			// Compare full module name case-insensitively
			if (_wcsicmp(entry.szModule, module_name) == 0)
			{
				module_base = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
				break;
			}
		} while (Module32NextW(snapshot, &entry));
	}

	CloseHandle(snapshot);
	return module_base;
}


static HWND GetMainWindow(DWORD pid) {
	struct HandleData { DWORD pid; HWND hwnd; } data{ pid, nullptr };
	auto enumWindowsCallback = [](HWND hwnd, LPARAM lParam) -> BOOL {
		auto* pData = reinterpret_cast<HandleData*>(lParam);
		DWORD windowPid = 0;
		GetWindowThreadProcessId(hwnd, &windowPid);
		if (windowPid == pData->pid && IsWindowVisible(hwnd) && GetWindowTextLength(hwnd) > 0) { // Check for visible and non-empty title
			pData->hwnd = hwnd;
			return FALSE;
		}
		return TRUE;
		};
	EnumWindows(enumWindowsCallback, reinterpret_cast<LPARAM>(&data));
	return data.hwnd;
}


inline uintptr_t ResolvePointer(void* hProcess, uintptr_t baseAddress, const std::vector<uintptr_t>& offsets)
{
	uintptr_t currentAddress = baseAddress;
	for (size_t i = 0; i < offsets.size(); ++i) {
		if (!read(hProcess, currentAddress, currentAddress)) {
			return 0; // Failed to read pointer
		}
		currentAddress += offsets[i];
	}
	return currentAddress;
}