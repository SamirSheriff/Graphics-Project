#ifndef DRAWING_H_INCLUDED
#define DRAWING_H_INCLUDED

#define MAXENTRIES 600;


#endif // DRAWING_H_INCLUDED

#include <cmath>
#include <vector>

using namespace std;

//================
// Line Algorithms
//=================
void LineParametric(HDC hdc,int xs,int ys,int xe,int ye,COLORREF color);
void SimpleDDA(HDC hdc,int xs,int ys,int xe,int ye,COLORREF color);
void MidPoint(HDC hdc,int xs,int ys,int xe,int ye,COLORREF color);


//===================
// Circle Algorithms
//===================
void Draw8Points(HDC hdc,int xc,int yc, int a, int b,COLORREF color);
void CircleDirect(HDC hdc,int xc,int yc, int R,COLORREF color);
void CirclePolar(HDC hdc,int xc,int yc, int R,COLORREF color);
void CircleIterativePolar(HDC hdc,int xc,int yc, int R,COLORREF color);
void CircleBresenham(HDC hdc,int xc,int yc, int R,COLORREF color);
void CircleFasterBresenham(HDC hdc,int xc,int yc, int R,COLORREF color);


//===================
// Ellipse Algorithms
//===================



//=======================
// Curve Algorithms
//=======================
double DotProduct(vector<double>& a,vector<double>& b);

vector<double> GetHermiteCoeff(double x0, double s0, double x1, double s1);

void DrawHermiteCurve (HDC hdc, pair<int,int>& P0, pair<int,int>& T0, pair<int,int>& P1, pair<int,int>& T1 ,int numpoints);

void DrawBezierCurve(HDC hdc, vector<pair<int,int>>& points, int numpoints);

void DrawCardinalSpline(HDC hdc,Vector2 P[],int n,double c,int numpix);


//=======================
// Filling Algorithms
//=======================
struct Entry
{
    int xmin,xmax;
};

struct EdgeRec
{
    double x;
    double minv;
    int ymax;

    bool operator<(EdgeRec r)
    {
        return x<r.x;
    }
};

typedef list<EdgeRec> EdgeList;

void InitEntries(Entry table[]);

void ScanEdge(pair<int,int> v1,pair<int,int> v2,Entry table[]);

void DrawSanLines(HDC hdc,Entry table[],COLORREF color);

void ConvexFill(HDC hdc,pair<int,int> p[],int n,COLORREF color);

EdgeRec InitEdgeRec(pair<int,int>& v1,pair<int,int>& v2);

void InitEdgeTable(pair<int,int> *polygon,int n,EdgeList table[])

void GeneralPolygonFill(HDC hdc,pair<int,int> *polygon,int n,COLORREF c);

void FloodFill(HDC hdc,int x,int y,COLORREF Cb,COLORREF Cf);

void NRFloodFill(HDC hdc,int x,int y,COLORREF Cb,COLORREF Cf);


//=======================
// Clipping Algorithms
//=======================
