//
// Created by gosktin on 31.10.16.
//

#ifndef RATIONAL_H
#define RATIONAL_H

#include <cmath>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

template <typename T>
T absolute(const T p) {
    return p >= 0 ? p : -p;
}

template <typename T>
inline T max(T left, T right) {
    return left > right ? left : right;
}

inline double tpow(double object, int degree) {
    if (degree < 0)
        object = 1 / object;

    degree = absolute(degree);

    if (degree == 0)
        return 1;
    if (degree == 1)
        return object;
    if (degree == 2)
        return object * object;

    double s = object;
    for (int i = 0; i < degree; ++i)
        s *= object;

    return s;
}

class BigInteger {
public:
    BigInteger();
    BigInteger(int n);
    BigInteger(const BigInteger &object);
    BigInteger(const std::string &s);

    ~BigInteger();

    BigInteger &operator=(const BigInteger &right);
    BigInteger &operator+=(const BigInteger &right);
    BigInteger &operator-=(const BigInteger &right);
    BigInteger &operator*=(const BigInteger &right);
    BigInteger &operator/=(const BigInteger &rig);
    BigInteger &operator%=(const BigInteger &right);

    BigInteger operator-() const;
    BigInteger &operator++();
    BigInteger operator++(int);
    BigInteger &operator--();
    BigInteger operator--(int);

    BigInteger operator+(const BigInteger &right) const;
    BigInteger operator-(const BigInteger &right) const;
    BigInteger operator*(const BigInteger &right) const;
    BigInteger operator/(const BigInteger &right) const;
    BigInteger operator%(const BigInteger &right) const;

    bool operator==(const BigInteger &right) const;
    bool operator!=(const BigInteger &right) const;
    bool operator<(const BigInteger &right) const;
    bool operator>(const BigInteger &right) const;
    bool operator<=(const BigInteger &right) const;
    bool operator>=(const BigInteger &right) const;

    bool isNegative() const;
    bool isPositive() const;

    size_t size() const { return number_.size(); }

    BigInteger abs() const;

    explicit operator bool() const;

    std::string toString() const;

    std::vector <int> raw() const {
        return number_;
    }

    void fill(size_t n);

private:
    bool positive_;
    std::vector <int> number_;

    void copy(const BigInteger &object);
    void canonify();
};

void BigInteger::fill(size_t n) {
    std::reverse(number_.begin(), number_.end());
    size_t i = 0;
    while (i < n) {
        number_.push_back(0);
        ++i;
    }
    std::reverse(number_.begin(), number_.end());
}

BigInteger::BigInteger() : positive_(true) {}

BigInteger::BigInteger(int n) : positive_(n >= 0) {
    if (n == 0)
        number_.push_back(0);
    n = absolute(n);
    for (int i = n; i > 0; i /= 10)
        number_.push_back(i % 10);
}

BigInteger::BigInteger(const BigInteger &object) {
    copy(object);
}

BigInteger::BigInteger(const std::string &s) {
    number_.clear();

    if (s.length() == 2 && s[0] == '-' && s[1] == '0') {
        number_.push_back(0);
        positive_ = true;
        return;
    }

    for (long long i = static_cast<long long>(s.length() - 1); i > 0; --i) {
        number_.push_back(s[i] - 48);
    }
    if (s[0] == '-')
        positive_ = false;
    else {
        number_.push_back(s[0] - 48);
        positive_ = true;
    }
}

BigInteger::~BigInteger() {
    number_.clear();
}

BigInteger &BigInteger::operator=(const BigInteger &right) {
    if (this == &right)
        return *this;

    copy(right);

    return *this;
}

void BigInteger::copy(const BigInteger &object) {
    positive_ = object.positive_;

    number_.clear();
    number_.reserve(object.number_.size());

    for (std::vector <int>::const_iterator iter = object.number_.begin(); iter != object.number_.end(); ++iter)
        number_.push_back(*iter);
}

void BigInteger::canonify() {
    for (long long i = static_cast<long long>(number_.size() - 1); i >= 0; --i)
        if (number_[i] == 0)
            number_.pop_back();
        else
            break;

    if (size() == 0)
        number_.push_back(0);
    if (size() == 1 && number_[0] == 0)
        positive_ = true;
}

