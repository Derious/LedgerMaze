#!/bin/bash
# dir="/home/vivi/下载/openssl-master"
libdir="/usr/local/ssl/lib64"
incdir="/usr/include/openssl"
g++ -std=c++11 -pthread -O3 ../test/test_twisted_elgamal.cpp -L ${libdir} -lssl\
 -L ${libdir} -lcrypto -o test_enc -I ${incdir}