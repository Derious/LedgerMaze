#!/bin/bash
dir="/Users/chenyu/Documents/openssl-master"

g++ -std=c++11 -O3 test_nizk_plaintext_equality.cpp -l ssl -l crypto -o test_pteq