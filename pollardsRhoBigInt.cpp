#include <iostream>
#include <vector>
#include <random>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <array>
#include <string>
#include <iomanip>
#include <cstdint>

using namespace std;

const size_t bitLimit = 512;

class bigInt {
private:
    array<uint64_t, bitLimit> data; // Changed from bigInt to uint64_t array

    static string addStrings(string s1, const string& s2) {
        string res = "";
        int i = s1.size() - 1, j = s2.size() - 1, carry = 0;
        while (i >= 0 || j >= 0 || carry) {
            int sum = carry + (i >= 0 ? s1[i--] - '0' : 0) + (j >= 0 ? s2[j--] - '0' : 0);
            res.push_back(sum % 10 + '0');
            carry = sum / 10;
        }
        reverse(res.begin(), res.end());
        return res;
    }

public:
    bigInt(uint64_t low = 0) {
        data.fill(0);
        data[0] = low;
    }

    const array<uint64_t, bitLimit>& getData() const {
        return data;
    }

    string toDecimalString() const {
        string decimalStr = "0";
        string powerOfTwo = "1";

        for (size_t i = 0; i < bitLimit; ++i) {
            uint64_t limb = data[i];
            for (int bit = 0; bit < 64; ++bit) {
                if ((limb >> bit) & 1) {
                    decimalStr = addStrings(decimalStr, powerOfTwo);
                }
                powerOfTwo = addStrings(powerOfTwo, powerOfTwo);
            }
        }
        return decimalStr;
    }

    // Addition
    bigInt operator+(const bigInt& other) const { 
        bigInt result;
        unsigned __int128 carry = 0;
        for (size_t i = 0; i < bitLimit; ++i) {
            unsigned __int128 sum = (unsigned __int128)data[i] + other.data[i] + carry;
            result.data[i] = (uint64_t)sum;
            carry = sum >> 64;
        }
        return result;
    }

    // Subtraction
    bigInt operator-(const bigInt& other) const {
        bigInt result;
        unsigned __int128 borrow = 0;
        for (size_t i = 0; i < bitLimit; ++i) {
            unsigned __int128 a = data[i];
            unsigned __int128 b = (unsigned __int128)other.data[i] + borrow;
            if (a < b) {
                result.data[i] = (uint64_t)(a + ((unsigned __int128)1 << 64) - b);
                borrow = 1;
            } else {
                result.data[i] = (uint64_t)(a - b);
                borrow = 0;
            }
        }
        return result;
    }

    // Bitwise Shift Right
    bigInt operator>>(int shift) const {
        bigInt result;
        int word_shift = shift / 64;
        int bit_shift = shift % 64;
        for (int i = 0; i < (int)bitLimit; ++i) {
            if (i + word_shift < (int)bitLimit) {
                uint64_t val = data[i + word_shift] >> bit_shift;
                if (bit_shift > 0 && i + word_shift + 1 < (int)bitLimit) {
                    val |= (data[i + word_shift + 1] << (64 - bit_shift));
                }
                result.data[i] = val;
            }
        }
        return result;
    }

    // Bitwise Shift Left
    bigInt operator<<(int shift) const {
        bigInt result;
        int word_shift = shift / 64;
        int bit_shift = shift % 64;
        for (int i = (int)bitLimit - 1; i >= 0; --i) {
            if (i - word_shift >= 0) {
                uint64_t val = data[i - word_shift] << bit_shift;
                if (bit_shift > 0 && i - word_shift - 1 >= 0) {
                    val |= (data[i - word_shift - 1] >> (64 - bit_shift));
                }
                result.data[i] = val;
            }
        }
        return result;
    }

    // Multiplication
    bigInt operator*(const bigInt& other) const {
        bigInt result;
        for (size_t i = 0; i < bitLimit; ++i) {
            if (data[i] == 0) continue;
            unsigned __int128 carry = 0;
            for (size_t j = 0; i + j < bitLimit; ++j) {
                unsigned __int128 prod = (unsigned __int128)data[i] * other.data[j] + result.data[i + j] + carry;
                result.data[i + j] = (uint64_t)prod;
                carry = prod >> 64;
            }
        }
        return result;
    }

    // Division & Modulo helper
    pair<bigInt, bigInt> divmod(const bigInt& divisor) const {
        if (divisor == 0) throw runtime_error("Division by zero");
        bigInt q = 0, r = 0;
        for (int i = (int)bitLimit * 64 - 1; i >= 0; --i) {
            r = r << 1;
            if ((data[i / 64] >> (i % 64)) & 1) {
                r.data[0] |= 1;
            }
            if (r >= divisor) {
                r = r - divisor;
                q.data[i / 64] |= ((uint64_t)1 << (i % 64));
            }
        }
        return {q, r};
    }

    bigInt operator/(const bigInt& other) const { return divmod(other).first; }
    bigInt operator%(const bigInt& other) const { return divmod(other).second; }

    bigInt operator&(const bigInt& other) const {
        bigInt res;
        for (size_t i = 0; i < bitLimit; ++i) res.data[i] = data[i] & other.data[i];
        return res;
    }

    // Compound assignment operators
    bigInt& operator+=(const bigInt& o) { *this = *this + o; return *this; }
    bigInt& operator-=(const bigInt& o) { *this = *this - o; return *this; }
    bigInt& operator*=(const bigInt& o) { *this = *this * o; return *this; }
    bigInt& operator/=(const bigInt& o) { *this = *this / o; return *this; }
    bigInt& operator%=(const bigInt& o) { *this = *this % o; return *this; }

