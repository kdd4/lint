#ifndef LINT_HPP_INCLUDED
#define LINT_HPP_INCLUDED

#include <iostream>
#include <iomanip>

typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;

struct lint
{
private:
    static void _fill(size_t begin, size_t end, uint32_t* arr, uint32_t val);
    static void _copy(size_t begin, size_t end, uint32_t* arrl, uint32_t* arrr);
    template<typename T> static T _min(T a, T b);
    template<typename T> static T _max(T a, T b);

    void _resize(size_t valSize, bool optimized = true);
    void _lsh();
    void _rsh();
    void _lshb();
    void _rshb();

    uint32_t* _lvalue;
    size_t _size;
    size_t _capable;

public:
    lint();
    lint(uint64_t val);
    lint(const lint& right);
    lint(uint32_t* right, size_t size);
    virtual ~lint();

    lint& operator+=(const lint& right);
    lint& operator-=(const lint& right);
    lint& operator*=(const lint& right);
    lint& operator/=(const lint& right);
    lint& operator%=(const lint& right);
    lint& operator&=(const lint& right);
    lint& operator|=(const lint& right);
    lint& operator^=(const lint& right);
    lint& operator<<=(const lint& right);
    lint& operator>>=(const lint& right);

    lint& operator=(const lint& right);
    lint operator+(const lint& right) const;
    lint operator-(const lint& right) const;
    lint operator-() const;
    lint operator*(const lint& right) const;
    lint operator/(const lint& right) const;
    lint operator%(const lint& right) const;
    lint& operator++();
    lint& operator--();
    lint operator++(int);
    lint operator--(int);

    bool operator==(const lint& right) const;
    bool operator!=(const lint& right) const;
    bool operator>(const lint& right) const;
    bool operator<(const lint& right) const;
    bool operator>=(const lint& right) const;
    bool operator<=(const lint& right) const;

    lint operator~() const;
    lint operator<<(const lint& right) const;
    lint operator>>(const lint& right) const;
    lint operator&(const lint& right) const;
    lint operator|(const lint& right) const;
    lint operator^(const lint& right) const;

    void compress();

    class random;
    class exceptionDivByZero;

    friend lint bin_pow(const lint& x, const lint& n, const lint& ost);
    friend lint bin_pow(const lint& x, const lint& n);
    friend std::ostream& operator<<(std::ostream&, const lint&);
};

class lint::random
{
private:
    uint32_t randX;

    const uint64_t a = 6364136223846793005;
    const uint64_t c = 1442695040888963407;

public:
    random() : randX(0) {};
    random(uint32_t x = 0) : randX(x) {};

    void seed(uint32_t x)
    {
        randX = x;
    }

    lint operator()(size_t rand_size)
    {
        lint result(0);
        result._resize(rand_size);
        for(size_t id = 0; id < rand_size; ++id)
        {
            randX = ((uint64_t)a*randX + c) >> 32;
            result._lvalue[id] = randX;
        }
        return result;
    }
};

class lint::exceptionDivByZero : public std::exception
{
public:
    const char* what() const noexcept override final
    {
        return "lint - error: div by 0";
    }
};

#endif // LINT_HPP_INCLUDED
