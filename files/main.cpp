#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include "shapes.h"
#include <tchar.h>
#include <sstream>
#include <commdlg.h>

using namespace std;


/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

vector<shape*> shapes;
COLORREF currentColor = RGB(0,0,0);
HBRUSH bgBrush = CreateSolidBrush(RGB(240,240,240));
HCURSOR currentCursor = LoadCursor(NULL, IDC_ARROW);

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

    string type;

    while(in >> type)
    {
        if(type == "LINE")
        {
            int algo, x1, y1, x2, y2;
            int r, g, b;

            in >> algo >> x1 >> y1 >> x2 >> y2 >> r >> g >> b;

            COLORREF color = RGB(r,g,b);

            shapes.push_back(new Line(x1,y1,x2,y2,algo,color));
        }

        else if(type == "CIRCLE")
        {
            int algo, xc, yc, rad;
            int r, g, b;

            in >> algo >> xc >> yc >> rad >> r >> g >> b;

            COLORREF color = RGB(r,g,b);

            shapes.push_back(new Circle(xc,yc,rad,algo,color));
        }

        // ================= ELLIPSE =================
//        else if(type == "Ellipse")
//        {
//            int algo;
//            int cx, cy;
//            int ax, ay, bx, by;
//            int r, g, b;
//
//            in >> algo >> cx >> cy
//               >> ax >> ay >> bx >> by
//               >> r >> g >> b;
//
//            COLORREF color = RGB(r,g,b);
//
//            myEllipse* e = new myEllipse();
//            e->center = {cx, cy};
//            e->a_ = {ax, ay};
//            e->b_ = {bx, by};
//            e->color = color;
//            e->algo = algo;
//
//            shapes.push_back(e);
//        }

        else if(type == "BezierCurve")
        {
            int x1,y1,x2,y2,x3,y3,x4,y4;
            int r,g,b;

            in >> x1 >> y1 >> x2 >> y2
               >> x3 >> y3 >> x4 >> y4
               >> r >> g >> b;

            COLORREF color = RGB(r,g,b);
            vector<pair<int,int>> p {make_pair(x1, y1), make_pair(x2, y2),
                                     make_pair(x3, y3), make_pair(x4, y4)};

            shapes.push_back(new BezierCurve(p, color));
        }

        else if(type == "HermiteCurve")
        {
            int x1,y1,t1x,t1y,x2,y2,t2x,t2y;
            int r,g,b;

            in >> x1 >> y1 >> t1x >> t1y
               >> x2 >> y2 >> t2x >> t2y
               >> r >> g >> b;

            COLORREF color = RGB(r,g,b);

            shapes.push_back(new HermiteCurve(
                make_pair(x1,y1),make_pair(t1x,t1y),make_pair(x2,y2),make_pair(t2x,t2y), color));
        }

        else if(type == "CardinalSplineCurve")
        {
            double c;
            in >> c;

            vector<pair<int,int>> pts;

            // Read unknown number of points until color remains
            // (assume last 3 ints are color)
            vector<int> temp;
            int val;

            // Read whole line
            string line;
            getline(in, line);
            stringstream ss(line);

            while(ss >> val)
                temp.push_back(val);

            if(temp.size() < 3) continue;

            int r = temp[temp.size()-3];
            int g = temp[temp.size()-2];
            int b = temp[temp.size()-1];

            for(size_t i=0;i<temp.size()-3;i+=2)
                pts.push_back({temp[i], temp[i+1]});

            COLORREF color = RGB(r,g,b);

            shapes.push_back(new CardinalSplineCurve(pts,c,color));
        }

        else if(type == "FillingCircles")
        {
            int algo, xc, yc, rad;
            int r,g,b;

            in >> algo >> xc >> yc >> rad >> r >> g >> b;

            shapes.push_back(new FillingCircles(xc,yc,rad,algo,RGB(r,g,b)));
        }

        else if(type == "FillingWithCurves")
        {
            int algo,xc,yc;
            int r,g,b;

            in >> algo >> xc >> yc >> r >> g >> b;

            shapes.push_back(new FillingWithCurves(xc,yc,algo,RGB(r,g,b)));
        }

        else if(type == "PolygonFilling")
        {
            int algo, sz;
            in >> algo >> sz;

            vector<pair<int,int>> pts;

            for(int i=0;i<sz;i++)
            {
                int x,y;
                in >> x >> y;
                pts.push_back({x,y});
            }

            int r,g,b;
            in >> r >> g >> b;

            shapes.push_back(new PolygonFilling(pts,sz,algo,RGB(r,g,b)));
        }

        else if(type == "FloodFill")
        {
            int algo,x,y;
            int r1,g1,b1,r2,g2,b2;

            in >> algo >> x >> y
               >> r1 >> g1 >> b1
               >> r2 >> g2 >> b2;

            shapes.push_back(new FloodFillShape(x,y,algo,RGB(r1,g1,b1),RGB(r2,g2,b2)));
        }

        else if(type == "clipping")
        {
            int alg;

            vector<pair<int,int>> pts;

            // Read unknown number of points until color remains
            // (assume last 3 ints are color)
            vector<int> temp;
            int val;

            in >> alg;

            // Read whole line
            string line;
            getline(in, line);
            stringstream ss(line);

            while(ss >> val)
                temp.push_back(val);

            if(temp.size() < 3) continue;

            int left = temp[temp.size()-7];
            int right = temp[temp.size()-6];
            int top = temp[temp.size()-5];
            int bottom = temp[temp.size()-4];
            int r = temp[temp.size()-3];
            int g = temp[temp.size()-2];
            int b = temp[temp.size()-1];

            for(size_t i=0;i<temp.size()-3;i+=2)
                pts.push_back(make_pair(temp[i], temp[i+1]));

            COLORREF color = RGB(r,g,b);

            shapes.push_back(new clipping(pts, left, right, top, bottom, alg, color));
        }

        else if(type == "Face")
        {
            int algo, xc, yc, rad;
            int r,g,b;

            in >> algo >> xc >> yc >> rad >> r >> g >> b;

            shapes.push_back(new Face(xc, yc, rad, algo, RGB(r,g,b)));
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
    wincl.hbrBackground = NULL;

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
    HMENU Cursor=CreateMenu();
    HMENU line=CreateMenu();
    HMENU circle=CreateMenu();
    HMENU Ellipse=CreateMenu();
    HMENU Curve=CreateMenu();
    HMENU Filling=CreateMenu();
    HMENU Clipping=CreateMenu();
    HMENU RectWind=CreateMenu();
    HMENU SqrWind=CreateMenu();
    HMENU CircleWind=CreateMenu();
    HMENU faces=CreateMenu();

    AppendMenu(file,MF_STRING,1,"Clear");
    AppendMenu(file,MF_STRING,2,"Save");
    AppendMenu(file,MF_STRING,3,"Load");

    AppendMenu(Pref,MF_STRING,10,"White background");
    AppendMenu(Pref,MF_POPUP,(UINT_PTR)Cursor,"Cursor");
    AppendMenu(Pref,MF_STRING,12,"Choose Color");

    AppendMenu(Cursor,MF_STRING,111,"Arrow Cursor");
    AppendMenu(Cursor,MF_STRING,112,"Cross  Cursor");
    AppendMenu(Cursor,MF_STRING,113,"Forbidden sign");

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

    AppendMenu(Clipping,MF_POPUP,(UINT_PTR)RectWind,"Rectangle Window");
    AppendMenu(Clipping,MF_POPUP,(UINT_PTR)SqrWind,"Square Window");
    AppendMenu(Clipping,MF_POPUP,(UINT_PTR)CircleWind,"Circle Window");

    AppendMenu(RectWind,MF_STRING,70,"Point");
    AppendMenu(RectWind,MF_STRING,71,"Line,");
    AppendMenu(RectWind,MF_STRING,72,"Polygon");

    AppendMenu(SqrWind,MF_STRING,73,"Point");
    AppendMenu(SqrWind,MF_STRING,74,"Line,");

    AppendMenu(CircleWind,MF_STRING,75,"Point");
    AppendMenu(CircleWind,MF_STRING,76,"Line,");

    AppendMenu(faces,MF_STRING,80,"Happy Face");
    AppendMenu(faces,MF_STRING,81,"Sad Face");

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
    static int currentAlgo = 0;

    // Store clicked points
    static vector<pair<int,int>> points;
    static int clipLeft, clipRight, clipTop, clipBottom;
    static bool isClipWind = true;

    switch (message)
    {

    // ======================================================
    // MENU COMMANDS
    // ======================================================
    case WM_COMMAND:
    {
        switch(LOWORD(wParam))
        {

        // ================= FILE =================

        case 1: // Clear
            ClearShapes(shapes, hwnd);
            break;

        case 2: // Save
            SaveShapes("data.txt");
            MessageBox(hwnd, "Saved Successfully", "Save", MB_OK);
            break;

        case 3: // Load
            {
                LoadShapes("data.txt", shapes);
                HDC hdc = GetDC(hwnd);
                for(auto s : shapes)
                    s->draw(hdc);
            }
            break;


        // ================= PREFERENCES =================

        case 10:
            DeleteObject(bgBrush);
            bgBrush = CreateSolidBrush(RGB(255,255,255));
            InvalidateRect(hwnd, NULL, TRUE);
            break;

        case 111:
            currentCursor = LoadCursor(NULL, IDC_ARROW);
            break;

        case 112:
            currentCursor = LoadCursor(NULL, IDC_CROSS);
            break;

        case 113:
            currentCursor = LoadCursor(NULL, IDC_NO);
            break;

        case 12:
            {
                CHOOSECOLOR cc = { sizeof(CHOOSECOLOR) };
                static COLORREF colors[16];

                cc.lpCustColors = colors;
                cc.rgbResult = currentColor;
                cc.Flags = CC_FULLOPEN | CC_RGBINIT;

                if(ChooseColor(&cc))
                    currentColor = cc.rgbResult;
            }
            break;


        // ================= LINE =================

        case 20: // Parametric
            currentAlgo = 20;
            points.clear();
            break;

        case 21: // DDA
            currentAlgo = 21;
            points.clear();
            break;

        case 22: // Midpoint
            currentAlgo = 22;
            points.clear();
            break;


        // ================= CIRCLE =================

        case 30:
            currentAlgo = 30;
            points.clear();
            break;

        case 31:
            currentAlgo = 31;
            points.clear();
            break;

        case 32:
            currentAlgo = 32;
            points.clear();
            break;

        case 33:
            currentAlgo = 33;
            points.clear();
            break;

        case 34:
            currentAlgo = 34;
            points.clear();
            break;


        // ================= ELLIPSE =================

        case 40:
            currentAlgo = 40;
            points.clear();
            break;

        case 41:
            currentAlgo = 41;
            points.clear();
            break;

        case 42:
            currentAlgo = 42;
            points.clear();
            break;


        // ================= CURVES =================

        case 50:
            currentAlgo = 50;
            points.clear();
            break;


        // ================= FILLING =================

        case 60:
            currentAlgo = 60;
            points.clear();
            break;

        case 61:
            currentAlgo = 61;
            points.clear();
            break;

        case 62:
            currentAlgo = 62;
            points.clear();
            break;

        case 63:
            currentAlgo = 63;
            points.clear();
            break;

        case 64:
            currentAlgo = 64;
            points.clear();
            break;

        case 65:
            currentAlgo = 65;
            points.clear();
            break;

        case 66:
            currentAlgo = 66;
            points.clear();
            break;

        case 67:
            currentAlgo = 67;
            points.clear();
            break;

        // ================= Clipping =================

        case 70:
            currentAlgo = 70;
            points.clear();
            break;

        case 71:
            currentAlgo = 71;
            points.clear();
            break;

        case 72:
            currentAlgo = 72;
            points.clear();
            break;

        case 73:
            currentAlgo = 73;
            points.clear();
            break;

        case 74:
            currentAlgo = 74;
            points.clear();
            break;

        case 75:
            currentAlgo = 75;
            points.clear();
            break;

        case 76:
            currentAlgo = 76;
            points.clear();
            break;

        // ================= Faces =================

        case 80:
            currentAlgo = 80;
            points.clear();
            break;

        case 81:
            currentAlgo = 81;
            points.clear();
            break;
        }
    }
    break;


    // ======================================================
    // MOUSE LEFT CLICK
    // ======================================================
    case WM_LBUTTONUP:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        points.push_back(make_pair(x,y));

        HDC hdc = GetDC(hwnd);

        // ================= FILLING CURVES =================

        if(points.size() == 1)
        {
            pair<int,int> p1 = points[0];

            switch(currentAlgo)
            {
            case 62:
            case 63:
                {
                    int algo = currentAlgo - 61;
                    shape *s = new FillingWithCurves(p1.first, p1.second, algo, currentColor);
                    shapes.push_back(s);
                    s->draw(hdc);
                    points.clear();
                }
                break;

            case 66:
            case 67:
                {
                    int algo = currentAlgo - 65;
                    shape *s = new FloodFillShape(p1.first, p1.second, algo, currentColor, currentColor);
                    shapes.push_back(s);
                    s->draw(hdc);
                    points.clear();
                }
                break;
            }
        }

        else if(points.size() == 2)
        {
            pair<int,int> p1 = points[0];
            pair<int,int> p2 = points[1];

            switch(currentAlgo)
            {

            // ================= LINES =================

            case 20:
            case 21:
            case 22:
                {
                    int algo = currentAlgo - 19;
                    shape *s = new Line(p1.first, p1.second, p2.first, p2.second, algo, currentColor);
                    shapes.push_back(s);
                    s->draw(hdc);
                    points.clear();
                }
                break;

            // ================= CIRCLES =================

            case 30:
            case 31:
            case 32:
            case 33:
            case 34:
            {
                int dx = p2.first - p1.first;
                int dy = p2.second - p1.second;

                int r = sqrt(dx*dx + dy*dy);

                int algo = currentAlgo - 29;

                shape *s = new Circle(p1.first, p1.second, r, algo, currentColor);
                shapes.push_back(s);
                s->draw(hdc);
                points.clear();
            }
            break;

            case 60:
            case 61:
                {
                    int dx = p2.first - p1.first;
                    int dy = p2.second - p1.second;

                    int r = sqrt(dx*dx + dy*dy);

                    int algo = currentAlgo - 59;

                    shape *s = new FillingCircles(p1.first, p1.second, r, algo, currentColor);
                    shapes.push_back(s);
                    s->draw(hdc);
                    points.clear();
                }
                break;

            case 74:
                {
                    if(isClipWind)
                    {
                        int xc = points[0].first;
                        int yc = points[0].second;
                        int x = points[1].first;
                        int y = points[0].second;

                        int halfSide = max(abs(x - xc), abs(y - yc));

                        clipLeft   = xc - halfSide;
                        clipRight  = xc + halfSide;
                        clipTop    = yc - halfSide;
                        clipBottom = yc + halfSide;

                        shape *s1 = new Line(clipLeft, clipTop, clipRight, clipTop, 2, currentColor);
                        shape *s2 = new Line(clipRight, clipTop, clipRight, clipBottom, 2, currentColor);
                        shape *s3 = new Line(clipRight, clipBottom, clipLeft, clipBottom, 2, currentColor);
                        shape *s4 = new Line(clipLeft, clipBottom, clipLeft, clipTop, 2, currentColor);

                        shapes.push_back(s1);
                        shapes.push_back(s2);
                        shapes.push_back(s3);
                        shapes.push_back(s4);

                        s1->draw(hdc);
                        s2->draw(hdc);
                        s3->draw(hdc);
                        s4->draw(hdc);

                        points.clear();
                    }
                    else
                    {
                        int x1 = points[0].first;
                        int y1 = points[0].second;
                        int x2 = points[1].first;
                        int y2 = points[0].second;

                        shape *s1 = new Line(x1, y1, x2, y2, 2, currentColor);
                        shape *s2 = new clipping(points, clipLeft, clipRight, clipTop, clipBottom, 2, currentColor);

                        shapes.push_back(s1);
                        shapes.push_back(s2);

                        s1->draw(hdc);
                        s2->draw(hdc);

                        points.clear();
                    }
                }
            break;

            case 80:
            case 81:
                {
                    int dx = p2.first - p1.first;
                    int dy = p2.second - p1.second;

                    int r = sqrt(dx*dx + dy*dy);

                    int algo = currentAlgo - 79;
                    shape *s = new Face(p1.first, p1.second, r, algo, currentColor);
                    shapes.push_back(s);
                    s->draw(hdc);
                    points.clear();
                }
                break;
            }
        }

        // ================= CardinalSplineCurve =================

        else if(points.size() == 4 && currentAlgo == 64)
        {
            shape *s = new PolygonFilling(points, 4, 1, currentColor);
            shapes.push_back(s);
            s->draw(hdc);
            points.clear();

        }

        else if(points.size() == 6 && currentAlgo == 50)
        {
            shape *s = new CardinalSplineCurve(points, 0.5, currentColor);
            shapes.push_back(s);
            s->draw(hdc);
            points.clear();
        }

        else if(points.size() == 8 && currentAlgo == 65)
        {
            shape *s = new PolygonFilling(points, 8, 2, currentColor);
            shapes.push_back(s);
            s->draw(hdc);
            points.clear();
        }

        ReleaseDC(hwnd, hdc);
    }

    break;

    // ======================================================
    // PAINT
    // ======================================================
    case WM_PAINT:
    {
        PAINTSTRUCT ps;

        HDC hdc = BeginPaint(hwnd, &ps);

        for(auto s : shapes)
            s->draw(hdc);

        EndPaint(hwnd, &ps);
    }
    break;

    case WM_SETCURSOR:
        SetCursor(currentCursor);
        return TRUE;

    case WM_ERASEBKGND:
    {
        RECT rect;
        GetClientRect(hwnd, &rect);
        FillRect((HDC)wParam, &rect, bgBrush);
        return 1;
    }

    // ======================================================
    // DESTROY
    // ======================================================
    case WM_DESTROY:

        for(auto s : shapes)
            delete s;

        DeleteObject(bgBrush);

        PostQuitMessage(0);
        break;


    // ======================================================
    // DEFAULT
    // ======================================================
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}


