#include <iostream>
#include <vector>
#include <array>
#include <limits>

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::swap;
using std::array;

const double INF = std::numeric_limits<double>::infinity();

struct CPoint {
    double x, y, z;

    CPoint* previous;//Предыдущая точка.
    CPoint* next;//Следующая точка.

    CPoint(): x(INF), y(INF), z(INF), previous(nullptr), next(nullptr) {}
    CPoint(double _x, double _y, double _z, CPoint* _previous, CPoint* _next):
         x(_x), y(_y), z(_z), previous(_previous), next(_next) {}

    void Update() {
        //Вставка точки в список.
        if (previous->next != this) {
                previous->next = next->previous = this;
        }
        //Удаление точки из списка.
        else {
            previous->next = next;
            next->previous = previous;
        }
    }
 };

 static CPoint nill = CPoint();

 CPoint *nullPoint = &nill;

 inline double turn(const CPoint *p, const CPoint *q, const CPoint *r) {
    if (p == nullPoint || q == nullPoint || r == nullPoint) {return 1;}
    return (q->x - p->x) * (r->y - p->y) - (r->x - p->x) * (q->y - p->y);
 }

 inline double time(const CPoint *p, const CPoint *q, const CPoint *r) {
    if (p == nullPoint || q == nullPoint || r == nullPoint) {return INF;}
    return ((q->x - p->x) * (r->z - p->z) - (r->x - p->x) * (q->z - p->z)) / turn(p, q, r);
}

 //Сортировка слиянием.
void MergeSort(const vector<CPoint>& points, vector<int>& sort_points, const int begin, const int end) {
    int n = end - begin + 1;
    if (n == 1) {
        sort_points.push_back(begin);
        return;
    }
    vector<int> sort_points_first, sort_points_second;
    MergeSort(points, sort_points_first, begin, (begin + end) / 2);
    MergeSort(points, sort_points_second, (begin + end) / 2 + 1, end);
    int i = 0, j = 0;
    while (i < sort_points_first.size() && j < sort_points_second.size()) {
        if (points[sort_points_first[i]].x < points[sort_points_second[j]].x) {
            sort_points.push_back(sort_points_first[i]);
            i++;
        }
        else {
            sort_points.push_back(sort_points_second[j]);
            j++;
        }
    }
    while (i < sort_points_first.size()) {
        sort_points.push_back(sort_points_first[i]);
        i++;
    }
    while (j < sort_points_second.size()) {
        sort_points.push_back(sort_points_second[j]);
        j++;
    }
    return;
 }

void sort(vector<CPoint>& points)
{
    int n = points.size();
    vector<int> sort_nums;
    MergeSort(points, sort_nums, 0, n - 1);
    vector<CPoint> sorted_points(n);
    for (int i = 0; i < sort_nums.size(); ++i) {
        sorted_points[i] = points[sort_nums[i]];
    }
    points.swap(sorted_points);
    for (int i = 0; i < points.size() - 1; ++i) {
        points[i].next = &points[i + 1];
    }
    points[points.size() - 1].next = nullPoint;
}

//Слияние через мост
void Merge(vector<CPoint*>& A, const vector<CPoint*>& B, const int n, CPoint *u, CPoint *v, const CPoint *mid)
{
    array<double, 6> t;
    double oldt, newt;
    int i, j, k, l, minl;
    for (i = k = 0, j = n / 2 * 2, oldt = -INF; ; oldt = newt) {
        //Ищем время свамого раннего события каждого типа.
        t[0] = time(B[i]->previous, B[i], B[i]->next);
        t[1] = time(B[j]->previous, B[j], B[j]->next);
        t[2] = time(u, u->next, v);
        t[3] = time(u->previous, u, v);
        t[4] = time(u, v->previous, v);
        t[5] = time(u, v, v->next);
        //Ищем самое раннее событие.
        for (newt = INF, l = 0; l < 6; l++) {
            if (t[l] > oldt && t[l] < newt) {
                minl = l;
                newt = t[l];
            }
        }
        if (newt == INF) {
            break;
        }
        switch (minl) {
        case 0: if (B[i]->x < u->x) A[k++] = B[i]; B[i++]->Update(); break;
        case 1: if (B[j]->x > v->x) A[k++] = B[j]; B[j++]->Update(); break;
        case 2: A[k++] = u = u->next; break;
        case 3: A[k++] = u; u = u->previous; break;
        case 4: A[k++] = v = v->previous; break;
        case 5: A[k++] = v; v = v->next; break;
        }
    }
    A[k] = nullPoint;
    u->next = v;
    v->previous = u;
    for (k--; k >= 0; k--) {
        if (A[k]->x <= u->x || A[k]->x >= v->x) {
            A[k]->Update();
            if (A[k] == u) {
                u = u->previous;
            } else if (A[k] == v) {
                v = v->next;
            }
        } else {
            u->next = A[k];
            A[k]->previous = u;
            v->previous = A[k];
            A[k]->next = v;
            if (A[k]->x < mid->x) {
                u = A[k];
            } else {
                v = A[k];
            }
        }
    }
}

