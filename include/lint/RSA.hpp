#ifndef RSA_HPP_INCLUDED
#define RSA_HPP_INCLUDED

#include "lint.hpp"

namespace RSA
{
    struct Key
    {
        Key(const lint& A, const lint& B, const lint& minE = 17);
        Key();
        lint N;
        lint E;
        lint D;
    };

    struct secretKey
    {
        secretKey(const Key&);
        secretKey(const lint& N, const lint& D);
        lint N;
        lint D;
    };

    struct openKey
    {
        openKey(const Key&);
        openKey(const lint& N, const lint& E);
        lint N;
        lint E;
    };

    lint encode(const lint& M, const openKey& key);
    lint decode(const lint& C, const secretKey& key);
}

#endif // RSA_HPP_INCLUDED
