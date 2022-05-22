#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;

const double xmin = -10, xmax = 10, ymin = -10, ymax = 10, eps = 0.5;
const int width = 700, height = 700;
const double EPSmin = 0.0000001;

void reshape(int width, int height)
{
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(xmin, xmax, ymin, ymax);
}

class Point
{
public:
    double x;
    double y;
    double polarAngle;

    Point()
    {
        x = 0;
        y = 0;
    }

    Point(double x_, double y_) : x(x_), y(y_) {}

    Point operator- (Point const arg)
    {
        return (Point(x-arg.x, y - arg.y));
    }
};

//векотры всех точек, выпуклой оболочки
vector <Point> p, h;

//приведение координат точек, поставленных мышью, к относительным координатам
Point Convert(Point arg)
{
    double scale_x = (xmax - xmin) / width;
    double scale_y = (ymax - ymin) / height;

    return Point(arg.x * scale_x + xmin, ymax - arg.y * scale_y);
}

//вычисление псевдокалярного (косого) произведения
double skewProduct(Point A, Point A1, Point B, Point B1)
{
    return ((A1.x - A.x) * (B1.y - B.y) - (A1.y - A.y) * (B1.x - B.x));
}

//вычисление скалярного произведения
double dotProduct(Point A, Point A1, Point B, Point B1)
{
    return ((A1.x - A.x) * (B1.x - B.x) + (A1.y - A.y) * (B1.y - B.y));
}

double lenght(Point A, Point B)
{
    return sqrt(pow(B.x - A.x, 2) + pow(B.y - A.y, 2));
}

void drawPoint(Point& argA, string color)
{
    glPointSize(5);
    glBegin(GL_POINTS);

    if (color == "white")
    {
        glColor3f(1, 1, 1);
    }
    if (color == "green")
    {
        glColor3f(0, 1, 0);
    }

    glVertex2f(argA.x, argA.y);
    glEnd();
    glFlush();
}

bool comp(Point a, Point b)
{
    return a.polarAngle < b.polarAngle;
}

void showBackground()
{
    glLineWidth(2);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(5, 0xAAAA);
    glBegin(GL_LINES);
    for (int i=xmin; i<xmax; i++)
    {
        glVertex2f(i, ymin);
        glVertex2f(i, ymax);
        glVertex2f(xmin, i);
        glVertex2f(xmax, i);
    }
    glEnd();
    glDisable(GL_LINE_STIPPLE);
}

void drawConvexHull()
{
    glColor3f(1, 1, 1);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    for (double i = 0; i < h.size(); i++)
    {
        glVertex2f(h[i].x, h[i].y);
    }
    glEnd();
}

void drawPoints()
{
    for (double i = 0; i < p.size(); i++)
    {
        drawPoint(p[i], "green");
    }

    for (double i = 0; i < h.size(); i++)
    {
        drawPoint(h[i], "white");
    }
}

void display(void)
{
    glClearColor(0.0, 0.0, 0.1, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3ub(150, 150, 150);

    showBackground();

    if (p.size() > 1)
        drawConvexHull();

    drawPoints();

    glutSwapBuffers();
}

void makeConvexHull()
{
    double iMin = 0;

    for (double i = 0; i < p.size(); ++i)
    {
        if (p[iMin].y <= p[i].y)
        {
            iMin = i;
        }
    }

    for (double i = 0; i < p.size(); ++i)
    {
        p[i].polarAngle = atan2(skewProduct(p[iMin], Point(1000, p[iMin].y), p[iMin], p[i]),
                                dotProduct(p[iMin], Point(1000, p[iMin].y), p[iMin], p[i]));
    }

    sort(p.begin(), p.end(), comp);

    h.push_back(p[0]);
    h.push_back(p[1]);

    for (double i = 2; i < p.size(); ++i)
    {
        h.push_back(p[i]);

        while (skewProduct(h[h.size() - 2], h[h.size() - 1], h[h.size() - 2], h[h.size() - 3]) < 0
               && h.size() > 2)
        {
            h.erase(h.begin() + h.size() - 2);
        }
    }
}

void mouse(int button, int state, int x, int y)
{
    //расстановка точек
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        h.clear();

        Point temp = Convert(Point(x,y));
        p.push_back(temp);

        if (p.size() == 2)
        {
            h.push_back(p[0]);
            h.push_back(p[1]);
        }

        if (p.size() > 2)
        {
            makeConvexHull();
        }

        display();
    }

    //перезагрузка
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
    {
        h.clear();
        p.clear();
        display();
    }
}

int main(int argc, char* argv[])
{
    srand(time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(500, 50);
    glutCreateWindow("gr@ph!cs");
    glShadeModel(GL_FLAT);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);

    glutMainLoop();
    return 0;
}

