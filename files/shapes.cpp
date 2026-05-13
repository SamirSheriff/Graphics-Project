#include "shapes.h"
#include <string>


//===================
// Parent Class
//===================
shape::shape(COLORREF col)
{
    color = col;
}

shape::~shape() {}


//================
// Line Class
//=================
Line::Line(int a, int b, int c, int d, int alg, COLORREF col): shape(col)
{
    x1 = a;
    y1 = b;
    x2 = c;
    y2 = d;
    algo = alg;
}

void Line::draw(HDC hdc)
{
    switch(algo)
    {
    case 1:
         LineParametric(hdc, x1, y1, x2, y2, color);
         break;
    case 2:
        SimpleDDA(hdc, x1, y1, x2, y2, color);
        break;
    case 3:
        MidPoint(hdc, x1, y1, x2, y2, color);
        break;
    default:
        cout << "no algo in line shape" << endl;
    }
}

void Line::save(ofstream& out)
{
    out << "LINE " << algo << " " << x1 << " " << y1 << " "
        << x2 << " " << y2 << " " << (int)GetRValue(color) << " " << (int)GetGValue(color) << " " <<  (int)GetBValue(color) << " " << "\n";
}

Line::~Line() {}

//===================
// Circle Class
//===================
Circle::Circle(int a, int b, int c, int alg, COLORREF col): shape(col)
{
    xc = a;
    yc = b;
    r = c;
    algo = alg;
}


void Circle::draw(HDC hdc)
{
    switch(algo)
    {
    case 1:
         CircleDirect(hdc, xc, yc, r, color);
         break;
    case 2:
        CirclePolar(hdc, xc, yc, r, color);
        break;
    case 3:
        CircleIterativePolar(hdc, xc, yc, r, color);
        break;
    case 4:
        CircleBresenham(hdc, xc, yc, r, color);
        break;
    case 5:
        CircleFasterBresenham(hdc, xc, yc, r, color);
        break;
    default:
        cout << "No algo in Circle shape" << endl;
    }
}

void Circle::save(ofstream& out)
{
    out << "CIRCLE " << algo << " " << xc << " " << yc << " " << r << " "
    << (int)GetRValue(color) << " " << (int)GetGValue(color) << " " <<  (int)GetBValue(color) << " " << "\n";
}

Circle::~Circle() {}


//===================
// Ellips Class
//===================
myEllipse::myEllipse(pair<int, int> c, pair<int, int> a, pair<int, int> b, int mode, COLORREF col):shape(col)
{
    center = c;
    a_ = a;
    b_ = b;
    m_ = mode;
}

void myEllipse::compute_powers(double theta) {
    powers.clear();
    powers.push_back(1.0); // theta^0
    for (int i = 1; i <= 10; i++) {
        powers.push_back(theta * powers.back());
    }
}

void myEllipse::compute_factorials(int number) {
    factorials.clear();
    factorials.push_back(1.0); // 0! is 1
    for (int i = 1; i <= number; i++) {
        factorials.push_back(i * factorials.back());
    }
}

void myEllipse::direct(HDC hdc) {
    double a2 = (double)a_.first * a_.first;
    int x = 0;

    short int f =2;
    pair<int,int> arr[4];

    while (x <= a_.first)
    {
        int y = round(b_.second * sqrt(1.0 - ((double)x * x) / a2));

        // Symmetry points
        SetPixel(hdc, center.first + x, center.second + y, color);
        SetPixel(hdc, center.first - x, center.second + y, color);
        SetPixel(hdc, center.first + x, center.second - y, color);
        SetPixel(hdc, center.first - x, center.second - y, color);

        if (f==2)
        {
            arr[0] = make_pair(center.first +x , center.second+y);
            arr[1] = make_pair(center.first -x , center.second+y);
            arr[2] = make_pair(center.first +x , center.second -y);
            arr[3] = make_pair(center.first -x , center.second-y);
            f = 0;
        }
    else
    {
        HPEN pen = CreatePen(PS_SOLID, 1, color);
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);

        MoveToEx(hdc,center.first + x, center.second + y,NULL);
        LineTo(hdc,arr[0].first, arr[0].second);

        MoveToEx(hdc,center.first - x, center.second + y,NULL);
        LineTo(hdc,arr[1].first, arr[1].second);

        MoveToEx(hdc,center.first + x, center.second - y,NULL);
        LineTo(hdc,arr[2].first, arr[2].second);

        MoveToEx(hdc,center.first - x, center.second - y,NULL);
        LineTo(hdc,arr[3].first, arr[3].second);

        SelectObject(hdc, oldPen);
        DeleteObject(pen);

        f=1;

        arr[0] = make_pair(center.first +x , center.second+y);
        arr[1] = make_pair(center.first -x , center.second+y);
        arr[2] = make_pair(center.first +x , center.second -y);
        arr[3] = make_pair(center.first -x , center.second-y);
    }
        x++;
    }
}