BigInteger &BigInteger::operator+=(const BigInteger &right) {
    if (&right == this) {
        BigInteger temp(*this);
        operator+=(temp);

        return *this;
    }

    bool add = false;
    if (positive_ == right.positive_) {
        for (size_t i = 0; i < max(right.number_.size(), number_.size()); ++i) {
            if (number_.size() <= i)
                number_.push_back(0);
            if (add) {
                ++number_[i];
                add = false;
            }
            if (right.number_.size() > i)
                number_[i] += right.number_[i];
            if (number_[i] > 9) {
                add = true;
                number_[i] %= 10;
            }
        }
        if (add)
            number_.push_back(1);
    } else {
        bool minus = false;
        if (abs() > right.abs()) {
            for (size_t i = 0; i < number_.size(); ++i) {
                if (minus) {
                    --number_[i];
                    minus = false;
                }
                if (i >= right.number_.size()) {
                    if (number_[i] < 0) {
                        minus = true;
                        number_[i] += 10;
                    }
                } else {
                    number_[i] -= right.number_[i];
                    if (number_[i] < 0) {
                        minus = true;
                        number_[i] += 10;
                    }
                }
            }
        } else if (abs() == right.abs()) {
            number_.clear();
            number_.push_back(0);
            positive_ = true;
        } else {
            operator=(right.abs() - abs());
            positive_ = right.positive_;
        }
    }

    canonify();

    return *this;
}

BigInteger &BigInteger::operator-=(const BigInteger &right) {
    return operator+=(-right);
}

BigInteger &BigInteger::operator*=(const BigInteger &right) {
    bool old_positive = positive_;

    if (right.number_.size() == 2 && right.number_[0] == 0 && right.number_[1] == 1) {
        int temp = number_[0], t;
        number_[0] = 0;
        for (size_t i = 1; i < number_.size(); ++i) {
            t = number_[i];
            number_[i] = temp;
            temp = t;
        }
        number_.push_back(temp);
    } else {
        int plus = 0;
        BigInteger *big = new BigInteger;
        for (long long i = static_cast<long long>(right.number_.size() - 1); i >= 0; --i) {
            BigInteger temp;
            temp.positive_ = true;
            if (*big)
                *big *= 10;
            else
                *big = 0;

            for (size_t j = 0; j < number_.size(); ++j) {
                if (plus) {
                    temp.number_.push_back(plus);
                    plus = 0;
                } else
                    temp.number_.push_back(0);
                temp.number_[temp.number_.size() - 1] += number_[j] * right.number_[i];
                if (temp.number_[temp.number_.size() - 1] / 10) {
                    plus = temp.number_[temp.number_.size() - 1] / 10;
                    temp.number_[temp.number_.size() - 1] %= 10;
                }
            }
            if (plus)
                temp.number_.push_back(plus);
            *big = *big + temp;
            plus = 0;
        }
        operator=(*big);
        delete big;
    }

    canonify();

    if (old_positive != right.positive_)
        positive_ = false;

    return *this;
}

BigInteger &BigInteger::operator/=(const BigInteger &rig) {
    BigInteger temp1, right(rig.abs());
    if (!operator bool())
        return operator=(0);
    if (abs() < right)
        return operator=(0);
    positive_ = positive_ == rig.positive_;

    temp1.number_.clear();
    std::vector <int> ans;
    for (long long i = static_cast<long>(number_.size() - 1); i >= 0; --i) {
        temp1.positive_ = true;
        if (temp1.number_.size() == 0) {
            temp1.number_.push_back(0);
        } else
            temp1 *= 10;
        if (number_[i] != 0)
            temp1 = temp1 + BigInteger(number_[i]);
        if (temp1 < right) {
            if (ans.size() > 0)
                ans.push_back(0);
            continue;
        }
        ans.push_back(0);
        while (temp1 > right) {
            temp1 -= right;
            ++ans[ans.size() - 1];
        }
        if (temp1 == right) {
            temp1 = 0;
            ++ans[ans.size() - 1];
        }

        for (long long j = static_cast<long long>(temp1.number_.size() - 1); j >= 0; --j)
            if (temp1.number_[j] == 0)
                temp1.number_.pop_back();
            else
                break;
    }
    for (long long j = 0; j < static_cast<long long>(ans.size()); --j)
        if (ans[j] == 0)
            ans.pop_back();
        else
            break;

    number_.clear();
    for (std::vector <int>::reverse_iterator it = ans.rbegin(); it != ans.rend(); ++it)
        number_.push_back(*it);

    canonify();

    return *this;
}

BigInteger &BigInteger::operator%=(const BigInteger &right) {
    if (size() == 0 || (size() == 1 && number_[0] == 0))
        return operator=(0);

    bool old_positive = positive_;

    BigInteger temp1 = abs(), temp2 = right.abs();
    operator=(temp1 - (temp1 / temp2) * temp2);

    if (!old_positive)
        positive_ = false;

    canonify();

    return *this;
}

bool BigInteger::isPositive() const {
    return positive_;
}

