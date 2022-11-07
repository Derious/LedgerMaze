#include "../LedgerMaze/LedgerMaze.hpp"
#include "../backend/backend.hpp"
#define TESTIMES 10

void build_test_enviroment()
{
    SplitLine_print('-'); 
    cout << "Build test enviroment for LedgerMaze >>>" << endl; 
    SplitLine_print('-'); 
    cout << "Setup LedgerMaze system" << endl; 
    // setup LedgerMaze system
    size_t RANGE_LEN = 32; // set the range to be [0, 2^32-1]
    size_t AGG_NUM = 2; 
    size_t SN_LEN = 4; 
    size_t DEC_THREAD_NUM = 4;
    size_t IO_THREAD_NUM = 4; 
    size_t TUNNING = 7; 
    LedgerMaze_PP pp; 
    LedgerMaze_PP_new(pp, RANGE_LEN, AGG_NUM); 
    LedgerMaze_Setup(pp, RANGE_LEN, AGG_NUM, SN_LEN, DEC_THREAD_NUM, IO_THREAD_NUM, TUNNING); 
    string LedgerMaze_PP_file = "LedgerMaze.pp"; 
    LedgerMaze_PP_serialize(pp, LedgerMaze_PP_file); 
    LedgerMaze_Initialize(pp);

    // create accounts for Alice and Bob
    // SplitLine_print('-'); 
    // cout << "generate three accounts" << endl; 
    // SplitLine_print('-'); 

    // BIGNUM *alice_balance = BN_new(); 
    // BIGNUM *alice_sn = BN_new(); BN_one(alice_sn); 
    // BN_set_word(alice_balance, 512);
    // LedgerMaze_Account Acct_Alice; 
    // LedgerMaze_Account_new(Acct_Alice); 
    // LedgerMaze_Create_Account(pp, "zck", alice_balance, alice_sn, Acct_Alice); 
    // string Alice_ACCT_file = "zck"; 
    // LedgerMaze_Account_serialize(Acct_Alice, Alice_ACCT_file); 
    // LedgerMaze_Account_free(Acct_Alice);

    // BIGNUM *bob_balance = BN_new(); 
    // BIGNUM *bob_sn = BN_new(); BN_one(bob_sn); 
    // BN_set_word(bob_balance, 256); 
    // LedgerMaze_Account Acct_Bob; 
    // LedgerMaze_Account_new(Acct_Bob); 
    // LedgerMaze_Create_Account(pp, "Bob", bob_balance, bob_sn, Acct_Bob); 
    // string Bob_ACCT_file = "Bob"; 
    // LedgerMaze_Account_serialize(Acct_Bob, Bob_ACCT_file); 
    // LedgerMaze_Account_free(Acct_Bob);

    // BIGNUM *tax_balance = BN_new(); 
    // BIGNUM *tax_sn = BN_new(); BN_one(tax_sn); 
    // BN_set_word(tax_balance, 0); 
    // LedgerMaze_Account Acct_Tax; 
    // LedgerMaze_Account_new(Acct_Tax); 
    // LedgerMaze_Create_Account(pp, "Tax", tax_balance, tax_sn, Acct_Tax);  
    // string Tax_ACCT_file = "Tax"; 
    // LedgerMaze_Account_serialize(Acct_Tax, Tax_ACCT_file); 
    // LedgerMaze_Account_free(Acct_Tax); 

    LedgerMaze_PP_free(pp); 
} 

