#define DEBUG

#include "../depends/twisted_elgamal/twisted_elgamal.hpp"

void test_signature()
{

    SplitLine_print('-'); 
    cout << "begin the basic correctness test >>>" << endl; 
    
    Twisted_ElGamal_PP pp; 
    Twisted_ElGamal_PP_new(pp);
    size_t MSG_LEN = 32; 
    size_t TUNNING = 7; 
    size_t DEC_THREAD_NUM = 4;
    size_t IO_THREAD_NUM = 4;      
    Twisted_ElGamal_Setup(pp, MSG_LEN, TUNNING, DEC_THREAD_NUM, IO_THREAD_NUM);
    Twisted_ElGamal_Initialize(pp); 

    Twisted_ElGamal_KP keypair;
    Twisted_ElGamal_KP_new(keypair); 
    Twisted_ElGamal_KeyGen(pp, keypair); 

    Twisted_ElGamal_SIG SIG;
    Twisted_ElGamal_SIG_new(SIG);

    Hashed_ElGamal_CT PKECT;
    Hashed_ElGamal_CT_new(PKECT);

    BIGNUM *m = BN_new(); 

    SplitLine_print('-'); 
    cout << "begin the signature test >>>" << endl; 

    BN_random(m); 
    BN_mod(m, m, pp.BN_MSG_SIZE, bn_ctx);
    //BN_print(m, "m");

    Twisted_ElGamal_Sig(pp,keypair.sk,m,SIG);
    if(!Twisted_ElGamal_Ver(pp,keypair.pk,m,SIG)){
        cout << "Signature test fail!>>>" << endl;
        
        return;
    }

    cout << "Signature test success!>>>" << endl;

    SplitLine_print('-'); 


    
    cout << "begin the PKE enc test >>>" << endl; 
    BN_random(m); 
    BN_mod(m, m, pp.BN_MSG_SIZE, bn_ctx);
    BN_print(m,"Plaintext:");

    BIGNUM *r = BN_new(); 
    BN_random(r); 
    //BN_mod(r, r, pp.BN_MSG_SIZE, bn_ctx);

    Hashed_ElGamal_Enc(pp,keypair.pk,m,r,PKECT);

    BIGNUM *p = BN_new(); 
    Hashed_ElGamal_Dec(pp,keypair.sk,PKECT,p);

}

void test_twisted_elgamal()
{
    SplitLine_print('-'); 
    cout << "begin the basic correctness test >>>" << endl; 
    
    Twisted_ElGamal_PP pp; 
    Twisted_ElGamal_PP_new(pp);
    size_t MSG_LEN = 32; 
    size_t TUNNING = 7; 
    size_t DEC_THREAD_NUM = 4;
    size_t IO_THREAD_NUM = 4;      
    Twisted_ElGamal_Setup(pp, MSG_LEN, TUNNING, DEC_THREAD_NUM, IO_THREAD_NUM);
    Twisted_ElGamal_Initialize(pp); 

    Twisted_ElGamal_KP keypair;
    Twisted_ElGamal_KP_new(keypair); 
    Twisted_ElGamal_KeyGen(pp, keypair); 

    Twisted_ElGamal_CT CT; 
    Twisted_ElGamal_CT_new(CT); 

    BIGNUM *m = BN_new(); 
    BIGNUM *m_prime = BN_new();

    /* random test */ 
    SplitLine_print('-'); 
    cout << "begin the random test >>>" << endl; 
    BN_random(m); 
    BN_mod(m, m, pp.BN_MSG_SIZE, bn_ctx);
    BN_print(m, "m"); 
    Twisted_ElGamal_Enc_fixed(pp, keypair.pk, m, CT);
    Twisted_ElGamal_Parallel_Dec(pp, keypair.sk, CT, m_prime); 
    BN_print(m_prime, "m'"); 

    // boundary test
    SplitLine_print('-'); 
    cout << "begin the left boundary test >>>" << endl; 
    BN_zero(m);
    BN_print(m, "m"); 
    Twisted_ElGamal_Enc_fixed(pp, keypair.pk, m, CT);
    Twisted_ElGamal_Parallel_Dec(pp, keypair.sk, CT, m_prime); 
    BN_print(m_prime, "m'"); 

    SplitLine_print('-'); 
    cout << "begin the right boundary test >>>" << endl; 
    BN_sub(m, pp.BN_MSG_SIZE, BN_1);  
    BN_print(m, "m");
    Twisted_ElGamal_Enc_fixed(pp, keypair.pk, m, CT);
    Twisted_ElGamal_Parallel_Dec(pp, keypair.sk, CT, m_prime); 
    BN_print(m_prime, "m'"); 
 
    Twisted_ElGamal_PP_free(pp); 
    Twisted_ElGamal_KP_free(keypair); 
    Twisted_ElGamal_CT_free(CT); 
    BN_free(m);
    BN_free(m_prime); 
}

int main()
{  
    // curve id = NID_secp256k1
    //global_initialize(NID_X9_62_prime256v1);    
    global_initialize(NID_secp256k1); 
    //test_twisted_elgamal();
    test_signature();
    // EC_POINT *ECP_startpoint = EC_POINT_new(group); 

    // ECP_random(ECP_startpoint);
    
    // BIGNUM *h = BN_new();  
    // BN_random(h);
    // BIGNUM *t = BN_new();  

    // clock_t start, finish;
	// start = clock();

    // for (int i = 0; i < 100; i++)
    // {
    //     BN_mod_mul(t,m,h,order,bn_ctx);
    //     //BN_print(t);
    //     /* code */
    // }
    
    

    // finish = clock();
	// cout << "程序执行时间：" << (double)(finish - start)/100 << endl;

    // BN_to_montgomery(m,m,bn_mont_ctx,bn_ctx);
    // BN_to_montgomery(h,h,bn_mont_ctx,bn_ctx);

    // for (int i = 0; i < 100; i++)
    // {
        
    //     BN_mod_mul_montgomery(t,m,h,bn_mont_ctx,bn_ctx);
        
    //     //BN_print(t);
    // }
    // BN_from_montgomery(t,t,bn_mont_ctx,bn_ctx);
    // finish = clock();
	// cout << "程序执行时间：" << (double)(finish - start)/100 << endl;
    
    // Hash_ECP_to_BIGNUM(ECP_startpoint,m,h);
    // BN_print(h);
    // Hash_ECP_to_BIGNUM(ECP_startpoint,m,h);
    // BN_print(h);
    global_finalize();
    
    return 0; 
}



