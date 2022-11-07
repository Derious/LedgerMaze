/****************************************************************************
this hpp implements twisted ElGamal encrypt scheme
*****************************************************************************
* @author     This file is part of PGC, developed by Yu Chen
* @paper      https://eprint.iacr.org/2019/319
* @copyright  MIT license (see LICENSE file)
*****************************************************************************/
#ifndef __ENC__
#define __ENC__

#include "../common/global.hpp"
#include "../common/hash.hpp"
#include "../common/print.hpp"
#include "../common/routines.hpp"

#include "calculate_dlog.hpp"

const string hashmap_file  = "point2index.table"; // name of hashmap file

// define the structure of PP
struct Twisted_ElGamal_PP
{
    size_t MSG_LEN; // the length of message space, also the length of the DLOG interval  
    BIGNUM *BN_MSG_SIZE; // the size of message space
    size_t TUNNING; //increase this parameter in [0, RANGE_LEN/2]: larger table leads to less running time
    size_t IO_THREAD_NUM; // optimized number of threads for faster building hash map 
    size_t DEC_THREAD_NUM; // optimized number of threads for faster decryption: CPU dependent

    EC_POINT *g; 
    EC_POINT *h; // two random generators 
};

// define the structure of keypair
struct Twisted_ElGamal_KP
{
    EC_POINT *pk;  // define pk
    BIGNUM *sk;    // define sk
};

// define the structure of ciphertext
struct Twisted_ElGamal_CT
{
    EC_POINT *X; // X = pk^r 
    EC_POINT *Y; // Y = g^m h^r 
};

// define the structure of PKE ciphertext
struct Hashed_ElGamal_CT
{
    EC_POINT *X; // X = g^r 
    BIGNUM *Y; // Y = Hash(pk^r) xor m
};

// define the structure of signature
struct Twisted_ElGamal_SIG
{
    EC_POINT *R; // R = g^r
    BIGNUM *z; // z = r+sk*t / t = H(m,R)
};

// define the structure of zk_balance
struct ZK_BALANCE_CT
{
    Twisted_ElGamal_CT C;
    EC_POINT *pk;
    BIGNUM *v;   //cipher of v
    BIGNUM *sn; 
    BIGNUM *r; 
};

// define the structure of two-recipients one-message ciphertext (MR denotes multiple recipients)
struct MR_Twisted_ElGamal_CT
{
    EC_POINT *X1; // X = pk1^r
    EC_POINT *X2; // X = pk2^r 
    EC_POINT *Y; // Y = G^m H^r 
};

/* allocate memory for PP */ 
void Twisted_ElGamal_PP_new(Twisted_ElGamal_PP &pp)
{ 
    pp.g = EC_POINT_new(group);
    pp.h = EC_POINT_new(group); 
    pp.BN_MSG_SIZE = BN_new(); 
}

/* free memory of PP */ 
void Twisted_ElGamal_PP_free(Twisted_ElGamal_PP &pp)
{ 
    EC_POINT_free(pp.g);
    EC_POINT_free(pp.h);
    BN_free(pp.BN_MSG_SIZE); 
}

void Twisted_ElGamal_KP_new(Twisted_ElGamal_KP &keypair)
{
    keypair.pk = EC_POINT_new(group); 
    keypair.sk = BN_new(); 
}

void Twisted_ElGamal_KP_free(Twisted_ElGamal_KP &keypair)
{
    EC_POINT_free(keypair.pk); 
    BN_free(keypair.sk);
}

void Twisted_ElGamal_CT_new(Twisted_ElGamal_CT &CT)
{
    CT.X = EC_POINT_new(group); 
    CT.Y = EC_POINT_new(group);
}

void Twisted_ElGamal_CT_free(Twisted_ElGamal_CT &CT)
{
    EC_POINT_free(CT.X); 
    EC_POINT_free(CT.Y);
}

void Twisted_ElGamal_CT_copy(Twisted_ElGamal_CT &CT1,Twisted_ElGamal_CT &CT2)
{
    EC_POINT_copy(CT1.X,CT2.X);
    EC_POINT_copy(CT1.Y,CT2.Y);
}

void Twisted_ElGamal_SIG_new(Twisted_ElGamal_SIG &SIG)
{
    SIG.R = EC_POINT_new(group); 
    SIG.z = BN_new();
}

