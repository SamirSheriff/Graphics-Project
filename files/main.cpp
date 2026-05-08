#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <sstream>
#include "shapes.h"

using namespace std;


/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

vector<shape*> shapes;

void SaveShapes(const string filename)
{
    ofstream out(filename);

    for(auto s : shapes)
        s->save(out);
}


void ClearShapes(vector<shape*>& shapes, HWND hwnd)
{
    // 1. Free memory
    for(auto s : shapes)
        delete s;

    // 2. Clear vector
    shapes.clear();

    // 3. Redraw window
    InvalidateRect(hwnd, NULL, TRUE);
}

void LoadShapes(const char* filename, vector<shape*>& shapes)
{
    ifstream in(filename);
    if(!in) return;

    shapes.clear();

    string type;

    while(in >> type)
    {
        // ================= LINE =================
        if(type == "LINE")
        {
            int algo, x1, y1, x2, y2;
            int r, g, b;

            in >> algo >> x1 >> y1 >> x2 >> y2 >> r >> g >> b;

            COLORREF color = RGB(r,g,b);

            shapes.push_back(new Line(x1,y1,x2,y2,algo,color));
        }
    }
}

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

    // MENU
    HMENU menu=CreateMenu();
    HMENU file=CreateMenu();
    HMENU Pref=CreateMenu();
    HMENU line=CreateMenu();
    HMENU circle=CreateMenu();
    HMENU Ellipse=CreateMenu();
    HMENU Curve=CreateMenu();
    HMENU Filling=CreateMenu();
    HMENU Clipping=CreateMenu();
    HMENU faces=CreateMenu();

    AppendMenu(file,MF_STRING,1,"Clear");
    AppendMenu(file,MF_STRING,2,"Save");
    AppendMenu(file,MF_STRING,3,"Load");

    AppendMenu(Pref,MF_STRING,10,"Make background white");
    AppendMenu(Pref,MF_STRING,11,"Cursor");
    AppendMenu(Pref,MF_STRING,12,"Color");

    AppendMenu(line,MF_STRING,20,"Parametric");
    AppendMenu(line,MF_STRING,21,"DDA");
    AppendMenu(line,MF_STRING,22,"Midpoint");

    AppendMenu(circle,MF_STRING,30,"Direct");
    AppendMenu(circle,MF_STRING,31,"Polar,");
    AppendMenu(circle,MF_STRING,32,"Iterative Polar");
    AppendMenu(circle,MF_STRING,33,"Midpoint");
    AppendMenu(circle,MF_STRING,34,"Modified Midpoint");

    AppendMenu(Ellipse,MF_STRING,40,"Direct,");
    AppendMenu(Ellipse,MF_STRING,41,"Polar");
    AppendMenu(Ellipse,MF_STRING,42,"Midpoint");

    AppendMenu(Curve,MF_STRING,50,"Cardinal Spline");

    AppendMenu(Filling,MF_STRING,60,"Filling Circle with lines");
    AppendMenu(Filling,MF_STRING,61,"Filling Circle with CircleS");
    AppendMenu(Filling,MF_STRING,62,"Filling Square with Hermit");
    AppendMenu(Filling,MF_STRING,63,"Filling Rectangle with Bezier");
    AppendMenu(Filling,MF_STRING,64,"Convex Filling");
    AppendMenu(Filling,MF_STRING,65,"Non-Convex Filling");
    AppendMenu(Filling,MF_STRING,66,"Recursive Flood Fill");
    AppendMenu(Filling,MF_STRING,67,"Non-Recursive Flood Fill");

    AppendMenu(menu,MF_POPUP,(UINT_PTR)file,"File");
    AppendMenu(menu,MF_POPUP,(UINT_PTR)Pref,"Preferences");
    AppendMenu(menu,MF_POPUP,(UINT_PTR)line,"Lines");
    AppendMenu(menu,MF_POPUP,(UINT_PTR)circle,"Circles");
    AppendMenu(menu,MF_POPUP,(UINT_PTR)Ellipse,"Ellipse");
    AppendMenu(menu,MF_POPUP,(UINT_PTR)Curve,"Curve");
    AppendMenu(menu,MF_POPUP,(UINT_PTR)Filling,"Filling");
    AppendMenu(menu,MF_POPUP,(UINT_PTR)Clipping,"Clipping");
    AppendMenu(menu,MF_POPUP,(UINT_PTR)faces,"faces");

    SetMenu(hwnd,menu);

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


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
