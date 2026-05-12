#include "drawing.h"

//================
// Line Algorithms
//=================

void LineParametric(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = max(abs(dx), abs(dy));
    for (int i = 0; i <= steps; i++)
    {
        double t = (double)i / steps;
        int x = x1 + t * dx;
        int y = y1 + t * dy;
        SetPixel(hdc, x, y, color);
    }
}

void SimpleDDA(HDC hdc,int xs,int ys,int xe,int ye,COLORREF color)
{
    int dx=xe-xs;
    int dy=ye-ys;
    SetPixel(hdc,xs,ys,color);
    if(abs(dx)>=abs(dy))
    {
        int x=xs,xinc= dx>0?1:-1;
        double y=ys,yinc=(double)dy/dx*xinc;
        while(x!=xe)
        {
            x+=xinc;
            y+=yinc;
            SetPixel(hdc,x,round(y),color);
        }
    }
    else
    {
        int y=ys,yinc= dy>0?1:-1;
        double x=xs,xinc=(double)dx/dy*yinc;
        while(y!=ye)
        {
            x+=xinc;
            y+=yinc;
            SetPixel(hdc, round(x), y, RGB(0,0,0));
        }
    }
}

void MidPoint(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    int x = x1;
    int y = y1;

    int x_inc = (dx > 0) ? 1 : -1;
    int y_inc = (dy > 0) ? 1 : -1;

    dx = abs(dx);
    dy = abs(dy);

    SetPixel(hdc, x, y, color);

    if (dx >= dy) // Case 1: |slope| <= 1
    {
        int d = 2 * dy - dx;
        while (x != x2)
        {
            x += x_inc;
            if (d < 0)
            {
                d += 2 * dy;
            }
            else
            {
                y += y_inc;
                d += 2 * (dy - dx);
            }

            SetPixel(hdc, x, y, color);
        }
    }
    else // Case 2: |slope| > 1
    {
        int d = 2 * dx - dy;
        while (y != y2)
        {
            y += y_inc;
            if (d < 0)
            {
                d += 2 * dx;
            }
            else
            {
                x += x_inc;
                d += 2 * (dx - dy);
            }

            SetPixel(hdc, x, y, color);
        }
    }
}


//===================
// Circle Algorithms
//===================

void Draw8Points(HDC hdc,int xc,int yc, int a, int b,COLORREF color)
{
    SetPixel(hdc, xc+a, yc+b, color);
    SetPixel(hdc, xc-a, yc+b, color);
    SetPixel(hdc, xc-a, yc-b, color);
    SetPixel(hdc, xc+a, yc-b, color);
    SetPixel(hdc, xc+b, yc+a, color);
    SetPixel(hdc, xc-b, yc+a, color);
    SetPixel(hdc, xc-b, yc-a, color);
    SetPixel(hdc, xc+b, yc-a, color);
}

void CircleDirect(HDC hdc,int xc,int yc, int R,COLORREF color)
{
    int x=0,y=R;
    int R2=R*R;
    Draw8Points(hdc,xc,yc,x,y,color);
    while(x<y)
    {
        x++;
        y=round(sqrt((double)(R2-x*x)));
        Draw8Points(hdc,xc,yc,x,y,color);
    }
}

void CirclePolar(HDC hdc,int xc,int yc, int R,COLORREF color)
{
    int x=R,y=0;
    double theta=0,dtheta=1.0/R;
    Draw8Points(hdc,xc,yc,x,y,color);
    while(x>y)
    {
        theta+=dtheta;
        x=round(R*cos(theta));
        y=round(R*sin(theta));
        Draw8Points(hdc,xc,yc,x,y,color);
    }
}

void CircleIterativePolar(HDC hdc,int xc,int yc, int R,COLORREF color)
{
    double x=R,y=0;
    double dtheta=1.0/R;
    double cdtheta=cos(dtheta),sdtheta=sin(dtheta);
    Draw8Points(hdc,xc,yc,R,0,color);
    while(x>y)
    {
        double x1=x*cdtheta-y*sdtheta;
        y=x*sdtheta+y*cdtheta;
        x=x1;
        Draw8Points(hdc,xc,yc,round(x),round(y),color);
    }
}