void Twisted_ElGamal_SIG_free(Twisted_ElGamal_SIG &SIG)
{
    EC_POINT_free(SIG.R);
    BN_free(SIG.z);
}

void Hashed_ElGamal_CT_new(Hashed_ElGamal_CT &CT)
{
    CT.X = EC_POINT_new(group); 
    CT.Y = BN_new();
}

void Hashed_ElGamal_CT_free(Hashed_ElGamal_CT &CT)
{
    EC_POINT_free(CT.X); 
    BN_free(CT.Y);
}

void Hashed_ElGamal_CT_copy(Hashed_ElGamal_CT &CT1,Hashed_ElGamal_CT &CT2)
{
    EC_POINT_copy(CT1.X,CT2.X);
    BN_copy(CT1.Y,CT2.Y);
}

void MR_Twisted_ElGamal_CT_new(MR_Twisted_ElGamal_CT &CT)
{
    CT.X1 = EC_POINT_new(group);
    CT.X2 = EC_POINT_new(group);  
    CT.Y = EC_POINT_new(group);
}

void MR_Twisted_ElGamal_CT_free(MR_Twisted_ElGamal_CT &CT)
{
    EC_POINT_free(CT.X1);
    EC_POINT_free(CT.X2); 
    EC_POINT_free(CT.Y);
}

void ZK_BALANCE_CT_new(ZK_BALANCE_CT &CT)
{
    Twisted_ElGamal_CT_new(CT.C);
    CT.pk=EC_POINT_new(group);
    CT.v=BN_new();
    CT.sn=BN_new();
    CT.r=BN_new();
}

void ZK_BALANCE_CT_free(ZK_BALANCE_CT &CT)
{
    Twisted_ElGamal_CT_free(CT.C);
    EC_POINT_free(CT.pk);
    BN_free(CT.v);
    BN_free(CT.sn);
    BN_free(CT.r);
}
void ZK_BALANCE_CT_copy(ZK_BALANCE_CT &zk_balance,ZK_BALANCE_CT &new_zk_balance)
{
    BN_copy(zk_balance.r,new_zk_balance.r);
    BN_copy(zk_balance.sn,new_zk_balance.sn);
    BN_copy(zk_balance.v,new_zk_balance.v);
    EC_POINT_copy(zk_balance.pk,new_zk_balance.pk);
    Twisted_ElGamal_CT_copy(zk_balance.C,new_zk_balance.C);
}
void Twisted_ElGamal_PP_print(Twisted_ElGamal_PP &pp)
{
    cout << "the length of message space = " << pp.MSG_LEN << endl; 
    cout << "the tunning parameter for fast decryption = " << pp.TUNNING << endl;
    ECP_print(pp.g, "pp.g"); 
    ECP_print(pp.h, "pp.h"); 
} 

void Twisted_ElGamal_KP_print(Twisted_ElGamal_KP &keypair)
{
    ECP_print(keypair.pk, "pk"); 
    BN_print(keypair.sk, "sk"); 
} 

void Twisted_ElGamal_CT_print(Twisted_ElGamal_CT &CT)
{
    ECP_print(CT.X, "CT.X");
    ECP_print(CT.Y, "CT.Y");
} 

void Twisted_ElGamal_SIG_print(Twisted_ElGamal_SIG &SIG)
{
    ECP_print(SIG.R, "SIG.R");
    BN_print(SIG.z, "SIG.z");
} 

void Hashed_ElGamal_CT_print(Hashed_ElGamal_CT &CT)
{
    ECP_print(CT.X, "CT.X");
    BN_print(CT.Y, "CT.Y");
} 

void ZK_BALANCE_CT_print(ZK_BALANCE_CT &CT)
{
    cout << "cipher:" << endl; 
    Twisted_ElGamal_CT_print(CT.C);
    ECP_print(CT.pk, "pk"); 
    BN_print(CT.sn, "sn");
    // BN_print(CT.v,"value");
    BN_print_dec(CT.v,"value");
    BN_print(CT.r, "r");
    
}

void MR_Twisted_ElGamal_CT_print(MR_Twisted_ElGamal_CT &CT)
{
    ECP_print(CT.X1, "CT.X1");
    ECP_print(CT.X2, "CT.X2");
    ECP_print(CT.Y, "CT.Y");
} 