bool BigInteger::isNegative() const {
    return !positive_;
}

BigInteger BigInteger::abs() const {
    BigInteger temp(*this);
    temp.positive_ = true;

    return temp;
}

std::string BigInteger::toString() const {
    std::string s;
    if (number_.size() == 0) {
        s = "0";
        return s;
    }
    if (number_.size() == 1 && number_[0] == 0) {
        s = "0";
        return s;
    }

    if (!positive_)
        s += '-';
    for (long long i = static_cast<long long>(number_.size() - 1); i >= 0; --i)
        s += std::to_string(number_[i]);

    return s;
}

BigInteger::operator bool() const {
    return !((number_.size() == 1 && number_.at(0) == 0) || number_.size() == 0);
}

BigInteger BigInteger::operator-() const {
    BigInteger temp(*this);
    temp.positive_ = !temp.positive_;

    if (temp.size() == 0 || (temp.size() == 1 && temp.number_[0] == 0))
        temp.positive_ = true;

    return temp;
}

BigInteger &BigInteger::operator--() {
    return operator-=(1);
}

BigInteger BigInteger::operator--(int) {
    BigInteger temp(*this);
    operator-=(1);

    return temp;
}


BigInteger &BigInteger::operator++() {
    return operator+=(1);
}

BigInteger BigInteger::operator++(int) {
    BigInteger temp(*this);
    operator+=(1);

    return temp;
}

bool BigInteger::operator==(const BigInteger &right) const {
    if (right.number_.size() != number_.size() || positive_ != right.positive_)
        return false;

    for (size_t i = 0; i < number_.size(); ++i)
        if (number_[i] != right.number_[i])
            return false;

    return true;
}

bool BigInteger::operator!=(const BigInteger &right) const {
    return !(operator==(right));
}

bool BigInteger::operator<(const BigInteger &right) const {
    if ((isNegative() && right.isPositive()) || (isNegative() && number_.size() > right.number_.size()) ||
        (isPositive() && number_.size() < right.number_.size()))
        return true;
    if ((isPositive() && right.isNegative()) || (isNegative() && number_.size() < right.number_.size()) ||
        (isPositive() && number_.size() > right.number_.size()))
        return false;
    for (long long i = static_cast<long long>(number_.size() - 1); i >= 0; --i) {
        if ((isPositive() && number_[i] > right.number_[i]) || (isNegative() && number_[i] < right.number_[i]))
            return false;
        if ((isPositive() && number_[i] < right.number_[i]) || (isNegative() && number_[i] > right.number_[i]))
            return true;
    }
    return false;
}

bool BigInteger::operator>(const BigInteger &right) const {
    return (right < *this);
}

bool BigInteger::operator<=(const BigInteger &right) const {
    return !(operator>(right));
}

bool BigInteger::operator>=(const BigInteger &right) const {
    return !(operator<(right));
}

BigInteger BigInteger::operator+(const BigInteger &right) const {
    BigInteger temp(*this);

    return temp += right;
}

BigInteger BigInteger::operator-(const BigInteger &right) const {
    BigInteger temp(*this);

    return temp -= right;
}

BigInteger BigInteger::operator%(const BigInteger &right) const {
    BigInteger temp(*this);

    return temp %= right;
}

BigInteger BigInteger::operator*(const BigInteger &right) const {
    BigInteger temp(*this);

    return temp *= right;
}

BigInteger BigInteger::operator/(const BigInteger &right) const {
    BigInteger temp(*this);

    return temp /= right;
}

std::ostream &operator<<(std::ostream &out, const BigInteger &object) {
    out << object.toString();
    return out;
}

std::istream &operator>>(std::istream &in, BigInteger &object) {
    std::string s;
    in >> s;
    object = BigInteger(s);

    return in;
}

BigInteger operator+(int n, BigInteger big) {
    return big + BigInteger(n);
}

BigInteger operator-(int n, BigInteger big) {
    return BigInteger(n) - big;
}

BigInteger operator*(int n, BigInteger big) {
    return big * BigInteger(n);
}

BigInteger operator/(int n, BigInteger big) {
    return BigInteger(n) / big;
}

class Rational {
private:
    BigInteger numerator_;
    BigInteger denominator_;

    BigInteger gcd(const BigInteger &a, const BigInteger &b) const {
        return b ? gcd (b, a % b) : a;
    }

public:
    Rational();
    Rational(int old);
    Rational(BigInteger big);

    ~Rational();

    Rational operator-() const;

    Rational& operator=(const Rational right);
    Rational& operator+=(const Rational right);
    Rational& operator*=(const Rational right);
    Rational& operator/=(const Rational right);
    Rational& operator-=(const Rational right);