void myEllipse::polar(HDC hdc) {
    double rx = a_.first;
    double ry = b_.second;

    // Pre-calculate factorials once to save CPU
    compute_factorials(10);

    // Determine step size based on circumference approximation
    double num_steps = max(rx, ry) * 2;
    double d_theta = (M_PI / 2.0) / num_steps;
    double theta = 0;

    for (int i = 0; i <= num_steps; i++) {
        compute_powers(theta);

        // Taylor Series for Sin and Cos
        double s_val = powers[1] - (powers[3]/factorials[3]) + (powers[5]/factorials[5]) - (powers[7]/factorials[7]) + (powers[9]/factorials[9]);
        double c_val = powers[0] - (powers[2]/factorials[2]) + (powers[4]/factorials[4]) - (powers[6]/factorials[6]) + (powers[8]/factorials[8]);

        int x_p = round(rx * c_val);
        int y_p = round(ry * s_val);

        // Symmetry points for all four quadrants
        SetPixel(hdc, center.first + x_p, center.second + y_p, color);
        SetPixel(hdc, center.first - x_p, center.second + y_p, color);
        SetPixel(hdc, center.first + x_p, center.second - y_p, color);
        SetPixel(hdc, center.first - x_p, center.second - y_p, color);

        theta += d_theta;
    }
}

void myEllipse::midpoint(HDC hdc) {
    double a2 = (double)a_.first * a_.first;
    double b2 = (double)b_.second * b_.second;
    double a2b2 = a2 * b2;

    // --- Region 1: Top to Side (x is the fast variable) ---
    int x = 0;
    int y = b_.second;
    // Boundary check: b^2 * x <= a^2 * y
    while (b2 * x <= a2 * y) {
        // Test the midpoint between (x+1, y) and (x+1, y-1)
        double decision = b2 * (x + 1) * (x + 1) + a2 * (y - 0.5) * (y - 0.5) - a2b2;
        if (decision > 0) y--;

        SetPixel(hdc, center.first + x, center.second + y, color);
        SetPixel(hdc, center.first - x, center.second + y, color);
        SetPixel(hdc, center.first + x, center.second - y, color);
        SetPixel(hdc, center.first - x, center.second - y, color);

        x++;
    }

    // --- Region 2: Side to Bottom (y is the fast variable) ---
    x = a_.first;
    y = 0;
    // Boundary check: a^2 * y <= b^2 * x
    while (a2 * y <= b2 * x) {
        // Test the midpoint between (x, y+1) and (x-1, y+1)
        double decision = b2 * (x - 0.5) * (x - 0.5) + a2 * (y + 1) * (y + 1) - a2b2;
        if (decision > 0) x--;

        SetPixel(hdc, center.first + x, center.second + y, color);
        SetPixel(hdc, center.first - x, center.second + y, color);
        SetPixel(hdc, center.first + x, center.second - y, color);
        SetPixel(hdc, center.first - x, center.second - y, color);

        y++;
    }
}


void myEllipse::draw(HDC hdc) {
        // Ensure a and b are relative distances from center
        // if they were passed as absolute coordinates
        a_.second = center.second;
        b_.first = center.first;

        switch (m_) {
            case 1:
                direct(hdc);
                break;
            case 2:
                polar(hdc);
                break;
            case 3:
                midpoint(hdc);
                break;
            default:
                break;
        }
}

void myEllipse::save(ofstream& out)
{
    out << "Ellipse " << m_ << " " << center.first << " " << center.second << " "
        << a_.first << " " << a_.second << " " << b_.first << " " << b_.second << " "
        << (int)GetRValue(color) << " " << (int)GetGValue(color) << " " <<  (int)GetBValue(color) << " " << "\n";
}

myEllipse::~myEllipse() {}


//===================
// Curve Classes
//===================

// Bezier Functions
BezierCurve::BezierCurve(vector<pair<int,int>>& p, COLORREF col): shape(col)
{
    points = p;
}

void BezierCurve::draw(HDC hdc)
{
    DrawBezierCurve(hdc, points, 50);
}

void BezierCurve::save(ofstream& out)
{
    out << "BezierCurve " << points[0].first << " " << points[0].second << " " << points[1].first << " " << points[1].second<< " "
        << points[2].first << " " << points[2].second << " " << points[3].first << " " << points[3].second << " "
        << (int)GetRValue(color) << " " << (int)GetGValue(color) << " " <<  (int)GetBValue(color) << " " << "\n";
}