void Twisted_ElGamal_CT_to_string(Twisted_ElGamal_CT &CT,char* buffer)
{

    strcat(buffer,EC_POINT_point2hex(group, CT.X, POINT_CONVERSION_COMPRESSED, bn_ctx));
    strcat(buffer,EC_POINT_point2hex(group, CT.X, POINT_CONVERSION_COMPRESSED, bn_ctx));
 
}

void Twisted_ElGamal_CT_serialize(Twisted_ElGamal_CT &CT, ofstream &fout)
{
    ECP_serialize(CT.X, fout); 
    ECP_serialize(CT.Y, fout); 
} 
void Hashed_ElGamal_CT_serialize(Hashed_ElGamal_CT &CT, ofstream &fout)
{
    ECP_serialize(CT.X, fout); 

    unsigned char buffer[2*BN_LEN+1];
    BN_bn2binpad(CT.Y, buffer, 2*BN_LEN+1);
    fout.write(reinterpret_cast<char *>(buffer), 2*BN_LEN+1);   // write to outfiles
}

void Twisted_ElGamal_SIG_serialize(Twisted_ElGamal_SIG &CT,ofstream &fout)
{
    ECP_serialize(CT.R, fout); 
    BN_serialize(CT.z,fout);
}
void ZK_BALANCE_CT_serialize(ZK_BALANCE_CT &CT, ofstream &fout)
{
    Twisted_ElGamal_CT_serialize(CT.C,fout);
    ECP_serialize(CT.pk,fout);
    BN_serialize(CT.r,fout);
    BN_serialize(CT.sn,fout);
    BN_serialize(CT.v,fout);
}



void Twisted_ElGamal_CT_deserialize(Twisted_ElGamal_CT &CT, ifstream &fin)
{
    ECP_deserialize(CT.X, fin); 
    ECP_deserialize(CT.Y, fin); 
} 

void Hashed_ElGamal_CT_deserialize(Hashed_ElGamal_CT &CT,ifstream &fin)
{
    ECP_deserialize(CT.X, fin); 
    char buffer[2*BN_LEN+1];
    fin.read(buffer, 2*BN_LEN+1);
    BN_bin2bn(reinterpret_cast<unsigned char *>(buffer), 2*BN_LEN+1, CT.Y);
}

void Twisted_ElGamal_SIG_deserialize(Twisted_ElGamal_SIG &CT,ifstream &fin)
{
    ECP_deserialize(CT.R, fin); 
    BN_deserialize(CT.z,fin);
}
void ZK_BALANCE_CT_deserialize(ZK_BALANCE_CT &CT, ifstream &fin)
{
    Twisted_ElGamal_CT_deserialize(CT.C,fin);
    ECP_deserialize(CT.pk,fin);
    BN_deserialize(CT.r,fin);
    BN_deserialize(CT.sn,fin);
    BN_deserialize(CT.v,fin);
}

void MR_Twisted_ElGamal_CT_serialize(MR_Twisted_ElGamal_CT &CT, ofstream& fout)
{
    ECP_serialize(CT.X1, fout); 
    ECP_serialize(CT.X2, fout);
    ECP_serialize(CT.Y,  fout); 
} 

void MR_Twisted_ElGamal_CT_deserialize(MR_Twisted_ElGamal_CT &CT, ifstream& fin)
{
    ECP_deserialize(CT.X1, fin); 
    ECP_deserialize(CT.X2, fin); 
    ECP_deserialize(CT.Y, fin); 
}

/* Setup algorithm */ 
void Twisted_ElGamal_Setup(Twisted_ElGamal_PP &pp, size_t MSG_LEN, size_t TUNNING, 
                           size_t IO_THREAD_NUM, size_t DEC_THREAD_NUM)
{ 
    pp.MSG_LEN = MSG_LEN; 
    pp.TUNNING = TUNNING; 
    pp.IO_THREAD_NUM = IO_THREAD_NUM;
    pp.DEC_THREAD_NUM = DEC_THREAD_NUM;  
    /* set the message space to 2^{MSG_LEN} */
    BN_set_word(pp.BN_MSG_SIZE, uint64_t(pow(2, pp.MSG_LEN))); 

    // #ifdef DEBUG
    // cout << "message space = [0, ";   
    // cout << BN_bn2hex(pp.BN_MSG_SIZE) << ')' << endl; 
    // #endif
  
    EC_POINT_copy(pp.g, generator); 
    /* generate pp.h via deterministic manner */
    Hash_ECP_to_ECP(pp.g, pp.h); 

    // #ifdef DEBUG
    // cout << "generate the public parameters for twisted ElGamal >>>" << endl; 
    // Twisted_ElGamal_PP_print(pp); 
    // #endif
}

