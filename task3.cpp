// Задача 3 (варіант 6): обчислення виразу z
#include <iostream>
#include <cmath>
#include <cfloat>
#include <cstring>
using namespace std;

const double EPS = 1e-12;   // |знаменник| < EPS вважаємо нулем

bool good(double v) {
    if (isnan(v) || isinf(v)) return false;             // переповнення
    if (v != 0.0 && fabs(v) < DBL_MIN) return false;    // зникнення порядку
    return true;
}

// x = sqrt(a^2 (c+1) + d),  y = b - c sqrt(4 + e^d)
// z = x^2 - 1/y,        x > 0
// z = y^3 - x^4 + 1,    x <= 0
// Повертає false, якщо обчислити z неможливо
bool calcZ(double a, double b, double c, double d, double& x, double& y, double& z) {
    // --- x ---
    double a2 = a * a;               if (!good(a2)) return false;
    double arg = a2 * (c + 1.0);     if (!good(arg)) return false;
    arg = arg + d;                   if (!good(arg)) return false;
    if (arg < 0) return false;                        // корінь з від'ємного
    x = sqrt(arg);                   if (!good(x)) return false;
    // --- y ---
    double ed = exp(d);              if (!good(ed)) return false;
    double r = sqrt(4.0 + ed);       if (!good(r)) return false;
    double cr = c * r;               if (!good(cr)) return false;
    y = b - cr;                      if (!good(y)) return false;
    // --- z ---
    if (x > 0) {
        if (fabs(y) < EPS) return false;              // ділення на нуль
        double q = 1.0 / y;          if (!good(q)) return false;
        double x2 = x * x;           if (!good(x2)) return false;
        z = x2 - q;                  return good(z);
    } else {
        double y3 = y * y * y;       if (!good(y3)) return false;
        double x2 = x * x;           if (!good(x2)) return false;
        double x4 = x2 * x2;         if (!good(x4)) return false;
        z = y3 - x4 + 1.0;           return good(z);
    }
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
    double x, y, z;
    // гілка x > 0
    {
        double ex = sqrt(1.0 * 1.5 + 0.3);
        double ey = 2.0 - 0.5 * sqrt(4.0 + exp(0.3));
        double ez = ex * ex - 1.0 / ey;
        check(calcZ(1, 2, 0.5, 0.3, x, y, z) && near(x, ex) && near(y, ey) && near(z, ez) && x > 0,
              "гілка x > 0: a=1 b=2 c=0.5 d=0.3");
    }
    check(calcZ(2, 1, 1, 0.5, x, y, z) && x > 0, "гілка x > 0: a=2 b=1 c=1 d=0.5");
    // гілка x <= 0 (x = 0 при a = 0, d = 0)
    {
        double ey = 1.0 - 1.0 * sqrt(5.0);
        double ez = ey * ey * ey - 0.0 + 1.0;
        check(calcZ(0, 1, 1, 0, x, y, z) && x == 0 && near(y, ey) && near(z, ez),
              "гілка x <= 0: a=0 b=1 c=1 d=0");
    }
    // особливі ситуації
    check(!calcZ(1, 1, -2, 0, x, y, z), "корінь з від'ємного: a^2(c+1)+d < 0");
    check(!calcZ(1, sqrt(5.0), 1, 0, x, y, z), "ділення на нуль: y = 0 при x > 0");
    check(!calcZ(1, 1, 1, 1000, x, y, z), "переповнення: e^1000");
    check(!calcZ(1e200, 1, 1, 1, x, y, z), "переповнення: a^2");
    check(!calcZ(1, 1, 1, -740, x, y, z), "зникнення порядку: e^(-740)");
    cout << "Passed " << total - failed << " of " << total << " tests." << endl;
    return failed == 0 ? 0 : 1;
}

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "test") == 0) return runTests();
    double a, b, c, d, x, y, z;
    cout << "Task 3. Enter a b c d (Ctrl+Z / Ctrl+D to finish):" << endl;
    while (cin >> a >> b >> c >> d) {
        if (calcZ(a, b, c, d, x, y, z))
            cout << "x = " << x << ", y = " << y << ", z = " << z << endl;
        else
            cout << "It is impossible to calculate z." << endl;
    }
    return 0;
}
