// #define DEBUG
#define TESTIMES 100
#include "../depends/twisted_elgamal/twisted_elgamal.hpp"
#include "../depends/bulletproofs/aggregate_bulletproof.hpp"
#include "../depends/nizk/nizk_plaintext_knowledge.hpp"
#include "../depends/nizk/nizk_plaintext_equality.hpp"
#include "../depends/nizk/nizk_dlog_equality.hpp"
#include "../LedgerMaze/LedgerMaze.hpp"



void generate_random_instance_witness_range(Bullet_PP &pp, 
                                      Bullet_Instance &instance, 
                                      Bullet_Witness &witness, 
                                      bool STATEMENT_FLAG)
{
    // if(STATEMENT_FLAG == true) cout << "generate a true statement pair" << endl; 
    // else cout << "generate a random statement (false with overwhelming probability)" << endl; 
    BIGNUM *exp = BN_new(); 
    BN_set_word(exp, pp.RANGE_LEN);

    BIGNUM *BN_range_size = BN_new(); 
    BN_mod_exp(BN_range_size, BN_2, exp, order, bn_ctx); 
    // cout << "range = [" << 0 << "," << BN_bn2hex(BN_range_size) <<")"<<endl; 
    for(auto i = 0; i < pp.AGG_NUM; i++)
    {
        BN_random(witness.r[i]);
        BN_random(witness.v[i]); 
        if (STATEMENT_FLAG == true){
            BN_mod(witness.v[i], witness.v[i], BN_range_size, bn_ctx);  
        }
        EC_POINT_mul(group, instance.C[i], witness.r[i], pp.h, witness.v[i], bn_ctx); 
    }
    // cout << "random instance generation finished" << endl; 
}

void generate_random_instance_witness_ptknow(Plaintext_Knowledge_PP &pp, 
                               Plaintext_Knowledge_Instance &instance, 
                               Plaintext_Knowledge_Witness &witness)
{
    // SplitLine_print('-');  
    // cout << ">>> generate a valid twisted elgamal ciphertext" << endl; 

    BN_random(witness.r); 
    BN_random(witness.v);

    instance.pk = EC_POINT_new(group); 
    ECP_random(instance.pk); 
    Twisted_ElGamal_PP enc_pp; 
    enc_pp.g = pp.g; 
    enc_pp.h = pp.h;  

    Twisted_ElGamal_CT CT; 
    Twisted_ElGamal_CT_new(CT); 
    Twisted_ElGamal_Enc(enc_pp, instance.pk, witness.v, witness.r, CT); 
    EC_POINT_copy(instance.X, CT.X); 
    EC_POINT_copy(instance.Y, CT.Y);
    Twisted_ElGamal_CT_free(CT);  
}
void generate_random_instance_witness_ptequal(Plaintext_Equality_PP &pp, 
                                Plaintext_Equality_Instance &instance, 
                                Plaintext_Equality_Witness &witness, 
                                bool flag)
                                