void Twisted_ElGamal_Initialize(Twisted_ElGamal_PP &pp)
{
    cout << "Initialize Twisted ElGamal >>>" << endl; 
    /* generate or load the point2index.table */
    if(!FILE_exist(hashmap_file))
    {
        // generate and serialize the point_2_index table
        Parallel_HASHMAP_serialize(pp.h, hashmap_file, pp.MSG_LEN, pp.TUNNING, pp.IO_THREAD_NUM); 
    }
    // load the table from file 
    HASHMAP_deserialize(hashmap_file, pp.MSG_LEN, pp.TUNNING);            
}

/* KeyGen algorithm */ 
void Twisted_ElGamal_KeyGen(Twisted_ElGamal_PP &pp, Twisted_ElGamal_KP &keypair)
{ 
    BN_random(keypair.sk); // sk \sample Z_p
    EC_POINT_mul(group, keypair.pk, keypair.sk, NULL, NULL, bn_ctx); // pk = g^sk  

    #ifdef DEBUG
    cout << "key generation finished >>>" << endl;  
    Twisted_ElGamal_KP_print(keypair); 
    #endif
}

/* Encryption algorithm: compute CT = Enc(pk, m; r) */ 
void Twisted_ElGamal_Enc_fixed(Twisted_ElGamal_PP &pp, 
                         EC_POINT* &pk, 
                         BIGNUM* &m, 
                         Twisted_ElGamal_CT &CT)
{ 
    // generate the random coins 
    BIGNUM *r = BN_new(); 
    BN_random(r);

    // begin encryption
    EC_POINT_mul(group, CT.X, NULL, pk, r, bn_ctx); // X = pk^r
    EC_POINT_mul(group, CT.Y, r, pp.h, m, bn_ctx);  // Y = g^r h^m
    
    BN_free(r); 

    #ifdef DEBUG
        cout << "twisted ElGamal encryption finishes >>>"<< endl;
        Twisted_ElGamal_CT_print(CT); 
    #endif
}

/* Encryption algorithm: compute CT = Enc(pk, m; r): with explicit randomness */ 
void Twisted_ElGamal_Enc(Twisted_ElGamal_PP &pp, 
                         EC_POINT* &pk, 
                         BIGNUM* &m, 
                         BIGNUM* &r, 
                         Twisted_ElGamal_CT &CT)
{ 
    // begin encryption
    EC_POINT_mul(group, CT.X, NULL, pk, r, bn_ctx); // X = pk^r
    EC_POINT_mul(group, CT.Y, r, pp.h, m, bn_ctx); // Y = g^r h^m

    #ifdef DEBUG
        cout << "twisted ElGamal encryption finishes >>>"<< endl;
        Twisted_ElGamal_CT_print(CT); 
    #endif
}

/* Decryption algorithm: compute m = Dec(sk, CT) */ 
void Twisted_ElGamal_Dec(Twisted_ElGamal_PP &pp, 
                         BIGNUM* &sk, 
                         Twisted_ElGamal_CT &CT, 
                         BIGNUM* &m)
{ 
    //begin decryption  
    BIGNUM *sk_inverse = BN_new(); 
    BN_mod_inverse(sk_inverse, sk, order, bn_ctx);  // compute the inverse of sk in Z_q^* 

    EC_POINT *M = EC_POINT_new(group); 
    EC_POINT_mul(group, M, NULL, CT.X, sk_inverse, bn_ctx); // M = X^{sk^{-1}} = g^r 
    EC_POINT_invert(group, M, bn_ctx);          // M = -g^r
    EC_POINT_add(group, M, CT.Y, M, bn_ctx);    // M = h^m

    //Brute_Search(m, pp.h, M); 
    bool success = Shanks_DLOG(m, pp.h, M, pp.MSG_LEN, pp.TUNNING); // use Shanks's algorithm to decrypt
  
    BN_free(sk_inverse); 
    EC_POINT_free(M);
    if(success == false)
    {
        cout << "decyption fails in the specified range"; 
        exit(EXIT_FAILURE); 
    }  
}

