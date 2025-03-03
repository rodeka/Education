#define UNICODE
#define _UNICODE
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>

// размер экрана и макс.кол-во кнопок
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define MAX_BUTTONS 1000

// структура данных для кнопки
typedef struct {
    HWND hwnd;
    RECT rect;
} ButtonInfo;

ButtonInfo buttons[MAX_BUTTONS];
int buttonCount = 0;

// глоб.парам.для кнопки (текст, высота, широта)
WCHAR g_buttonText[256];
int g_buttonWidth = 100;
int g_buttonHeight = 50;

// проверка на пересечение
BOOL DoesRectOverlap(RECT newRect) {
    for (int i = 0; i < buttonCount; i++) {
        RECT r = buttons[i].rect;
        if (!(newRect.right <= r.left || newRect.left >= r.right ||
              newRect.bottom <= r.top || newRect.top >= r.bottom)) {
            return TRUE;
        }
    }
    return FALSE;
}

// проц.окна
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
    case WM_CREATE: { // создание кнопки
        srand((unsigned)time(NULL));
        // нач.кнопка по центру
        int x = (WINDOW_WIDTH - g_buttonWidth) / 2;
        int y = (WINDOW_HEIGHT - g_buttonHeight) / 2;
        HWND btn = CreateWindowW(L"BUTTON", g_buttonText,
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            x, y, g_buttonWidth, g_buttonHeight,
            hwnd, (HMENU)(UINT_PTR)(buttonCount + 1), ((LPCREATESTRUCT)lParam)->hInstance, NULL);
        
        buttons[buttonCount].hwnd = btn;
        buttons[buttonCount].rect.left = x;
        buttons[buttonCount].rect.top = y;
        buttons[buttonCount].rect.right = x + g_buttonWidth;
        buttons[buttonCount].rect.bottom = y + g_buttonHeight;
        buttonCount++;
        break;
    }
    case WM_COMMAND: { // создании кнопки по нажатию
        if (HIWORD(wParam) == BN_CLICKED) {
            RECT newRect;
            int attempts = 100;
            BOOL validPos = FALSE;
            int newX, newY;
            while (attempts-- > 0) {
                newX = rand() % (WINDOW_WIDTH - g_buttonWidth + 1);
                newY = rand() % (WINDOW_HEIGHT - g_buttonHeight + 1);
                newRect.left = newX;
                newRect.top = newY;
                newRect.right = newX + g_buttonWidth;
                newRect.bottom = newY + g_buttonHeight;
                if (!DoesRectOverlap(newRect)) {
                    validPos = TRUE;
                    break;
                }
            }
            if (validPos && buttonCount < MAX_BUTTONS) {
                HWND btn = CreateWindowW(L"BUTTON", g_buttonText,
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    newX, newY, g_buttonWidth, g_buttonHeight,
                    hwnd, (HMENU)(UINT_PTR)(buttonCount + 1), GetModuleHandle(NULL), NULL);
                buttons[buttonCount].hwnd = btn;
                buttons[buttonCount].rect = newRect;
                buttonCount++;
            }
        }
        break;
    }
    case WM_DESTROY: // закрыть окно
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// входная функция
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argc < 4) {
        MessageBoxW(NULL, L"Usage: program.exe buttonText buttonHeight buttonWidth", L"Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    wcscpy_s(g_buttonText, 256, argv[1]);
    g_buttonHeight = _wtoi(argv[2]);
    g_buttonWidth = _wtoi(argv[3]);
    LocalFree(argv);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MyWindowClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    RECT rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hwnd = CreateWindowW(L"MyWindowClass", L"Button Spawner",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, hInstance, NULL);

    if (!hwnd) {
        return 0;
    }
    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
