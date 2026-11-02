// Задача 2 (варіант 6): чи має рівняння ax^2 + bx + c = 0 дійсні корені
#include <iostream>
#include <cmath>
#include <cfloat>
#include <cstring>
using namespace std;

const double EPS = 1e-12;     // |cos h| < EPS => tg(h) не визначений
const double EPS_D = 1e-8;    // |D| < EPS_D => один корінь

bool good(double v) {
    if (isnan(v) || isinf(v)) return false;             // переповнення
    if (v != 0.0 && fabs(v) < DBL_MIN) return false;    // зникнення порядку
    return true;
}

// a = e^h + sqrt(1 + h^2 sin^2(h+2) + h^4 sin^4(h+4))
// b = ln a - sqrt(4 + tg^2(h) + ln^2(a))
// c = a h ch(bh) + b h sh(ah)
// Повертає false, якщо обчислити коефіцієнти неможливо
bool coeffs(double h, double& a, double& b, double& c) {
    // --- коефіцієнт a ---
    double eh = exp(h);                 if (!good(eh)) return false;
    double h2 = h * h;                  if (!good(h2)) return false;
    double h4 = h2 * h2;                if (!good(h4)) return false;
    double s2 = sin(h + 2.0), s4 = sin(h + 4.0);
    double t1 = h2 * s2 * s2;           if (!good(t1)) return false;
    double t2 = h4 * (s4 * s4) * (s4 * s4); if (!good(t2)) return false;
    double r1 = sqrt(1.0 + t1 + t2);    if (!good(r1)) return false;
    a = eh + r1;                        if (!good(a)) return false;
    // --- коефіцієнт b ---
    double la = log(a);                 if (!good(la)) return false;
    double cs = cos(h);
    if (fabs(cs) < EPS) return false;                   // tg(h) не існує
    double tg = sin(h) / cs;            if (!good(tg)) return false;
    double in = 4.0 + tg * tg + la * la; if (!good(in)) return false;
    double r2 = sqrt(in);               if (!good(r2)) return false;
    b = la - r2;                        if (!good(b)) return false;
    // --- коефіцієнт c ---
    double ah = a * h;                  if (!good(ah)) return false;
    double bh = b * h;                  if (!good(bh)) return false;
    double chbh = cosh(bh);             if (!good(chbh)) return false;
    double shah = sinh(ah);             if (!good(shah)) return false;
    double p1 = ah * chbh;              if (!good(p1)) return false;
    double p2 = bh * shah;              if (!good(p2)) return false;
    c = p1 + p2;                        return good(c);
}

// Розв'язання ax^2+bx+c=0. Повертає кількість коренів (0,1,2) або -1, якщо D неможливо обчислити
int solve(double a, double b, double c, double& d, double& x1, double& x2) {
    double b2 = b * b;          if (!good(b2)) return -1;
    double ac = 4.0 * a * c;    if (!good(ac)) return -1;
    d = b2 - ac;                if (!good(d)) return -1;
    if (fabs(d) < EPS_D) { x1 = x2 = -b / (2.0 * a); return 1; }
    if (d < 0) return 0;
    double s = sqrt(d);
    x1 = (-b + s) / (2.0 * a);
    x2 = (-b - s) / (2.0 * a);
    return 2;
}

// ---------- Тести ----------
int total = 0, failed = 0;
void check(bool cond, const char* name) {
    total++;
    if (cond) cout << "[ OK ] " << name << endl;
    else { failed++; cout << "[FAIL] " << name << endl; }
}
bool near(double a, double b) { return fabs(a - b) <= 1e-9 * (1 + fabs(b)); }

const double HGOOD = 0.1;   // D > 0: два корені
const double HBAD = 0.3;    // D < 0: коренів немає

int runTests() {
    double a, b, c, d, x1, x2;
    // розв'язувач на відомих рівняннях
    check(solve(1, -3, 2, d, x1, x2) == 2 && near(x1, 2) && near(x2, 1), "x^2-3x+2: корені 2 і 1");
    check(solve(1, -2, 1, d, x1, x2) == 1 && near(x1, 1), "x^2-2x+1: один корінь 1");
    check(solve(1, 0, 1, d, x1, x2) == 0, "x^2+1: коренів немає");
    check(solve(1e200, 1e200, 1, d, x1, x2) == -1, "переповнення в b^2");
    // коефіцієнти та корені
    check(coeffs(HGOOD, a, b, c), "h = 0.1: коефіцієнти обчислено");
    check(solve(a, b, c, d, x1, x2) == 2, "h = 0.1: два корені");
    check(fabs(a * x1 * x1 + b * x1 + c) < 1e-8 * (1 + fabs(c)), "h = 0.1: x1 — корінь");
    check(fabs(a * x2 * x2 + b * x2 + c) < 1e-8 * (1 + fabs(c)), "h = 0.1: x2 — корінь");
    check(near(x1 + x2, -b / a) && near(x1 * x2, c / a), "h = 0.1: теорема Вієта");
    check(coeffs(HBAD, a, b, c) && solve(a, b, c, d, x1, x2) == 0, "h = 0.3: коренів немає");
    check(coeffs(0, a, b, c) && near(a, 2.0), "h = 0: a = 2");
    // особливі ситуації
    check(!coeffs(acos(-1.0) / 2, a, b, c), "h = pi/2: tg(h) не існує");
    check(!coeffs(800, a, b, c),  "h = 800: переповнення e^h");
    check(!coeffs(-740, a, b, c), "h = -740: зникнення порядку e^h");
    cout << "Passed " << total - failed << " of " << total << " tests." << endl;
    return failed == 0 ? 0 : 1;
}

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "test") == 0) return runTests();
    double h, a, b, c, d, x1, x2;
    cout << "Task 2. Enter real h (Ctrl+Z / Ctrl+D to finish):" << endl;
    while (cin >> h) {
        cout << "h = " << h << ": ";
        if (!coeffs(h, a, b, c)) { cout << "the problem is unsolvable." << endl; continue; }
        int n = solve(a, b, c, d, x1, x2);
        if (n < 0) { cout << "the problem is unsolvable." << endl; continue; }
        cout << "a = " << a << ", b = " << b << ", c = " << c << ", D = " << d << endl;
        if (n == 0) cout << "  No real roots." << endl;
        else if (n == 1) cout << "  One root: x = " << x1 << endl;
        else cout << "  Two roots: x1 = " << x1 << ", x2 = " << x2 << endl;
    }
    return 0;
}
