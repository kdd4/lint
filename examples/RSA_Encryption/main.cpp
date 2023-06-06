#include <iostream>
#include "lint/RSA.hpp"

int main()
{
    try {
		uint32_t valA[] = {2543680161, 4007169358, 2385640586, 458211641, 1138343871,
			3741312131, 933003109, 3281193688, 2662400839, 1717558988, 335573491, 2077230716, 1340980793};

		uint32_t valB[] = {1346217561, 293887796, 197553051, 1203536241, 2547430464,
			2584020720, 2947087957, 3707387304, 3889348146, 295125621, 776458126, 485920793, 1561421419};


        lint A(valA, 13);
        lint B(valB, 13);
        RSA::Key key(A, B, 257);

        RSA::openKey pubKey(key);
        RSA::secretKey secKey(key);

        std::cout << "N: " << key.N << "\n";
        std::cout << "e: " << key.E << "\n";
        std::cout << "d: " << key.D << "\n\n";

        lint M(1029384756);
        std::cout << "Message: " << M << "\n";
        M = RSA::encode(M, pubKey);
        std::cout << "Encrypted message: " << M << "\n";
        M = RSA::decode(M, secKey);
        std::cout << "Decrypted message: " << M << "\n";

    } catch(const std::exception& ext) {
        std::cout << "Exception was caught. Message: " << ext.what() << "\n";
    }

    return 0;
}

