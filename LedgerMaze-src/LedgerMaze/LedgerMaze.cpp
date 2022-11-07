#include"LedgerMaze.h"

void LedgerMaze_Account_serialize(LedgerMaze_Account &user, string LedgerMaze_account_file)
{
    ofstream fout; 
    fout.open(LedgerMaze_account_file, ios::binary);
    fout.write((char *)(&user.identity), sizeof(user.identity));

    ECP_serialize(user.keypair.pk, fout);              
    BN_serialize(user.keypair.sk, fout);
    ZK_BALANCE_CT_serialize(user.zk_balance,fout);
    BN_serialize(user.pt_balance, fout);
    fout.close();  
};
void LedgerMaze_Account_deserialize(LedgerMaze_Account &user, string LedgerMaze_account_file)
{
    ifstream fin; 
    fin.open(LedgerMaze_account_file, ios::binary);
    fin.read((char *)(&user.identity), sizeof(user.identity));

    ECP_deserialize(user.keypair.pk, fin);              
    BN_deserialize(user.keypair.sk, fin);   
    ZK_BALANCE_CT_deserialize(user.zk_balance, fin);
    BN_deserialize(user.pt_balance, fin);
    fin.close();  
};
string Get_ctxfilename(LedgerMaze_CTx &newCTx)
{
    string ctx_file = ECP_ep2string(newCTx.pk1) + "_" + BN_bn2string(newCTx.sn)+".ctx"; 
    return ctx_file; 
}

void LedgerMaze_PP_print(LedgerMaze_PP &pp)
{
    SplitLine_print('-');
    cout << "pp content >>>>>>" << endl; 
    cout << "RANGE_LEN = " << pp.RANGE_LEN << endl; 
    cout << "LOG_RANGE_LEN = " << pp.LOG_RANGE_LEN << endl; 
    cout << "AGG_NUM = " << pp.AGG_NUM << endl; // number of sub-argument (for now, we require m to be the power of 2)

    cout << "SN_LEN = " << pp.SN_LEN << endl;  
    cout << "IO_THREAD_NUM = " << pp.IO_THREAD_NUM << endl; 
    cout << "DEC_THREAD_NUM = " << pp.DEC_THREAD_NUM << endl; 
    cout << "TUNNING = " << pp.TUNNING << endl; 

    ECP_print(pp.g, "g"); 
    ECP_print(pp.h, "h");
    ECP_print(pp.u, "u"); 
    ECP_vec_print(pp.vec_g, "vec_g"); 
    ECP_vec_print(pp.vec_h, "vec_h"); 
    
    SplitLine_print('-'); 
}

void LedgerMaze_Account_print(LedgerMaze_Account &Acct)
{
    cout << Acct.identity << " account information >>> " << endl;     
    ECP_print(Acct.keypair.pk, "pk");
    BN_print(Acct.keypair.sk, "sk"); 
    BN_print(Acct.pt_balance, "pt_balance");   // current zk_balance
    cout << "zk_balance:" << endl; 
    ZK_BALANCE_CT_print(Acct.zk_balance);  // current zk_balance
    // BN_print_dec(Acct.m, "m");  // dangerous (should only be used for speeding up the proof generation)
    // BN_print(Acct.sn, "sn"); 
    SplitLine_print('-'); 
}

void LedgerMaze_CTx_print(LedgerMaze_CTx &newCTx)
{
    SplitLine_print('-');
    string ctx_file = Get_ctxfilename(newCTx);  
    cout << ctx_file << " content >>>>>>" << endl; 
    printf("type=%d\n",newCTx.type);
    BN_print(newCTx.sn,"sn");
    ECP_print(newCTx.pk1, "pk1"); 
    cout << endl;  
    cout << "rest_balance/refresh value >>>" << endl;
    Twisted_ElGamal_CT_print(newCTx.rest_balance);
    cout << endl; 

    switch (newCTx.type)
    {
        case MINT:
            cout << "pi_mint_pi_random >>>" << endl; 
            DLOG_Equality_Proof_print(newCTx.proof.pi_mint.pi_random);
            cout << "pi_mint_pi_range >>>" << endl; 
            Bullet_Proof_print(newCTx.proof.pi_mint.pi_range);
            cout << endl; 
            /* code */
            break;
        case REDEEM:
            cout << "pi_redeem_pi_random >>>" << endl; 
            DLOG_Equality_Proof_print(newCTx.proof.pi_redeem.pi_random);
            cout << "pi_redeem_pi_range >>>" << endl; 
            Bullet_Proof_print(newCTx.proof.pi_redeem.pi_range);
            cout << endl; 
            /* code */
            break;
        case SENDCHEQUE:
            cout << "transfer >>>" << endl;
            Twisted_ElGamal_CT_print(newCTx.transfer);
            cout << "aux >>>" << endl;
            Hashed_ElGamal_CT_print(newCTx.aux);
            cout << "pi1>>>" << endl; 
            cout << "pi_sendcheque.pi1.pi_enc >>>" << endl; 
            Plaintext_Knowledge_Proof_print(newCTx.proof.pi_sendcheque.pi1.pi_enc);
            cout << "pi_sendcheque.pi1.pi_range >>>" << endl; 
            Bullet_Proof_print(newCTx.proof.pi_sendcheque.pi1.pi_range);
            cout << "pi2>>>" << endl; 
            cout << "pi_sendcheque.pi2.pi_enc >>>" << endl; 
            Plaintext_Knowledge_Proof_print(newCTx.proof.pi_sendcheque.pi2.pi_enc);
            cout << "pi_sendcheque.pi2.pi_range >>>" << endl; 
            Bullet_Proof_print(newCTx.proof.pi_sendcheque.pi2.pi_range);
            cout << "pi3>>>" << endl; 
            cout << "pi_sendcheque.pi3.pi_dualenc >>>" << endl; 
            Plaintext_Equality_Proof_print(newCTx.proof.pi_sendcheque.pi3.pi_dualenc);
            cout << endl; 
            /* code */
            break;
        case DEPOSITCHEQUE:
            cout << "pi_depositcheque.pi_random >>>" << endl; 
            DLOG_Equality_Proof_print(newCTx.proof.pi_depositcheque.pi_random);
            cout << "pi_depositcheque.pi_range >>>" << endl; 
            Bullet_Proof_print(newCTx.proof.pi_depositcheque.pi_range);
            cout << endl; 
            /* code */
            break;
        case REFRESHCHEQUE:
            ECP_print(newCTx.tag, "tag"); 
            cout << "pi1>>>" << endl; 
            cout << "pi_refreshcheque.pi1.pi_oneoutofn1 >>>" << endl; 
            One_Out_of_N_Proof_print(newCTx.proof.pi_refreshcheque.pi1.pi_oneoutofn1);
            cout << "pi_refreshcheque.pi1.pi_oneoutofn2 >>>" << endl; 
            One_Out_of_N_Proof_print(newCTx.proof.pi_refreshcheque.pi1.pi_oneoutofn2);
            cout << "pi_refreshcheque.pi1.pi_oneoutofn3 >>>" << endl; 
            One_Out_of_N_Proof_print(newCTx.proof.pi_refreshcheque.pi1.pi_oneoutofn3);
            cout << "pi2>>>" << endl; 
            cout << "pi_refreshcheque.pi2.pi_enc >>>" << endl; 
            Plaintext_Knowledge_Proof_print(newCTx.proof.pi_refreshcheque.pi2.pi_enc);
            cout << "pi_refreshcheque.pi2.pi_range >>>" << endl; 
            Bullet_Proof_print(newCTx.proof.pi_refreshcheque.pi2.pi_range);
            cout << endl; 
            /* code */
            break;
        default:
            break;
    }
    cout << "signature of CT >>>" << endl;
    Twisted_ElGamal_SIG_print(newCTx.sig);

    SplitLine_print('-'); 
}

void LedgerMaze_PP_new(LedgerMaze_PP &pp, size_t RANGE_LEN, size_t AGG_NUM)
{
    pp.BN_MAXIMUM_COINS = BN_new(); 
    pp.g = EC_POINT_new(group); 
    pp.h = EC_POINT_new(group);    
    pp.u = EC_POINT_new(group);

    pp.vec_g.resize(RANGE_LEN*AGG_NUM); ECP_vec_new(pp.vec_g); 
    pp.vec_h.resize(RANGE_LEN*AGG_NUM); ECP_vec_new(pp.vec_h); 
}

void LedgerMaze_PP_free(LedgerMaze_PP &pp)
{
    BN_free(pp.BN_MAXIMUM_COINS); 
    EC_POINT_free(pp.g);
    EC_POINT_free(pp.h);
    EC_POINT_free(pp.u); // used for inside innerproduct statement
    ECP_vec_free(pp.vec_g); 
    ECP_vec_free(pp.vec_h); 
}


void LedgerMaze_Account_new(LedgerMaze_Account &newAcct)
{
    Twisted_ElGamal_KP_new(newAcct.keypair);
    ZK_BALANCE_CT_new(newAcct.zk_balance);  // current zk_balance
    newAcct.pt_balance = BN_new(); 
}

