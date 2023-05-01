#include "RSA.hpp"

RSA::Key::Key(const lint& A, const lint& B, const lint& minE)
{
    N = A * B;
    lint f = (A - 1) * (B - 1);
    E = minE-1;
    while (true)
    {
        ++E;
        if ((f % E) != 0)
        {
            lint k = 1;
            while ((((f*k + 1) % E) != 0) && (k <= E))
            {
                ++k;
            }
            if (((f*k + 1) % E) != 0)
            {
                continue;
            }
            D = (f*k + 1) / E;
            break;
        }
    }
}

RSA::Key::Key(){}

RSA::secretKey::secretKey(const Key& right) : N(right.N), D(right.D) {}

RSA::secretKey::secretKey(const lint& N, const lint& D) : N(N), D(D) {}

RSA::openKey::openKey(const Key& right) : N(right.N), E(right.E) {}

RSA::openKey::openKey(const lint& N, const lint& E) : N(N), E(E) {}

lint RSA::encode(const lint& M, const openKey& key)
{
    return bin_pow(M, key.E, key.N);
}

lint RSA::decode(const lint& C, const secretKey& key)
{
    return bin_pow(C, key.D, key.N);
}