void emulate_ctx()
{
    size_t RANGE_LEN = 32; // set the range to be [0, 2^32-1]
    size_t AGG_NUM = 2; 
    
    LedgerMaze_PP pp; 
    LedgerMaze_PP_new(pp, RANGE_LEN, AGG_NUM); 
    LedgerMaze_PP_deserialize(pp, "LedgerMaze.pp"); 
    //LedgerMaze_Initialize(pp); 

    LedgerMaze_Account Acct_Alice; 

    LedgerMaze_Account_new(Acct_Alice); 
    LedgerMaze_Account_deserialize(Acct_Alice, "zck"); 
    //LedgerMaze_Account_print(Acct_Alice); 

    LedgerMaze_Account Acct_Bob; 
    LedgerMaze_Account_new(Acct_Bob); 
    LedgerMaze_Account_deserialize(Acct_Bob, "Bob"); 
    //LedgerMaze_Account_print(Acct_Bob); 

    LedgerMaze_Account Acct_Tax; 
    LedgerMaze_Account_new(Acct_Tax); 
    LedgerMaze_Account_deserialize(Acct_Tax, "Tax"); 
    //LedgerMaze_Account_print(Acct_Tax);

    cout << "begin to emulate transactions among Alice, Bob and Tax" << endl; 
    SplitLine_print('-'); 
    cout << "before transactions ......" << endl; 
    SplitLine_print('-');
     
    BIGNUM *v = BN_new(); 
    BIGNUM *v_redeem = BN_new(); 
    bool V_mint,V_redeem;
    cout << "1st Valid CTx" << endl;

    LedgerMaze_CTx ctx_mint,ctx_redeem;
    LedgerMaze_CTx_new(pp,ctx_mint,MINT); 
    LedgerMaze_CTx_new(pp,ctx_redeem,REDEEM); 

    LedgerMaze_CTx ctx1;
    LedgerMaze_CTx_new(pp,ctx1,MINT); 
    LedgerMaze_CTx ctx2;
    LedgerMaze_CTx_new(pp,ctx2,SENDCHEQUE);
    LedgerMaze_CTx ctx3;
    LedgerMaze_CTx_new(pp,ctx3,REFRESHCHEQUE);
    LedgerMaze_CTx ctx4;
    LedgerMaze_CTx_new(pp,ctx4,DEPOSITCHEQUE);
    LedgerMaze_Cheque cheque;
    LedgerMaze_Cheque_new(cheque);
    LedgerMaze_Cheque cheque2;
    LedgerMaze_Cheque_new(cheque2);
    BN_set_word(v, 10); 
    BN_set_word(v_redeem, 5);
    BIGNUM *new_pt_balance = BN_new();
    ZK_BALANCE_CT new_zk_balance;
    ZK_BALANCE_CT_new(new_zk_balance);
    cout<<endl;
    SplitLine_print('*');
    BN_print(v,"Alice PT->ZK");


    cout << "bench mint" << endl;
    auto start_time = chrono::steady_clock::now();
    for (int i = 0; i < 1; i++){
        LedgerMaze_Mint(pp,Acct_Alice,v,ctx_mint,new_pt_balance,new_zk_balance);
    } 
    auto end_time = chrono::steady_clock::now(); // end to count the time
    auto running_time = end_time - start_time;  
    cout << "LedgerMaze_Mint takes time = " 
        << chrono::duration <double, milli> (running_time).count()/1 << " ms" << endl;




    
    // V_mint=LedgerMaze_Verify_CTx(pp,ctx_mint,Acct_Alice.zk_balance.r,new_zk_balance.r,new_zk_balance.v);
    // if(V_mint)
    // {
    //     LedgerMaze_Update_Acct(Acct_Alice,new_pt_balance,new_zk_balance);
    //     cout << ">>>>>> This Mint trans is valid"<< endl; 
    // }
    // else    
    //     cout << ">>>>>> This Mint trans is invalid"<< endl; 
    // SplitLine_print('*');
    // cout<<endl;


    // LedgerMaze_CTx ctx_mint1;
    // LedgerMaze_CTx_new(pp,ctx_mint1,MINT); 
    // SplitLine_print('*');
    // BN_print(v,"Alice PT->ZK");
    // LedgerMaze_Mint(pp,Acct_Alice,v,ctx_mint1,new_pt_balance,new_zk_balance);
    // V_mint=LedgerMaze_Verify_CTx(pp,ctx_mint1,Acct_Alice.zk_balance.r,new_zk_balance.r,new_zk_balance.v);
    // if(V_mint)
    // {
    //     LedgerMaze_Update_Acct(Acct_Alice,new_pt_balance,new_zk_balance);
    //     cout << ">>>>>> This Mint trans is valid"<< endl;     
    // }
    // else
    //     cout << ">>>>>> This Mint trans is invalid"<< endl; 
    // SplitLine_print('*');
    // cout<<endl;



    SplitLine_print('*');
    BN_print(v_redeem,"Alice ZK->PT");


    cout << "bench LedgerMaze_Redeem" << endl;
    start_time = chrono::steady_clock::now();
    for (int i = 0; i < 1; i++){
        LedgerMaze_Redeem(pp,Acct_Alice,v_redeem,ctx_redeem,new_pt_balance,new_zk_balance);
    } 
    end_time = chrono::steady_clock::now(); // end to count the time
    running_time = end_time - start_time;  
    cout << "LedgerMaze_Redeem takes time = " 
        << chrono::duration <double, milli> (running_time).count()/1 << " ms" << endl;


    
    // V_redeem=LedgerMaze_Verify_CTx(pp,ctx_redeem,Acct_Alice.zk_balance.r,new_zk_balance.r,new_zk_balance.v);
    // if(V_redeem)
    // {
    //     LedgerMaze_Update_Acct(Acct_Alice,new_pt_balance,new_zk_balance);
    //     cout << ">>>>>> This Redeem trans is  valid" << endl; 
    // }
    // else
    //     cout << ">>>>>> This Redeem trans is invalid" << endl; 
    // SplitLine_print('*');
    // cout<<endl;


    SplitLine_print('*');
    BN_set_word(v, 1); 
    cout << "Alice is going to transfer "<< BN_bn2dec(v) << " coins(zk_balance) to Bob" << endl; 

    cout << "bench LedgerMaze_SendCheque" << endl;
    start_time = chrono::steady_clock::now();
    for (int i = 0; i < 1; i++){
        LedgerMaze_SendCheque(pp,Acct_Alice,Acct_Bob,v,ctx2,new_pt_balance,new_zk_balance,cheque);
    } 
    end_time = chrono::steady_clock::now(); // end to count the time
    running_time = end_time - start_time;  
    cout << "LedgerMaze_SendCheque takes time = " 
        << chrono::duration <double, milli> (running_time).count()/1 << " ms" << endl;   


    
    // cout << "checkout the trans_sendcheque if valid" << endl; 
    // if(!LedgerMaze_Verify_CTx(pp,ctx2,NULL,NULL,NULL))
    // {
    //     cout << ">>>>>> the trans_sendcheque is invalid!!" << endl;
    // }
    // else
    // {
    //     LedgerMaze_Update_Acct(Acct_Alice,new_pt_balance,new_zk_balance);
    //     cout << ">>>>>> the trans_sendcheque is  valid!!" << endl;
    // }    
    // cout << "Bob run the trans_RefreshCheque" << endl; 


    cout << "bench LedgerMaze_RefreshCheque" << endl;
    start_time = chrono::steady_clock::now();
    for (int i = 0; i < TESTIMES; i++){
        LedgerMaze_RefreshCheque(pp,Acct_Bob,ctx2,ctx3,cheque2);
    } 
    end_time = chrono::steady_clock::now(); // end to count the time
    running_time = end_time - start_time;  
    cout << "LedgerMaze_RefreshCheque takes time = " 
        << chrono::duration <double, milli> (running_time).count()/TESTIMES << " ms" << endl;   
    

    

    
    
    // cout << "checkout the trans_RefreshCheque if valid" << endl;
    // if(!LedgerMaze_Verify_CTx(pp,ctx3,NULL,NULL,NULL))
    // {
    //     cout << ">>>>>> the trans_sendcheque is invalid!!" << endl;
    // }
    // else
    //     cout << ">>>>>> the trans_refresh is valid!!" << endl;
    // SplitLine_print('*');
    // cout<<endl;

    // SplitLine_print('*');

    cout << "bench LedgerMaze_Depositcheque" << endl;
    start_time = chrono::steady_clock::now();
    for (int i = 0; i < 1; i++){
        LedgerMaze_Depositcheque(pp,Acct_Bob,cheque2,ctx4,new_pt_balance,new_zk_balance);
    } 
    end_time = chrono::steady_clock::now(); // end to count the time
    running_time = end_time - start_time;  
    cout << "LedgerMaze_Depositcheque takes time = " 
        << chrono::duration <double, milli> (running_time).count()/1 << " ms" << endl;   

    




    // if(!LedgerMaze_Verify_CTx(pp,ctx4,NULL,NULL,NULL))
    // {
    //     cout << ">>>>>> the trans_Depositcheque is invalid!!" << endl;
    // }
    // else
    // {
    //     LedgerMaze_Update_Acct(Acct_Bob,new_pt_balance,new_zk_balance);
    //     cout << ">>>>>> the trans_Depositcheque is valid!!" << endl;
    // }
    // SplitLine_print('*');

    // LedgerMaze_Create_CTx(pp, Acct_Alice, v, Acct_Bob.pk, ctx1);
    // LedgerMaze_Miner(pp, ctx1, Acct_Alice, Acct_Bob); 
    // SplitLine_print('-'); 

    // cout << "Wrong Case 1: Invalid CTx --- wrong encryption => equality proof will reject" << endl; 
    // LedgerMaze_CTx wrong_ctx1; LedgerMaze_CTx_new(wrong_ctx1);  
    // BN_set_word(v, 128); 
    // cout << "Alice is going to transfer "<< BN_bn2dec(v) << " to Bob" << endl; 
    // LedgerMaze_Create_CTx(pp, Acct_Alice, v, Acct_Bob.pk, wrong_ctx1);

    // EC_POINT* noisy = EC_POINT_new(group); 
    // ECP_random(noisy); 
    // EC_POINT_add(group, wrong_ctx1.transfer.X1, wrong_ctx1.transfer.X1, noisy, bn_ctx);
    // EC_POINT_free(noisy); 
    // LedgerMaze_Miner(pp, wrong_ctx1, Acct_Alice, Acct_Bob); 
    // LedgerMaze_CTx_free(wrong_ctx1); 
    // SplitLine_print('-'); 

    // cout << "Wrong Case 2: Invalid CTx --- wrong interval of transfer amount => range proof will reject" << endl; 
    // LedgerMaze_CTx wrong_ctx2; LedgerMaze_CTx_new(wrong_ctx2);  
    // BN_set_word(v, 4294967296); 
    // cout << "Alice is going to transfer "<< BN_bn2dec(v) << " to Bob" << endl; 
    // LedgerMaze_Create_CTx(pp, Acct_Alice, v, Acct_Bob.pk, wrong_ctx2);
    // LedgerMaze_Miner(pp, ctx1, Acct_Alice, Acct_Bob); 
    // LedgerMaze_CTx_free(wrong_ctx2); 
    // SplitLine_print('-'); 

    // cout << "Wrong Case 3: Invalid CTx --- balance is not enough => range proof will reject" << endl; 
    // LedgerMaze_CTx wrong_ctx3; LedgerMaze_CTx_new(wrong_ctx3);
    // BN_set_word(v, 385);  
    // cout << "Alice is going to transfer "<< BN_bn2dec(v) << " coins to Bob" << endl; 
    // LedgerMaze_Create_CTx(pp, Acct_Alice, v, Acct_Bob.pk, wrong_ctx3);
    // LedgerMaze_Miner(pp, wrong_ctx3, Acct_Alice, Acct_Bob); 
    // LedgerMaze_CTx_free(wrong_ctx3); 
    // SplitLine_print('-'); 

    // cout << "2nd Valid CTx" << endl; 
    // LedgerMaze_CTx ctx2; LedgerMaze_CTx_new(ctx2);
    // BN_set_word(v, 384);  
    // cout << "Alice is going to transfer "<< BN_bn2dec(v) << " coins to Bob" << endl; 
    // LedgerMaze_Create_CTx(pp, Acct_Alice, v, Acct_Bob.pk, ctx2);
    // LedgerMaze_Miner(pp, ctx2, Acct_Alice, Acct_Bob); 
    // SplitLine_print('-'); 

    // cout << "3rd Valid CTx" << endl; 
    // LedgerMaze_CTx ctx3; LedgerMaze_CTx_new(ctx3);
    // BN_set_word(v, 32);  
    // cout << "Bob is going to transfer "<< BN_bn2dec(v) << " coins to Tax" << endl; 
    // LedgerMaze_Create_CTx(pp, Acct_Bob, v, Acct_Tax.pk, ctx3);
    // LedgerMaze_Miner(pp, ctx3, Acct_Bob, Acct_Tax); 
    // SplitLine_print('-'); 

    // cout << "after transactions ......" << endl; 
    // SplitLine_print('-'); 
    // LedgerMaze_Account_print(Acct_Alice); 
    // LedgerMaze_Account_print(Acct_Bob); 
    // LedgerMaze_Account_print(Acct_Tax); 


    // cout << "begin to test extended auditing policies" << endl; 
    // SplitLine_print('-'); 
    // cout << "test open policy over " << Get_ctxfilename(ctx1) << endl; 
    // SplitLine_print('-'); 
    // OPEN_POLICY predicate_open; 
    // OPEN_POLICY_new(predicate_open); 

    // BN_set_word(predicate_open.v, 128);  
    // BN_print_dec(predicate_open.v, "Alice claims the transfer amount"); 
    // BN_print_dec(predicate_open.v, "Alice generates a proof for CTx transfer amount"); 
    // DLOG_Equality_Proof Alice_open_proof; 
    // NIZK_DLOG_Equality_Proof_new(Alice_open_proof); 

    // LedgerMaze_Justify_open_policy(pp, Acct_Alice, ctx1, predicate_open, Alice_open_proof);
    // if (LedgerMaze_Audit_open_policy(pp, Acct_Alice.pk, ctx1, predicate_open, Alice_open_proof))
    //     cout << "Alice succeeds to justify " << Get_ctxfilename(ctx1) << endl;
    // else cout << "Alice fails to justify " << Get_ctxfilename(ctx1) << endl;  
    // NIZK_DLOG_Equality_Proof_free(Alice_open_proof); 
    // SplitLine_print('-'); 

    // BN_set_word(predicate_open.v, 127);  
    // BN_print_dec(predicate_open.v, "Bob claims the transfer amount"); 
    // BN_print_dec(predicate_open.v, "Bob generates a proof for CTx transfer amount"); 
    // DLOG_Equality_Proof Bob_open_proof; 
    // NIZK_DLOG_Equality_Proof_new(Bob_open_proof); 

    // LedgerMaze_Justify_open_policy(pp, Acct_Bob, ctx1, predicate_open, Bob_open_proof);
    // if (LedgerMaze_Audit_open_policy(pp, Acct_Bob.pk, ctx1, predicate_open, Bob_open_proof))
    //     cout << "Bob succeeds to justify " << Get_ctxfilename(ctx1) << endl;
    // else cout << "Bob fails to justify " << Get_ctxfilename(ctx1) << endl;    
    // NIZK_DLOG_Equality_Proof_free(Bob_open_proof); 
    // OPEN_POLICY_free(predicate_open);  
    // SplitLine_print('-'); 

    // cout << "test rate policy over " << Get_ctxfilename(ctx1) << " and " << Get_ctxfilename(ctx3) << endl; 
    // SplitLine_print('-'); 
    // RATE_POLICY predicate_rate; 
    // RATE_POLICY_new(predicate_rate); 
    // BN_set_word(predicate_rate.t1, 1);
    // BN_set_word(predicate_rate.t2, 4);

    // DLOG_Equality_Proof rate_proof; 
    // NIZK_DLOG_Equality_Proof_new(rate_proof); 
    // LedgerMaze_Justify_rate_policy(pp, Acct_Bob, ctx1, ctx3, predicate_rate, rate_proof);
    // if(LedgerMaze_Audit_rate_policy(pp, Acct_Bob.pk, ctx1, ctx3, predicate_rate, rate_proof))
    //     cout << "Bob paid the tax according to the rule" << endl; 
    // else cout << "Bob did not pay the tax according to the rule" << endl;
    // NIZK_DLOG_Equality_Proof_free(rate_proof);  
    // RATE_POLICY_free(predicate_rate); 
    // SplitLine_print('-'); 

    // cout << "test limit policy over " << Get_ctxfilename(ctx1) << " and " << Get_ctxfilename(ctx2) << endl; 
    // SplitLine_print('-'); 
    // LIMIT_POLICY predicate_limit; 
    // predicate_limit.RANGE_LEN = 10;
    // cout << "limit = " << pow(2, predicate_limit.RANGE_LEN) -1 << endl;  
    
    // Gadget2_Proof limit_proof; 
    // Gadget2_Proof_new(limit_proof); 
    // vector<LedgerMaze_CTx> ctx_set; 
    // ctx_set.push_back(ctx1); ctx_set.push_back(ctx2); 
    // LedgerMaze_Justify_limit_policy(pp, Acct_Alice, ctx_set, predicate_limit, limit_proof);
    // if(LedgerMaze_Audit_limit_policy(pp, Acct_Alice.pk, ctx_set, predicate_limit, limit_proof))
    //     cout << "the sum of Alice's transfer amounts does not exceed limit" << endl; 
    // else cout << "the sum of Alice's transfer amounts exceeds limit" << endl;
    // Gadget2_Proof_free(limit_proof);  
    // SplitLine_print('-');  

    // predicate_limit.RANGE_LEN = 9;
    // cout << "limit = " << pow(2, predicate_limit.RANGE_LEN) -1 << endl;  
    
    // Gadget2_Proof_new(limit_proof);  
    // LedgerMaze_Justify_limit_policy(pp, Acct_Alice, ctx_set, predicate_limit, limit_proof);
    // if(LedgerMaze_Audit_limit_policy(pp, Acct_Alice.pk, ctx_set, predicate_limit, limit_proof))
    //     cout << "the sum of Alice's transfer amounts does not exceed limit" << endl; 
    // else cout << "the sum of Alice's transfer amounts exceeds limit" << endl;
    // Gadget2_Proof_free(limit_proof);  
    // SplitLine_print('-');  

    BN_free(v); 
    LedgerMaze_PP_free(pp);
    LedgerMaze_Account_new(Acct_Alice);  
    LedgerMaze_Account_new(Acct_Bob); 
    LedgerMaze_Account_new(Acct_Tax); 

    LedgerMaze_CTx_free(ctx_mint); 
    LedgerMaze_CTx_free(ctx_redeem); 
    // LedgerMaze_CTx_free(ctx2); 
    // LedgerMaze_CTx_free(ctx3); 
}