void LedgerMaze_Account_free(LedgerMaze_Account &newAcct)
{
    Twisted_ElGamal_KP_free(newAcct.keypair);
    ZK_BALANCE_CT_free(newAcct.zk_balance);  // current balance
    BN_free(newAcct.pt_balance);
}

void LedgerMaze_CTx_new(LedgerMaze_PP &pp,LedgerMaze_CTx &newCTx,int CTtype)
{
    newCTx.sn = BN_new(); 
    newCTx.pk1 = EC_POINT_new(group);
    newCTx.tag = EC_POINT_new(group); 
    newCTx.value = BN_new(); 
    newCTx.type=CTtype;
    Twisted_ElGamal_CT_new(newCTx.rest_balance);
    Twisted_ElGamal_CT_new(newCTx.transfer);
    Hashed_ElGamal_CT_new(newCTx.aux);
    Twisted_ElGamal_SIG_new(newCTx.sig);
    //for REFRESHCHEQUE type CT
    One_Out_of_N_Proof_Instance instance;
    One_Out_of_N_Proof_Instance_new(instance);

    Bullet_PP bullet_pp; 
    Get_Bullet_PP(pp, bullet_pp);
    switch (newCTx.type)
    {
        case MINT:
            NIZK_DLOG_Equality_Proof_new(newCTx.proof.pi_mint.pi_random);
            NIZK_DLOG_Equality_Instance_new(newCTx.proof.pi_mint.pi_random_instance);
            Bullet_Proof_new(newCTx.proof.pi_mint.pi_range);
            Bullet_Instance_new(bullet_pp,newCTx.proof.pi_mint.pi_range_instance);
            /* code */
            break;
        case REDEEM:
            NIZK_DLOG_Equality_Proof_new(newCTx.proof.pi_redeem.pi_random);
            NIZK_DLOG_Equality_Instance_new(newCTx.proof.pi_redeem.pi_random_instance);
            Bullet_Proof_new(newCTx.proof.pi_redeem.pi_range);
            Bullet_Instance_new(bullet_pp,newCTx.proof.pi_redeem.pi_range_instance);
            /* code */
            break;
        case SENDCHEQUE:
            NIZK_Plaintext_Knowledge_Proof_new(newCTx.proof.pi_sendcheque.pi1.pi_enc);
            NIZK_Plaintext_Knowledge_Instance_new(newCTx.proof.pi_sendcheque.pi1.pi_enc_instance);
            Bullet_Proof_new(newCTx.proof.pi_sendcheque.pi1.pi_range);
            Bullet_Instance_new(bullet_pp,newCTx.proof.pi_sendcheque.pi1.pi_range_instance);
            NIZK_Plaintext_Knowledge_Proof_new(newCTx.proof.pi_sendcheque.pi2.pi_enc);
            NIZK_Plaintext_Knowledge_Instance_new(newCTx.proof.pi_sendcheque.pi2.pi_enc_instance);
            Bullet_Proof_new(newCTx.proof.pi_sendcheque.pi2.pi_range);
            Bullet_Instance_new(bullet_pp,newCTx.proof.pi_sendcheque.pi2.pi_range_instance);
            NIZK_Plaintext_Equality_Proof_new(newCTx.proof.pi_sendcheque.pi3.pi_dualenc);
            NIZK_Plaintext_Equality_Instance_new(newCTx.proof.pi_sendcheque.pi3.pi_dualenc_instance);
            /* code */
            break;
        case DEPOSITCHEQUE:
            NIZK_DLOG_Equality_Proof_new(newCTx.proof.pi_depositcheque.pi_random);
            NIZK_DLOG_Equality_Instance_new(newCTx.proof.pi_depositcheque.pi_random_instance);
            Bullet_Proof_new(newCTx.proof.pi_depositcheque.pi_range);
            Bullet_Instance_new(bullet_pp,newCTx.proof.pi_depositcheque.pi_range_instance);

            /* code */
            break;
        case REFRESHCHEQUE:
            One_Out_of_N_Proof_new(newCTx.proof.pi_refreshcheque.pi1.pi_oneoutofn1,instance);
            One_Out_of_N_Proof_new(newCTx.proof.pi_refreshcheque.pi1.pi_oneoutofn2,instance);
            One_Out_of_N_Proof_new(newCTx.proof.pi_refreshcheque.pi1.pi_oneoutofn3,instance);
            NIZK_Plaintext_Knowledge_Proof_new(newCTx.proof.pi_refreshcheque.pi2.pi_enc);
            NIZK_Plaintext_Knowledge_Instance_new(newCTx.proof.pi_refreshcheque.pi2.pi_enc_instance);
            Bullet_Proof_new(newCTx.proof.pi_refreshcheque.pi2.pi_range);
            Bullet_Instance_new(bullet_pp,newCTx.proof.pi_refreshcheque.pi2.pi_range_instance);
            /* code */
            break;
        default:
            break;
    }

}

void LedgerMaze_CTx_free(LedgerMaze_CTx &newCTx)
{

    BN_free(newCTx.sn); 
    EC_POINT_free(newCTx.pk1);
    EC_POINT_free(newCTx.tag);
    BN_free(newCTx.value); 
    Twisted_ElGamal_CT_free(newCTx.rest_balance); 
    Twisted_ElGamal_CT_free(newCTx.transfer);
    Hashed_ElGamal_CT_free(newCTx.aux); 

    switch (newCTx.type)
    {
        case MINT:
            NIZK_DLOG_Equality_Proof_free(newCTx.proof.pi_mint.pi_random);
            NIZK_DLOG_Equality_Instance_free(newCTx.proof.pi_mint.pi_random_instance);
            Bullet_Proof_free(newCTx.proof.pi_mint.pi_range);
            Bullet_Instance_free(newCTx.proof.pi_mint.pi_range_instance);
            /* code */
            break;
        case REDEEM:
            NIZK_DLOG_Equality_Proof_free(newCTx.proof.pi_redeem.pi_random);
            NIZK_DLOG_Equality_Instance_free(newCTx.proof.pi_redeem.pi_random_instance);
            Bullet_Proof_free(newCTx.proof.pi_redeem.pi_range);
            Bullet_Instance_free(newCTx.proof.pi_redeem.pi_range_instance);
            /* code */
            break;
        case SENDCHEQUE:
            NIZK_Plaintext_Knowledge_Proof_free(newCTx.proof.pi_sendcheque.pi1.pi_enc);
            NIZK_Plaintext_Knowledge_Instance_free(newCTx.proof.pi_sendcheque.pi1.pi_enc_instance);
            Bullet_Proof_free(newCTx.proof.pi_sendcheque.pi1.pi_range);
            Bullet_Instance_free(newCTx.proof.pi_sendcheque.pi1.pi_range_instance);
            NIZK_Plaintext_Knowledge_Proof_free(newCTx.proof.pi_sendcheque.pi2.pi_enc);
            NIZK_Plaintext_Knowledge_Instance_free(newCTx.proof.pi_sendcheque.pi2.pi_enc_instance);
            Bullet_Proof_free(newCTx.proof.pi_sendcheque.pi2.pi_range);
            Bullet_Instance_free(newCTx.proof.pi_sendcheque.pi2.pi_range_instance);
            NIZK_Plaintext_Equality_Proof_free(newCTx.proof.pi_sendcheque.pi3.pi_dualenc);
            NIZK_Plaintext_Equality_Instance_free(newCTx.proof.pi_sendcheque.pi3.pi_dualenc_instance);
            /* code */
            break;
        case DEPOSITCHEQUE:
            NIZK_DLOG_Equality_Proof_free(newCTx.proof.pi_depositcheque.pi_random);
            NIZK_DLOG_Equality_Instance_free(newCTx.proof.pi_depositcheque.pi_random_instance);
            Bullet_Proof_free(newCTx.proof.pi_depositcheque.pi_range);
            Bullet_Instance_free(newCTx.proof.pi_depositcheque.pi_range_instance);

            /* code */
            break;
        case REFRESHCHEQUE:
            One_Out_of_N_Proof_free(newCTx.proof.pi_refreshcheque.pi1.pi_oneoutofn1);
            One_Out_of_N_Proof_free(newCTx.proof.pi_refreshcheque.pi1.pi_oneoutofn2);
            One_Out_of_N_Proof_free(newCTx.proof.pi_refreshcheque.pi1.pi_oneoutofn3);
            // One_Out_of_N_Proof_Instance_free()
            NIZK_Plaintext_Knowledge_Proof_new(newCTx.proof.pi_refreshcheque.pi2.pi_enc);
            NIZK_Plaintext_Knowledge_Instance_free(newCTx.proof.pi_refreshcheque.pi2.pi_enc_instance);
            Bullet_Proof_free(newCTx.proof.pi_refreshcheque.pi2.pi_range);
            Bullet_Instance_free(newCTx.proof.pi_refreshcheque.pi2.pi_range_instance);
            /* code */
            break;
        default:
            break;
    }
    
}
void LedgerMaze_Setup(LedgerMaze_PP &pp, size_t RANGE_LEN, size_t AGG_NUM, 
               size_t SN_LEN, size_t DEC_THREAD_NUM, size_t IO_THREAD_NUM, size_t TUNNING)
{
    pp.RANGE_LEN = RANGE_LEN; 
    pp.LOG_RANGE_LEN = log2(RANGE_LEN); 
    pp.AGG_NUM = AGG_NUM; 
    pp.SN_LEN = SN_LEN;
    pp.DEC_THREAD_NUM = DEC_THREAD_NUM;
    pp.IO_THREAD_NUM = IO_THREAD_NUM;  
    pp.TUNNING = TUNNING; 

    EC_POINT_copy(pp.g, generator); 
    Hash_ECP_to_ECP(pp.g, pp.h);  
    ECP_random(pp.u); // used for inside innerproduct statement
    
    BN_set_word(pp.BN_MAXIMUM_COINS, uint64_t(pow(2, pp.RANGE_LEN)));  

    ECP_vec_random(pp.vec_g); 
    ECP_vec_random(pp.vec_h);
}