/* rerandomize ciphertext CT with given randomness r */ 
void Twisted_ElGamal_ReRand(Twisted_ElGamal_PP &pp, 
                             EC_POINT* &pk, 
                             BIGNUM* &sk, 
                             Twisted_ElGamal_CT &CT, 
                             Twisted_ElGamal_CT &CT_new, 
                             BIGNUM* &r)
{ 
    // begin partial decryption  
    BIGNUM *sk_inverse = BN_new(); 
    BN_mod_inverse(sk_inverse, sk, order, bn_ctx);  // compute the inverse of sk in Z_q^* 

    EC_POINT *M = EC_POINT_new(group); 
    EC_POINT_mul(group, M, NULL, CT.X, sk_inverse, bn_ctx); // M = X^{sk^{-1}} = g^r 
    EC_POINT_invert(group, M, bn_ctx);          // M = -g^r
    EC_POINT_add(group, M, CT.Y, M, bn_ctx);    // M = h^m

    // begin re-encryption with the given randomness 
    EC_POINT_mul(group, CT_new.X, NULL, pk, r, bn_ctx); // CT_new.X = pk^r 
    EC_POINT_mul(group, CT_new.Y, r, NULL, NULL, bn_ctx); // CT_new.Y = g^r 

    EC_POINT_add(group, CT_new.Y, CT_new.Y, M, bn_ctx);    // M = h^m

    #ifdef DEBUG
        cout << "refresh ciphertext succeeds >>>"<< endl;
        Twisted_ElGamal_CT_print(CT_new); 
    #endif

    BN_free(sk_inverse); 
    EC_POINT_free(M); 
}


/* homomorphic add */
void Twisted_ElGamal_HomoAdd(Twisted_ElGamal_CT &CT_result, Twisted_ElGamal_CT &CT1, Twisted_ElGamal_CT &CT2)
{ 
    EC_POINT_add(group, CT_result.X, CT1.X, CT2.X, bn_ctx);  
    EC_POINT_add(group, CT_result.Y, CT1.Y, CT2.Y, bn_ctx);  
}

/* homomorphic sub */
void Twisted_ElGamal_HomoSub(Twisted_ElGamal_CT &CT_result, Twisted_ElGamal_CT &CT1, Twisted_ElGamal_CT &CT2)
{ 
    EC_POINT_sub(CT_result.X, CT1.X, CT2.X);  
    EC_POINT_sub(CT_result.Y, CT1.Y, CT2.Y);  
}

/* scalar operation */
void Twisted_ElGamal_ScalarMul(Twisted_ElGamal_CT &CT_result, Twisted_ElGamal_CT &CT, BIGNUM *&k)
{ 
    EC_POINT_mul(group, CT_result.X, NULL, CT.X, k, bn_ctx);  
    EC_POINT_mul(group, CT_result.Y, NULL, CT.Y, k, bn_ctx);  
}


/* Encryption algorithm (2-recipients 1-message) with given random coins
output X1 = pk1^r, X2 = pk2^r, Y = g^r h^m
Here we make the randomness explict for the ease of generating the ZKP */
void MR_Twisted_ElGamal_Enc(Twisted_ElGamal_PP &pp, 
                            EC_POINT* &pk1, 
                            EC_POINT* &pk2, 
                            BIGNUM* &m, 
                            BIGNUM* &r, 
                            MR_Twisted_ElGamal_CT &CT)
{ 
    EC_POINT_mul(group, CT.X1, NULL, pk1, r, bn_ctx); // CT_new.X1 = pk1^r
    EC_POINT_mul(group, CT.X2, NULL, pk2, r, bn_ctx); // CT_new.X2 = pk2^r
    EC_POINT_mul(group, CT.Y, r, pp.h, m, bn_ctx); // Y = g^r h^m
   
    #ifdef DEBUG
        cout << "2-recipient 1-message twisted ElGamal encryption finishes >>>"<< endl;
        MR_Twisted_ElGamal_CT_print(CT); 
    #endif
}