void Hull(CPoint *list, vector<CPoint*>& A, const int n) {
    CPoint *u, *v, *mid;
    if (n == 1) {
        A[0] = list->previous = list->next = nullPoint;
        return;
    }
    int i;
    for (u = list, i = 0; i < n / 2 - 1; u = u->next, i++);
    mid = v = u->next;
    //Рекурсия
    vector<CPoint*> B(n / 2 * 2);
    vector<CPoint*> C(n - n / 2);
    Hull(list, B, n / 2);
    Hull(mid, C, n - n / 2);
    for (int i = 0; i < n - n / 2; ++i) {
        B.insert(B.end(), C.begin(), C.end());
    }
    //Находим мост.
    for ( ; ; ) {
        if (turn(u, v, v->next) < 0) {v = v->next;}
        else if (turn(u->previous, u, v) < 0) {u = u->previous;}
        else break;
    }

    Merge(A, B, n, u, v, mid);
}

struct CVector
{
    double x, y;

    CVector(double x1, double y1, double x2, double y2): x(x2 - x1), y(y2 - y1){}
    CVector(const CPoint& first, const CPoint& second): x(second.x - first.x), y(second.y - first.y){}
};

double CrossProduct(const CVector& v1, const CVector& v2)
{
    return v1.x * v2.y - v1.y * v2.x;
}

void FindHighests(const vector<CPoint>& points, const vector<int>& hull_l, const vector<int>& hull_r, const int begin_l,
                  const int end_l, const int begin_r, const int end_r, int& highest_l, int& highest_r)
{
    //Используется, что никакие 3 точки не лежат на одной прямой, а, значит,
    //CrossProduct != 0, если только в нём не нулевые вектора.
    while (true) {
        int a = (highest_l + 1) % hull_l.size();//вспомогательная переменная
        if (CrossProduct(CVector(points[hull_l[highest_l]], points[hull_l[a]]),
                         CVector(points[hull_l[highest_l]], points[hull_r[highest_r]])) < 0) {
            highest_l = a;
            continue;
        }
        a = (highest_r - 1 + hull_r.size()) % hull_r.size();
        if (CrossProduct(CVector(points[hull_r[highest_r]], points[hull_r[a]]),
                         CVector(points[hull_r[highest_r]], points[hull_l[highest_l]])) > 0) {
            highest_r = a;
            continue;
        }
        break;
    }
}

void FindLowests(const vector<CPoint>& points, const vector<int>& hull_l, const vector<int>& hull_r, const int begin_l,
                 const int end_l, const int begin_r, const int end_r, int& lowest_l, int& lowest_r)
{
    //Используется, что никакие 3 точки не лежат на одной прямой, а, значит,
    //CrossProduct != 0, если только в нём не нулевые вектора.
    while (true) {
        int a = (lowest_l - 1 + hull_l.size()) % hull_l.size();//вспомогательная переменная
        if (CrossProduct(CVector(points[hull_l[lowest_l]], points[hull_l[a]]),
                         CVector(points[hull_l[lowest_l]], points[hull_r[lowest_r]])) > 0) {
            lowest_l = a;
            continue;
        }
        a = (lowest_r + 1) % hull_r.size();
        if (CrossProduct(CVector(points[hull_r[lowest_r]], points[hull_r[a]]),
                         CVector(points[hull_r[lowest_r]], points[hull_l[lowest_l]])) < 0) {
            lowest_r = a;
            continue;
        }
        break;
    }
}