void LedgerMaze_Initialize(LedgerMaze_PP &pp)
{
    cout << "Initialize LedgerMaze >>>" << endl; 
    Twisted_ElGamal_PP enc_pp; 
    Get_Enc_PP(pp, enc_pp);  
    Twisted_ElGamal_Initialize(enc_pp); 
    SplitLine_print('-'); 
}



void LedgerMaze_Create_Account(LedgerMaze_PP &pp, string identity, BIGNUM *&init_balance, BIGNUM *&sn, LedgerMaze_Account &newAcct)
{
    newAcct.identity = identity;
    BN_copy(newAcct.zk_balance.sn, sn);  
    Twisted_ElGamal_PP enc_pp;
    Get_Enc_PP(pp, enc_pp); // enc_pp.g = pp.g, enc_pp.h = pp.h;  

    Twisted_ElGamal_KP keypair; 
    Twisted_ElGamal_KP_new(keypair); 
    Twisted_ElGamal_KeyGen(enc_pp, keypair); // generate a keypair
    EC_POINT_copy(newAcct.zk_balance.pk, keypair.pk); 
    EC_POINT_copy(newAcct.keypair.pk, keypair.pk);
    BN_copy(newAcct.keypair.sk, keypair.sk);  
    Twisted_ElGamal_KP_free(keypair);  

    // BN_copy(newAcct.m, init_balance); 
    BN_copy(newAcct.pt_balance, init_balance); 

    // initialize account zk_balance with 0 coins
    BIGNUM *r = BN_new(); 
    Hash_String_to_BN(newAcct.identity, r); 
    BN_copy(newAcct.zk_balance.r, r); 
    BN_copy(newAcct.zk_balance.v, BN_0);
    Twisted_ElGamal_Enc(enc_pp, newAcct.zk_balance.pk, BN_0,r,newAcct.zk_balance.C);
    

    #ifdef DEMO
    cout << identity << "'s account creation succeeds" << endl;
    ECP_print(newAcct.zk_balance.pk, "pk"); 
    cout << identity << "'s initial pt_balance = "; 
    BN_print_dec(init_balance); 
    cout << identity << "'s initial zk_balance = "; 
    Twisted_ElGamal_CT_print(newAcct.zk_balance.C);
    cout << identity << "'s initial zk_balance(real value) = "; 
    BN_print_dec(BN_0); 
    SplitLine_print('-'); 
    #endif 
}

//output the update zk_balance and pt_balance,but don't update them in the Acct until verify success
// void LedgerMaze_Mint(LedgerMaze_PP &pp, LedgerMaze_Account &Acct, BIGNUM *&value,LedgerMaze_CTx &newCT,BIGNUM *&new_pt_balance,ZK_BALANCE_CT &new_zk_balance)
// {
//     #ifdef DEMO
//     cout << "begin to Mint and genetate CTx >>>>>>" << endl; 
//     #endif
//     SplitLine_print('-'); 

//     if(BN_cmp(value,BN_0)==0||BN_cmp(value, Acct.pt_balance)>0)
//     {
//         cout << "the mint transfer value should in (0,pt_balance]" << endl;
//         return;
//     }
//     Twisted_ElGamal_PP enc_pp;
//     Get_Enc_PP(pp, enc_pp);
//     //set CT fixed value
//     EC_POINT_copy(newCT.pk1,Acct.keypair.pk);
//     BN_copy(newCT.value, value);

//     BN_sub(new_pt_balance,Acct.pt_balance,value); //3. pt_balance*=pt_balance-Vtransfer

//     BIGNUM *r = BN_new(); //4. r*
//     BN_random(r); 

//     BIGNUM *new_realzk_balance = BN_new(); 
//     BN_add(new_realzk_balance,Acct.zk_balance.v,value);//5. Vu*=Vu+Vtransfer

//     //6. CVu*=ISE.Enc(pkU,Vu*;r*);
//     Twisted_ElGamal_Enc(enc_pp,Acct.keypair.pk,new_realzk_balance,r,newCT.rest_balance);
    
<<<<<<< HEAD
    BN_add(newCT.sn,Acct.zk_balance.sn,BN_1); //7. sn++

    //8. set zk_balance*
    Twisted_ElGamal_CT_copy(new_zk_balance.C,newCT.rest_balance);
    EC_POINT_copy(new_zk_balance.pk,Acct.keypair.pk);
    BN_copy(new_zk_balance.r,r);
    BN_copy(new_zk_balance.sn,newCT.sn);   //set sn++
    BN_copy(new_zk_balance.v,new_realzk_balance);


    //9. generate pi_mint
    BIGNUM *w_r = BN_new();
    BN_mod_sub(w_r, r, Acct.zk_balance.r, order, bn_ctx);  //w_r=r*-r;
    // generate NIZK proof for validity of new zk_balance and old zk_balance
    DLOG_Equality_PP dlogeq_pp; 
    NIZK_DLOG_Equality_PP_new(dlogeq_pp);
    Get_DLOG_Equality_PP(pp, dlogeq_pp);
    DLOG_Equality_Instance dlogeq_instance;
    NIZK_DLOG_Equality_Instance_new(dlogeq_instance); 
    //g1=g^w_r=(g^w_r*h^v)/h^v
    EC_POINT *v_H;
    EC_POINT_mul(group,v_H,NULL,enc_pp.h,value,bn_ctx); //bn=h^v
    EC_POINT_sub(dlogeq_instance.g1, newCT.rest_balance.Y, Acct.zk_balance.C.Y);//g1=g^w_r*h^v
    EC_POINT_sub(dlogeq_instance.g1, dlogeq_instance.g1,v_H);//g1=g^w_r
    //h1 = pk^w_r
    EC_POINT_sub(dlogeq_instance.h1, newCT.rest_balance.X, Acct.zk_balance.C.X);
    DLOG_Equality_Witness dlogeq_witness; 
    NIZK_DLOG_Equality_Witness_new(dlogeq_witness); 
    BN_copy(dlogeq_witness.w, w_r); 
    cout<< "****************" << endl;
    BN_print(dlogeq_witness.w,"w_r");
    string transcript_str = "";
    NIZK_DLOG_Equality_Prove(dlogeq_pp, dlogeq_instance, dlogeq_witness, transcript_str, newCT.proof.pi_mint.pi_random);
=======
//     BN_add(newCT.sn,Acct.zk_balance.sn,BN_1); //7. sn++

//     //8. set zk_balance*
//     Twisted_ElGamal_CT_copy(new_zk_balance.C,newCT.rest_balance);
//     EC_POINT_copy(new_zk_balance.pk,Acct.keypair.pk);
//     BN_copy(new_zk_balance.r,r);
//     BN_copy(new_zk_balance.sn,newCT.sn);   //set sn++
//     BN_copy(new_zk_balance.v,new_realzk_balance);


//     //9. generate pi_mint
//     BIGNUM *w_r = BN_new();
//     BN_mod_sub(w_r, r, Acct.zk_balance.r, order, bn_ctx);  //w_r=r*-r;
//     // generate NIZK proof for validity of new zk_balance and old zk_balance
//     DLOG_Equality_PP dlogeq_pp; 
//     NIZK_DLOG_Equality_PP_new(dlogeq_pp);
//     Get_DLOG_Equality_PP(pp, dlogeq_pp);
//     DLOG_Equality_Instance dlogeq_instance;
//     NIZK_DLOG_Equality_Instance_new(dlogeq_instance); 
//     //g1=g^w_r=(g^w_r*h^v)/h^v
//     EC_POINT *v_H;
//     EC_POINT_mul(group,v_H,NULL,enc_pp.h,value,bn_ctx); //bn=h^v
//     EC_POINT_sub(dlogeq_instance.g1, newCT.rest_balance.Y, Acct.zk_balance.C.Y);//g1=g^w_r*h^v
//     EC_POINT_sub(dlogeq_instance.g1, dlogeq_instance.g1,v_H);//g1=g^w_r
//     //h1 = pk^w_r
//     EC_POINT_sub(dlogeq_instance.h1, newCT.rest_balance.X, Acct.zk_balance.C.X);
//     DLOG_Equality_Witness dlogeq_witness; 
//     NIZK_DLOG_Equality_Witness_new(dlogeq_witness); 
//     BN_copy(dlogeq_witness.w, w_r); 
//     string transcript_str = "";
//     NIZK_DLOG_Equality_Prove(dlogeq_pp, dlogeq_instance, dlogeq_witness, transcript_str, newCT.proof.pi_mint.pi_random);
>>>>>>> bb983d65289d592b1e49a7d76fc4bae8853dcdac
    