/* parallel implementation */

/*
    https://www.openssl.org/docs/manmaster/man3/BN_CTX_new.html
    A given BN_CTX must only be used by a single thread of execution. 
    No locking is performed, and the internal pool allocator will not properly handle multiple threads of execution. 
    Thus, in multithread programming, a lazy and safe approach is setting bn_ctx = NULL
*/

// parallel encryption
inline void exp_operation(EC_POINT *&RESULT, EC_POINT *&A, BIGNUM *&r) 
{ 
    EC_POINT_mul(group, RESULT, NULL, A, r, NULL); // RESULT = A^r
} 

inline void builtin_exp_operation(EC_POINT *&RESULT, BIGNUM *&r) 
{ 
    EC_POINT_mul(group, RESULT, r, NULL, NULL, NULL);  // RESULT = g^r 
} 

inline void multiexp_operation(EC_POINT *&RESULT, EC_POINT *&h, BIGNUM *&r, BIGNUM *&m) 
{ 
    EC_POINT_mul(group, RESULT, r, h, m, bn_ctx);  // Y = g^r h^m
} 

/* Parallel Encryption algorithm: compute CT = Enc(pk, m; r) */
void Twisted_ElGamal_Parallel_Enc(Twisted_ElGamal_PP &pp, EC_POINT *&pk, BIGNUM *&m, Twisted_ElGamal_CT &CT)
{ 
    /* generate fresh randomness */ 
    BIGNUM *r = BN_new(); 
    BN_random(r);

    thread enc_thread1(exp_operation, std::ref(CT.X), std::ref(pk), std::ref(r));     // spawn new thread that calls foo()
    thread enc_thread2(multiexp_operation, std::ref(CT.Y), std::ref(pp.h), std::ref(r), std::ref(m));

    // synchronize threads
    enc_thread1.join();                // pauses until first finishes
    enc_thread2.join();               // pauses until second finishes

    BN_free(r); 
}


/* Decryption algorithm: compute m = Dec(sk, CT) */
void Twisted_ElGamal_Parallel_Dec(Twisted_ElGamal_PP &pp, BIGNUM *&sk, Twisted_ElGamal_CT &CT, BIGNUM *&m)
{ 
    /* begin to decrypt */  
    BIGNUM *sk_inverse = BN_new(); 
    BN_mod_inverse(sk_inverse, sk, order, bn_ctx);  // compute the inverse of sk in Z_p^* 

    EC_POINT *M = EC_POINT_new(group); 
    EC_POINT_mul(group, M, NULL, CT.X, sk_inverse, bn_ctx); // M = X^{sk^{-1}} = g^r 
    EC_POINT_invert(group, M, bn_ctx);          // M = -g^r
    EC_POINT_add(group, M, CT.Y, M, bn_ctx);    // M = h^m

    bool success = Parallel_Shanks_DLOG(m, pp.h, M, pp.MSG_LEN, pp.TUNNING, pp.DEC_THREAD_NUM); // use Shanks's algorithm to decrypt
  
    BN_free(sk_inverse); 
    EC_POINT_free(M);

    if(success == false)
    {
        cout << "decyption fails: cannot find the message in the specified range"; 
        exit(EXIT_FAILURE); 
    }  
}

// parallel re-randomization
void Twisted_ElGamal_Parallel_ReRand(Twisted_ElGamal_PP &pp, EC_POINT *&pk, BIGNUM *&sk, 
                            Twisted_ElGamal_CT &CT, Twisted_ElGamal_CT &CT_new, BIGNUM *&r)
{ 
    /* partial decryption: only recover M = h^m */  
    BIGNUM *sk_inverse = BN_new(); 
    BN_mod_inverse(sk_inverse, sk, order, bn_ctx);  // compute the inverse of sk in Z_p^* 

    EC_POINT *M = EC_POINT_new(group); 
    EC_POINT_mul(group, M, NULL, CT.X, sk_inverse, bn_ctx); // M = X^{sk^{-1}} = g^r 
    EC_POINT_invert(group, M, bn_ctx);          // M = -g^r
    EC_POINT_add(group, M, CT.Y, M, bn_ctx);    // M = h^m

    /* re-encryption with the given randomness */
    thread rerand_thread1(exp_operation, std::ref(CT.X), std::ref(pk), std::ref(r));
    thread rerand_thread2(builtin_exp_operation, std::ref(CT.Y), std::ref(r));

    rerand_thread1.join(); 
    rerand_thread2.join(); 

    EC_POINT_add(group, CT_new.Y, CT_new.Y, M, bn_ctx);    // Y = g^r h^m

    BN_free(sk_inverse); 
    EC_POINT_free(M); 
}

