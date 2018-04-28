#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::vector;

struct CPoint
{
    double x;
    double y;

    CPoint(const double a, const double b): x(a), y(b) {}
};

double VectorProduct(const double x1, const double y1, const double x2, const double y2) {
    return x1 * y2 - x2 * y1;
}

void AmountOfMinkowski(const vector<CPoint>& A, const vector<CPoint>& B,
                       vector<CPoint>& sum, const int n, const int m, const int start1, const int start2)
{
    int i = start1, j = start2;
    bool first_A_step = true, first_B_step = true;
    do {
        sum.push_back(CPoint(A[i].x + B[j].x, A[i].y + B[j].y));
        double step = VectorProduct(A[(i + 1) % n].x - A[i].x, A[(i + 1) % n].y - A[i].y,
                                    B[(j + 1) % m].x - B[j].x, B[(j + 1) % m].y - B[j].y);
        if (step < 0 && (i != start1 || first_A_step)) {
            i = (i + 1) % n;
            first_A_step = false;
        } else if (step > 0 && (j != start2 || first_B_step)) {
            j = (j + 1) % m;
            first_B_step = false;
        } else {
            if (i != start1 || first_A_step) {
                i = (i + 1) % n;
                first_A_step = false;
            }
            if (j != start2 || first_B_step){
                j = (j + 1) % m;
                first_B_step = false;
            }
        }
    } while (i != start1 || j != start2);
}

void InputA(vector<CPoint>& A, int& n, int& start1)
{
    int miny = 80001, minx = 80001;
    cin >> n;
    for (int i = 0; i < n; ++i) {
        double x, y;
        cin >> x >> y;
        A.push_back(CPoint(x, y));
        if (y < miny || (y == miny && x < minx)) {
            miny = y;
            minx = x;
            start1 = A.size() - 1;
        }
    }
}

void InputB(vector<CPoint>& B, int& m, int& start2)
{
    int miny = 80001, minx = 80001;
    cin >> m;
    for (int i = 0; i < m; ++i) {
        double x, y;
        cin >> x >> y;
        //Reflect the polygon with respect to the origin.
        x = -x;
        y = -y;
        B.push_back(CPoint(x, y));
        if (y < miny || (y == miny && x < minx)) {
            miny = y;
            minx = x;
            start2 = B.size() - 1;
        }
    }
}

int main()
{
    int n, m;
    vector<CPoint> A, B;
    int start1, start2, miny = 80001, minx = 80001;
    InputA(A, n, start1);
    InputB(B, m, start2);
    vector<CPoint> sum;
    AmountOfMinkowski(A, B, sum, n, m, start1, start2);
    for (int i = 0; i < sum.size(); ++i) {
        if (VectorProduct(sum[i].x, sum[i].y,
                          sum[(i + 1) % sum.size()].x, sum[(i + 1) % sum.size()].y) > 0) {
            cout << "NO" << endl;
            return 0;
        }
    }
    cout << "YES" << endl;
    return 0;
}