    bool operator<(const Rational &right) const;
    bool operator>(const Rational &right) const;
    bool operator==(const Rational &right) const;
    bool operator!=(const Rational &right) const;
    bool operator<=(const Rational &right) const;
    bool operator>=(const Rational &right) const;

    std::string toString() const;

    std::string asDecimal(size_t precision = 0) const;

    explicit operator double() const;

    std::pair<BigInteger, BigInteger> p() const {
        return std::make_pair(numerator_, denominator_);
    };
};

Rational::Rational() {
    numerator_ = 0;
    denominator_ = 1;
}

Rational::Rational(int old) {
    numerator_ = old;
    denominator_ = 1;
}

Rational::Rational(BigInteger big) {
    numerator_ = big;
    denominator_ = 1;
}

Rational::~Rational() {}

Rational Rational::operator-() const {
    Rational temp(*this);
    temp.numerator_ = -temp.numerator_;
    return temp;
}

Rational::operator double() const {
    BigInteger big(numerator_);
    big.fill(2);
    big /= denominator_;
    std::vector<int> numb(big.raw());
    size_t sz = numb.size() - 2;
    double d = 0;
    double c = tpow(10, static_cast<int>(sz - 1));
    for (long long i = static_cast<long long>(numb.size()) - 1; i >= 0; --i) {
        d += numb[i] * c;
        c /= 10;
    }
    if (numerator_ < 0)
        d = -d;

    return d;
}

std::string Rational::toString() const {
    std::string s;

    s += numerator_.toString();
    if (denominator_ != 1)
        s += '/' + denominator_.toString();

    return s;
}

Rational& Rational::operator+=(const Rational right) {
    BigInteger t(right.numerator_);
    numerator_ *= right.denominator_;
    t *= denominator_;
    denominator_ *= right.denominator_;

    numerator_ += t;

    BigInteger g(gcd(numerator_, denominator_));

    numerator_ /= g;
    denominator_ /= g;

    return *this;
}

Rational& Rational::operator-=(const Rational right) {
    return *this += -right;
}

bool Rational::operator<(const Rational &right) const {
    return (numerator_ * right.denominator_ < right.numerator_ * denominator_);
}

bool Rational::operator>(const Rational &right) const {
    return right < *this;
}

bool Rational::operator<=(const Rational &right) const {
    return !(*this > right);
}

bool Rational::operator>=(const Rational &right) const {
    return !(*this < right);
}

bool Rational::operator==(const Rational &right) const {
    return (*this <= right && *this >= right);
}

bool Rational::operator!=(const Rational &right) const {
    return !(*this == right);
}

Rational& Rational::operator*=(const Rational right) {
    numerator_ *= right.numerator_;
    denominator_ *= right.denominator_;
    BigInteger g(gcd(numerator_, denominator_));
    numerator_ /= g;
    denominator_ /= g;

    return *this;
}

Rational& Rational::operator/=(const Rational right) {
    numerator_ *= right.denominator_;
    denominator_ *= right.numerator_;

    BigInteger g(gcd(numerator_, denominator_));
    numerator_ /= g;
    denominator_ /= g;

    if (denominator_ < 0) {
        denominator_ = -denominator_;
        numerator_ = -numerator_;
    }

    return *this;
}

Rational operator+(const Rational &left, const Rational &right) {
    Rational temp(left);

    return temp += right;
}

Rational operator-(const Rational &left, const Rational &right) {
    Rational temp(left);

    return temp -= right;
}

Rational operator/(const Rational &left, const Rational &right) {
    Rational temp(left);

    return temp /= right;
}

Rational operator*(const Rational &left, const Rational &right) {
    Rational temp(left);

    return temp *= right;
}

Rational& Rational::operator=(const Rational right) {
    numerator_ = right.numerator_;
    denominator_ = right.denominator_;

    return *this;
}

std::string Rational::asDecimal(size_t precision) const {
    BigInteger big(numerator_);
    size_t sz = (numerator_/denominator_).raw().size();
    big.fill(precision);

    big /= denominator_;

    std::vector<int> numb(big.raw());
    std::string s;
    if (numerator_ < 0)
        s += '-';
    bool mayAdd = true;
    if (numb.size() <= precision) {
        mayAdd = false;
        s += "0.";
        while (precision > numb.size()) {
            --precision;
            s += '0';
        }
    }
    for (int i = static_cast<int>(numb.size()) - 1; i >= 0; --i) {
        s += std::to_string(numb[i]);
        if (mayAdd && numb.size() - i == sz && i != 0)
            s += '.';
    }

    return s;
}



#endif //NUMERICAL_RATIONAL_H
