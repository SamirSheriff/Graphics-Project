#ifndef SHAPES_H_INCLUDED
#define SHAPES_H_INCLUDED

#include "drawing.h"
#include <fstream>
#include <iostream>

using namespace std;

//================
// Parent class
//================
class shape
{
    public:
        COLORREF color;

        shape(COLORREF col);

        virtual void draw(HDC hdc) = 0;
        virtual void save(ofstream& out) = 0;

        virtual ~shape();
};


//================
// Line Class
//================
class Line : public shape
{
public:
    int x1, y1, x2, y2;
    int algo;

    Line(int a, int b, int c, int d, int alg, COLORREF col);

    void draw(HDC hdc) override;
    void save(ofstream& out) override;

    ~Line();
};


//===================
// Circle Class
//===================
class Circle : public shape
{
public:
    int xc, yc, r;
    int algo;

    Circle(int a, int b, int c, int algo, COLORREF col);

    void draw(HDC hdc) override;
    void save(ofstream& out) override;

     ~Circle();
};


//===================
// Ellipse Class
//===================
class myEllipse : public shape
{
private:
    pair<int, int> center, a_, b_;
    int m_;
    vector<double> factorials;
    vector<double> powers;

    // Generates powers of theta up to theta^10
    void compute_powers(double theta);

    // Generates factorials up to number!
    void compute_factorials(int number);

    void direct(HDC hdc);

    void polar(HDC hdc);

    void midpoint(HDC hdc);


public:

    myEllipse(pair<int, int> c, pair<int, int> a, pair<int, int> b, int mode, COLORREF col);

    void draw(HDC hdc) override;

    void save(ofstream& out) override;

    ~myEllipse();
};


//===================
// Curve Classes
//===================
class BezierCurve : public shape
{
public:
    vector<pair<int,int>> points;

    BezierCurve(vector<pair<int,int>>& p, COLORREF col);

    void draw(HDC hdc) override;
    void save(ofstream& out) override;

    ~BezierCurve();
};


class HermiteCurve : public shape
{
public:
    pair<int,int> p1, t1, p2, t2;

    HermiteCurve(pair<int,int> a, pair<int,int> b, pair<int,int> c, pair<int,int> d, COLORREF col);

    void draw(HDC hdc) override;
    void save(ofstream& out) override;

    ~HermiteCurve();
};


class CardinalSplineCurve: public shape
{
public:
    vector<pair<int,int>> points;
    double c;

    CardinalSplineCurve(vector<pair<int,int>> p, double a, COLORREF col);

    void draw(HDC hdc) override;
    void save(ofstream& out) override;

    ~CardinalSplineCurve();
};



//===================
// Filling Classes
//===================
class FillingCircles : public shape
{
public:
    int xc, yc, r;
    int x, y;
    int algo;

    FillingCircles(int _x, int _y, int _xc, int _yc, int R, int alg, COLORREF col);

    void draw(HDC hdc) override;
    void save(ofstream& out) override;

    ~FillingCircles();
};


class FillingWithCurves : public shape
{
public:
    vector<pair<int,int>> pts;
    int algo;

    FillingWithCurves(vector<pair<int,int>> p, int alg, COLORREF col);

    void draw(HDC hdc) override;
    void save(ofstream& out) override;

    ~FillingWithCurves();
};


class PolygonFilling : public shape
{
public:
    vector<pair<int,int>> points;
    int algo;

    PolygonFilling(vector<pair<int,int>> p, int alg, COLORREF col);

    void draw(HDC hdc) override;
    void save(ofstream& out) override;

    ~PolygonFilling();
};


class FloodFillShape : public shape
{
public:
    int x, y;
    int algo;
    COLORREF cb, cf;

    FloodFillShape(int a, int b, int alg, COLORREF color1, COLORREF color2);

    void draw(HDC hdc) override;
    void save(ofstream& out) override;

     ~FloodFillShape();
};


//===================
// Clipping Class
//===================
class clipping : public shape
{
public:
    vector<pair<int,int>> pts;
    int left, right, top, bottom;
    int xc, yc, r;
    int algo;

    clipping(vector<pair<int,int>> p, int l, int r, int t, int b, int alg, COLORREF c);
    clipping(vector<pair<int,int>> p, int _xc, int _yc, int rad, int alg, COLORREF c);

    void draw(HDC hdc) override;
    void save(ofstream& out) override;

    ~clipping();
};


//===================
// Face Class
//===================
class Face : public shape
{
public:
    int xc, yc, r;
    int algo;

    Face(int a, int b, int c, int alg, COLORREF col);

    void draw(HDC hdc) override;
    void save(ofstream& out) override;

     ~Face();
};

#endif // SHAPES_H_INCLUDED
