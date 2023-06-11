#include "lint/lint.hpp"

void lint::_fill(size_t begin, size_t end, uint32_t* arr, uint32_t val)
{
    uint32_t* larr = arr + begin;
    uint32_t* rarr = arr + end;

    for (uint32_t* el = larr; el < rarr; ++el)
    {
        (*el) = val;
    }
}

void lint::_copy(size_t begin, size_t end, uint32_t* arrl, uint32_t* arrr)
{
    for (size_t pos = begin; pos < end; ++pos)
    {
        arrl[pos] = arrr[pos];
    }
}

template<typename T>
T lint::_min(T a, T b)
{
    return a < b ? a : b;
}

template<typename T>
T lint::_max(T a, T b)
{
    return a > b ? a : b;
}

void lint::_resize(size_t valSize, bool optimized)
{
    if (!optimized || valSize > _capable)
    {
        _capable = optimized ? _max(_capable*2, valSize) : valSize;
        uint32_t* newlvalue = new uint32_t[_capable];
        size_t size = _min(valSize, _size);
        _copy(0, size, newlvalue, _lvalue);
        _fill(size, _capable, newlvalue, 0);
        delete[] _lvalue;
        _lvalue = newlvalue;
    }
    else
    {
        _fill(valSize, _size, _lvalue, 0);
    }
    _size = valSize;
}

void lint::_lsh()
{
    uint32_t shift = 0;
    uint64_t val = 0;

    for(size_t pos = 0; pos < _size; ++pos)
    {
        val = ((uint64_t)_lvalue[pos] << 1) | shift;
        _lvalue[pos] = val & 0xFFFFFFFF;
        shift = (val >> 32) & 0x1;
    }

    if (shift)
    {
        _resize(_size+1);
        _lvalue[_size-1] = shift;
    }
}

void lint::_rsh()
{
    uint32_t shift = 0;
    uint64_t val = 0;

    for(size_t pos = _size; pos > 0; --pos)
    {
        val = (((uint64_t)_lvalue[pos-1] >> 1) & 0x7FFFFFFF) | (((uint64_t)shift<<31) & 0x80000000);
        shift = (uint64_t)_lvalue[pos-1] & 0x1;
        _lvalue[pos-1] = val & 0xFFFFFFFF;
    }
}

void lint::_lshb()
{
    uint32_t val1 = 0, val2 = 0;
    for(size_t pos = 0; pos < _size; ++pos)
    {
        val2= _lvalue[pos];
        _lvalue[pos] = val1;
        val1 = val2;
    }

    if (val1)
    {
        _resize(_size+1);
        _lvalue[_size-1] = val1;
    }
}

void lint::_rshb()
{
    for(size_t pos = 0; pos < _size-1; ++pos)
    {
        _lvalue[pos] = _lvalue[pos + 1];
    }
    _lvalue[_size-1] = 0;
}

lint::lint() : _lvalue(new uint32_t[2]), _size(2), _capable(2)
{
    _fill(0, _size, _lvalue, 0);
}

lint::lint(uint64_t val) : _lvalue(new uint32_t[2]), _size(2), _capable(2)
{
    _lvalue[0] = val & 0xFFFFFFFF;
    _lvalue[1] = val >> 32;
}

lint::lint(const lint& right) : _lvalue(new uint32_t[right._size]), _size(right._size), _capable(right._size)
{
    _copy(0, _size, _lvalue, right._lvalue);
}

lint::lint(uint32_t* right, size_t size) : _lvalue(new uint32_t[size]), _size(size), _capable(size)
{
    _copy(0, _size, _lvalue, right);
}

lint::~lint()
{
    delete[] _lvalue;
}

