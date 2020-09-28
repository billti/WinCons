#include "common.h"
#include "logging.h"

const wchar_t className[] = L"MainClass";
const wchar_t titleText[] = L"My app";

DiagOutput Log;

BOOL RegisterClass(HINSTANCE hInst);
HWND CreateInstance(HINSTANCE hInst, int cmdShow);
LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE hPrev,
                      LPWSTR cmdLine, int cmdShow)
{
  // __debugbreak();
  Log.CreateConsole();
  Log.Write("\n\nApplication started\n");

  if (!RegisterClass(hInst))
    return E_FAIL;

  HWND hwnd = CreateInstance(hInst, cmdShow);
  if (!hwnd)
    return E_FAIL;

  int msgResult;
  MSG msg;

  Log.Write("Entering the message loop\n");
  // Use 0 for the hWnd, to process all messages for this thread
  while ((msgResult = GetMessage(&msg, 0, 0, 0)) != 0)
  {
    if (msgResult == -1)
    {
      // Error condition
      Log.Write("Error returned on GetMessage\n");
      return GetLastError();
    }
    else
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
  Log.Write("Exiting the message loop\n");

  // Return code from WM_QUIT message
  return msg.wParam;
}

BOOL RegisterClass(HINSTANCE hInst)
{
  WNDCLASSEXW wndClass;
  ZeroMemory(&wndClass, sizeof(wndClass));
  wndClass.cbSize = sizeof(wndClass);
  wndClass.lpfnWndProc = WinProc;
  wndClass.hInstance = hInst;
  wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  wndClass.lpszClassName = className;
  wndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
  // May also want to specify a window style, background color, or menu

  return RegisterClassEx(&wndClass);
}

HWND CreateInstance(HINSTANCE hInst, int cmdShow)
{
  HWND hwnd = CreateWindowEx(
      0, // ExStyle
      className,
      titleText,
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, // x & y position
      CW_USEDEFAULT, CW_USEDEFAULT, // width & height
      (HWND)NULL, (HMENU)NULL,      // owner & menu
      hInst, nullptr);
  if (!hwnd)
    return NULL;

  ShowWindow(hwnd, cmdShow);
  UpdateWindow(hwnd);
  return hwnd;
}

LRESULT WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
  case WM_CREATE:
    Log.Write("WM_CREATE message received\n");
    // Could extract state passed from CreateWindowEx here
    break;
  case WM_PAINT:
    Log.Write("WM_PAINT message received\n");
    break;
  case WM_SIZE:
    Log.Write("WM_SIZE message received\n");
    break;
  case WM_CLOSE:
    Log.Write("WM_CLOSE message received\n");
    // Could ask for confirmation before calling DestroyWindow here
    break;
  case WM_DESTROY:
    Log.Write("WM_DESTROY message received\n");
    // If destroying the main window, quit the app.
    PostQuitMessage(0);
    return 0;
  default:
    // _cwprintf(L"Got message with id: %d\n", uMsg);
    break;
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