/* parallel homomorphic add */
inline void add_operation(EC_POINT *&RESULT, EC_POINT *&X, EC_POINT *&Y) 
{ 
    EC_POINT_add(group, RESULT, X, Y, NULL);  
} 

void Twisted_ElGamal_Parallel_HomoAdd(Twisted_ElGamal_CT &CT_result, Twisted_ElGamal_CT &CT1, Twisted_ElGamal_CT &CT2)
{ 
    thread add_thread1(add_operation, std::ref(CT_result.X), std::ref(CT1.X), std::ref(CT2.X));
    thread add_thread2(add_operation, std::ref(CT_result.Y), std::ref(CT1.Y), std::ref(CT2.Y));

    add_thread1.join(); 
    add_thread2.join(); 
}

/* parallel homomorphic sub */
inline void sub_operation(EC_POINT *&RESULT, EC_POINT *&X, EC_POINT *&Y) 
{ 
    EC_POINT_sub_without_bnctx(RESULT, X, Y);  
}

void Twisted_ElGamal_Parallel_HomoSub(Twisted_ElGamal_CT &CT_result, Twisted_ElGamal_CT &CT1, Twisted_ElGamal_CT &CT2)
{ 
    thread sub_thread1(sub_operation, std::ref(CT_result.X), std::ref(CT1.X), std::ref(CT2.X));
    thread sub_thread2(sub_operation, std::ref(CT_result.Y), std::ref(CT1.Y), std::ref(CT2.Y));

    sub_thread1.join(); 
    sub_thread2.join(); 
}

/* parallel scalar operation */
void Twisted_ElGamal_Parallel_ScalarMul(Twisted_ElGamal_CT &CT_result, Twisted_ElGamal_CT &CT, BIGNUM *&k)
{ 
    thread scalar_thread1(exp_operation, std::ref(CT_result.X), std::ref(CT.X), std::ref(k));
    thread scalar_thread2(exp_operation, std::ref(CT_result.Y), std::ref(CT.Y), std::ref(k));
    
    // synchronize threads
    scalar_thread1.join(); 
    scalar_thread2.join(); 
}


/* Signature algorithm: compute SIG = Sign(sk, m; r) */ 
void Twisted_ElGamal_Sig(Twisted_ElGamal_PP &pp, 
                         BIGNUM* &sk, 
                         BIGNUM* &m, 
                         Twisted_ElGamal_SIG &SIG)
{

    // generate the random coins 
    BIGNUM *r = BN_new(); 
    BN_random(r);

    // generate the commit t 
    BIGNUM *t = BN_new(); 

    // begin encryption
    EC_POINT_mul(group, SIG.R, r, NULL, NULL, bn_ctx); // R = g^r
    Hash_ECP_to_BIGNUM(SIG.R, m, t);    //t = H(m,R)
    BN_mod_mul(t,t,sk,order,bn_ctx); // t = t*sk mod q q is order
    BN_mod_add_quick(SIG.z,r,t,order);

    
    BN_free(r); 
    BN_free(t); 

    #ifdef DEBUG
        cout << "twisted ElGamal signature finishes >>>"<< endl;
        Twisted_ElGamal_SIG_print(SIG); 
    #endif
}

