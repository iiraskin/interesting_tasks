#include <iostream>
#include <vector>
#include <cmath>
#include <queue>
#include <set>

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::sqrt;
using std::queue;
using std::set;
using std::min;
using std::swap;

struct CPoint
{
    int x, y, z;
    CPoint(const int _x, const int _y, const int _z): x(_x), y(_y), z(_z) {}
    CPoint(): x(0), y(0), z(0) {}
};

struct CVector
{
    long x, y, z;
    CVector(CPoint A, CPoint B): x(B.x  - A.x), y(B.y - A.y), z(B.z - A.z) {}
    CVector(const long _x, const long _y, const long _z): x(_x), y(_y), z(_z) {}
    double Mod()
    {
        return sqrt(x * x + y * y + z * z);
    }
};

CVector VectorProduct(const CVector a, const CVector b)
{
     return CVector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

long DotProduct(const CVector a, const CVector b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

//Нормаль к плоскости face.
CVector Normal(const vector<CPoint>& points, const int A, const int B, const int C)
{
    return VectorProduct(CVector(points[A], points[B]), CVector(points[A], points[C]));
}

//Внешняя нормаль к плоскости face.
CVector OuterNormal(const vector<CPoint>& points, const int A, const int B, const int C)
{
    CVector n = Normal(points, A, B, C);
    int somePoint;//Индекс точки множества, не лежащая в грани.
    //Выбираем точку, не совпадающую с A, B и C.
    for (int i = 0; i < 4; ++i) {
        if (A == i) {continue;}
        if (B == i) {continue;}
        if (C == i) {continue;}
        somePoint = i;
        break;
    }
    //Проверка, что нормаль внешняя.
    if (DotProduct(n, CVector(points[A], points[somePoint])) > 0) {
        n.x = - n.x;
        n.y = - n.y;
        n.z = - n.z;
    }
    return n;
}

struct CFace
{
    int A, B, C;

    CFace() {}

    CFace(const int a, const int b, const int c, const vector<CPoint>&  points): A(a), B(b), C(c)
    {
        while (B < A || C < A) {
            swap(A, B);
            swap(B,C);
        }
    }

    void SetOrientation(const vector<CPoint>&  points)
    {
        CVector vp = Normal(points, A, B, C);
        CVector n = OuterNormal(points, A, B, C);
        if (vp.x * n.x <= 0 && vp.y * n.y <= 0 && vp.z * n.z <= 0) {
            swap(B, C);
        }
    }

    bool operator<(const CFace& other) const
    {
        return (A < other.A || (A == other.A && (B < other.B ||
                                 B == other.B && C < other.C))) ? true : false;
    }

    bool operator==(const CFace& other) const
    {
        return (A == other.A && B == other.B && C == other.C) ? true : false;
    }
};

//Находит косинус угла между плоскостями F и S.
double FindAngleBetweenPlanes(const CPoint F1, const CPoint F2, const CPoint F3,
                              const CPoint S1, const CPoint S2, const CPoint S3)
{
    CVector normalF = VectorProduct(CVector(F1, F2), CVector(F1, F3)); //Нормаль к плоскости F.
    CVector normalS = VectorProduct(CVector(S1, S2), CVector(S1, S3)); //Нормаль к плоскости S.
    return DotProduct(normalF, normalS) / normalF.Mod() / normalS.Mod();
}

int FindPointWithMaxAngle(const vector<CPoint>& points, const CPoint point1, const CPoint point2,
                          const CPoint point3, const vector<int>& bans)
{
    int res;
    double max_angle = -2;
    for (int i = 0; i < points.size(); ++i) {
        bool is_in_ban = false;
        for (int j = 0; j < bans.size(); ++j) {
            if (bans[j] == i) {
                is_in_ban = true;
                break;
            }
        }
        if (is_in_ban) {
            continue;
        }
        double angle = FindAngleBetweenPlanes(point1, point2, point3, point1, point2, points[i]);
        if (angle >= max_angle) {
            max_angle = angle;
            res = i;
        }
    }
    return res;
}

void Initialization(const vector<CPoint>& points, const int first, int& second, int& third)
{
    //Находим вторую точку первой грани.
    if (second != -1 && third != -1) {
        return;
    }
    vector<int>  bans(1, first);
    CPoint somePoint = points[first];
    if (second == -1) {
        CPoint someAnotherPoint = points[first];
        somePoint.x -= 1;
        someAnotherPoint.y -= 1;
        second = FindPointWithMaxAngle(points, points[first], somePoint, someAnotherPoint, bans);
    } else {
        somePoint.x += points[second].x;
        somePoint.y += points[second].y + 1;
    }
    //Находим третью точку первой грани.
    bans.push_back(second);
    third = FindPointWithMaxAngle(points, points[first], points[second], somePoint, bans);
}

//Находит грани, смежные с face по ребрам.
void SharedFaces(const vector<CPoint>& points, set<CFace>& hull, queue<CFace>& faces)
{
    //Найдём нормаль к плоскости.
    CFace face = faces.front();
    CVector n = OuterNormal(points, face.A, face.B, face.C);
    double g1 = -2, g2 = -2, g3 = -2;//Косинусы, минимума которых необходимо достигнуть.
    CFace f1, f2, f3;
    for (int i = 0 ; i < points.size(); ++i) {
        if (face.A != i && face.B != i && face.C != i) {
            CFace f1_test = CFace(face.A, i, face.B, points);
            CFace f2_test = CFace(face.B, i, face.C, points);
            CFace f3_test = CFace(face.C, i, face.A, points);
            CVector n1 = Normal(points, f1_test.A, f1_test.B, f1_test.C);
            CVector n2 = Normal(points, f2_test.A, f2_test.B, f2_test.C);
            CVector n3 = Normal(points, f3_test.A, f3_test.B, f3_test.C);
            double new_g1 = DotProduct(n, n1) / n.Mod() / n1.Mod();
            double new_g2 = DotProduct(n, n2) / n.Mod() / n2.Mod();
            double new_g3 = DotProduct(n, n3) / n.Mod() / n3.Mod();
            if (new_g1 > g1) {
                g1 = new_g1;
                f1 = f1_test;
            }
            if (new_g2 > g2) {
                g2 = new_g2;
                f2 = f2_test;
            }
            if (new_g3 > g3) {
                g3 = new_g3;
                f3 = f3_test;
            }
        }
    }
    //Проверим, что грани новые.
    bool b1 = true, b2 = true, b3 = true;
    if (hull.count(f1) != 0){b1 = false;}
    if (hull.count(f2) != 0){b2 = false;}
    if (hull.count(f3) != 0){b3 = false;}
    if (f1 == f2) {b1 = false;}
    if (f1 == f3) {b2 = false;}
    if (f2 == f3) {b3 = false;}
    if (b1) {
        faces.push(f1);
        hull.insert(f1);
    }
    if (b2) {
        faces.push(f2);
        hull.insert(f2);
    }
    if (b3) {
        faces.push(f3);
        hull.insert(f3);
    }
}

void FindHull(const vector<CPoint>& points, set<CFace>& hull, const int first, int second, int third)
{
    Initialization(points, first, second, third);
    queue<CFace> faces;
    CFace firstFace = CFace(first, second, third, points);
    firstFace.SetOrientation(points);
    faces.push(firstFace);
    hull.insert(faces.front());
    while (!faces.empty()) {
        SharedFaces(points, hull, faces);
        faces.pop();
    }
}

void Input(vector<CPoint>& points, int& first, int& second, int& third)
{
    int n;
    cin >> n;
    for (int i = 0; i < n; ++i) {
        int x, y, z;
        cin >> x >> y >> z;
        points.push_back(CPoint(x, y, z));
        if (z < points[first].z) {
            first = points.size() - 1;
            second = -1;
            third = -1;
        } else if (z == points[first].z){
            if (y < points[first].y || (y == points[first].y && x < points[first].x)) {
                if (second == -1) {
                    second = first;
                } else {
                    third = first;
                }
                first = points.size() - 1;
            } else if (y != points[first].y || x != points[first].x) {
                if (second == -1) {
                    second = points.size() - 1;
                } else {
                    third = points.size() - 1;
                }
            }
        }
    }
}

int main()
{
    int m;
    cin >> m;
    for (int j = 0; j < m; ++j) {
        vector<CPoint> points;
        int first = 0, second = -1, third = -1;
        Input(points, first, second, third);
        set<CFace> hull;
        FindHull(points, hull, first, second, third);
        cout << hull.size() << endl;
        for (CFace face: hull) {
            cout << 3 << ' ' << face.A << ' ' << face.B << ' ' << face.C << endl;
        }
    }
    return 0;
}