void CircleBresenham(HDC hdc,int xc,int yc, int R,COLORREF color)
{
    int x=0,y=R;
    int d=1-R;
    Draw8Points(hdc,xc,yc,x,y,color);
    while(x<y)
    {
        if(d<0) d+=2*x+2;
        else
        {
            d+=2*(x-y)+5;
            y--;
        }
        x++;
        Draw8Points(hdc,xc,yc,x,y,color);
    }
}

void CircleFasterBresenham(HDC hdc,int xc,int yc, int R,COLORREF color)
{
    int x=0,y=R;
    int d=1-R;
    int c1=3, c2=5-2*R;
    Draw8Points(hdc,xc,yc,x,y,color);
    while(x<y)
    {
        if(d<0)
        {
            d+=c1;
            c2+=2;
        }
        else
        {
            d+=c2;
            c2+=4;
            y--;
        }
        c1+=2;
        x++;
        Draw8Points(hdc,xc,yc,x,y,color);
    }
}


//===================
// Curve Algorithms
//===================
double DotProduct(vector<double>& a,vector<double>& b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

vector<double> GetHermiteCoeff(double x0, double s0, double x1, double s1)
{
    int basis[4][4] = {
        {2, 1, -2, 1},
        {-3, -2, 3, -1},
        {0, 1, 0, 0},
        {1, 0, 0, 0}
    };
    vector<double> v = {x0, s0, x1, s1};
    vector<double> res(4);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            res[i] += basis[i][j] * v[j];
    return res;
}

void DrawHermiteCurve(HDC hdc, pair<int,int>& P0, pair<int,int>& T0, pair<int,int>& P1, pair<int,int>& T1 ,int numpoints)
{
    vector<double> xcoeff = GetHermiteCoeff(P0.first, T0.first, P1.first, T1.first);
    vector<double> ycoeff = GetHermiteCoeff(P0.second, T0.second, P1.second, T1.second);

    if(numpoints < 2) return;

    double dt = 1.0 / (numpoints - 1);
    for(double t = 0; t <= 1; t += dt)
    {
        vector<double> vt(4);

        vt[3]=1;
        for(int i = 2; i >= 0 ; i--)
        {
            vt[i] = vt[i+1] * t;
        }

        int x = round(DotProduct(xcoeff, vt));
        int y = round(DotProduct(ycoeff, vt));

        if(t == 0)
            MoveToEx(hdc,x,y,NULL);
        else
            LineTo(hdc,x,y);
    }
}

void DrawBezierCurve(HDC hdc, vector<pair<int,int>>& points, int numpoints)
{
    pair<int,int> P0 = points[0];
    pair<int,int> P1 = points[1];
    pair<int,int> P2 = points[2];
    pair<int,int> P3 = points[3];

    pair<int,int> T0 = make_pair(3 * (P1.first - P0.first), 3 * (P1.second - P0.second));
    pair<int,int> T1 = make_pair(3 * (P3.first - P2.first), 3 * (P3.second - P2.second));

    DrawHermiteCurve(hdc, P0, T0, P3, T1, numpoints);
}


void DrawCardinalSpline(HDC hdc, vector<pair<int,int>>& P,double c,int numpix)
{
    double c1=1-c;
    pair<int,int> T0(c1*(P[2].first-P[0].first),c1*(P[2].second-P[0].second));
    for(int i=2;i<P.size()-1;i++)
    {
        pair<int,int> T1(c1*(P[i+1].first-P[i-1].first),c1*(P[i+1].second-P[i-1].second));
        DrawHermiteCurve(hdc,P[i-1],T0,P[i],T1,numpix);
        T0=T1;
    }
}


//=======================
// Filling Algorithms
//=======================
void InitEntries(Entry table[])
{
    for(int i=0;i<MAXENTRIES;i++)
    {
        table[i].xmin=INT_MAX;
        table[i].xmax=-2000000;
    }
}

void ScanEdge(pair<int,int> v1,pair<int,int> v2,Entry table[])
{
    if(v1.second==v2.second)return;
    if(v1.second>v2.second)swap(v1,v2);
    double minv=(double)(v2.first-v1.first)/(v2.second-v1.second);
    double x=v1.first;
    int y=v1.second;
    while(y<v2.second)
    {
        if(x<table[y].xmin)table[y].xmin=(int)ceil(x);
        if(x>table[y].xmax)table[y].xmax=(int)floor(x);
        y++;
        x+=minv;
    }
}

void DrawSanLines(HDC hdc,Entry table[],COLORREF color)
{
    for(int y=0;y<MAXENTRIES;y++)
        if(table[y].xmin<table[y].xmax)
            for(int x=table[y].xmin;x<=table[y].xmax;x++)
                SetPixel(hdc,x,y,color);
}

void ConvexFill(HDC hdc,vector<pair<int,int>> p,COLORREF color)
{
    Entry *table=new Entry[MAXENTRIES];
    InitEntries(table);
    pair<int,int> v1 = p[p.size()-1];
    for(int i=0;i<p.size();i++)
    {
        pair<int,int> v2=p[i];
        ScanEdge(v1,v2,table);
        v1=p[i];
    }
    DrawSanLines(hdc,table,color);
    delete[] table;
}


EdgeRec InitEdgeRec(pair<int,int>& v1, pair<int,int>& v2)
{
    if(v1.second>v2.second)swap(v1,v2);
    EdgeRec rec;
    rec.x=v1.first;
    rec.ymax=v2.second;
    rec.minv=(double)(v2.first-v1.first)/(v2.second-v1.second);
    return rec;
}

void InitEdgeTable(vector<pair<int,int>> polygon,int n,EdgeList table[])
{
    pair<int,int> v1=polygon[n-1];
    for(int i=0;i<n;i++)
    {
        pair<int,int> v2=polygon[i];
        if(v1.second==v2.second){v1=v2;continue;}
        EdgeRec rec=InitEdgeRec(v1, v2);
        table[v1.second].push_back(rec);
        v1=polygon[i];
    }
}

void GeneralPolygonFill(HDC hdc,vector<pair<int,int>> polygon,int n,COLORREF c)
{
    EdgeList *table = new EdgeList [MAXENTRIES];
    InitEdgeTable(polygon, n, table);
    int y=0;
    while(y < MAXENTRIES && table[y].size() == 0) y++;
    if(y == MAXENTRIES)return;
    EdgeList ActiveList = table[y];
    while (ActiveList.size()>0)
    {
        ActiveList.sort();
        for(EdgeList::iterator it=ActiveList.begin(); it != ActiveList.end(); it++)
        {
            int x1 = (int)ceil(it->x);
            it++;
            int x2 = (int)floor(it->x);
            for(int x=x1;x<=x2;x++)
                SetPixel(hdc,x,y,c);
        }
        y++;
        EdgeList::iterator it = ActiveList.begin();
        while(it != ActiveList.end())
            if(y == it->ymax)
                it = ActiveList.erase(it);
            else
                it++;
        for(EdgeList::iterator it = ActiveList.begin(); it != ActiveList.end(); it++)
            it->x += it->minv;
        ActiveList.insert(ActiveList.end(), table[y].begin(), table[y].end());
    }
    delete[] table;
}

void FloodFill(HDC hdc,int x,int y,COLORREF Cb,COLORREF Cf)
{
    COLORREF C=GetPixel(hdc,x,y);
    if(C==Cb || C==Cf)return;
    SetPixel(hdc,x,y,Cf);
    FloodFill(hdc,x+1,y,Cb,Cf);
    FloodFill(hdc,x-1,y,Cb,Cf);
    FloodFill(hdc,x,y+1,Cb,Cf);
    FloodFill(hdc,x,y-1,Cb,Cf);
}

void NRFloodFill(HDC hdc,int x,int y,COLORREF Cb,COLORREF Cf)
{
    stack<pair<int,int>> S;
    S.push(make_pair(x,y));
    while(!S.empty())
    {
        pair<int,int> v=S.top();
        S.pop();
        COLORREF c=GetPixel(hdc,v.first,v.second);
        if(c==Cb || c==Cf)continue;
        SetPixel(hdc,v.first,v.second,Cf);
        S.push(make_pair(v.first+1,v.second));
        S.push(make_pair(v.first-1,v.second));
        S.push(make_pair(v.first,v.second+1));
        S.push(make_pair(v.first,v.second-1));
    }
}

void FillSquareWithHermite(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    for(int x = x1; x <= x2; x += 1)
    {
        pair<int, int> p1 = make_pair(x, y2);
        pair<int, int> p2 = make_pair(x, y1);
        pair<int, int> t1 = make_pair(0, 525); // the value of t1 that actually draw the line
        pair<int, int> t2 = make_pair(0, 525);
        DrawHermiteCurve(hdc, p1,t1, p2, t2, 100);
    }
}

void FillRectangleWithBezier(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    vector<pair<int,int>> points;

    for(int y = y1; y <= y2; y += 1)
    {
        points.push_back(make_pair(x1, y));
        points.push_back(make_pair(x1 + (x2-x1)/3, y));
        points.push_back(make_pair(x1 + 2*(x2-x1)/3, y));
        points.push_back(make_pair(x2, y));
        DrawBezierCurve(hdc, points, 100);

        points.clear();
    }
}

void fillCircleWithCircles(HDC hdc,int xc,int yc, int R,COLORREF color)
{
    for(int r = 0; r <= R; r++)
    {
        CircleFasterBresenham(hdc, xc, yc, r, color);
    }
}

void FillingCircleWithLines(HDC hdc, int xc, int yc, int r, COLORREF color)
{
    int x = 0, y = r;
    int d = 1 - r;

    while(x <= y)
    {
        // draw horizontal lines instead of points
        for(int i = xc - x; i <= xc + x; i++)
        {
            SetPixel(hdc, i, yc + y, color);
            SetPixel(hdc, i, yc - y, color);
        }
        for(int i = xc - y; i <= xc + y; i++)
        {
            SetPixel(hdc, i, yc + x, color);
            SetPixel(hdc, i, yc - x, color);
        }

        if(d < 0)
            d += 2*x + 3;
        else
        {
            d += 2*(x - y) + 5;
            y--;
        }
        x++;
    }
}


//=======================
// Clipping Algorithms
//=======================

void PointClipping(HDC hdc, int x,int y,int xleft,int ytop,int xright,int ybottom,COLORREF color)
{
    if(x>=xleft && x<= xright && y>=ytop && y<=ybottom)
    SetPixel(hdc,x,y,color);
}

OutCode GetOutCode(double x,double y,int xleft,int ytop,int xright,int ybottom)
{
    OutCode out;
    out.All=0;
    if(x<xleft)out.left=1;
    else if(x>xright)out.right=1;
    if(y<ytop)out.top=1;
    else if(y>ybottom)out.bottom=1;
    return out;
}


void VIntersect(double xs,double ys,double xe,double ye,int x,double *xi,double *yi)
{
    *xi=x;
    *yi=ys+(x-xs)*(ye-ys)/(xe-xs);
}
void HIntersect(double xs,double ys,double xe,double ye,int y,double *xi,double *yi)
{
    *yi=y;
    *xi=xs+(y-ys)*(xe-xs)/(ye-ys);
}

void CohenSuth(HDC hdc,int xs,int ys,int xe,int ye,int xleft,int ytop,int xright,int ybottom)
{
    double x1=xs,y1=ys,x2=xe,y2=ye;
    OutCode out1=GetOutCode(x1,y1,xleft,ytop,xright,ybottom);
    OutCode out2=GetOutCode(x2,y2,xleft,ytop,xright,ybottom);
    while( (out1.All || out2.All) && !(out1.All & out2.All))
    {
    double xi,yi;
    if(out1.All)
    {
        if(out1.left)VIntersect(x1,y1,x2,y2,xleft,&xi,&yi);
        else if(out1.top)HIntersect(x1,y1,x2,y2,ytop,&xi,&yi);
        else if(out1.right)VIntersect(x1,y1,x2,y2,xright,&xi,&yi);
        else HIntersect(x1,y1,x2,y2,ybottom,&xi,&yi);
        x1=xi;
        y1=yi;
        out1=GetOutCode(x1,y1,xleft,ytop,xright,ybottom);
    } else
    {
        if(out2.left)VIntersect(x1,y1,x2,y2,xleft,&xi,&yi);
        else if(out2.top)HIntersect(x1,y1,x2,y2,ytop,&xi,&yi);
        else if(out2.right)VIntersect(x1,y1,x2,y2,xright,&xi,&yi);
        else HIntersect(x1,y1,x2,y2,ybottom,&xi,&yi);
        x2=xi;
        y2=yi;
        out2=GetOutCode(x2,y2,xleft,ytop,xright,ybottom);
    }
    }
    if(!out1.All && !out2.All)
    {
        MoveToEx(hdc,round(x1),round(y1),NULL);
        LineTo(hdc,round(x2),round(y2));
    }
}

VertexList ClipWithEdge(VertexList p,int edge,IsInFunc In,IntersectFunc Intersect)
{
    VertexList OutList;
    Vertex v1=p[p.size()-1];
    bool v1_in=In(v1,edge);
    for(int i=0;i<(int)p.size();i++)
    {
        Vertex v2=p[i];
        bool v2_in=In(v2,edge);
        if(!v1_in && v2_in)
        {
            OutList.push_back(Intersect(v1,v2,edge));
            OutList.push_back(v2);
        }
        else if(v1_in && v2_in) OutList.push_back(v2);
        else if(v1_in) OutList.push_back(Intersect(v1,v2,edge));
        v1=v2;
        v1_in=v2_in;
        }
    return OutList;
}

bool InLeft(Vertex& v,int edge)
{
    return v.x>=edge;
}

bool InRight(Vertex& v,int edge)
{
    return v.x<=edge;
}

bool InTop(Vertex& v,int edge)
{
    return v.y>=edge;
}

bool InBottom(Vertex& v,int edge)
{
    return v.y<=edge;
}

Vertex VIntersect(Vertex& v1,Vertex& v2,int xedge)
{
    Vertex res;
    res.x=xedge;
    if(v2.x == v1.x)
        res.y = v1.y;
    else
        res.y = v1.y + (xedge-v1.x)*(v2.y-v1.y)/(v2.x-v1.x);
    return res;
}

Vertex HIntersect(Vertex& v1,Vertex& v2,int yedge)
{
    Vertex res;
    res.y=yedge;
    if(v2.y == v1.y)
        res.x = v1.x;
    else
        res.x = v1.x + (yedge-v1.y)*(v2.x-v1.x)/(v2.y-v1.y);

    return res;
}

void PolygonClip(HDC hdc,const vector<pair<int,int>>& p, int xleft,int ytop,int xright,int ybottom)
{
    VertexList vlist;
    for(int i = 0; i < p.size(); i++)
        vlist.push_back(Vertex(p[i].first, p[i].second));
    vlist=ClipWithEdge(vlist,xleft,InLeft,VIntersect);
    vlist=ClipWithEdge(vlist,ytop,InTop,HIntersect);
    vlist=ClipWithEdge(vlist,xright,InRight,VIntersect);
    vlist=ClipWithEdge(vlist,ybottom,InBottom,HIntersect);
    if(vlist.empty()) return;
    Vertex v1=vlist[vlist.size()-1];
    for(int i=0;i<(int)vlist.size();i++)
    {
        Vertex v2=vlist[i];
        MoveToEx(hdc,round(v1.x),round(v1.y),NULL);
        LineTo(hdc,round(v2.x),round(v2.y));
        v1=v2;
    }
}

bool PointClippingInsideCircle(int x,int y,int xc,int yc,int r) ////////////////////////////////////////////////////////////////////////////////////
{
    int dx = x - xc;
    int dy = y - yc;

    return (dx * dx + dy * dy <= r * r);
}

void ClipLineCircle(HDC hdc, int x1, int y1, int x2,int y2, int xc, int yc, int r, COLORREF color)
{
    double dx = x2 - x1;
    double dy = y2 - y1;

    //------------------------------------
    // Quadratic Equation
    //------------------------------------
    double a = dx * dx + dy * dy;

    double b =
        2 * (
            dx * (x1 - xc)
            + dy * (y1 - yc)
        );

    double c =
        (x1 - xc) * (x1 - xc)
        + (y1 - yc) * (y1 - yc)
        - r * r;

    //------------------------------------
    // Discriminant
    //------------------------------------
    double disc = b * b - 4 * a * c;

    //------------------------------------
    // No Intersection
    //------------------------------------
    if (disc < 0)
    {
        bool inside1 = PointClippingInsideCircle(x1, y1, xc, yc, r);
        bool inside2 = PointClippingInsideCircle(x2, y2, xc, yc, r);

        // Entirely inside
        if (inside1 && inside2)
        {
            SimpleDDA(
                hdc,
                x1, y1,
                x2, y2,
                RGB(0, 0, 0)
            );
        }
        else
        {
            SimpleDDA(
                hdc,
                x1, y1,
                x2, y2,
                color
            );
        }

        return;
    }

    //------------------------------------
    // Intersections Exist
    //------------------------------------
    double sqrtDisc = sqrt(disc);

    double t1 =
        (-b - sqrtDisc) / (2 * a);

    double t2 =
        (-b + sqrtDisc) / (2 * a);

    if (t1 > t2)
        swap(t1, t2);

    //------------------------------------
    // Completely Outside
    //------------------------------------
    if (t1 > 1 || t2 < 0)
    {
        SimpleDDA(
            hdc,
            x1, y1,
            x2, y2,
            color
        );

        return;
    }

    //------------------------------------
    // Clamp Values
    //------------------------------------
    double tt1 = max(0.0, t1);
    double tt2 = min(1.0, t2);

    //------------------------------------
    // Intersection Points
    //------------------------------------
    int ix1 = round(x1 + tt1 * dx);
    int iy1 = round(y1 + tt1 * dy);

    int ix2 = round(x1 + tt2 * dx);
    int iy2 = round(y1 + tt2 * dy);

    //------------------------------------
    // First Outside Segment
    //------------------------------------
    if (tt1 > 0)
    {
        SimpleDDA(
            hdc,
            x1, y1,
            ix1, iy1,
            color
        );
    }

    //------------------------------------
    // Inside Segment (RED)
    //------------------------------------
    SimpleDDA(
        hdc,
        ix1, iy1,
        ix2, iy2,
        RGB(0, 0, 0)
    );

    //------------------------------------
    // Second Outside Segment
    //------------------------------------
    if (tt2 < 1)
    {
        SimpleDDA(
            hdc,
            ix2, iy2,
            x2, y2,
            color
        );
    }
}


//=======================
// Faces Algorithms
//=======================

void DrawHappyFace(HDC hdc, int xc, int yc, int r, COLORREF color)
{
    // ================= FACE =================
    CircleBresenham(hdc, xc, yc, r, color);

    // ================= EYES =================
    CircleBresenham(hdc, xc - r/3, yc - r/3, r/10, color);
    CircleBresenham(hdc, xc + r/3, yc - r/3, r/10, color);

    // ================= NOSE =================
    MidPoint(hdc, xc, yc - r/8, xc, yc + r/4, color);

    // ================= MOUTH =================
    vector<pair<int,int>> mouth;

    mouth.push_back({xc - r/2, yc + r/4});
    mouth.push_back({xc - r/4, yc + r/2});
    mouth.push_back({xc + r/4, yc + r/2});
    mouth.push_back({xc + r/2, yc + r/4});

    DrawBezierCurve(hdc, mouth, 100);
}

void DrawSadFace(HDC hdc, int xc, int yc, int r, COLORREF color)
{
    // ================= FACE =================
    CircleBresenham(hdc, xc, yc, r, color);

    // ================= EYES =================
    CircleBresenham(hdc, xc - r/3, yc - r/3, r/10, color);
    CircleBresenham(hdc, xc + r/3, yc - r/3, r/10, color);

    // ================= NOSE =================
    MidPoint(hdc,
             xc,
             yc - r/8,
             xc,
             yc + r/4,
             color);

    // ================= MOUTH =================
    vector<pair<int,int>> mouth;

    mouth.push_back({xc - r/2, yc + r/2});
    mouth.push_back({xc - r/4, yc + r/4});
    mouth.push_back({xc + r/4, yc + r/4});
    mouth.push_back({xc + r/2, yc + r/2});

    DrawBezierCurve(hdc, mouth, 100);
}