lint& lint::operator+=(const lint& right)
{
    uint32_t shift = 0;
    uint64_t sum = 0;
    if (right._size > _size)
    {
        _resize(right._size);
    }

    for(size_t pos = 0; pos < right._size; ++pos)
    {
        sum = (uint64_t)_lvalue[pos] + right._lvalue[pos] + shift;
        _lvalue[pos] = sum & 0xFFFFFFFF;
        shift = sum >> 32;
    }

    for(size_t pos = right._size; pos < _size; ++pos)
    {
        sum = (uint64_t)_lvalue[pos] + shift;
        _lvalue[pos] = sum & 0xFFFFFFFF;
        shift = sum >> 32;
    }

    if (shift)
    {
        _resize(_size+1);
        _lvalue[_size-1] = shift;
    }

    return *this;
}

lint& lint::operator-=(const lint& right)
{
    uint32_t shift = 0;
    uint64_t sub = 0;
    if (right._size > _size)
    {
        _resize(right._size);
    }

    for (size_t pos = 0; pos < right._size; ++pos)
    {
        sub = (uint64_t)_lvalue[pos] - right._lvalue[pos] - shift;
        _lvalue[pos] = sub & 0xFFFFFFFF;
        shift = sub >> 63;
    }

    for (size_t pos = right._size; pos < _size; ++pos)
    {
        if (shift == 0)
            break;
        sub = (uint64_t)_lvalue[pos] - shift;
        _lvalue[pos] = sub & 0xFFFFFFFF;
        shift = sub >> 63;
    }

    return *this;
}

lint& lint::operator*=(const lint& right)
{
    lint cnt(right);
    lint cp(*this);
    _fill(0, _size, _lvalue, 0);
    while(cnt != lint())
    {
        if (cnt._lvalue[0] & 1)
        {
            (*this) += cp;
        }

        cnt._rsh();
        cp._lsh();
    }
    return *this;
}

lint& lint::operator/=(const lint& right)
{
    if (right == 0)
    {
        throw exceptionDivByZero();
    }

    lint div(right);
    lint num(*this);
    _fill(0, _size, _lvalue, 0);
    if (div > num)
        return *this;

    size_t pos = 0;
    while (div <= num)
    {
        div._lsh();
        ++pos;
    }

    while(pos--)
    {
        div._rsh();
        (*this)._lsh();
        if (div <= num)
        {
            _lvalue[0] |= 0x1;
            num -= div;
        }
    }
    return *this;
}

lint& lint::operator%=(const lint& right)
{
    if (right == 0)
    {
        throw exceptionDivByZero();
    }
    lint div(right);
    if (div > (*this))
    {
        return *this;
    }

    size_t pos = 0;
    while (div <= (*this))
    {
        div._lsh();
        ++pos;
    }

    while(pos--)
    {
        div._rsh();
        if (div <= (*this))
        {
            (*this) -= div;
        }
    }
    return *this;
}

lint& lint::operator&=(const lint& right)
{
    size_t wSize = _min(_size, right._size);

    for(size_t pos = 0; pos < wSize; ++pos)
    {
        _lvalue[pos] &= right._lvalue[pos];
    }
    for(size_t pos = wSize; pos < _size; ++pos)
    {
        _lvalue[pos] = 0;
    }
    return *this;
}

lint& lint::operator|=(const lint& right)
{
    if (_size < right._size)
    {
        _resize(right._size);
    }

    for(size_t pos = 0; pos < _size; ++pos)
    {
        _lvalue[pos] |= right._lvalue[pos];
    }
    return *this;
}

lint& lint::operator^=(const lint& right)
{
    if (_size < right._size)
    {
        _resize(right._size);
    }

    for(size_t pos = 0; pos < _size; ++pos)
    {
        _lvalue[pos] ^= right._lvalue[pos];
    }
    return *this;
}

lint& lint::operator<<=(const lint& right)
{
    lint cnt = right;
    while (cnt >= 32)
    {
        _lshb();
        cnt -= 32;
    }
    while (cnt != 0)
    {
        _lsh();
        --cnt;
    }
    return *this;
}

