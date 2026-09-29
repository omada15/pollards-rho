#include <iostream>
#include <vector>
#include <random>

using uint128 = unsigned __int128;
using ull = unsigned long long;
using namespace std;

ull modularMultiply(ull a, ull b, ull mod) {
    unsigned __int128 res = (unsigned __int128)a * b;
    return (ull)(res % mod);
}

ull modularPower(ull base, ull exp, ull mod) {
    ull res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1) {
            res = modularMultiply(res, base, mod);
        }
        base = modularMultiply(base, base, mod);
        exp /= 2;
    }
    return res;
}

bool mrptS(ull d, ull n, ull a) {
    ull x = modularPower(a, d, n);

    if (x == 1 || x == n - 1) {
        return true;
    }

    while (d != n - 1) {
        x = modularMultiply(x, x, n);
        d *= 2;

        if (x == 1) return false;
        if (x == n - 1) return true;
    }

    return false; // Composite
}

bool mrpt(ull n, int iterations = 5) {
    if (n <= 1 || n == 4) return false;
    if (n <= 3) return true;
    if (n % 2 == 0) return false;

    ull d = n - 1;
    while (d % 2 == 0) {
        d /= 2;
    }
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<ull> dis(2, n - 2);

    for (int i = 0; i < iterations; i++) {
        ull a = dis(gen);
        if (!mrptS(d, n, a)) {
            return false;
        }
    }

    return true;
}

int main() {
    return 0;
}

ull f(int v, int c, int n) {
    return (ull)(((v * v) + c) % n);
}



ull rho(int n) {
    if (n % 2 == 0) {
        return 2;
    }
    int c = 1;

}