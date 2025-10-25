#pragma once

#include <windows.h>
#include <TlHelp32.h>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
#include <dwmapi.h>
#include <d3d11.h>
#include <unordered_map>
#include <iostream>
#include <time.h>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include<chrono>
#include <thread>

#pragma comment(lib, "dwmapi.lib")

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"