int main()
{
    // generate the system-wide public parameters   
    global_initialize(NID_secp256k1); 
    // setup the system and generate three accounts
    build_test_enviroment(); 

    Backend_start();
    
    // emulate transactions among Alice, Bob, and Tax office 
    //emulate_ctx(); 
    // size_t RANGE_LEN = 32; // set the range to be [0, 2^32-1]
    // size_t AGG_NUM = 2; 
    // LedgerMaze_PP pp; 
    // LedgerMaze_PP_new(pp, RANGE_LEN, AGG_NUM); 
    // LedgerMaze_PP_deserialize(pp, "LedgerMaze.pp"); 
    // LedgerMaze_Initialize(pp); 

    // for (int i = 0; i < 100; i++)
    // {
    //     emulate_ctx(); 
    //     /* code */
    // }
    




    // size_t RANGE_LEN = 32; // set the range to be [0, 2^32-1]
    // size_t AGG_NUM = 2; 
    
    // LedgerMaze_PP pp; 
    // LedgerMaze_PP_new(pp, RANGE_LEN, AGG_NUM); 
    // LedgerMaze_PP_deserialize(pp, "LedgerMaze.pp"); 
    // LedgerMaze_Initialize(pp); 
    // LedgerMaze_PP_print(pp);


    // LedgerMaze_Account Alice; 

    // LedgerMaze_Account_new(Alice); 
    
    // LedgerMaze_Account_deserialize(Alice, "zck"); 
    // LedgerMaze_Account_print(Alice); 

    
    global_finalize(); 

    return 0; 
}