//     // aggregated range proof for new_realzk_balance lie in the right range 
//     //newCv.Y=g^r*h^new_realzk_balance
//     Bullet_PP bullet_pp; 
//     Get_Bullet_PP(pp, bullet_pp);
//     Bullet_Instance bullet_instance;
//     Bullet_Instance_new(bullet_pp, bullet_instance); 
//     EC_POINT_copy(bullet_instance.C[0], newCT.rest_balance.Y);
//     Bullet_Witness bullet_witness; 
//     Bullet_Witness_new(bullet_pp, bullet_witness);
//     BN_copy(bullet_witness.r[0], r);
//     BN_copy(bullet_witness.v[0], new_realzk_balance);
    
//     Bullet_Prove(bullet_pp, bullet_instance, bullet_witness, transcript_str, newCT.proof.pi_mint.pi_range);
    
//     //convert to memo=(pkU,sn,Vtransfer,CVu*)
//     std::string memo(ECP_ep2string(newCT.pk1));
//     memo.append(BN_bn2hex((const BIGNUM*)newCT.sn));
//     memo.append(BN_bn2hex((const BIGNUM*)value)); 
//     char buffer[POINT_LEN*2+1] = {0};
//     Twisted_ElGamal_CT_to_string(newCT.rest_balance,buffer);
//     memo.append(reinterpret_cast<const char*>(buffer));
//     //pi_random
//     memo.append(ECP_ep2string(newCT.proof.pi_mint.pi_random.A1));
//     memo.append(ECP_ep2string(newCT.proof.pi_mint.pi_random.A2));
//     memo.append(BN_bn2hex(newCT.proof.pi_mint.pi_random.z));
//     //pi_range
//     memo.append(ECP_ep2string(newCT.proof.pi_mint.pi_range.A));
//     memo.append(ECP_ep2string(newCT.proof.pi_mint.pi_range.S));
//     memo.append(ECP_ep2string(newCT.proof.pi_mint.pi_range.T1));
//     memo.append(ECP_ep2string(newCT.proof.pi_mint.pi_range.T2));
//     memo.append(BN_bn2hex(newCT.proof.pi_mint.pi_range.taux));
//     memo.append(BN_bn2hex(newCT.proof.pi_mint.pi_range.mu));
//     memo.append(BN_bn2hex(newCT.proof.pi_mint.pi_range.tx));
//     for(int i=0;i<newCT.proof.pi_mint.pi_range.ip_proof.vec_L.size();i++)
//     {
//         memo.append(ECP_ep2string(newCT.proof.pi_mint.pi_range.ip_proof.vec_L[i]));
//     }
//     for(int i=0;i<newCT.proof.pi_mint.pi_range.ip_proof.vec_R.size();i++)
//     {
//         memo.append(ECP_ep2string(newCT.proof.pi_mint.pi_range.ip_proof.vec_R[i]));
//     }
//     memo.append(BN_bn2hex(newCT.proof.pi_mint.pi_range.ip_proof.a));
//     memo.append(BN_bn2hex(newCT.proof.pi_mint.pi_range.ip_proof.b));
//     BIGNUM *memo_bn;
//     Hash_String_to_BN(memo,memo_bn);
//     //10. sigma
//     Twisted_ElGamal_Sig(enc_pp,Acct.keypair.sk,memo_bn,newCT.sig);

//     #ifdef DEMO
//         cout << "the context of CTx >>>>>>" << endl; 
//         LedgerMaze_CTx_print(newCT);
//     #endif

// }

// void LedgerMaze_Redeem(LedgerMaze_PP &pp, LedgerMaze_Account &Acct, BIGNUM *&value,LedgerMaze_CTx &newCT,BIGNUM *&new_pt_balance,ZK_BALANCE_CT &new_zk_balance)
// {
//     #ifdef DEMO
//     cout << "begin to Redeem and genetate CTx >>>>>>" << endl; 
//     #endif
//     SplitLine_print('-'); 
//     //2.
//     if(BN_cmp(value,BN_0)==0||BN_cmp(value,Acct.zk_balance.v)>0)
//     {
//         cout << "the redeem transfer value should in (0,zk_balance.v]" << endl;
//         return;
//     }
//     Twisted_ElGamal_PP enc_pp;
//     Get_Enc_PP(pp, enc_pp);

//     //set CT fixed value
//     EC_POINT_copy(newCT.pk1,Acct.keypair.pk);
//     BN_copy(newCT.value, value);

//     //3. pt_balance*=pt_balance+Vtransfer
//     BN_sub(new_pt_balance,Acct.pt_balance,value);

//     //4. r*
//     BIGNUM *r = BN_new(); 
//     BN_random(r); 

//     //5. Vu*=Vu-Vtransfer
//     BIGNUM *new_realzk_balance = BN_new(); 
//     BN_sub(new_realzk_balance,Acct.zk_balance.v,value);

//     //6. CVu*=ISE.Enc(pkU,Vu*;r*);
//     Twisted_ElGamal_Enc(enc_pp,Acct.keypair.pk,new_realzk_balance,r,newCT.rest_balance);


//     //7. sn++
//     BN_add(newCT.sn,Acct.zk_balance.sn,BN_1);

//     //8. set zk_balance*
//     Twisted_ElGamal_CT_copy(new_zk_balance.C,newCT.rest_balance);
//     EC_POINT_copy(new_zk_balance.pk,Acct.keypair.pk);
//     BN_copy(new_zk_balance.r,r);
//     BN_copy(new_zk_balance.sn,newCT.sn);   //set sn++
//     BN_copy(new_zk_balance.v,new_realzk_balance);

//     //9. generate pi_redeem
//     BIGNUM *w_r = BN_new();
//     BN_mod_sub(w_r, r, Acct.zk_balance.r, order, bn_ctx); //w_r=r*-r mod n;
//     // generate NIZK proof for validity of new zk_balance and old zk_balance
//     DLOG_Equality_PP dlogeq_pp; 
//     NIZK_DLOG_Equality_PP_new(dlogeq_pp);
//     Get_DLOG_Equality_PP(pp, dlogeq_pp);
//     DLOG_Equality_Instance dlogeq_instance;
//     NIZK_DLOG_Equality_Instance_new(dlogeq_instance); 
//     //g1=g^w_r=(g^w_r*h^v)/h^v
//     EC_POINT *v_H;
//     EC_POINT_mul(group,v_H,NULL,enc_pp.h,value,bn_ctx); //bn=h^v
//     EC_POINT_sub(dlogeq_instance.g1, newCT.rest_balance.Y, Acct.zk_balance.C.Y);//g1=g^w_r*h^v
//     EC_POINT_sub(dlogeq_instance.g1, dlogeq_instance.g1,v_H);//g1=g^w_r
//     //h1 = pk^w_r
//     EC_POINT_sub(dlogeq_instance.h1, newCT.rest_balance.X, Acct.zk_balance.C.X);
//     DLOG_Equality_Witness dlogeq_witness; 
//     NIZK_DLOG_Equality_Witness_new(dlogeq_witness); 
//     BN_copy(dlogeq_witness.w, w_r); 
//     string transcript_str = "";
//     NIZK_DLOG_Equality_Prove(dlogeq_pp, dlogeq_instance, dlogeq_witness, transcript_str, newCT.proof.pi_mint.pi_random);

//     // aggregated range proof for new_realzk_balance lie in the right range 
//     //newCv.Y=g^r*h^new_realzk_balance
//     Bullet_PP bullet_pp; 
//     Get_Bullet_PP(pp, bullet_pp);
//     Bullet_Instance bullet_instance;
//     Bullet_Instance_new(bullet_pp, bullet_instance); 
//     EC_POINT_copy(bullet_instance.C[0], newCT.rest_balance.Y);
//     Bullet_Witness bullet_witness; 
//     Bullet_Witness_new(bullet_pp, bullet_witness);
//     BN_copy(bullet_witness.r[0], r);
//     BN_copy(bullet_witness.v[0], new_realzk_balance);
    
//     Bullet_Prove(bullet_pp, bullet_instance, bullet_witness, transcript_str, newCT.proof.pi_mint.pi_range);

