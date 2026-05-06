#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <vector>
#include <utility>
#include "ellipse.hpp" // Ensure your class is named 'myEllipse' here

// Global/Static objects
myEllipse e;
std::vector<POINT> clicks;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_LBUTTONDOWN:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        clicks.push_back({ x, y });

        // Logic for 3 clicks
        if (clicks.size() == 3)
        {
            std::pair<int, int> c = { clicks[0].x, clicks[0].y };
            
            // rx is the horizontal distance from center (click 0) to click 1
            std::pair<int, int> a = { abs(clicks[1].x - clicks[0].x), 0 };
            
            // ry is the vertical distance from center (click 0) to click 2
            std::pair<int, int> b = { 0, abs(clicks[2].y - clicks[0].y) };

            auto pts = e.draw_ellipse(c, a, b, 'm');

            HDC hdc = GetDC(hwnd);

	    short line = 0;
	    int point = 0;
            for (auto &pt : pts)
            {
		if (line > 3){
			// if you make it -1 the ellipse will be filled and -4 will make strange line go from ellipse to 
			// boundry and -3 is combination with -4 and -1
			//MoveToEx(hdc,pts[point-1].first, pts[point-1].second,NULL);
			//LineTo(hdc,pts[point].first, pts[point].second);
			}
	// if(t==0)MoveToEx(hdc,x,y,NULL);else LineTo(hdc,x,y);
                SetPixel(hdc, pt.first, pt.second, RGB(255, 0, 0)); 
		line ++ ;
		point++;
            }

            ReleaseDC(hwnd, hdc);
            clicks.clear(); // Reset for next ellipse
        }
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, PWSTR pCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"EllipseWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS_NAME;
    // Set a cursor so you know where you are clicking
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Ellipse Drawer - 3 Clicks (Center, Width, Height)",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL, NULL, hInst, NULL
    );

    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
