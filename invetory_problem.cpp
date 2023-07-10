// invetory_problem.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;

int next(const int& i) {
    if (i == 2) return 0;
    return i + 1;
}

bool tol(const double& x, const double& y) {
    if (abs(x - y) < 1e-5) return true;
    return false;
}

class Point {
public:
    double x, y;
    Point() { x = 0; y = 0;}
    Point(double xx,double yy):x(xx),y(yy) {}
    Point operator+(const Point& P) const {
        return Point(x + P.x, y + P.y);
    }
    Point operator-(const Point& P) const {
        return Point(x - P.x, y - P.y);
    }
    double operator*(const Point& P) const {
        return x*P.y - y*P.x;
    }

    int xSegment(const Point& P1, const Point& P2) const {
        if (P2.x < P1.x && P1.x < x)             return 1;
        if (P2.x > P1.x && P1.x > x)             return 1;

        if (P1.x == x || P2.x == x)             return 0;
        return -1;
    }

    int ySegment(const Point& P1, const Point& P2) const {
        if (P2.y < P1.y && P1.y < y)             return 1;
        if (P2.y > P1.y && P1.y > y)             return 1;

        if (P1.y == y || P2.y == y)             return 0;
        return -1;
    }
};

typedef vector<Point> Points;
class Face {
public:
    int P[3];

    bool checkXAxisIntersection(const Point& P0, const Points& p) const {
        return P0.xSegment(p[P[0]], p[P[1]]) == 1 && P0.xSegment(p[P[1]], p[P[2]]) == 1;
    }
    bool checkYAxisIntersection(const Point& P0, const Points& p) const  {
        return P0.ySegment(p[P[0]], p[P[1]]) == 1 && P0.ySegment(p[P[1]], p[P[2]]) == 1;
    }

    int PMC_cross_product_test(const Point& P0, const Points& p) const {
        double area[3];
        for (size_t i = 0; i < 3; i++) {
            const Point& Pi = p[P[i]];
            const Point& Pj = p[P[next(i)]];
            area[i] = (Pi - P0) * (Pj - P0);
            if (tol(area[i], 0)) 
                return 0;
        }

        if (area[0] > 0 && area[1] > 0 && area[2] > 0) return 1;
        if (area[0] < 0 && area[1] < 0 && area[2] < 0) return 1;
        return -1;
    }

    int PMC(const Point P0, const Points& p) const {
        //Most faces would resolve in the axis in tersection tests.
        if (checkXAxisIntersection(P0, p)) return 1;
        if (checkYAxisIntersection(P0, p)) return 1;

        // check Axis was inconclusive, returning cross product test
        return PMC_cross_product_test(P0, p);
    }
};

typedef vector<Face> Faces;
#include <execution>
bool PMC(const Faces& faces, const Points& points, const Point& P) {
    bool isOutside = true;
    std::for_each(
        std::execution::par,
        faces.begin(),
        faces.end(),
        [&P, &points, &isOutside] ( const Face& face) {
            if (isOutside && face.PMC(P, points) == 1)
                isOutside = false;
        });

    return !isOutside;
}

int main()
{
    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