void Merge2DHullsInitialization(const vector<int>& hull_l, const vector<int>& hull_r, int& highest_l,
                              int& highest_r, int& lowest_l, int& lowest_r, const int begin_l, const int end_r)
{
    int max_l = begin_l - 1, min_r = end_r + 1;
    for (int i = 0; i < hull_l.size(); ++i) {
        if (hull_l[i] > max_l) {
                max_l = hull_l[i];
            highest_l = i;
        }
    }
    for (int i = 0; i < hull_r.size(); ++i) {
        if (hull_r[i] < min_r) {
            min_r = hull_r[i];
            highest_r = i;
        }
    }
    lowest_l = highest_l, lowest_r = highest_r;
}

void Merge2DHulls(const vector<CPoint>& points, vector<int>& hull, const vector<int>& hull_l,
                  const vector<int>& hull_r, const int begin_l, const int end_r) {
    //Везде далее l - left, r - right.
    int end_l = (begin_l + end_r) / 2;
    int begin_r = end_l + 1;
    //Достраиваем оболочку.
    int highest_l, highest_r, lowest_l, lowest_r;
    Merge2DHullsInitialization(hull_l, hull_r, highest_l, highest_r, lowest_l, lowest_r, begin_l, end_r);
    FindHighests(points, hull_l, hull_r, begin_l, end_l, begin_r, end_r, highest_l, highest_r);
    FindLowests(points, hull_l, hull_r, begin_l, end_l, begin_r, end_r, lowest_l, lowest_r);
    for (int i = highest_l; i != lowest_l; i = (i + 1) % hull_l.size()) {
        hull.push_back(hull_l[i]);
    }
    hull.push_back(hull_l[lowest_l]);
    for (int i = lowest_r; i != highest_r; i = (i + 1) % hull_r.size()) {
        hull.push_back(hull_r[i]);
    }
    hull.push_back(hull_r[highest_r]);
}

void Find2DHull(const vector<CPoint>& points, vector<int>& hull, const int begin, const int end)
{
    switch (end - begin) {
    case 0:
        //Выпуклая оболочка состоит из одной точки.
        hull.push_back(begin);
        return;
    case 1:
        {
            //Выпуклая оболочка состоит из двух точек.
            hull.push_back(begin);
            hull.push_back(end);
            return;
        }
    default:
        {
            vector<int> hull_left, hull_right;
            Find2DHull(points, hull_left, begin, (end + begin) / 2);
            Find2DHull(points, hull_right, (end + begin) / 2 + 1, end);
            Merge2DHulls(points, hull, hull_left, hull_right, begin, end);
        }
    }
}

int main() {
    int n;
    vector<CPoint> points;
    double x, y;
    while (cin >> x) {
        cin >> y;
        points.push_back(CPoint(x, y, x * x + y * y, nullptr, nullptr));
    }
    sort(points);
    n = points.size();
    vector<int> hull;
    Find2DHull(points, hull, 0, points.size() - 1);
    vector<bool> is_in_hull(n, false);
    for (int i = 0; i < hull.size(); ++i) {
        is_in_hull[hull[i]] = true;
    }
    CPoint* list = &points[0];
    vector<CPoint*> A(2 * n);
    Hull(list, A, n);
    vector<int> a(n, 0);          //Вспомогательный вектор.
    for (int i = 0; A[i] != nullPoint;
        A[i++]->Update()) {
        a[A[i]->previous - &points[0]]++;
        a[A[i] - &points[0]]++;
        a[A[i]->next - &points[0]]++;
    }
    double b = 0;
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        if (!is_in_hull[i]) {
            sum += a[i];
            b++;
        }
    }
    if (b == 0) {cout << 0;}
    else {cout << sum / b;}
    return 0;
}