/* Signature verify algorithm: compute output = Verify(pk, SIG, m) */ 
int Twisted_ElGamal_Ver(Twisted_ElGamal_PP &pp, 
                         EC_POINT* &pk, 
                         BIGNUM* &m,
                         Twisted_ElGamal_SIG &SIG)
{

    // generate the commit t 
    BIGNUM *t = BN_new(); 
    Hash_ECP_to_BIGNUM(SIG.R, m, t);    //t = H(m,R)

    //compute g^z
    EC_POINT* gz = EC_POINT_new(group);
    EC_POINT_mul(group, gz, SIG.z, NULL, NULL, bn_ctx); // gz = g^z

    //compute pk^t
    EC_POINT* pkt = EC_POINT_new(group);
    EC_POINT_mul(group, pkt, NULL, pk, t, bn_ctx); // pkt = pk^t
    EC_POINT_add(group,pkt,pkt,SIG.R,bn_ctx);

    #ifdef DEBUG
        cout << "twisted ElGamal verify finishes >>>"<< endl;
        ECP_print(gz);
        ECP_print(pkt);
        
    #endif

    if(!EC_POINT_cmp(group,gz,pkt,bn_ctx)){

        BN_free(t);
        EC_POINT_free(gz);
        EC_POINT_free(pkt);

        return 1;

    }

    
    EC_POINT_free(gz);
    EC_POINT_free(pkt);     
    BN_free(t); 

    return 0;

}

/* Encryption algorithm: compute PKE_CT = Enc(pk, m; r): with explicit randomness */ 
void Hashed_ElGamal_Enc(Twisted_ElGamal_PP &pp, 
                         EC_POINT* &pk, 
                         BIGNUM* &m, 
                         BIGNUM* &r, 
                         Hashed_ElGamal_CT &CT)
{ 
    // begin encryption
    EC_POINT_mul(group, CT.X, r, NULL, NULL, bn_ctx); // X = g^r 


    //compute pk^r
    EC_POINT* pkr = EC_POINT_new(group);
    EC_POINT_mul(group, pkr, NULL, pk, r, bn_ctx); // Y = Hash(pk^r) xor m

    BIGNUM *Hpkr = BN_new();
    Hash_ECP_to_BIGNUM(pkr,NULL,Hpkr); // Y = Hash(pk^r)

    unsigned char m_bin[2*BN_LEN+1] = {0};
    unsigned char Hpkr_bin[2*BN_LEN+1] = {0};
    unsigned char CTY_bin[2*BN_LEN+1] = {0};
    BN_bn2binpad(m,m_bin,2*BN_LEN+1);
    BN_bn2binpad(Hpkr,Hpkr_bin,2*BN_LEN+1);


    for (int i = 0; i < 2*BN_LEN+1; i++)
    {
        CTY_bin[i] = m_bin[i]^Hpkr_bin[i];
        /* code */
    }

    BN_bin2bn(CTY_bin,2*BN_LEN+1,CT.Y);

    EC_POINT_free(pkr);
    BN_free(Hpkr);

    
    #ifdef DEBUG
        cout << "Hashed ElGamal encryption finishes >>>"<< endl;
        Hashed_ElGamal_CT_print(CT); 
    #endif
}

/* Decryption algorithm: compute m = Dec(sk, CT) */ 
void Hashed_ElGamal_Dec(Twisted_ElGamal_PP &pp, 
                         BIGNUM* &sk, 
                         Hashed_ElGamal_CT &CT, 
                         BIGNUM* &m)
{
    // begin decryption

    EC_POINT* CXsk = EC_POINT_new(group);
    EC_POINT_mul(group, CXsk, NULL, CT.X, sk, bn_ctx); // CX^sk 

    BIGNUM *HCXsk = BN_new();
    Hash_ECP_to_BIGNUM(CXsk,NULL,HCXsk); // Y = Hash(pk^r)

    unsigned char m_bin[2*BN_LEN+1] = {0};
    unsigned char HCXsk_bin[2*BN_LEN+1] = {0};
    unsigned char CTY_bin[2*BN_LEN+1] = {0};    
    BN_bn2binpad(CT.Y,CTY_bin,2*BN_LEN+1);
    BN_bn2binpad(HCXsk,HCXsk_bin,2*BN_LEN+1);

    for (int i = 0; i < 2*BN_LEN+1; i++)
    {
        m_bin[i] = CTY_bin[i]^HCXsk_bin[i];
        
        /* code */
    }


    BN_bin2bn(m_bin,2*BN_LEN+1,m);

    EC_POINT_free(CXsk);
    BN_free(HCXsk);

     #ifdef DEBUG
        cout << "Hashed ElGamal decryption finishes >>>"<< endl;
        BN_print(m,"nessage:");
    #endif



}



#endif