//     //convert to memo
//     std::string memo(ECP_ep2string(newCT.pk1));
//     memo.append(BN_bn2hex((const BIGNUM*)newCT.sn));
//     memo.append(BN_bn2hex((const BIGNUM*)value)); 
//     char buffer[POINT_LEN*2+1] = {0};
//     Twisted_ElGamal_CT_to_string(newCT.rest_balance,buffer);
//     memo.append(reinterpret_cast<const char*>(buffer));
//     //pi_random
//     memo.append(ECP_ep2string(newCT.proof.pi_mint.pi_random.A1));
//     memo.append(ECP_ep2string(newCT.proof.pi_mint.pi_random.A2));
//     memo.append(BN_bn2hex(newCT.proof.pi_mint.pi_random.z));
//     //pi_range
//     memo.append(ECP_ep2string(newCT.proof.pi_mint.pi_range.A));
//     memo.append(ECP_ep2string(newCT.proof.pi_mint.pi_range.S));
//     memo.append(ECP_ep2string(newCT.proof.pi_mint.pi_range.T1));
//     memo.append(ECP_ep2string(newCT.proof.pi_mint.pi_range.T2));
//     memo.append(BN_bn2hex(newCT.proof.pi_mint.pi_range.taux));
//     memo.append(BN_bn2hex(newCT.proof.pi_mint.pi_range.mu));
//     memo.append(BN_bn2hex(newCT.proof.pi_mint.pi_range.tx));
//     for(int i=0;i<newCT.proof.pi_mint.pi_range.ip_proof.vec_L.size();i++)
//     {
//         memo.append(ECP_ep2string(newCT.proof.pi_mint.pi_range.ip_proof.vec_L[i]));
//     }
//     for(int i=0;i<newCT.proof.pi_mint.pi_range.ip_proof.vec_R.size();i++)
//     {
//         memo.append(ECP_ep2string(newCT.proof.pi_mint.pi_range.ip_proof.vec_R[i]));
//     }
//     memo.append(BN_bn2hex(newCT.proof.pi_mint.pi_range.ip_proof.a));
//     memo.append(BN_bn2hex(newCT.proof.pi_mint.pi_range.ip_proof.b));
//     BIGNUM *memo_bn;
//     Hash_String_to_BN(memo,memo_bn);
//     //10. sigma
//     Twisted_ElGamal_Sig(enc_pp,Acct.keypair.sk,memo_bn,newCT.sig);

//     #ifdef DEMO
//         cout << "the context of CTx >>>>>>" << endl; 
//         LedgerMaze_CTx_print(newCT);
//     #endif

// }



