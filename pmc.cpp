#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
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

    bool operator==(const Point& P) const {
        return x == P.x && y == P.y && z == P.z;
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

    string zIntercept(const Point P0, const Points& p) const {
        const Point& Pref0 = p[P[0]] - P0;
        // Face intersection
        if (normal.z > 0) {
            double z = normal.z * (normal.x * Pref0.x + normal.y * Pref0.y + normal.z * Pref0.z);
            if (z > 0) return "z_Positive intersection";
            if (z < 0) return "z_Negative intersection";
            return "z_on_Body";
        }

        // Tetra volume
        if(tetraVol(p[P[0]], p[P[1]], p[P[2]],P0)==0)
            return "z_on_Body";
    }

    string zAxisIntersection(const Point P0, const Points& p, unordered_set<int>& pList) const {
        // check X,Y bounding box for face.
        if (checkXAxisIntersection(P0, p)) return "No_Intersection";
        if (checkYAxisIntersection(P0, p)) return "No_Intersection";

        // Check Point intersections
        for (size_t i = 0; i < 3; i++)
        {
            if (P0.x == p[P[i]].x && P0.y == p[P[i]].y && p[P[i]].isProjection()) {
                if (pList.count(P[i]) == 0) {
                    pList.emplace(P[i]);
                    if (p[P[i]].z > P0.z) "z_Positive intersection";
                    if (p[P[i]].z < P0.z) "z_Negative intersection";
                }
                return "No_Intersection";
            }
        }

        // Check Edge intersections
        double area[3];
        for (size_t i = 0; i < 3; i++) {
            const Point& Pi = p[P[i]];
            const Point& Pj = p[P[next(i)]];
            area[i] = (Pi - P0) * (Pj - P0);
            if (area[i] == 0) {
                if (P[i] < P[next(i)] && !EDGE(P[i] , P[next(i)]).isProjection())
                    return "edge_intersection";
                else
                    return "no_intersection";
            }
        }

        if (area[0] > 0 && area[1] > 0 && area[2] > 0) return "no_intersection";
        if (area[0] < 0 && area[1] < 0 && area[2] < 0) return "no_intersection";


        // check Axis was inconclusive, returning cross product test
        if (PMCz_cross_product_test(P0, p) == "no_intersection") 
            return "No_Intersection";

        // Check Face intersections
        return zIntercept(P0, p);
    }
};

typedef vector<Face> Faces;
int PMC( const Faces& faces, const Points& points, const Point& P, unordered_set<int>& pList) {
    std::atomic<int> z_positve = 0, z_negative = 0;
    bool notOnBody = true;

    std::for_each(
        std::execution::par_unseq,
        faces.begin(),
        faces.end(),
        [&P, &points, &z_positve, &z_negative, &notOnBody] ( const Face& face) {
            if (notOnBody) {
                auto res = face.zAxisIntersection(P, points, pList);
                if (res == "z_on_Body")                     notOnBody = false;
                if (res == "z_Positive intersection")                              z_positve++;
                if (res == "z_Negative intersection")                              z_negative++;
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
    unordered_set<int> pList;
    auto res = PMC(faces, points, P, pList);
    if (res == 1) cout << "Outside the body";
    if (res == 0) cout << "On the body";
    if (res == -1) cout << "Inside the body";
    return 0;
}
//int test(vector<int> v) {
//
//    vector<int> indices(v.size(), 0);
//
//    int len = v.size(), sum = 0, imax = len - 1, imin = 0;
//
//    for (size_t i = 0; i < len; i++)
//        indices[i] = i;
//
//    sort(indices.begin(), indices.end(), &[v](int i, int j) { return v[i] <= v[j]; });
//
//    for (size_t i = 0; i < len; i++) {
//        int index = indices[i];
//        if (v[index] != 0) {
//            sum = sum + v[index];
//            if (index != imax) v[index + 1] = 0;
//            if (index != imin) v[index - 1] = 0;
//        }
//    }
//
//    return sum;
//}
//
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
