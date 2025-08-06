#include "WindowsWindow.h"

#if defined(DAISY_PLATFORM_WINDOWS) && defined(_WIN32)

namespace Daisy {

bool WindowsWindow::s_classRegistered = false;

WindowsWindow::~WindowsWindow() {
    Shutdown();
}

bool WindowsWindow::Initialize(const WindowProperties& props) {
    m_data.title = props.title;
    m_data.width = props.width;
    m_data.height = props.height;
    m_data.vsync = props.vsync;
    
    m_hinstance = GetModuleHandle(nullptr);
    
    // Register window class if not already registered
    if (!s_classRegistered) {
        WNDCLASSEXA wc = {};
        wc.cbSize = sizeof(WNDCLASSEXA);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = m_hinstance;
        wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = "DaisyEngineWindow";
        wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
        
        if (!RegisterClassExA(&wc)) {
            DAISY_ERROR("Failed to register window class");
            return false;
        }
        s_classRegistered = true;
    }
    
    // Calculate window size including borders
    RECT windowRect = {0, 0, m_data.width, m_data.height};
    DWORD style = WS_OVERLAPPEDWINDOW;
    if (props.resizable) {
        style |= WS_THICKFRAME;
    }
    
    AdjustWindowRect(&windowRect, style, FALSE);
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;
    
    // Create window using ANSI version
    m_hwnd = CreateWindowExA(
        0,
        "DaisyEngineWindow",
        m_data.title.c_str(),
        style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowWidth, windowHeight,
        nullptr, nullptr, m_hinstance, this
    );
    
    if (!m_hwnd) {
        DAISY_ERROR("Failed to create window");
        return false;
    }
    
    // Show window
    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
    
    DAISY_INFO("Created Windows window: {} ({}x{})", m_data.title, m_data.width, m_data.height);
    return true;
}

void WindowsWindow::Shutdown() {
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
}

void WindowsWindow::Update() {
    MSG msg = {};
    while (PeekMessage(&msg, m_hwnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT CALLBACK WindowsWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    WindowsWindow* window = nullptr;
    
    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<WindowsWindow*>(create->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    } else {
        window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    
    if (window) {
        switch (uMsg) {
            case WM_CLOSE:
                window->m_shouldClose = true;
                return 0;
                
            case WM_SIZE: {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);
                window->m_data.width = width;
                window->m_data.height = height;
                
                // TODO: Notify renderer of resize
                break;
            }
            
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP: {
                // TODO: Handle keyboard input
                break;
            }
            
            case WM_MOUSEMOVE:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP: {
                // TODO: Handle mouse input
                break;
            }
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

}

#endif