//output the update zk_balance and pt_balance,but don't update them in the Acct until verify success
//SendCheque 
void LedgerMaze_SendCheque(LedgerMaze_PP &pp, LedgerMaze_Account &AcctU, LedgerMaze_Account &AcctR, BIGNUM *&Vcheque, 
LedgerMaze_CTx &newCT,BIGNUM *&new_pt_balance,ZK_BALANCE_CT &new_zk_balance, LedgerMaze_Cheque &ChequeR)
{

    #ifdef DEMO
    cout << "begin to SendCheque and genetate CTx >>>>>>" << endl; 
    #endif
    SplitLine_print('-'); 
    if(BN_cmp(Vcheque,BN_0)==0||BN_cmp(Vcheque,AcctU.zk_balance.v)>0)
    {
        cout << "the send value should in (0,zk_balance]" << endl;
        return;
    }


    BIGNUM *r, *r1, *r2, *Vnew;
    string transcript_str; 
    r1 = BN_new();
    r2 = BN_new();
    Vnew = BN_new();
    BN_copy(r,AcctU.zk_balance.r);
    BN_random(r1);
    BN_random(r2);

    BN_sub(Vnew,AcctU.zk_balance.v,Vcheque);

    Twisted_ElGamal_PP enc_pp;
    Get_Enc_PP(pp, enc_pp);

    //newCv=ISE.Enc(pk_U,Vnew,r1);
    Twisted_ElGamal_CT newCv;
    Twisted_ElGamal_CT_new(newCv);
    Twisted_ElGamal_Enc(enc_pp,AcctU.keypair.pk,Vnew,r1,newCv);

    //Cv_cheque=ISE.Enc(pk_R,Vcheque,r2);
    Twisted_ElGamal_CT Cv_cheque;
    Twisted_ElGamal_CT_new(Cv_cheque);   
    Twisted_ElGamal_Enc(enc_pp,AcctR.keypair.pk,Vcheque,r2,Cv_cheque);
 

    //set new_zk_balance
    Twisted_ElGamal_CT_copy(new_zk_balance.C,newCv);
    EC_POINT_copy(new_zk_balance.pk,AcctU.zk_balance.pk);
    BN_copy(new_zk_balance.r,r1);
    BN_copy(new_zk_balance.sn,AcctU.zk_balance.sn);
    BN_copy(new_zk_balance.v,Vnew);

    //set ChequeR
    Twisted_ElGamal_CT_copy(ChequeR.C,Cv_cheque);
    EC_POINT_copy(ChequeR.pk, AcctR.keypair.pk);
    BN_copy(ChequeR.r,r1);
    BN_copy(ChequeR.transfer,Vcheque);

    //set CT.aux = PKE.enc(pkR,r2,Vcheque)
    Hashed_ElGamal_CT PKECT;
    Hashed_ElGamal_CT_new(PKECT);

    //encode (Vcheque,r2)
    EC_POINT* Vcheque_r2;
    BIGNUM* Vcheque_r2_bn;
    Vcheque_r2_bn = BN_new();
    Vcheque_r2 = EC_POINT_new(group);

    EC_POINT_set_affine_coordinates(group,Vcheque_r2,Vcheque,r2,bn_ctx);

    EC_POINT_point2bn(group,Vcheque_r2,POINT_CONVERSION_UNCOMPRESSED,Vcheque_r2_bn,bn_ctx);


    Hashed_ElGamal_Enc(enc_pp,AcctR.keypair.pk, Vcheque_r2_bn, r2, PKECT);
    Hashed_ElGamal_CT_copy(newCT.aux,PKECT);

    //compute Pi_sendcheque = NIZK{(r,r1,r2,vU,vU⋆,Vcheque)

    //Pi1.pi_enc
    Plaintext_Knowledge_PP PI1_enc_pp; 
    NIZK_Plaintext_Knowledge_PP_new(PI1_enc_pp); 
    NIZK_Plaintext_Knowledge_Setup(PI1_enc_pp);
    // Plaintext_Knowledge_Instance PI1_enc_instance;
    // NIZK_Plaintext_Knowledge_Instance_new(PI1_enc_instance);  
    EC_POINT_copy(newCT.proof.pi_sendcheque.pi1.pi_enc_instance.pk,AcctU.keypair.pk);
    EC_POINT_copy(newCT.proof.pi_sendcheque.pi1.pi_enc_instance.X,newCv.X);
    EC_POINT_copy(newCT.proof.pi_sendcheque.pi1.pi_enc_instance.Y,newCv.Y);
    Plaintext_Knowledge_Witness PI1_enc_witness; 
    NIZK_Plaintext_Knowledge_Witness_new(PI1_enc_witness); 
    BN_copy(PI1_enc_witness.r,r1);
    BN_copy(PI1_enc_witness.v,Vnew);
    transcript_str = ""; 
    NIZK_Plaintext_Knowledge_Prove(PI1_enc_pp,newCT.proof.pi_sendcheque.pi1.pi_enc_instance,PI1_enc_witness,transcript_str,newCT.proof.pi_sendcheque.pi1.pi_enc);

    //Pi1.pi_range
    Bullet_PP PI1_range_pp; 
    Bullet_PP_new(PI1_range_pp,pp.RANGE_LEN,pp.AGG_NUM);
    Get_Bullet_PP(pp, PI1_range_pp);
    // Bullet_Instance PI1_range_instance;
    // Bullet_Instance_new(PI1_range_pp, PI1_range_instance); 
    EC_POINT_copy(newCT.proof.pi_sendcheque.pi1.pi_range_instance.C[0], newCv.Y);
    Bullet_Witness PI1_range_witness; 
    Bullet_Witness_new(PI1_range_pp, PI1_range_witness);
    BN_copy(PI1_range_witness.r[0], r1);
    BN_copy(PI1_range_witness.v[0], Vnew);
    transcript_str = "";
    Bullet_Prove(PI1_range_pp,newCT.proof.pi_sendcheque.pi1.pi_range_instance,PI1_range_witness,transcript_str,newCT.proof.pi_sendcheque.pi1.pi_range);

    //Pi2.pi_enc
    Plaintext_Knowledge_PP PI2_enc_pp; 
    NIZK_Plaintext_Knowledge_PP_new(PI2_enc_pp); 
    NIZK_Plaintext_Knowledge_Setup(PI2_enc_pp);
    // Plaintext_Knowledge_Instance PI2_enc_instance;
    // NIZK_Plaintext_Knowledge_Instance_new(PI2_enc_instance);  
    EC_POINT_copy(newCT.proof.pi_sendcheque.pi2.pi_enc_instance.pk,AcctR.keypair.pk);
    EC_POINT_copy(newCT.proof.pi_sendcheque.pi2.pi_enc_instance.X,Cv_cheque.X);
    EC_POINT_copy(newCT.proof.pi_sendcheque.pi2.pi_enc_instance.Y,Cv_cheque.Y);
    Plaintext_Knowledge_Witness PI2_enc_witness; 
    NIZK_Plaintext_Knowledge_Witness_new(PI2_enc_witness); 
    BN_copy(PI2_enc_witness.r,r2);
    BN_copy(PI2_enc_witness.v,Vcheque);
    transcript_str = ""; 
    NIZK_Plaintext_Knowledge_Prove(PI2_enc_pp,newCT.proof.pi_sendcheque.pi2.pi_enc_instance,PI2_enc_witness,transcript_str,newCT.proof.pi_sendcheque.pi2.pi_enc);

    //Pi2.pi_range
    Bullet_PP PI2_range_pp; 
    Bullet_PP_new(PI2_range_pp,pp.RANGE_LEN,pp.AGG_NUM);
    Get_Bullet_PP(pp, PI2_range_pp);
    // Bullet_Instance PI2_range_instance;
    // Bullet_Instance_new(PI2_range_pp, PI2_range_instance); 
    EC_POINT_copy(newCT.proof.pi_sendcheque.pi2.pi_range_instance.C[0], Cv_cheque.Y);
    Bullet_Witness PI2_range_witness; 
    Bullet_Witness_new(PI2_range_pp, PI2_range_witness);
    BN_copy(PI2_range_witness.r[0], r2);
    BN_copy(PI2_range_witness.v[0], Vcheque);
    transcript_str = "";
    Bullet_Prove(PI2_range_pp,newCT.proof.pi_sendcheque.pi2.pi_range_instance,PI2_range_witness,transcript_str,newCT.proof.pi_sendcheque.pi2.pi_range);


    //Pi3.pi_dualenc
    Plaintext_Equality_PP PI3_dualenc_pp;
    NIZK_Plaintext_Equality_PP_new(PI3_dualenc_pp);    
    NIZK_Plaintext_Equality_Setup(PI3_dualenc_pp);

    // Plaintext_Equality_Instance PI3_dualenc_instance; 
    // NIZK_Plaintext_Equality_Instance_new(PI3_dualenc_instance); 

    MR_Twisted_ElGamal_CT MR_CT; 
    MR_Twisted_ElGamal_CT_new(MR_CT); 

    Plaintext_Equality_Witness PI3_dualenc_witness; 
    NIZK_Plaintext_Equality_Witness_new(PI3_dualenc_witness); 
    BN_copy(PI3_dualenc_witness.v,Vcheque);
    BN_copy(PI3_dualenc_witness.r,r2);

    MR_Twisted_ElGamal_Enc(enc_pp, AcctU.keypair.pk, AcctR.keypair.pk, PI3_dualenc_witness.v, PI3_dualenc_witness.r, MR_CT); 
    
    EC_POINT_copy(newCT.proof.pi_sendcheque.pi3.pi_dualenc_instance.X1, MR_CT.X1); 
    EC_POINT_copy(newCT.proof.pi_sendcheque.pi3.pi_dualenc_instance.X2, MR_CT.X2); 
    EC_POINT_copy(newCT.proof.pi_sendcheque.pi3.pi_dualenc_instance.Y, MR_CT.Y); 
    transcript_str = "";
    NIZK_Plaintext_Equality_Prove(PI3_dualenc_pp,newCT.proof.pi_sendcheque.pi3.pi_dualenc_instance,PI3_dualenc_witness,transcript_str,newCT.proof.pi_sendcheque.pi3.pi_dualenc);



    //SIGN
    //convert to memo
    std::string memo(ECP_ep2string(AcctU.keypair.pk));
    memo.append(BN_bn2hex((const BIGNUM*)new_zk_balance.sn));
    char buffer[POINT_LEN*2+1] = {0};
    Twisted_ElGamal_CT_to_string(Cv_cheque,buffer);
    memo.append(reinterpret_cast<const char*>(buffer));

    char buffer1[POINT_LEN*2+1] = {0};
    Twisted_ElGamal_CT_to_string(newCv,buffer1);
    memo.append(reinterpret_cast<const char*>(buffer1));

    // //pi_sendcheque pi1
    // memo.append(ECP_ep2string(newCT.proof.pi_sendcheque.pi1.pi_enc.A));
    // memo.append(ECP_ep2string(newCT.proof.pi_sendcheque.pi1.pi_enc.B));
    // memo.append(BN_bn2hex((const BIGNUM*)newCT.proof.pi_sendcheque.pi1.pi_enc.z1));
    // memo.append(BN_bn2hex((const BIGNUM*)newCT.proof.pi_sendcheque.pi1.pi_enc.z2));
    // //pi_range
    // memo.append(ECP_ep2string(newCT.proof.pi_sendcheque.pi1.pi_range.A));
    // memo.append(ECP_ep2string(newCT.proof.pi_sendcheque.pi1.pi_range.S));
    // memo.append(ECP_ep2string(newCT.proof.pi_sendcheque.pi1.pi_range.T1));
    // memo.append(ECP_ep2string(newCT.proof.pi_sendcheque.pi1.pi_range.T2));
    // memo.append(BN_bn2hex(newCT.proof.pi_sendcheque.pi1.pi_range.taux));
    // memo.append(BN_bn2hex(newCT.proof.pi_sendcheque.pi1.pi_range.mu));
    // memo.append(BN_bn2hex(newCT.proof.pi_sendcheque.pi1.pi_range.tx));
    // for(int i=0;i<newCT.proof.pi_sendcheque.pi1.pi_range.ip_proof.vec_L.size();i++)
    // {
    //     memo.append(ECP_ep2string(newCT.proof.pi_sendcheque.pi1.pi_range.ip_proof.vec_L[i]));
    // }
    // for(int i=0;i<newCT.proof.pi_sendcheque.pi1.pi_range.ip_proof.vec_R.size();i++)
    // {
    //     memo.append(ECP_ep2string(newCT.proof.pi_sendcheque.pi1.pi_range.ip_proof.vec_R[i]));
    // }
    // memo.append(BN_bn2hex(newCT.proof.pi_sendcheque.pi1.pi_range.ip_proof.a));
    // memo.append(BN_bn2hex(newCT.proof.pi_sendcheque.pi1.pi_range.ip_proof.b));

    // //pi_sendcheque pi2
    // memo.append(ECP_ep2string(newCT.proof.pi_sendcheque.pi2.pi_enc.A));
    // memo.append(ECP_ep2string(newCT.proof.pi_sendcheque.pi2.pi_enc.B));
    // memo.append(BN_bn2hex((const BIGNUM*)newCT.proof.pi_sendcheque.pi2.pi_enc.z1));
    // memo.append(BN_bn2hex((const BIGNUM*)newCT.proof.pi_sendcheque.pi2.pi_enc.z2));
    // //pi_range
    // memo.append(ECP_ep2string(newCT.proof.pi_sendcheque.pi2.pi_range.A));
    // memo.append(ECP_ep2string(newCT.proof.pi_sendcheque.pi2.pi_range.S));
    // memo.append(ECP_ep2string(newCT.proof.pi_sendcheque.pi2.pi_range.T1));
    // memo.append(ECP_ep2string(newCT.proof.pi_sendcheque.pi2.pi_range.T2));
    // memo.append(BN_bn2hex(newCT.proof.pi_sendcheque.pi2.pi_range.taux));
    // memo.append(BN_bn2hex(newCT.proof.pi_sendcheque.pi2.pi_range.mu));
    // memo.append(BN_bn2hex(newCT.proof.pi_sendcheque.pi2.pi_range.tx));
    // for(int i=0;i<newCT.proof.pi_sendcheque.pi2.pi_range.ip_proof.vec_L.size();i++)
    // {
    //     memo.append(ECP_ep2string(newCT.proof.pi_sendcheque.pi2.pi_range.ip_proof.vec_L[i]));
    // }
    // for(int i=0;i<newCT.proof.pi_sendcheque.pi2.pi_range.ip_proof.vec_R.size();i++)
    // {
    //     memo.append(ECP_ep2string(newCT.proof.pi_sendcheque.pi2.pi_range.ip_proof.vec_R[i]));
    // }
    // memo.append(BN_bn2hex(newCT.proof.pi_sendcheque.pi2.pi_range.ip_proof.a));
    // memo.append(BN_bn2hex(newCT.proof.pi_sendcheque.pi2.pi_range.ip_proof.b));

    // //pi_sendcheque pi3 dualenc
    // memo.append(ECP_ep2string(newCT.proof.pi_sendcheque.pi3.pi_dualenc.A1));
    // memo.append(ECP_ep2string(newCT.proof.pi_sendcheque.pi3.pi_dualenc.A2));
    // memo.append(ECP_ep2string(newCT.proof.pi_sendcheque.pi3.pi_dualenc.B));
    // memo.append(BN_bn2hex((const BIGNUM*)newCT.proof.pi_sendcheque.pi3.pi_dualenc.z1));
    // memo.append(BN_bn2hex((const BIGNUM*)newCT.proof.pi_sendcheque.pi3.pi_dualenc.z2));

    BIGNUM *memo_bn;
    Hash_String_to_BN(memo,memo_bn);

    Twisted_ElGamal_Sig(enc_pp,AcctU.keypair.sk,memo_bn,newCT.sig);

    EC_POINT_copy(newCT.pk1,AcctU.keypair.pk);
    Twisted_ElGamal_CT_copy(newCT.transfer,Cv_cheque);
    Twisted_ElGamal_CT_copy(newCT.rest_balance,newCv);
    BN_copy(newCT.sn,AcctR.zk_balance.sn);//sn+1 = new sn
}