{
    // SplitLine_print('-');  
    // if (flag == true){
    //     cout << ">>> generate a well-formed 1-message 2-reciepient twisted elgamal ciphertext" << endl; 
    // }
    // else{
    //      cout << ">>> generate an ill-formed 1-message 2-reciepient twisted elgamal ciphertext" << endl; 
    // }

    BN_random(witness.r);
    BN_random(witness.v); 

    ECP_random(instance.pk1);
    ECP_random(instance.pk2);

    Twisted_ElGamal_PP enc_pp; 
    enc_pp.g = pp.g; 
    enc_pp.h = pp.h; 
    MR_Twisted_ElGamal_CT CT; 
    MR_Twisted_ElGamal_CT_new(CT); 
    MR_Twisted_ElGamal_Enc(enc_pp, instance.pk1, instance.pk2, witness.v, witness.r, CT); 
    
    EC_POINT_copy(instance.X1, CT.X1); 
    EC_POINT_copy(instance.X2, CT.X2); 
    EC_POINT_copy(instance.Y, CT.Y); 

    if(flag == false){
        EC_POINT *noisy = EC_POINT_new(group); 
        ECP_random(noisy);
        EC_POINT_add(group, instance.Y, instance.Y, noisy, bn_ctx);
        EC_POINT_free(noisy);
    } 
    //Twisted_ElGamal_PP_Free(enc_pp); 
    MR_Twisted_ElGamal_CT_free(CT); 
}
void generate_random_instance_witness_dlequal(DLOG_Equality_PP &pp, 
                                      DLOG_Equality_Instance &instance, 
                                      DLOG_Equality_Witness &witness, 
                                      bool flag)
{
    // generate a true statement (false with overwhelming probability)
    // SplitLine_print('-'); 
    // if (flag == true){
    //     cout << ">>> generate a DDH tuple" << endl;
    // }
    // else{
    //     cout << ">>> generate a random tuple" << endl; 
    // } 
    witness.w = BN_new(); 
    BN_random(witness.w);  

    instance.g1 = EC_POINT_new(group); 
    ECP_random(instance.g1); 
    instance.g2 = EC_POINT_new(group); 
    ECP_random(instance.g2);

    instance.h1 = EC_POINT_new(group); 
    EC_POINT_mul(group, instance.h1, NULL, instance.g1, witness.w, bn_ctx); 
    instance.h2 = EC_POINT_new(group); 
    EC_POINT_mul(group, instance.h2, NULL, instance.g2, witness.w, bn_ctx); 

    if(flag == false){
        EC_POINT *noisy = EC_POINT_new(group); 
        ECP_random(noisy);
        EC_POINT_add(group, instance.h2, instance.h2, noisy, bn_ctx);
        EC_POINT_free(noisy);
    } 
}

