#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <cmath>
#include <utility>
#include <vector>

using namespace std;

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Code::Blocks Template Windows App"),       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           544,                 /* The programs width */
           375,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

class Ellispe {
private:
    std::vector<std::pair<int, int>> ellipse;
    std::pair<int, int> center, a_, b_;
    char m_;

    std::vector<std::pair<int, int>> direct(std::pair<int, int> c,
                                            std::pair<int, int> a,
                                            std::pair<int, int> b) {
        double a2 = (double)a.first * a.first;
        int x = 0;

        while (x <= a.first) {
            // Essential Fix: Use b.second and force double division
            int y = std::round(b.second * std::sqrt(1.0 - ((double)x * x) / a2));

            // Essential Fix: Add symmetry (4 points) and center offset
            ellipse.push_back({ c.first + x, c.second + y });
            ellipse.push_back({ c.first - x, c.second + y });
            ellipse.push_back({ c.first + x, c.second - y });
            ellipse.push_back({ c.first - x, c.second - y });
            x++;
        }
        return ellipse;
    }

public:
    std::vector<std::pair<int, int>> draw_ellipse(std::pair<int, int> c,
                                                std::pair<int, int> a,
                                                std::pair<int, int> b,
                                                char m) {
	center = c;
        a_ = a;
        b_ = b;
        m_ = m;
        ellipse.clear();
        a.second = c.second;
        b.first = c.first;
        switch (m) {
            case 'd':
                return direct(c, a, b);
        }
        return ellipse;
    }

};

/*  This function is called by the Windows function DispatchMessage()  */

vector<pair<int, int>> vertices;

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
     HDC hdc = GetDC(hwnd);

    switch (message)                  /* handle the messages */
    {
        case WM_LBUTTONDBLCLK:

            vertices.push_back(make_pair(LOWORD(lParam), HIWORD(lParam)));
            if (vertices.size() == 3)
            {
                char mode = 'd';
                Ellispe elli;
                elli.draw_ellipse(vertices[0], vertices[1], vertices[2], mode);
                vertices.clear();
            }

            break;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