void LedgerMaze_RefreshCheque(LedgerMaze_PP &pp, LedgerMaze_Account &AcctR, LedgerMaze_CTx &sendCT,
LedgerMaze_CTx &refreshCT,BIGNUM *&new_pt_balance,ZK_BALANCE_CT &new_zk_balance, LedgerMaze_Cheque &newChequeR)
{
    //Obtain Trans_sendcheque from the current ledger

    //r,Vcheque = PKE.Dec(skR, auxR)

    Twisted_ElGamal_PP enc_pp;
    Get_Enc_PP(pp, enc_pp);
    BIGNUM* Vcheque_r_bn = BN_new();
    BIGNUM* Vcheque = BN_new();
    BIGNUM* r = BN_new();
    EC_POINT* Vcheque_r = EC_POINT_new(group);
    Hashed_ElGamal_Dec(enc_pp,AcctR.keypair.sk,sendCT.aux,Vcheque_r_bn);

    EC_POINT_bn2point(group,Vcheque_r_bn,Vcheque_r,bn_ctx);

    EC_POINT_get_affine_coordinates(group,Vcheque_r,Vcheque,r,bn_ctx);

    //get r_new from Zq
    BIGNUM* r_new = BN_new();
    BN_random(r_new);

    //Cv_cheque=ISE.Enc(pk_R,Vcheque,r2);
    Twisted_ElGamal_CT CR_new;
    Twisted_ElGamal_CT_new(CR_new);   
    Twisted_ElGamal_Enc(enc_pp,AcctR.keypair.pk,Vcheque,r_new,CR_new);

    //set newChequeR 
    Twisted_ElGamal_CT_copy(newChequeR.C,CR_new);
    EC_POINT_copy(newChequeR.pk,AcctR.keypair.pk);
    BN_copy(newChequeR.transfer,Vcheque);
    BN_copy(newChequeR.r,r_new);

    //get Cvcheque

    EC_POINT* TAG = EC_POINT_new(group);
    LedgerMaze_Commit(TAG,r);


    //compute 1 out of t proof
    One_Out_of_N_Proof_Instance pi_1outoft__instance;
    One_Out_of_N_Proof_Instance_new(pi_1outoft__instance);

    //get commit C_l = C(Rl,l)-CR_new
    Twisted_ElGamal_CT C_l;
    Twisted_ElGamal_CT_new(C_l); 
    Twisted_ElGamal_Parallel_HomoSub(C_l,sendCT.transfer,CR_new);

    //get commit A_l = C_l1/CR_new1 and get pi_1outoft_1
    EC_POINT* A_l = EC_POINT_new(group);
    EC_POINT* CR1_inv = EC_POINT_new(group);
    EC_POINT_copy(CR1_inv,CR_new.X);
    EC_POINT_invert(group,CR1_inv,bn_ctx);
    EC_POINT_add(group,A_l,C_l.X,CR1_inv,bn_ctx);

    secp_primitives::GroupElement Al_element;
    LedgerMaze_EC_POINTtoGroupElement(Al_element,A_l);

    secp_primitives::Scalar r_1_scalar;
    BIGNUM* r_1 = BN_new();

    BN_mod_sub(r_1,r,r_new,order,bn_ctx);
    LedgerMaze_BIGNUMtoScalar(r_1_scalar,r_1);//get r-r_new mod order
    One_Out_of_N_Proof_Prove(256,pi_1outoft__instance,refreshCT.proof.pi_refreshcheque.pi1.pi_oneoutofn1,Al_element,r_1_scalar);



    //get commit B_l = C_l2/CR_new2 and get pi_1outoft_2
    EC_POINT* B_l = EC_POINT_new(group);
    EC_POINT* CR2_inv = EC_POINT_new(group);
    EC_POINT_copy(CR2_inv,CR_new.Y);
    EC_POINT_invert(group,CR2_inv,bn_ctx);
    EC_POINT_add(group,B_l,C_l.Y,CR2_inv,bn_ctx);

    secp_primitives::GroupElement Bl_element;
    LedgerMaze_EC_POINTtoGroupElement(Bl_element,B_l);

    One_Out_of_N_Proof_Prove(256,pi_1outoft__instance,refreshCT.proof.pi_refreshcheque.pi1.pi_oneoutofn2,Bl_element,r_1_scalar);

    //get commit D_l = C_l1/TAG and get pi_1outoft_3
    EC_POINT* D_l = EC_POINT_new(group);
    EC_POINT* TAG_inv = EC_POINT_new(group);
    EC_POINT_copy(TAG_inv,TAG);
    EC_POINT_invert(group,TAG_inv,bn_ctx);
    EC_POINT_add(group,D_l,C_l.X,TAG_inv,bn_ctx);

    secp_primitives::GroupElement Dl_element;
    LedgerMaze_EC_POINTtoGroupElement(Dl_element,D_l);

    secp_primitives::Scalar r_2_scalar;
    LedgerMaze_BIGNUMtoScalar(r_2_scalar,r);

    One_Out_of_N_Proof_Prove(256,pi_1outoft__instance,refreshCT.proof.pi_refreshcheque.pi1.pi_oneoutofn3,Dl_element,r_2_scalar);


    //compute pi_refreshcheque_pi2
    string transcript_str;
    //compute pi_enc
    Plaintext_Knowledge_PP PI2_enc_pp; 
    NIZK_Plaintext_Knowledge_PP_new(PI2_enc_pp); 
    NIZK_Plaintext_Knowledge_Setup(PI2_enc_pp);
    // Plaintext_Knowledge_Instance PI2_enc_instance;
    // NIZK_Plaintext_Knowledge_Instance_new(PI2_enc_instance);  
    EC_POINT_copy(refreshCT.proof.pi_refreshcheque.pi2.pi_enc_instance.pk,AcctR.keypair.pk);
    EC_POINT_copy(refreshCT.proof.pi_refreshcheque.pi2.pi_enc_instance.X,CR_new.X);
    EC_POINT_copy(refreshCT.proof.pi_refreshcheque.pi2.pi_enc_instance.Y,CR_new.Y);
    Plaintext_Knowledge_Witness PI2_enc_witness; 
    NIZK_Plaintext_Knowledge_Witness_new(PI2_enc_witness); 
    BN_copy(PI2_enc_witness.r,r_new);
    BN_copy(PI2_enc_witness.v,Vcheque);
    transcript_str = ""; 
    NIZK_Plaintext_Knowledge_Prove(PI2_enc_pp,refreshCT.proof.pi_refreshcheque.pi2.pi_enc_instance,PI2_enc_witness,transcript_str,refreshCT.proof.pi_refreshcheque.pi2.pi_enc);
    //compute pi_range
    //Pi2.pi_range
    Bullet_PP PI2_range_pp; 
    Bullet_PP_new(PI2_range_pp,pp.RANGE_LEN,pp.AGG_NUM);
    Get_Bullet_PP(pp, PI2_range_pp);
    // Bullet_Instance PI2_range_instance;
    // Bullet_Instance_new(PI2_range_pp, PI2_range_instance); 
    EC_POINT_copy(refreshCT.proof.pi_refreshcheque.pi2.pi_range_instance.C[0], CR_new.Y);
    Bullet_Witness PI2_range_witness; 
    Bullet_Witness_new(PI2_range_pp, PI2_range_witness);
    BN_copy(PI2_range_witness.r[0], r_new);
    BN_copy(PI2_range_witness.v[0], Vcheque);
    transcript_str = "";
    Bullet_Prove(PI2_range_pp,refreshCT.proof.pi_refreshcheque.pi2.pi_range_instance,PI2_range_witness,transcript_str,refreshCT.proof.pi_refreshcheque.pi2.pi_range);

    //sign!!!!!!!!!!!!!!!

     //SIGN
    BIGNUM *memo_bn;

    std::string memo(ECP_ep2string(AcctR.keypair.pk));
    memo.append(ECP_ep2string(TAG));
    char buffer[POINT_LEN*2+1] = {0};
    Twisted_ElGamal_CT_to_string(CR_new,buffer);
    memo.append(reinterpret_cast<const char*>(buffer));
    // memo.append((char*)Twisted_ElGamal_CT_to_string(CR_new));
    // //pi1
    



    // //pi2 pi_range
    // memo.append(ECP_ep2string(refreshCT.proof.pi_refreshcheque.pi2.pi_range.A));
    // memo.append(ECP_ep2string(refreshCT.proof.pi_refreshcheque.pi2.pi_range.S));
    // memo.append(ECP_ep2string(refreshCT.proof.pi_refreshcheque.pi2.pi_range.T1));
    // memo.append(ECP_ep2string(refreshCT.proof.pi_refreshcheque.pi2.pi_range.T2));
    // memo.append(BN_bn2hex(refreshCT.proof.pi_refreshcheque.pi2.pi_range.taux));
    // memo.append(BN_bn2hex(refreshCT.proof.pi_refreshcheque.pi2.pi_range.mu));
    // memo.append(BN_bn2hex(refreshCT.proof.pi_refreshcheque.pi2.pi_range.tx));
    // for(int i=0;i<refreshCT.proof.pi_refreshcheque.pi2.pi_range.ip_proof.vec_L.size();i++)
    // {
    //     memo.append(ECP_ep2string(refreshCT.proof.pi_refreshcheque.pi2.pi_range.ip_proof.vec_L[i]));
    // }
    // for(int i=0;i<refreshCT.proof.pi_refreshcheque.pi2.pi_range.ip_proof.vec_R.size();i++)
    // {
    //     memo.append(ECP_ep2string(refreshCT.proof.pi_refreshcheque.pi2.pi_range.ip_proof.vec_R[i]));
    // }
    // memo.append(BN_bn2hex(refreshCT.proof.pi_refreshcheque.pi2.pi_range.ip_proof.a));
    // memo.append(BN_bn2hex(refreshCT.proof.pi_refreshcheque.pi2.pi_range.ip_proof.b));
    
    //////////////////////////////////////
    //////////////////////////////////////
    //////////////////////////////////////
    //////////////////////////////////////
    Twisted_ElGamal_Sig(enc_pp,AcctR.keypair.sk,memo_bn,refreshCT.sig);

    //get Trans_refersh
    EC_POINT_copy(refreshCT.pk1,AcctR.keypair.pk);
    Twisted_ElGamal_CT_copy(refreshCT.transfer,CR_new);
    EC_POINT_copy(refreshCT.tag,TAG);;

}

