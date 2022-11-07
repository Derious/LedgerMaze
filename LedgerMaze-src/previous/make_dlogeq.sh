#!/bin/bash
libdir="/usr/local/ssl/lib64"
incdir="/usr/include/openssl"
g++ -std=c++11 -pthread -O3 ../test/test_nizk_dlog_equality.cpp -L ${libdir} -lssl\
 -L ${libdir} -lcrypto -o test_dlogeq -I ${incdir}