lint& lint::operator>>=(const lint& right)
{
    lint cnt = right;
    while (cnt >= 32)
    {
        _rshb();
        cnt -= 32;
    }
    while (cnt != 0)
    {
        _rsh();
        --cnt;
    }
    return *this;
}

lint& lint::operator=(const lint& right)
{
    if (right._lvalue == this->_lvalue) return *this;
    delete[] _lvalue;
    _lvalue = new uint32_t[right._size];
    _size = right._size;
    _copy(0, _size, _lvalue, right._lvalue);
    return *this;
}

lint lint::operator+(const lint& right) const
{
    lint result(*this);
    result += right;
    return result;
}

lint lint::operator-(const lint& right) const
{
    lint result(*this);
    result -= right;
    return result;
}

lint lint::operator-() const
{
    lint result = ~(*this);
    ++result;
    return result;
}

lint lint::operator*(const lint& right) const
{
    lint result(*this);
    result *= right;
    return result;
}

lint lint::operator/(const lint& right) const
{
    lint result(*this);
    result /= right;
    return result;
}

lint lint::operator%(const lint& right) const
{
    lint result(*this);
    result %= right;
    return result;
}


lint& lint::operator++()
{
    (*this) += 1;
    return *this;
}

lint& lint::operator--()
{
    (*this) -= 1;
    return *this;
}

lint lint::operator++(int)
{
    lint result(*this);
    ++(*this);
    return result;
}

lint lint::operator--(int)
{
    lint result(*this);
    --(*this);
    return result;
}

bool lint::operator==(const lint& right) const
{
    size_t compSize = _min(_size, right._size);

    for (size_t pos = 0; pos < compSize; ++pos)
    {
        if (_lvalue[pos] != right._lvalue[pos])
        {
            return false;
        }
    }

    for (size_t pos = compSize; pos < _size; ++pos)
    {
        if (_lvalue[pos] != 0)
        {
            return false;
        }
    }

    for (size_t pos = compSize; pos < right._size; ++pos)
    {
        if (right._lvalue[pos] != 0)
        {
            return false;
        }
    }

    return true;
}

bool lint::operator!=(const lint& right) const
{
    size_t compSize = _min(_size, right._size);

    for (size_t pos = 0; pos < compSize; ++pos)
    {
        if (_lvalue[pos] != right._lvalue[pos])
        {
            return true;
        }
    }

    for (size_t pos = compSize; pos < _size; ++pos)
    {
        if (_lvalue[pos] != 0)
        {
            return true;
        }
    }

    for (size_t pos = compSize; pos < right._size; ++pos)
    {
        if (right._lvalue[pos] != 0)
        {
            return true;
        }
    }

    return false;
}

bool lint::operator>(const lint& right) const
{
    size_t compSize = _min(_size, right._size);

    for (size_t pos = compSize; pos < _size; ++pos)
    {
        if (_lvalue[pos] != 0)
        {
            return true;
        }
    }

    for (size_t pos = compSize; pos < right._size; ++pos)
    {
        if (right._lvalue[pos] != 0)
        {
            return false;
        }
    }

    for (size_t pos = compSize; pos > 0; --pos)
    {
        if (_lvalue[pos-1] != right._lvalue[pos-1])
        {
            return _lvalue[pos-1] > right._lvalue[pos-1];
        }
    }

    return false;
}

bool lint::operator<(const lint& right) const
{
    size_t compSize = _min(_size, right._size);

    for (size_t pos = compSize; pos < _size; ++pos)
    {
        if (_lvalue[pos] != 0)
        {
            return false;
        }
    }

    for (size_t pos = compSize; pos < right._size; ++pos)
    {
        if (right._lvalue[pos] != 0)
        {
            return true;
        }
    }

    for (size_t pos = compSize; pos > 0; --pos)
    {
        if (_lvalue[pos-1] != right._lvalue[pos-1])
        {
            return _lvalue[pos-1] < right._lvalue[pos-1];
        }
    }

    return false;
}

