#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <execution>

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
    double x, y ,z;
    Point() { x = 0; y = 0; z = 0;}
    Point(double xx,double yy, double zz ):x(xx),y(yy), z(zz) {}

    Point operator+(const Point& P) const {
        return Point(x + P.x, y + P.y, z + P.z);
    }

    Point operator-(const Point& P) const {
        return Point(x - P.x, y - P.y, z - P.z);
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
    Point normal;
    bool checkXAxisIntersection(const Point& P0, const Points& p) const {
        return P0.xSegment(p[P[0]], p[P[1]]) == 1 && P0.xSegment(p[P[1]], p[P[2]]) == 1;
    }

    bool checkYAxisIntersection(const Point& P0, const Points& p) const  {
        return P0.ySegment(p[P[0]], p[P[1]]) == 1 && P0.ySegment(p[P[1]], p[P[2]]) == 1;
    }

    int PMCz_cross_product_test(const Point& P0, const Points& p) const {
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

    int zIntercept(const Point P0, const Points& p) const {
        const Point& Pref = p[P[0]] - P0;
        double z = normal.z * (normal.x * Pref.x + normal.y * Pref.y + normal.z * Pref.z);
        if (z > 0) return 1;
        if (z < 0) return -1;
        return 2;
    }
    int PMCz(const Point P0, const Points& p) const {
        //Most faces would resolve in the axis in tersection tests.
        if (checkXAxisIntersection(P0, p)) return 0;
        if (checkYAxisIntersection(P0, p)) return 0;

        // check Axis was inconclusive, returning cross product test
        if (PMCz_cross_product_test(P0, p) == 1 ) return 0;

        return zIntercept(P0, p);
    }
};

typedef vector<Face> Faces;
int PMC( const Faces& faces, const Points& points, const Point& P ) {
    std::atomic<int> z_positve = 0, z_negative = 0;
    bool notOnBody = true;

    std::for_each(
        std::execution::par_unseq,
        faces.begin(),
        faces.end(),
        [&P, &points, &z_positve, &z_negative, &notOnBody] ( const Face& face) {
            if (notOnBody) {
                auto res = face.PMCz(P, points);
                if (res == 2)                    notOnBody = false;
                if (res == 1)                    z_positve++;
                if (res == -1)                   z_negative++;
            }
        });

    if (!notOnBody) return 0;
    if (z_positve % 2 == 1) return -1;
    return 1;
}

int main()
{
    Faces faces;
    Points points;
    Point P(0, 0, 0);
    
    auto res = PMC(faces, points, P);
    if (res == 1) cout << "Outside the body";
    if (res == 0) cout << "On the body";
    if (res == 1) cout << "Inside the body";
    return 0;
}