BezierCurve::~BezierCurve() {}


// Hermite Functions
HermiteCurve::HermiteCurve(pair<int,int> a, pair<int,int> b, pair<int,int> c, pair<int,int> d, COLORREF col): shape(col)
{
    p1 = a;
    t1 = b;
    p2 = c;
    t2 = d;
}

void HermiteCurve::draw(HDC hdc)
{
    DrawHermiteCurve (hdc, p1, t1, p2, t2 , 50);
}

void HermiteCurve::save(ofstream& out)
{
    out << "HermiteCurve " << p1.first << " " << p1.second << " " << t1.first << " " << t1.second<< " "
        << p2.first << " " << p2.second << " " << t2.first << " " << t2.second << " "
        << (int)GetRValue(color) << " " << (int)GetGValue(color) << " " <<  (int)GetBValue(color) << " " << "\n";
}

HermiteCurve::~HermiteCurve() {}


// CardinalSpline Functions
CardinalSplineCurve::CardinalSplineCurve(vector<pair<int,int>> p, double a, COLORREF col): shape(col)
{
    points = p;
    c = a;
}

void CardinalSplineCurve::draw(HDC hdc)
{
    DrawCardinalSpline(hdc, points, c, 50);
}

void CardinalSplineCurve::save(ofstream& out)
{
    out << "CardinalSplineCurve " << c << " ";
    for(size_t i = 0; i < points.size(); i++)
    {
        out << points[i].first << " " << points[i].second << " ";
    }
    out << (int)GetRValue(color) << " " << (int)GetGValue(color) << " " <<  (int)GetBValue(color) << " " << "\n";
}

CardinalSplineCurve::~CardinalSplineCurve() {}


//===================
// Filling Classes
//===================

FillingCircles::FillingCircles(int _x, int _y, int _xc, int _yc, int R, int alg, COLORREF col): shape(col)
{
    x = _x;
    y = _y;
    xc = _xc;
    yc = _yc;
    r = R;
    algo = alg;
}

void FillingCircles::draw(HDC hdc)
{
    switch(algo)
    {
    case 1:
        FillingCircleWithLines(hdc, x, y, xc, yc, r, color);
        break;
    case 2:
        {
            for(int R = 0; R < r; R++)
            fillCircleWithCircles(hdc, x, y, xc, yc, R, color);
        }

        break;
     default:
        cout << "No algo in FillingCircles shape" << endl;
    }
}

void FillingCircles::save(ofstream& out)
{
    out << "FillingCircles " << algo << " " << x  << " " << y  << " " << xc << " " << yc << " " << r << " ";;
    out << (int)GetRValue(color) << " " << (int)GetGValue(color) << " " <<  (int)GetBValue(color) << " " << "\n";
}

FillingCircles::~FillingCircles() {}


FillingWithCurves::FillingWithCurves(vector<pair<int,int>> p, int alg, COLORREF col): shape(col)
{
    pts = p;
    algo = alg;
}

void FillingWithCurves::draw(HDC hdc)
{
    switch(algo)
    {
    case 1:
        FillSquareWithHermite(hdc, pts[0].first - 50, pts[0].second, pts[0].first + 50, pts[0].second, color);
        break;
    case 2:
        FillRectangleWithBezier(hdc, pts[0].first, pts[0].second, pts[1].first, pts[1].second, color);
        break;
    default:
        cout << "No algo in filling with curves" << endl;
    }
}

void FillingWithCurves::save(ofstream& out)
{
    out << "FillingWithCurves " << algo << " ";
    for(size_t i = 0; i < pts.size(); i++)
    {
        out << pts[i].first << " " << pts[i].second << " ";
    }
    out << (int)GetRValue(color) << " " << (int)GetGValue(color) << " " <<  (int)GetBValue(color) << " " << "\n";
}

FillingWithCurves::~FillingWithCurves() {}


PolygonFilling::PolygonFilling(vector<pair<int,int>> p, int n, int alg, COLORREF col): shape(col)
{
    points = p;
    sz = n;
    algo = alg;
}

void PolygonFilling::draw(HDC hdc)
{
    switch(algo)
    {
    case 1:
         ConvexFill(hdc, points, color);
        break;
    case 2:
        GeneralPolygonFill(hdc, points, sz, color);
        break;
     default:
        cout << "No algo in FillingPolygon shape" << endl;
    }
}

