#pragma once
#include "Includes.h"
#include "ESP.h"


class Overlay
{
public:
	Overlay(const wchar_t* windowName);
	~Overlay();
	bool isMenuVisible();
	void toggleMenu();
	void startRender();
	void render();
	void endRender();
	void renderLoop();
	void handleMessages();

	bool showMenu = false;
	bool showEsp = true;

private:
	WNDCLASSEXW wc;
	HWND windowHandle = nullptr;

	ID3D11Device* d3dDevice = nullptr;
	ID3D11DeviceContext* d3dDeviceContext = nullptr;
	IDXGISwapChain* d3dSwapChain = nullptr;
	ID3D11RenderTargetView* d3dRenderTargetView = nullptr;

	unsigned int resizeWidth;
	unsigned int resizeHeight;

	bool styleApplied = false;
};