void bench_twisted_elgamal()
{
    cout << "============================== the twisted_elgamal bench(1 times) ==================================" << endl;
    global_initialize(NID_X9_62_prime256v1);  
    // SplitLine_print('-'); 
    cout << "----------------------------- the basic bench ------------------------" << endl;
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

    // cout << "Twisted ElGamal KeyGen>>>" << endl;     
    auto start_time = chrono::steady_clock::now();
    for (int i = 0; i < TESTIMES; i++){
        Twisted_ElGamal_KeyGen(pp, keypair); 
    } 
    auto end_time = chrono::steady_clock::now(); // end to count the time
    auto running_time = end_time - start_time;  
    cout << "Twisted ElGamal KeyGen takes time = " 
        << chrono::duration <double, milli> (running_time).count()/TESTIMES << " ms" << endl;
        
    Twisted_ElGamal_CT CT; 
    Twisted_ElGamal_CT_new(CT); 
    BIGNUM *m = BN_new(); 
    BIGNUM *m_prime = BN_new();
    cout << "----------------------------- the main bench ------------------------" << endl;
    BN_random(m); 
    BN_mod(m, m, pp.BN_MSG_SIZE, bn_ctx);
    start_time = chrono::steady_clock::now();
    for (int i = 0; i < TESTIMES; i++){
        Twisted_ElGamal_Enc_fixed(pp, keypair.pk, m, CT);
    } 
    end_time = chrono::steady_clock::now(); // end to count the time
    running_time = end_time - start_time;  
    cout << "Twisted ElGamal Enc takes time = " 
        << chrono::duration <double, milli> (running_time).count()/TESTIMES << " ms" << endl;
     
    start_time = chrono::steady_clock::now();
    for (int i = 0; i < TESTIMES; i++){
        Twisted_ElGamal_Parallel_Dec(pp, keypair.sk, CT, m_prime);
    } 
    end_time = chrono::steady_clock::now(); // end to count the time
    running_time = end_time - start_time;  
    cout << "Twisted ElGamal Dec takes time = " 
        << chrono::duration <double, milli> (running_time).count()/TESTIMES << " ms" << endl;
     
 
 
    Twisted_ElGamal_PP_free(pp); 
    Twisted_ElGamal_KP_free(keypair); 
    Twisted_ElGamal_CT_free(CT); 
    BN_free(m);
    BN_free(m_prime); 
    // test_twisted_elgamal();
    global_finalize();
}
void bench_schnorr()
{
    cout << "============================== the schnorr bench(1000 times)==================================" << endl;
    global_initialize(NID_X9_62_prime256v1); 
    
    DLOG_Equality_PP pp;
    NIZK_DLOG_Equality_PP_new(pp);  
    NIZK_DLOG_Equality_Setup(pp);
    DLOG_Equality_Instance instance; 
    NIZK_DLOG_Equality_Instance_new(instance); 
    DLOG_Equality_Witness witness; 
    NIZK_DLOG_Equality_Witness_new(witness); 
    DLOG_Equality_Proof proof; 
    NIZK_DLOG_Equality_Proof_new(proof); 

    string prove_transcript_str;
    string verify_transcript_str;  
    bool flag=true;
    // test the standard version
    prove_transcript_str = "";
    verify_transcript_str = ""; 
    generate_random_instance_witness_dlequal(pp, instance, witness, flag); 

    auto start_time = chrono::steady_clock::now(); // start to count the time
    for(int i=0;i<TESTIMES;i++)
    {
        NIZK_DLOG_Equality_Prove(pp, instance, witness, prove_transcript_str, proof);
    }
    auto end_time = chrono::steady_clock::now(); // end to count the time
    auto running_time = end_time - start_time;
    cout << "signature generation takes time = " 
    << chrono::duration <double, milli> (running_time).count() << " ms" << endl;
    
    start_time = chrono::steady_clock::now(); // start to count the time
    for(int i=0;i<TESTIMES;i++)
    {
        NIZK_DLOG_Equality_Verify(pp, instance, verify_transcript_str, proof);
    }
    end_time = chrono::steady_clock::now(); // end to count the time
    running_time = end_time - start_time;
    cout << "signature verification takes time = " 
    << chrono::duration <double, milli> (running_time).count() << " ms" << endl;
    

    NIZK_DLOG_Equality_PP_free(pp); 
    NIZK_DLOG_Equality_Instance_free(instance);
    NIZK_DLOG_Equality_Witness_free(witness);
    NIZK_DLOG_Equality_Proof_free(proof);

    global_finalize();  
}
void bench_bulletproof()
{
    cout << "============================== the pi_range bench(1 times)==================================" << endl;
    global_initialize(NID_X9_62_prime256v1);  

    size_t RANGE_LEN = 32; // range size
    size_t AGG_NUM = 2;  // number of sub-argument
    bool STATEMENT_FLAG=true;
    Bullet_PP pp; 
    Bullet_PP_new(pp, RANGE_LEN, AGG_NUM);  
    Bullet_Setup(pp, RANGE_LEN, AGG_NUM);
    Bullet_Instance instance; 
    Bullet_Witness witness; 
    Bullet_Proof proof; 
    Bullet_Instance_new(pp, instance); 
    Bullet_Witness_new(pp, witness); 
    Bullet_Proof_new(proof); 
    generate_random_instance_witness_range(pp, instance, witness, STATEMENT_FLAG); 
    string transcript_str; 
    
    auto start_time = chrono::steady_clock::now(); // start to count the time
    transcript_str = ""; 
    Bullet_Prove(pp, instance, witness, transcript_str, proof);
    auto end_time = chrono::steady_clock::now(); // end to count the time
    auto running_time = end_time - start_time;
    cout << "proof generation takes time = " 
    << chrono::duration <double, milli> (running_time).count() << " ms" << endl;

    start_time = chrono::steady_clock::now(); // start to count the time
    transcript_str = ""; 
    Bullet_Verify(pp, instance, transcript_str, proof);
    end_time = chrono::steady_clock::now(); // end to count the time
    running_time = end_time - start_time;
    cout << "proof verification takes time = " 
    << chrono::duration <double, milli> (running_time).count() << " ms" << endl;

    Bullet_PP_free(pp); 
    Bullet_Instance_free(instance); 
    Bullet_Witness_free(witness); 
    Bullet_Proof_free(proof); 

    global_finalize();
}
void  bench_pt_knowledge()
{
    cout << "============================== the pi_enc bench(1 times)==================================" << endl;
    global_initialize(NID_X9_62_prime256v1); 
    
    Plaintext_Knowledge_PP pp; 
    NIZK_Plaintext_Knowledge_PP_new(pp); 
    NIZK_Plaintext_Knowledge_Setup(pp);
    Plaintext_Knowledge_Instance instance;
    NIZK_Plaintext_Knowledge_Instance_new(instance);  
    Plaintext_Knowledge_Witness witness; 
    NIZK_Plaintext_Knowledge_Witness_new(witness); 
    Plaintext_Knowledge_Proof proof; 
    NIZK_Plaintext_Knowledge_Proof_new(proof); 

    generate_random_instance_witness_ptknow(pp, instance, witness); 

    string transcript_str; 

    auto start_time = chrono::steady_clock::now(); // start to count the time
    transcript_str = ""; 
    NIZK_Plaintext_Knowledge_Prove(pp, instance, witness, transcript_str, proof); 
    auto end_time = chrono::steady_clock::now(); // end to count the time
    auto running_time = end_time - start_time;
    cout << "proof generation takes time = " 
    << chrono::duration <double, milli> (running_time).count() << " ms" << endl;

    start_time = chrono::steady_clock::now(); // start to count the time
    transcript_str = ""; 
    NIZK_Plaintext_Knowledge_Verify(pp, instance, transcript_str, proof); 
    end_time = chrono::steady_clock::now(); // end to count the time
    running_time = end_time - start_time;
    cout << "proof verification takes time = " 
    << chrono::duration <double, milli> (running_time).count() << " ms" << endl;

    NIZK_Plaintext_Knowledge_PP_free(pp);
    NIZK_Plaintext_Knowledge_Instance_free(instance);
    NIZK_Plaintext_Knowledge_Witness_free(witness);
    NIZK_Plaintext_Knowledge_Proof_free(proof);

    global_finalize();  

}
void bench_pt_equal()
{
    cout << "============================== the pi_dualenc bench(1 times)==================================" << endl;
    global_initialize(NID_X9_62_prime256v1); 
    
    // test_nizk_plaintext_equality(true);
    // test_nizk_plaintext_equality(false); 
    bool flag=true;
    Plaintext_Equality_PP pp;
    NIZK_Plaintext_Equality_PP_new(pp);    
    NIZK_Plaintext_Equality_Setup(pp);
    Plaintext_Equality_Instance instance; 
    NIZK_Plaintext_Equality_Instance_new(instance); 
    Plaintext_Equality_Witness witness; 
    NIZK_Plaintext_Equality_Witness_new(witness); 
    Plaintext_Equality_Proof proof; 
    NIZK_Plaintext_Equality_Proof_new(proof); 
    string transcript_str; 
    generate_random_instance_witness_ptequal(pp, instance, witness, flag); 
    auto start_time = chrono::steady_clock::now(); // start to count the time
    transcript_str = ""; 
    NIZK_Plaintext_Equality_Prove(pp, instance, witness, transcript_str, proof); 
    auto end_time = chrono::steady_clock::now(); // end to count the time
    auto running_time = end_time - start_time;
    cout << "proof generation takes time = " 
    << chrono::duration <double, milli> (running_time).count() << " ms" << endl;

    start_time = chrono::steady_clock::now(); // start to count the time
    transcript_str = ""; 
    NIZK_Plaintext_Equality_Verify(pp, instance, transcript_str, proof);
    end_time = chrono::steady_clock::now(); // end to count the time
    running_time = end_time - start_time;
    cout << "proof verification takes time = " 
    << chrono::duration <double, milli> (running_time).count() << " ms" << endl;

    NIZK_Plaintext_Equality_PP_free(pp); 
    NIZK_Plaintext_Equality_Instance_free(instance);
    NIZK_Plaintext_Equality_Witness_free(witness);
    NIZK_Plaintext_Equality_Proof_free(proof);  

    global_finalize();  
}
void bench_dlog_equal()
{
    cout << "============================== the pi_random bench(1 times)==================================" << endl;
    global_initialize(NID_X9_62_prime256v1); 
    
    DLOG_Equality_PP pp;
    NIZK_DLOG_Equality_PP_new(pp);  
    NIZK_DLOG_Equality_Setup(pp);
    DLOG_Equality_Instance instance; 
    NIZK_DLOG_Equality_Instance_new(instance); 
    DLOG_Equality_Witness witness; 
    NIZK_DLOG_Equality_Witness_new(witness); 
    DLOG_Equality_Proof proof; 
    NIZK_DLOG_Equality_Proof_new(proof); 

    string prove_transcript_str;
    string verify_transcript_str;  
    bool flag=true;
    // test the standard version
    prove_transcript_str = "";
    verify_transcript_str = ""; 
    generate_random_instance_witness_dlequal(pp, instance, witness, flag); 

    auto start_time = chrono::steady_clock::now(); // start to count the time
    NIZK_DLOG_Equality_Prove(pp, instance, witness, prove_transcript_str, proof); 
    auto end_time = chrono::steady_clock::now(); // end to count the time
    auto running_time = end_time - start_time;
    cout << "proof generation takes time = " 
    << chrono::duration <double, milli> (running_time).count() << " ms" << endl;
    
    start_time = chrono::steady_clock::now(); // start to count the time
    NIZK_DLOG_Equality_Verify(pp, instance, verify_transcript_str, proof);
    end_time = chrono::steady_clock::now(); // end to count the time
    running_time = end_time - start_time;
    cout << "proof verification takes time = " 
    << chrono::duration <double, milli> (running_time).count() << " ms" << endl;
    

    NIZK_DLOG_Equality_PP_free(pp); 
    NIZK_DLOG_Equality_Instance_free(instance);
    NIZK_DLOG_Equality_Witness_free(witness);
    NIZK_DLOG_Equality_Proof_free(proof);

    global_finalize();  
}

