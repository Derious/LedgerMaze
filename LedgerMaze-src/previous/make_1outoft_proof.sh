# g++ test_oneoutoft.cpp ../depends/oneoutofnproofs/src/params.cpp  ../depends/oneoutofnproofs/bitcoin/*.cpp ../depends/oneoutofnproofs/bitcoin/crypto/*.cpp  -g  ../depends/oneoutofnproofs/secp256k1/.libs/libsecp256k1.a   -lssl -lcrypto -lgmp  -lboost_unit_test_framework -std=c++11 -o 1ottest

g++ test_oneoutoft.cpp ../depends/oneoutofnproofs/src/params.cpp   ../depends/oneoutofnproofs/bitcoin/crypto/*.cpp  -g  ../depends/oneoutofnproofs/secp256k1/.libs/libsecp256k1.a   -lssl -lcrypto -lgmp  -lboost_unit_test_framework -std=c++11 -pthread -o 1ottest