bool lint::operator>=(const lint& right) const
{
    size_t compSize = _min(_size, right._size);

    for (size_t pos = compSize; pos < _size; ++pos)
    {
        if (_lvalue[pos] != 0)
        {
            return true;
        }
    }

    for (size_t pos = compSize; pos < right._size; ++pos)
    {
        if (right._lvalue[pos] != 0)
        {
            return false;
        }
    }

    for (size_t pos = compSize; pos > 0; --pos)
    {
        if (_lvalue[pos-1] != right._lvalue[pos-1])
        {
            return _lvalue[pos-1] > right._lvalue[pos-1];
        }
    }

    return true;
}

bool lint::operator<=(const lint& right) const
{
    size_t compSize = _min(_size, right._size);

    for (size_t pos = compSize; pos < _size; ++pos)
    {
        if (_lvalue[pos] != 0)
        {
            return false;
        }
    }

    for (size_t pos = compSize; pos < right._size; ++pos)
    {
        if (right._lvalue[pos] != 0)
        {
            return true;
        }
    }

    for (size_t pos = compSize; pos > 0; --pos)
    {
        if (_lvalue[pos-1] != right._lvalue[pos-1])
        {
            return _lvalue[pos-1] < right._lvalue[pos-1];
        }
    }

    return true;
}

lint lint::operator~() const
{
    lint result(*this);
    for (size_t pos = 0; pos < _size; ++pos)
    {
        result._lvalue[pos] = ~(result._lvalue[pos]);
    }
    return result;
}

lint lint::operator<< (const lint& right) const
{
    lint result(*this);
    result <<= right;
    return result;
}

lint lint::operator>> (const lint& right) const
{
    lint result(*this);
    result >>= right;
    return result;
}

lint lint::operator&(const lint& right) const
{
    lint result(*this);
    result &= right;
    return result;
}

lint lint::operator|(const lint& right) const
{
    lint result(*this);
    result |= right;
    return result;
}

lint lint::operator^(const lint& right) const
{
    lint result(*this);
    result ^= right;
    return result;
}

void lint::compress()
{
    size_t pos = _size;
    while(--pos)
    {
        if (_lvalue[pos])
        {
            _resize(pos+1, false);
            break;
        }
    }
}

lint bin_pow(const lint& x, const lint& n, const lint& ost)
{
    lint result(1);
    lint cnt(n);
    lint mlp(x);
    while(cnt != 0)
    {
        if (cnt._lvalue[0] & 1)
        {
            result *= mlp;
            result %= ost;
        }

        cnt._rsh();
        mlp *= mlp;
        mlp %= ost;
    }
    return result;
}

lint bin_pow(const lint& x, const lint& n)
{
    lint result(1);
    lint cnt(n);
    lint mlp(x);
    while(cnt != 0)
    {
        if (cnt._lvalue[0] & 1)
        {
            result *= mlp;
        }

        cnt._rsh();
        mlp *= mlp;
    }
    return result;
}

std::ostream& operator<<(std::ostream& stream, const lint& right)
{
    stream << "0x";
    bool f = 0;
    for (size_t pos = right._size; pos > 0; --pos)
    {
        if (!f)
        {
            f = right._lvalue[pos-1];
            if (f)
            {
                stream  << std::setfill('0')
                        << std::hex
                        << right._lvalue[pos-1];
            }
        }
        else
        {
            stream  << std::setfill('0')
                    << std::setw(8)
                    << std::hex
                    << right._lvalue[pos-1];
        }
    }
    if (!f)
    {
        stream << "0";
    }
    return stream;
}

lint::random::random() : randX(0) {}
lint::random::random(uint32_t x) : randX(x) {}

void lint::random::seed(uint32_t x)
{
    randX = x;
}

lint lint::random::operator()(size_t rand_size)
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
