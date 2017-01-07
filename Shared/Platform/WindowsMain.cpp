#include <Globals.h>
#include <VulkanAPI/Device.h>
#include <VulkanAPI/Instance.h>
#include <Platform/NativeWindow.h>
#include <Platform/Application.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static void OnMouseMoveEvent(WPARAM w, LPARAM l)
{
    std::uint32_t buttons = 0;
    int x = LOWORD(l), y = HIWORD(l);
    buttons |= (w & MK_LBUTTON) ? LBUTTON_DOWN : 0;
    buttons |= (w & MK_RBUTTON) ? RBUTTON_DOWN : 0;
    Application::MoveInputPointerTo(x, y, buttons);
}

LRESULT WINAPI WndProc(HWND _wnd, UINT _msg, WPARAM _w, LPARAM _l)
{
    LRESULT retVal = 0;
    switch (_msg)
    {
    case WM_SIZE:
        break;
    case WM_CLOSE:
        PostQuitMessage(0);
        Application::Shutdown();
        break;
    case WM_MOUSEMOVE:
        OnMouseMoveEvent(_w, _l);
        break;
    default:
        retVal = DefWindowProc(_wnd, _msg, _w, _l);
        break;
    }
    return retVal;
}

bool CreateNativeWindow(NativeWindow &window)
{
    WNDCLASS windowClass;
    windowClass.style = CS_OWNDC;
    windowClass.lpfnWndProc = (WNDPROC)WndProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = GetModuleHandle(NULL);
    windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = "VulkanDemo";
    if ( !RegisterClassA(&windowClass) )
    {
        return false;
    }

    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    int halfWidth = GetSystemMetrics(SM_CXSCREEN) >> 1;
    int halfHeight = GetSystemMetrics(SM_CYSCREEN) >> 1;
    window.Size.width = halfWidth + (halfWidth >> 1);
    window.Size.height = halfHeight + (halfHeight >> 1);
    SetRect(&window.Bounds, 0, 0, window.Size.width, window.Size.height);
    AdjustWindowRect(&window.Bounds, style, FALSE);
    int rows = window.Bounds.bottom - window.Bounds.top;
    int cols = window.Bounds.right - window.Bounds.left;
    int left = (GetSystemMetrics(SM_CXSCREEN) - cols) >> 1;
    int top = (GetSystemMetrics(SM_CYSCREEN) - rows) >> 1;
    window.Handle = CreateWindowA("VulkanDemo", "", style, left, top, cols, rows, NULL, NULL, GetModuleHandle(NULL), NULL);
    if (!window.Handle)
    {
        return false;
    }

    return true;
}

void RunNativeEventLoop(NativeWindow &window)
{
    window.KeepRunning = true;
    ShowWindow(window.Handle, SW_SHOWDEFAULT);
    while (window.KeepRunning)
    {
        while (PeekMessage(&window.Message, window.Handle, 0, 0, PM_REMOVE))
        {
            if (window.Message.message != WM_QUIT)
            {
                TranslateMessage(&window.Message);
                DispatchMessage(&window.Message);
                continue;
            }
            window.KeepRunning = false;
            break;
        }
    }
}

int WINAPI WinMain(HINSTANCE _inst, HINSTANCE _prev, LPSTR _cmd, int _show)
{
    NativeWindow nativeWindow;
    if (CreateNativeWindow(nativeWindow))
    {
        if (InitApp(nativeWindow))
        {
            Application::StartRenderThread();
            RunNativeEventLoop(nativeWindow);
        }
    }
    return 0;
}