void LedgerMaze_Commit(EC_POINT* tag, BIGNUM* r)
{


    //get the k^-r
    EC_POINT* k = EC_POINT_new(group);

    ECP_random(k);

    EC_POINT_invert(group,k,bn_ctx);

    EC_POINT_mul(group,tag,NULL,k,r,bn_ctx);
}

void LedgerMaze_EC_POINTtoGroupElement(secp_primitives::GroupElement &g,EC_POINT* &e)
{
    BIGNUM* X = BN_new();
    BIGNUM* Y = BN_new();
    EC_POINT_get_affine_coordinates_GFp(group,e,X,Y,bn_ctx);

    char* x = BN_bn2hex(X);
    char* y = BN_bn2hex(Y);

    secp_primitives::GroupElement temp((char*)x,(char*)y,16);
    g = temp;
}

void LedgerMaze_Verify_CTx(LedgerMaze_PP &pp, LedgerMaze_CTx &newCT)
{
    switch (newCT.type)
    {
        case MINT:
            LedgerMaze_Verify_Mint_CTx(pp, newCT);
            /* code */
            break;
        case REDEEM:
            LedgerMaze_Verify_Redeem_CTx(pp, newCT);
            /* code */
            break;
        case SENDCHEQUE:
            LedgerMaze_Verify_SendCheque_CTx(pp,newCT);
            /* code */
            break;
        case DEPOSITCHEQUE:
            LedgerMaze_Verify_DepositCheque_CTx(pp,newCT);
            /* code */
            break;
        case REFRESHCHEQUE:
            LedgerMaze_Verify_RefreshCheque_CTx(pp,newCT);
            /* code */
            break;
        default:
            break;
    }
}

bool LedgerMaze_Verify_Mint_CTx(LedgerMaze_PP &pp, LedgerMaze_CTx &newCT)
{
    #ifdef DEMO
    cout << "begin to verify Mint CTx >>>>>>" << endl; 
    #endif

    bool Validity;
    string transcript_str = "";
    //a. check if value in (0,pt_balance]


    
}
bool LedgerMaze_Verify_Redeem_CTx(LedgerMaze_PP &pp, LedgerMaze_CTx &newCT)
{

}

bool LedgerMaze_Verify_SendCheque_CTx(LedgerMaze_PP &pp, LedgerMaze_CTx &newCT)
{
    Twisted_ElGamal_PP enc_pp;
    Get_Enc_PP(pp, enc_pp);

    std::string memo(ECP_ep2string(newCT.pk1));
    memo.append(BN_bn2hex((const BIGNUM*)newCT.sn));
    char buffer[POINT_LEN*2+1];
    Twisted_ElGamal_CT_to_string(newCT.transfer,buffer);
    memo.append(reinterpret_cast<const char*>(buffer));
    char buffer1[POINT_LEN*2+1];
    Twisted_ElGamal_CT_to_string(newCT.rest_balance,buffer1);
    memo.append(reinterpret_cast<const char*>(buffer1));

    BIGNUM *memo_bn;
    Hash_String_to_BN(memo,memo_bn);

    if(!Twisted_ElGamal_Ver(enc_pp,newCT.pk1,memo_bn,newCT.sig)){
        cout << "SignatureU fail!>>>" << endl;
        
        return;
    }

    bool V1, V2, V3, V4, V5, Validity; // variables for checking results

    string transcript_str; 

    Plaintext_Knowledge_PP PI1_enc_pp; 
    NIZK_Plaintext_Knowledge_PP_new(PI1_enc_pp); 
    NIZK_Plaintext_Knowledge_Setup(PI1_enc_pp);
    transcript_str = "";
    V3 = NIZK_Plaintext_Knowledge_Verify(PI1_enc_pp, newCT.proof.pi_sendcheque.pi1.pi_enc_instance, transcript_str, newCT.proof.pi_sendcheque.pi1.pi_enc); 
    //pi2 range
    Bullet_PP PI2_range_pp; 
    Bullet_PP_new(PI2_range_pp,pp.RANGE_LEN,pp.AGG_NUM);
    Get_Bullet_PP(pp, PI2_range_pp);
    transcript_str = "";
    V4 = Bullet_Verify(PI2_range_pp,newCT.proof.pi_sendcheque.pi1.pi_range_instance,transcript_str,newCT.proof.pi_sendcheque.pi1.pi_range);

    //verify pi2 enc
    

    Plaintext_Knowledge_PP PI2_enc_pp; 
    NIZK_Plaintext_Knowledge_PP_new(PI2_enc_pp); 
    NIZK_Plaintext_Knowledge_Setup(PI2_enc_pp);
    transcript_str = "";
    V3 = NIZK_Plaintext_Knowledge_Verify(PI2_enc_pp, newCT.proof.pi_sendcheque.pi2.pi_enc_instance, transcript_str, newCT.proof.pi_sendcheque.pi2.pi_enc); 
    //pi2 range
    Bullet_PP PI2_range_pp; 
    Bullet_PP_new(PI2_range_pp,pp.RANGE_LEN,pp.AGG_NUM);
    Get_Bullet_PP(pp, PI2_range_pp);
    transcript_str = "";
    V4 = Bullet_Verify(PI2_range_pp,newCT.proof.pi_sendcheque.pi2.pi_range_instance,transcript_str,newCT.proof.pi_sendcheque.pi2.pi_range);

    //pi3 dualenc
    Plaintext_Equality_PP PI3_dualenc_pp;
    NIZK_Plaintext_Equality_PP_new(PI3_dualenc_pp);    
    NIZK_Plaintext_Equality_Setup(PI3_dualenc_pp);
    transcript_str = "";
    V5 = NIZK_Plaintext_Equality_Verify(PI3_dualenc_pp,newCT.proof.pi_sendcheque.pi3.pi_dualenc_instance,transcript_str,newCT.proof.pi_sendcheque.pi3.pi_dualenc);

    Validity = V1 && V2 && V3 && V4 && V5;

    return Validity;







}
bool LedgerMaze_Verify_DepositCheque_CTx(LedgerMaze_PP &pp, LedgerMaze_CTx &newCT)
{
}

void LedgerMaze_BIGNUMtoScalar(secp_primitives::Scalar &r, BIGNUM* a)
{
    std::string a_hex(BN_bn2hex(a));
    r.SetHex(a_hex);
}

bool LedgerMaze_Verify_RefreshCheque_CTx(LedgerMaze_PP &pp, LedgerMaze_CTx &newCT)
{

    

}