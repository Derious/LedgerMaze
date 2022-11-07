#!/bin/bash
g++ -std=c++11 -pthread -O3 -g ../bench/bench_all.cpp ../depends/oneoutofnproofs/src/params.cpp   ../depends/oneoutofnproofs/bitcoin/crypto/*.cpp ../depends/oneoutofnproofs/secp256k1/.libs/libsecp256k1.a \
-I/usr/include/glib-2.0 \
-I/usr/lib/x86_64-linux-gnu/glib-2.0/include \
-I/usr/include/mysql \
-lssl -levent -lcrypto -lgmp -lmysqlclient \
-o test_bench 