void bench_hashed_elgamal()
{
    cout << "============================== the signature bench(1 times)==================================" << endl;
    global_initialize(NID_X9_62_prime256v1); 
    
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

    // cout << "Twisted ElGamal KeyGen>>>" << endl;     
    auto start_time = chrono::steady_clock::now();
    for (int i = 0; i < TESTIMES; i++){
        Twisted_ElGamal_KeyGen(pp, keypair); 
    } 
    auto end_time = chrono::steady_clock::now(); // end to count the time
    auto running_time = end_time - start_time;  
    cout << "Twisted ElGamal KeyGen takes time = " 
        << chrono::duration <double, milli> (running_time).count()/TESTIMES << " ms" << endl;

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

    start_time = chrono::steady_clock::now(); // start to count the time
    for(int i=0;i<TESTIMES;i++)
    {
        Twisted_ElGamal_Sig(pp,keypair.sk,m,SIG);
    }
    end_time = chrono::steady_clock::now(); // end to count the time
    running_time = (end_time - start_time);
    cout << "signature generation takes time = " 
    << chrono::duration <double, milli> (running_time).count()/TESTIMES << " ms" << endl;

    start_time = chrono::steady_clock::now(); // start to count the time
    for(int i=0;i<TESTIMES;i++)
    {
        Twisted_ElGamal_Ver(pp,keypair.pk,m,SIG);
    }
    end_time = chrono::steady_clock::now(); // end to count the time
    running_time = (end_time - start_time);
    cout << "signature verify takes time = " 
    << chrono::duration <double, milli> (running_time).count()/TESTIMES << " ms" << endl;

    


    // if(!Twisted_ElGamal_Ver(pp,keypair.pk,m,SIG)){
    //     cout << "Signature test fail!>>>" << endl;
    //     global_finalize();  
    //     return;
    // }



    SplitLine_print('-'); 



    
    cout << "============================== the PKE bench(1 times)==================================" << endl;
    BN_random(m); 
    BN_mod(m, m, pp.BN_MSG_SIZE, bn_ctx);


    BIGNUM *r = BN_new(); 
    BIGNUM *p = BN_new(); 
    BN_random(r); 

    start_time = chrono::steady_clock::now(); // start to count the time
    for(int i=0;i<TESTIMES;i++)
    {
        Hashed_ElGamal_Enc(pp,keypair.pk,m,r,PKECT);
    }
    end_time = chrono::steady_clock::now(); // end to count the time
    running_time = (end_time - start_time);
    cout << "PKE enc takes time = " 
    << chrono::duration <double, milli> (running_time).count()/TESTIMES << " ms" << endl;

    start_time = chrono::steady_clock::now(); // start to count the time
    for(int i=0;i<TESTIMES;i++)
    {
        Hashed_ElGamal_Dec(pp,keypair.sk,PKECT,p);
    }
    end_time = chrono::steady_clock::now(); // end to count the time
    running_time = (end_time - start_time);
    cout << "PKE dec takes time = " 
    << chrono::duration <double, milli> (running_time).count()/TESTIMES << " ms" << endl;

    global_finalize();  

}

