# LedgerMaze: An Efficient Privacy-Preserving NIZK Scheme over Account-Model Blockchain
## Specifications

- OS: Linux x64

- Language: C++

- Requires: OpenSSL libsecp256k1

- The default elliptic curve is "NID_X9_62_prime256v1"


## Installation

The current implementation is based on OpenSSL library. See the installment instructions of OpenSSL as below:  

1. Download [openssl-master.zip](https://github.com/openssl/openssl.git)

2. make a directory "openssl" to save the source codes of MIRACL

```
    mkdir openssl
    mv openssl-master.zip /openssl
```

3. unzip it

4. install openssl on your machine

```
    ./config --prefix=/usr/local/ssl shared
    make 
    sudo make install
```
5. install libsecp256k1 on your machine

```
   cd ./depends/oneoutofnproofs/secp256k1
    ./autogen.sh
    ./configure
    make 
```

---

## Code Structure of LedgerMaze_openssl

/depends: depends module

- /common: common module 
  * global.hpp: generate global parameters
  * print.hpp: print split line for demo use
  * hash.hpp: case-tailored hash functions based on SHA2
  * routines.hpp: related routine algorithms

- /twisted_elgamal: ISE module
  * twisted_elgamal.hpp: twisted ElGamal ISE and PKE  
  * calculate_dlog.hpp: discrete logarithm searching algorithm

- /nizk: NIZK module, obtained by applying Fiat-Shamir transform to Sigma-protocols  
  * nizk_plaintext_equality.hpp: NIZKPoK for twisted ElGamal plaintext equality
  * nizk_plaintext_knowledge.hpp: NIZKPoK for twisted ElGamal plaintext and randomness knowledge
  * nizk_dlog_equality.hpp: NIZKPoK for discrete logarithm equality

- /bulletproofs: Bulletproofs module
  * basic_bulletproof.hpp: the basic bulletproofs
  * log_bulletproof.hpp: the logarithmic size bulletproofs
  * aggregate_bulletproof.hpp: the aggregating logarithmic size bulletproofs
  * innerproduct_proof.hpp: the inner product argument (used by Bulletproof to shrink the proof size) 

- /gadgets: Gadget module
  * gadgets.hpp: two useful gadgets for proving encrypted values lie in the right range

- /oneoutofnproofs: oneoutofnproofs module

/test: all test files

/bench: all bench files

/backend: all backend files for ledgermaze system

## Testing

In test\test_ledgermaze, there is a system test and it can open a backend for our LedgerMaze

```
  $ ./make_twisted_elgamal.sh
  $ ./test_enc # tests the twisted ElGamal ISE and PKE
```
You can find the bash files in /previous and do the unit test for any module

To compile and test the LedgerMaze system, do the following: 

```
  $ mkdir build && cd build
  $ cmake ..
  $ make
  $ ./test_ledgermaze
```
