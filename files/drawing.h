#ifndef DRAWING_H_INCLUDED
#define DRAWING_H_INCLUDED

#include <windows.h>
#include <cmath>
#include <vector>
#include <list>
#include <climits>
#include <stack>
#include <utility>
#include <string>

using namespace std;

#define MAXENTRIES 600


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
// Square Algorithm
//===================
void drawSquare(HDC hdc,const vector<pair<int,int>>& points,int& left,int& right,int& top,int& bottom,COLORREF color);

//===================
// Square Algorithm
//===================
void drawRect(HDC hdc,const vector<pair<int,int>>& points,int& Left,int& Right,int& Top,int& Bottom,COLORREF color);

//=======================
// Curve Algorithms
//=======================
double DotProduct(vector<double>& a,vector<double>& b);

vector<double> GetHermiteCoeff(double x0, double s0, double x1, double s1);

void DrawHermiteCurve(HDC hdc, pair<int,int>& P0, pair<int,int>& T0, pair<int,int>& P1, pair<int,int>& T1 ,int numpoints, COLORREF color);

void DrawBezierCurve(HDC hdc, vector<pair<int,int>>& points, int numpoints, COLORREF color);

void DrawCardinalSpline(HDC hdc,vector<pair<int,int>>& P,double c,int numpix, COLORREF color);


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
void ConvexFill(HDC hdc,vector<pair<int,int>> p,COLORREF color);

EdgeRec InitEdgeRec(pair<int,int>& v1,pair<int,int>& v2);
void InitEdgeTable(vector<pair<int,int>> polygon, int n,EdgeList table[]);
void GeneralPolygonFill(HDC hdc,vector<pair<int,int>> polygon,int n ,COLORREF c);

void FloodFill(HDC hdc,int x,int y,COLORREF Cb,COLORREF Cf);
void flip_p();
void NRFloodFill(HDC hdc,int x,int y,COLORREF Cb,COLORREF Cf);

void FillSquareWithHermite(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color);
void FillRectangleWithBezier(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color);

void FillingCircleWithLines(HDC hdc, int x, int y, int xc, int yc, int r, COLORREF color);
void fillCircleWithCircles(HDC hdc,int x, int y, int xc,int yc, int R,COLORREF color);


//=======================
// Clipping Algorithms
//=======================
union OutCode
{
    unsigned All:4;
    struct{unsigned left:1,top:1,right:1,bottom:1;};
};

struct Vertex
{
    double x,y;
    Vertex(int x1=0,int y1=0)
    {
        x=x1;
        y=y1;
    }
};

typedef vector<Vertex> VertexList;
typedef bool (*IsInFunc)(Vertex& v,int edge);
typedef Vertex (*IntersectFunc)(Vertex& v1,Vertex& v2,int edge);

void PointClipping(HDC hdc,int x,int y,int xleft,int ytop,int xright,int ybottom,COLORREF color);

OutCode GetOutCode(double x,double y,int xleft,int ytop,int xright,int ybottom);
void VIntersect(double xs,double ys,double xe,double ye,int x,double *xi,double *yi);
void HIntersect(double xs,double ys,double xe,double ye,int y,double *xi,double *yi);
void CohenSuth(HDC hdc,int xs,int ys,int xe,int ye,int xleft,int ytop,int xright,int ybottom);

VertexList ClipWithEdge(VertexList p,int edge,IsInFunc In,IntersectFunc Intersect);
bool InLeft(const Vertex& v,int edge);
bool InRight(const Vertex& v,int edge);
bool InTop(const Vertex& v,int edge);
bool InBottom(const Vertex& v,int edge);
Vertex VIntersect(Vertex& v1,Vertex& v2,int xedge);
Vertex HIntersect(Vertex& v1,Vertex& v2,int yedge);
void PolygonClip(HDC hdc,const vector<pair<int,int>>& p,int xleft,int ytop,int xright,int ybottom);

bool PointClippingInsideCircle(int x,int y,int xc,int yc,int r);

void ClipLineCircle(HDC hdc, int x1, int y1, int x2, int y2, int xc, int yc, int r, COLORREF color);

//===================
// Polygon Algorithm
//===================
void drawPolygon(HDC hdc, const vector<pair<int,int>>& pts, COLORREF color);

//=======================
// Faces Algorithms
//=======================

void DrawHappyFace(HDC hdc, int xc, int yc, int r, COLORREF color);
void DrawSadFace(HDC hdc, int xc, int yc, int r, COLORREF color);


#endif // DRAWING_H_INCLUDED
