// Задача 1 (варіант 6): належність точки області D та обчислення u
#include <iostream>
#include <cmath>
#include <cfloat>
#include <cstring>
using namespace std;

const double EPS = 1e-12;   // якщо |знаменник| < EPS, вважаємо його нулем

// Перевірка результату: false при переповненні (inf/nan) або зникненні порядку
bool good(double v) {
    if (isnan(v) || isinf(v)) return false;             // переповнення
    if (v != 0.0 && fabs(v) < DBL_MIN) return false;    // зникнення порядку
    return true;
}

// Належність точки (x,y) області D: хоча б одна з умов
bool inD(double x, double y) {
    return ((x + 1) * (x + 1) + (y - 1) * (y - 1) <= 1)            // круг, центр (-1;1), r=1
        || ((x - 3) * (x - 3) / 4 + (y - 1) * (y - 1) <= 1)        // еліпс, центр (3;1), піввісі 2 і 1
        || ((x - 2) * (x - 2) + (y + 1) * (y + 1) <= 1)            // круг, центр (2;-1), r=1
        || (x >= -3 && x <= -2 && y >= 0 && y <= 1)                // квадрат
        || (x >= 5 && x <= 6 && y >= 1 && y <= 2)                  // квадрат
        || (x >= 4 && x <= 5 && y >= -1 && y <= 0)                 // квадрат
        || (x >= -3 && x <= -1 && y >= -2 && y <= -1)              // прямокутник
        || (x >= 0 && x <= 1 && y >= -2 && y <= -1)                // квадрат
        || (x >= 3 && x <= 4 && y >= -2 && y <= -1)                // квадрат
        || (x >= 5 && x <= 6 && y >= -2 && y <= -1);               // квадрат
}

// u = sh(y^2 - cos x),                      (x,y) належить D
// u = tg(sin(y+x) + 10) - sqrt(ch^2(x+1)),  (x,y) не належить D
// Повертає false, якщо обчислити u неможливо
bool calcU(double x, double y, double& u) {
    if (inD(x, y)) {
        double y2 = y * y;            if (!good(y2)) return false;
        double t = y2 - cos(x);       if (!good(t)) return false;
        u = sinh(t);                  return good(u);
    }
    double s = sin(y + x) + 10.0;     if (!good(s)) return false;
    double cs = cos(s);
    if (fabs(cs) < EPS) return false;                 // ділення на нуль у tg
    double tg = sin(s) / cs;          if (!good(tg)) return false;
    double ch = cosh(x + 1.0);        if (!good(ch)) return false;
    double ch2 = ch * ch;             if (!good(ch2)) return false;
    if (ch2 < 0) return false;                        // корінь з від'ємного
    double r = sqrt(ch2);             if (!good(r)) return false;
    u = tg - r;                       return good(u);
}

// ---------- Тести ----------
int total = 0, failed = 0;
void check(bool cond, const char* name) {
    total++;
    if (cond) cout << "[ OK ] " << name << endl;
    else { failed++; cout << "[FAIL] " << name << endl; }
}
bool near(double a, double b) { return fabs(a - b) <= 1e-9 * (1 + fabs(b)); }

int runTests() {
    double u;
    // належність області D (по одній точці в кожній фігурі)
    check(inD(-1, 1),      "D: центр лівого круга");
    check(inD(3, 1),       "D: центр еліпса");
    check(inD(2, -1),      "D: центр нижнього круга");
    check(inD(-2.5, 0.5),  "D: квадрат [-3;-2]x[0;1]");
    check(inD(5.5, 1.5),   "D: квадрат [5;6]x[1;2]");
    check(inD(4.5, -0.5),  "D: квадрат [4;5]x[-1;0]");
    check(inD(-2, -1.5),   "D: прямокутник [-3;-1]x[-2;-1]");
    check(inD(0.5, -1.5),  "D: квадрат [0;1]x[-2;-1]");
    check(inD(3.5, -1.5),  "D: квадрат [3;4]x[-2;-1]");
    check(inD(5.5, -1.5),  "D: квадрат [5;6]x[-2;-1]");
    // точки поза областю
    check(!inD(0, 0),      "не D: початок координат");
    check(!inD(-2.5, -0.5),"не D: (-2.5; -0.5)");
    check(!inD(6.5, 0),    "не D: (6.5; 0)");
    check(!inD(2, -3),     "не D: (2; -3)");
    // значення u всередині D
    check(calcU(-1, 1, u) && near(u, sinh(1 - cos(-1.0))), "u у D: (-1; 1)");
    check(calcU(3, 1, u) && near(u, sinh(1 - cos(3.0))),   "u у D: (3; 1)");
    // значення u поза D
    check(calcU(0, 0, u) && near(u, tan(10.0) - cosh(1.0)), "u поза D: (0; 0)");
    check(calcU(7, 2, u) && near(u, tan(sin(9.0) + 10) - cosh(8.0)), "u поза D: (7; 2)");
    // особливі ситуації
    double yz = asin(3.5 * acos(-1.0) - 10.0);         // cos(sin(y)+10) = 0
    check(!inD(0, yz) && !calcU(0, yz, u), "ділення на нуль: cos(sin(y)+10) = 0");
    check(!calcU(400, 0, u),  "переповнення: ch^2(401)");
    check(!calcU(-400, 0, u), "переповнення: ch^2(-399)");
    check(inD(3, 1e-160) && !calcU(3, 1e-160, u), "зникнення порядку: y^2 у D");
    cout << "Passed " << total - failed << " of " << total << " tests." << endl;
    return failed == 0 ? 0 : 1;
}

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "test") == 0) return runTests();
    double x, y, u;
    cout << "Task 1. Enter x and y (Ctrl+Z / Ctrl+D to finish):" << endl;
    while (cin >> x >> y) {
        cout << (inD(x, y) ? "YES:" : "NO:") << " x = " << x << ", y = " << y;
        if (calcU(x, y, u)) cout << ", u = " << u << endl;
        else cout << ". It is impossible to calculate u." << endl;
    }
    return 0;
}
