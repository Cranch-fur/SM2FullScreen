#include "pch.h"
#include <Windows.h>
#include <thread>


#define FIND_WINDOW_COOLDOWN 1000 // Milliseconds
#define FIND_WINDOW_MAXATTEMPTS 15




bool GetStartupArgumentSpecified(const wchar_t* commandLineArgument) 
{
    LPWSTR lpCmdLine = GetCommandLineW();
    bool returnValue = wcsstr(lpCmdLine, commandLineArgument) != NULL;

    return returnValue;
}


bool GetIsWindowFullscreen(HWND hWnd)
{
    RECT windowRect;
    GetWindowRect(hWnd, &windowRect);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    bool returnValue = (windowRect.left == 0 && windowRect.top == 0 && windowRect.right == screenWidth && windowRect.bottom == screenHeight);

    return returnValue;
}


void Handle_FullScreen() 
{
    if (GetStartupArgumentSpecified(L"-customfullscreen") == true)
    {
        HWND hWnd = NULL;
        BYTE searchAttempts = 0;
        while (hWnd == NULL)
        {
            if (searchAttempts > FIND_WINDOW_MAXATTEMPTS) {
                return; // Force stop handle execution;
            }
            else {
                hWnd = FindWindowA("InsomWindowClass", "Marvel's Spider-Man 2");
                if (hWnd != NULL) {
                    continue;
                }

                hWnd = FindWindowA("InsomWindowClass", "Remnant-Win64-Shipping.exe");
                if (hWnd != NULL) {
                    continue;
                }

                hWnd = FindWindowA("InsomWindowClass", "i30.exe");
                if (hWnd != NULL) {
                    continue;
                }

                searchAttempts++;
                Sleep(FIND_WINDOW_COOLDOWN);
            }
        }

        if (GetIsWindowFullscreen(hWnd) == false) {
            // Due to WindowsAPI glitch, we have to SetWindowLong() before SetWindowPos(). Performing these actions in different order will result toolbar to appear on top of game window.
            SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_CAPTION & ~WS_THICKFRAME);
            SetWindowPos(hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
            SetFocus(hWnd);
        }
    }
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        std::thread(Handle_FullScreen).detach();
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}
    