void bench_commit()
{
    cout << "============================== the commit bench(1 times)==================================" << endl;
    LedgerMaze_PP pp;
    size_t RANGE_LEN = 32; // set the range to be [0, 2^32-1]
    size_t AGG_NUM = 2; 
    size_t SN_LEN = 4; 
    size_t DEC_THREAD_NUM = 4;
    size_t IO_THREAD_NUM = 4; 
    size_t TUNNING = 7; 
    LedgerMaze_PP_new(pp, RANGE_LEN, AGG_NUM); 
    LedgerMaze_Setup(pp, RANGE_LEN, AGG_NUM, SN_LEN, DEC_THREAD_NUM, IO_THREAD_NUM, TUNNING); 
    LedgerMaze_Initialize(pp);

    EC_POINT *r = EC_POINT_new(group); 
    BIGNUM *p = BN_new(); 
    ECP_random(r);
    BN_random(p); 

    auto start_time = chrono::steady_clock::now();
    for (int i = 0; i < TESTIMES; i++){
        LedgerMaze_Commit(pp,r,p);
    } 
    auto end_time = chrono::steady_clock::now(); // end to count the time
    auto running_time = end_time - start_time;  
    cout << "Twisted ElGamal KeyGen takes time = " 
        << chrono::duration <double, milli> (running_time).count()/TESTIMES << " ms" << endl;

    
}