void PolygonFilling::save(ofstream& out)
{
    out << "PolygonFilling " << algo << " " << sz << " ";
    for(size_t i = 0; i < points.size(); i++)
    {
        out << points[i].first << " " << points[i].second << " ";
    }
    out << (int)GetRValue(color) << " " << (int)GetGValue(color) << " " <<  (int)GetBValue(color) << " " << "\n";
}

PolygonFilling::~PolygonFilling() {}


FloodFillShape ::FloodFillShape (int a, int b, int alg, COLORREF color1, COLORREF color2): shape(color2)
{
    x = a;
    y = b;
    algo = alg;
    cb = color1;
    cf = color2;
}

void FloodFillShape::draw(HDC hdc)
{
     switch(algo)
    {
    case 1:
        FloodFill(hdc,x, y, cb, cf);
        break;
    case 2:
        NRFloodFill(hdc, x, y, cb, cf);
        break;
    default:
        cout << "No algo in FloodFill" << endl;
    }
}

void FloodFillShape::save(ofstream& out)
{
    out << "FloodFill " << algo << " " << x << " " << y << " "
        << (int)GetRValue(cb) << " " << (int)GetGValue(cb) << " " <<  (int)GetBValue(cb) << " "
        << (int)GetRValue(cf) << " " << (int)GetGValue(cf) << " " <<  (int)GetBValue(cf) << " "<< "\n";
}

FloodFillShape::~FloodFillShape() {}

//===================
// Clipping Class
//===================
clipping::clipping(vector<pair<int,int>> p, int l, int r, int t, int b, int alg, COLORREF c):shape(c)
{
    pts = p;
    left = l;
    right = r;
    top = t;
    bottom = b;
    algo = alg;
}

clipping::clipping(vector<pair<int,int>> p, int _xc, int _yc, int rad, int alg, COLORREF c):shape(c)
{
    pts = p;
    xc = _xc;
    yc = _yc;
    r = rad;
    algo = alg;
}

void clipping::draw(HDC hdc)
{
     switch(algo)
    {
    case 1:
        PointClipping(hdc, pts[0].first, pts[0].second, left, top, right, bottom, color);
        break;
    case 2:
        CohenSuth(hdc,pts[0].first, pts[0].second, pts[1].first, pts[1].second, left, top, right, bottom);
        break;
    case 3:
        PolygonClip(hdc, pts, left, top, right, bottom);
        break;
    case 4:
        {
            bool isInside = PointClippingInsideCircle(pts[0].first, pts[0].second, xc, yc, r);
            if (isInside)
                SetPixel(hdc, pts[0].first, pts[0].second, RGB(0,0,0));
            else
                SetPixel(hdc, pts[0].first, pts[0].second, color);
        }
        break;
    case 5:
        ClipLineCircle(hdc, pts[0].first, pts[0].second, pts[1].first, pts[1].second, xc, yc, r, color);
        break;
    default:
        cout << "No algo in clipping" << endl;
    }
}

void clipping::save(ofstream& out)
{
    if(algo == 1 || algo == 2 || algo == 3)
    {
        out << "clipping " << algo << " " << left << " " << right << " " << top << " " << bottom << " ";
        for(size_t i = 0; i < pts.size(); i++)
        {
            out << pts[i].first << " " << pts[i].second << " ";
        }
        out << (int)GetRValue(color) << " " << (int)GetGValue(color) << " " <<  (int)GetBValue(color) << " " << "\n";
    }
    else
    {
        out << "clippingCircle " << algo << " " << xc << " " << yc << " " << r << " ";
        for(size_t i = 0; i < pts.size(); i++)
        {
            out << pts[i].first << " " << pts[i].second << " ";
        }
        out << (int)GetRValue(color) << " " << (int)GetGValue(color) << " " <<  (int)GetBValue(color) << " " << "\n";
    }
}

clipping::~clipping() {}

//===================
// Face Class
//===================
Face::Face(int a, int b, int c, int alg, COLORREF col): shape(col)
{
    xc = a;
    yc = b;
    r = c;
    algo = alg;
}


void Face::draw(HDC hdc)
{
    switch(algo)
    {
    case 1:
         DrawHappyFace(hdc, xc, yc, r, color);
         break;
    case 2:
        DrawSadFace(hdc, xc, yc, r, color);
        break;
    default:
        cout << "No algo in faces shape" << endl;
    }
}

void Face::save(ofstream& out)
{
    out << "Face " << algo << " " << xc << " " << yc << " " << r << " "
    << (int)GetRValue(color) << " " << (int)GetGValue(color) << " " <<  (int)GetBValue(color) << " " << "\n";
}

Face::~Face() {}