    // Comparisons
    bool operator==(const bigInt& other) const {
        return data == other.data;
    }

    bool operator<(const bigInt& other) const {
        for (int i = (int)bitLimit - 1; i >= 0; --i) {
            if (data[i] < other.data[i]) return true;
            if (data[i] > other.data[i]) return false;
        }
        return false;
    }

    bool operator>(const bigInt& other) const { return other < *this; }
    bool operator<=(const bigInt& other) const { return !(*this > other); }
    bool operator>=(const bigInt& other) const { return !(*this < other); } 
    bool operator!=(const bigInt& other) const { return !(*this == other); }

    int length(bool hexMode = false) const {
        if (hexMode) {
            for (int i = (int)bitLimit - 1; i >= 0; --i) {
                if (data[i] != 0) {
                    int count = i * 16; 
                    uint64_t temp = data[i];
                    while (temp > 0) {
                        temp >>= 4;
                        count++;
                    }
                    return count;
                }
            }
            return 1;
        } else {
            string s = this->toDecimalString();
            return static_cast<int>(s.length());
        }
    }

    void print(bool hexMode = true) const {
        if (hexMode) {
            bool leadingZeros = true;
            for (int i = (int)bitLimit - 1; i >= 0; --i) {
                if (leadingZeros && data[i] == 0 && i != 0) continue;
                if (leadingZeros) {
                    cout << hex << data[i];
                    leadingZeros = false;
                } else {
                    cout << setfill('0') << setw(16) << hex << data[i];
                }
            }
            cout << dec << endl;
        } else {
            cout << toDecimalString() << endl;
        }
    }
};

// Stream insertion operator for bigInt
ostream& operator<<(ostream& os, const bigInt& b) {
    return os << b.toDecimalString();
}

// Custom hash support for std::unordered_map
namespace std {
    template <>
    struct hash<bigInt> {
        size_t operator()(const bigInt& b) const {
            size_t h = 0;
            for (uint64_t x : b.getData()) {
                h ^= hash<uint64_t>{}(x) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            return h;
        }
    };
}

ostream &operator<<(ostream &os, __int128 n) {
	if (n == 0)
		return os << "0";
	if (n < 0) {
		os << "-";
		n = -n;
	}
	string s;
	while (n > 0) {
		s += (char)('0' + (n % 10));
		n /= 10;
	}
	reverse(s.begin(), s.end());
	return os << s;
}

bigInt modularMultiply(bigInt a, bigInt b, bigInt mod) {
    bigInt res = a * b;
    return res % mod;
}

bigInt modularPower(bigInt base, bigInt exp, bigInt mod) {
    bigInt res = 1;
    base %= mod;
    while (base > 0 && exp > 0) {
        if ((exp & 1) == 1) {
            res = modularMultiply(res, base, mod);
        }
        base = modularMultiply(base, base, mod);
        exp /= 2;
    }
    return res;
}

bool mrptS(bigInt d, bigInt n, bigInt a) {
    bigInt x = modularPower(a, d, n);

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

// Helper for generating random bigInt in range [low, high]
bigInt getRandomBigInt(bigInt low, bigInt high, std::mt19937_64& gen) {
    bigInt range = high - low + 1;
    bigInt r;
    r %= range;
    return low + r;
}

bool mrpt(bigInt n, int iterations = 5) {
    if (n <= 1 || n == 4) return false;
    if (n <= 3) return true;
    if ((n & 1) == 0) return false;

    bigInt d = n - 1;
    while ((d & 1) == 0) {
        d /= 2;
    }
    std::random_device rd;
    std::mt19937_64 gen(rd());

    for (int i = 0; i < iterations; i++) {
        bigInt a = getRandomBigInt(2, n - 2, gen);
        if (!mrptS(d, n, a)) {
            return false;
        }
    }

    return true;
}

bool vfind(vector<bigInt> n, bigInt target) {
    return (find(n.begin(), n.end(), target) != n.end());
}

bigInt f(bigInt v, bigInt n, int c = 1) {
    return ((v * v) + c) % n;
}

bigInt bigIntGcd(bigInt a, bigInt b) {
    while (b != 0) {
        bigInt t = b;
        b = a % b;
        a = t;
    }
    return a;
}

bigInt rho(bigInt n, bigInt a, bigInt b) {
    if ((n & 1) == 0) {
        return 2;
    }
    int c = 1;
    bigInt x = a; bigInt y = b; bigInt gd;
    while (true) {
        x = f(x, n, c); y = f(f(y, n, c), n, c);
        gd = bigIntGcd(x > y ? x - y : y - x, n);
        if (gd > 1 && gd != n) {
            return gd;
        }
    }
    return 0;
}

int main() {
    unordered_map<bigInt, int> solution;
    vector<bigInt> seen;
    bigInt n = 12; bigInt ret; bigInt two = 2; bigInt three = 3;
    while (!mrpt(n) && n > 1) {
        ret = rho(n, 2, 3);
        cout << "current N " << n << endl;
        n = n / ret;
        cout << "new n "  << n << " prime? " << mrpt(n) << endl;
        if (vfind(seen, ret)) {
            solution[ret] += 1;
        } else {
            solution[ret] = 1;
            seen.push_back(ret);
        }
    }
    if (vfind(seen, n)) {
        solution[n] += 1;
    } else {
        solution[n] = 1;
        seen.push_back(n);
    }
    sort(seen.begin(), seen.end());
    for (size_t i = 0; i < seen.size(); i++) {
        cout << seen[i] << "^" << solution[seen[i]] << ", ";
    }
    
    return 0;
}