void bench_oneoutofnProof()
{
    cout << "============================== the oneoutofnProof bench(1 times)==================================" << endl;
    One_Out_of_N_Proof_Instance instance;
    One_Out_of_N_Proof_Instance_new(instance);

    One_Out_of_N_Proof proof;
    One_Out_of_N_Proof_new(proof,instance);


    for (int t = 1; t < 65536; t=t*2)
    {
        cout << "============================== the oneoutofnProof bench(size = "<<t<<"==================================" << endl;
        auto start_time = chrono::steady_clock::now();
        for (int i = 0; i < TESTIMES; i++){
            One_Out_of_N_Proof_Prove(t,instance,proof);
        } 
        auto end_time = chrono::steady_clock::now(); // end to count the time
        auto running_time = end_time - start_time;  
        cout << "oneoutofnProof takes time = " 
        << chrono::duration <double, milli> (running_time).count()/TESTIMES << " ms" << endl;

        start_time = chrono::steady_clock::now();
        for (int i = 0; i < TESTIMES; i++){
            One_Out_of_N_Proof_Verify(proof,instance); 
        } 
        end_time = chrono::steady_clock::now(); // end to count the time
        running_time = end_time - start_time;  
        cout << "oneoutofnProof verify takes time = " 
        << chrono::duration <double, milli> (running_time).count()/TESTIMES << " ms" << endl;
        /* code */
    }
    

    


    

    


    One_Out_of_N_Proof_free(proof);
}

