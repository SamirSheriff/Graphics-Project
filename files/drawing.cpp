#include "drawing.h"

//================
// Line Algorithms
//=================

void ParametricLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
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
    else
    {
        int y=ys,yinc= dy>0?1:-1;
        double x=xs,xinc=(double)dx/dy*yinc;
        while(y!=ye)
        {
            x+=xinc;
            y+=yinc;
            SetPixel(hdc,round(x),y,color);
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

void DrawHermiteCurve (HDC hdc, pair<int,int>& P0, pair<int,int>& T0, pair<int,int>& P1, pair<int,int>& T1 ,int numpoints)
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


void DrawCardinalSpline(HDC hdc,Vector2 P[],int n,double c,int numpix)
{
    double c1=1-c;
    Vector2 T0(c1*(P[2].x-P[0].x),c1*(P[2].y-P[0].y));
    for(int i=2;i<n-1;i++)
    {
        Vector2 T1(c1*(P[i+1].x-P[i-1].x),c1*(P[i+1].y-P[i-1].y));
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
        table[i].xmin=MAXINT;
        table[i].xmax=-MAXINT;
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

void ConvexFill(HDC hdc,pair<int,int> p[],int n,COLORREF color)
{
    Entry *table=new Entry[MAXENTRIES];
    InitEntries(table);
    pair<int,int> v1=p[n-1];
    for(int i=0;i<n;i++)
    {
        pair<int,int> v2=p[i];
        ScanEdge(v1,v2,table);
        v1=p[i];
    }
    DrawSanLines(hdc,table,color);
    delete table;
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

void InitEdgeTable(pair<int,int> *polygon,int n,EdgeList table[])
{
    pair<int,int> v1=polygon[n-1];
    for(int i=0;i<n;i++)
    {
        pair<int,int> v2=polygon[i];
        if(v1.second==v2.second){v1=v2;continue;}
        EdgeRec rec=InitEdgeRec(v1, v2);
        table[v1.y].push_back(rec);
        v1=polygon[i];
    }
}

void GeneralPolygonFill(HDC hdc,pair<int,int> *polygon,int n,COLORREF c)
{
    EdgeList *table=new EdgeList [MAXENTRIES];
    InitEdgeTable(polygon,n,table);
    int y=0;
    while(y<MAXENTRIES && table[y].size()==0)y++;
    if(y==MAXENTRIES)return;
    EdgeList ActiveList=table[y];
    while (ActiveList.size()>0)
    {
        ActiveList.sort();
        for(EdgeList::iterator it=ActiveList.begin();it!=ActiveList.end();it++)
        {
            int x1=(int)ceil(it->x);
            it++;
            int x2=(int)floor(it->x);
            for(int x=x1;x<=x2;x++)SetPixel(hdc,x,y,c);
        }
        y++;
        EdgeList::iterator it=ActiveList.begin();
        while(it!=ActiveList.end())
            if(y==it->ymax) it=ActiveList.erase(it); else it++;
        for(EdgeList::iterator it=ActiveList.begin();it!=ActiveList.end();it++)
            it->x+=it->minv;
        ActiveList.insert(ActiveList.end(),table[y].begin(),table[y].end());
    }
    delete[] table;
}

void FloodFill(HDC hdc,int x,int y,COLORREF Cb,COLORREF Cf)
{
    COLORREF C=GetPixel(hdc,x,y);
    if(C==Cb || C==Cf)return;
    SetPixel(hdc,x,y,cf);
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
        COLORREF c=GetPixel(v.x,v.y);
        if(c==Cb || c==Cf)continue;
        SetPixel(hdc,v.x,v.y,Cf);
        S.push(make_pair(v.first+1,v.second));
        S.push(make_pair(v.first-1,v.second));
        S.push(make_pair(v.first,v.second+1));
        S.push(make_pair(v.first,v.second-1));
    }
}