void bench_createAccount()
{
    cout << "============================== the commit bench(1 times)==================================" << endl;
    LedgerMaze_PP pp;
    size_t RANGE_LEN = 32; // set the range to be [0, 2^32-1]
    size_t AGG_NUM = 2; 
    size_t SN_LEN = 4; 
    size_t DEC_THREAD_NUM = 4;
    size_t IO_THREAD_NUM = 4; 
    size_t TUNNING = 7; 
    LedgerMaze_PP_new(pp, RANGE_LEN, AGG_NUM); 
    LedgerMaze_Setup(pp, RANGE_LEN, AGG_NUM, SN_LEN, DEC_THREAD_NUM, IO_THREAD_NUM, TUNNING); 
    LedgerMaze_Initialize(pp);
    BIGNUM *bob_balance = BN_new(); 
    BIGNUM *bob_sn = BN_new(); BN_one(bob_sn); 
    BN_set_word(bob_balance, 256); 
    LedgerMaze_Account Acct_Bob; 
    LedgerMaze_Account_new(Acct_Bob); 
    auto start_time = chrono::steady_clock::now();
    for (int i = 0; i < TESTIMES; i++){
        LedgerMaze_Create_Account(pp, "Bob", bob_balance, bob_sn, Acct_Bob); 
    } 
    auto end_time = chrono::steady_clock::now(); // end to count the time
    auto running_time = end_time - start_time;  
    cout << "Create_Account KeyGen takes time = " 
        << chrono::duration <double, milli> (running_time).count()/TESTIMES << " ms" << endl;
    

}

void bench_setup()
{
    cout << "============================== the setup bench(1 times)==================================" << endl;
    LedgerMaze_PP pp;
    size_t RANGE_LEN = 32; // set the range to be [0, 2^32-1]
    size_t AGG_NUM = 2; 
    size_t SN_LEN = 4; 
    size_t DEC_THREAD_NUM = 4;
    size_t IO_THREAD_NUM = 4; 
    size_t TUNNING = 7; 
    LedgerMaze_PP_new(pp, RANGE_LEN, AGG_NUM); 
    

    auto start_time = chrono::steady_clock::now();

    for (int i = 0; i < TESTIMES; i++){
            LedgerMaze_Setup(pp, RANGE_LEN, AGG_NUM, SN_LEN, DEC_THREAD_NUM, IO_THREAD_NUM, TUNNING); 
            //LedgerMaze_Initialize(pp);
        } 
    

    

    auto end_time = chrono::steady_clock::now(); // end to count the time
    auto running_time = end_time - start_time;  
    cout << "setup takes time = " 
        << chrono::duration <double, milli> (running_time).count()/TESTIMES << " ms" << endl;
    

}

int main()
{  
    global_initialize(NID_X9_62_prime256v1); 
    

    bench_setup();
    //bench_twisted_elgamal();
    //bench_schnorr();
    // bench_bulletproof();
    // bench_pt_knowledge();
    // bench_pt_equal();
    // bench_dlog_equal();
    //bench_createAccount();
    //bench_oneoutofnProof();

    //bench_hashed_elgamal();
    //bench_commit();
    
    return 0; 
}



