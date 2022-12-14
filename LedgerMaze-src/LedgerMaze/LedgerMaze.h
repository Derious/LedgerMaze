/****************************************************************************
this hpp implements the LedgerMaze functionality 
*****************************************************************************
* @author     This file is part of PGC, developed by Yu Chen
* @paper      https://eprint.iacr.org/2019/319
* @copyright  MIT license (see LICENSE file)
*****************************************************************************/
#ifndef __LEDGERMAZE__
#define __LEDGERMAZE__

#include "../depends/twisted_elgamal/twisted_elgamal.hpp"        // implement Twisted ElGamal  
#include "../depends/nizk/nizk_plaintext_equality.hpp" // NIZKPoK for plaintext equality
#include "../depends/nizk/nizk_plaintext_knowledge.hpp"        // NIZKPoK for ciphertext/honest encryption 
#include "../depends/nizk/nizk_dlog_equality.hpp"      // NIZKPoK for dlog equality
#include "../depends/bulletproofs/aggregate_bulletproof.hpp"    // implement Log Size Bulletproof
#include "../depends/gadgets/gadgets.hpp"                       // implement Log Size Bulletproof
#include "../depends/oneoutofnproofs/src/one_out_of_n_protocol.hpp" // implement one out of n proof

#define MINT 1
#define REDEEM 2
#define SENDCHEQUE 3
#define REFRESHCHEQUE 4
#define DEPOSITCHEQUE 5
#define DEMO           // demo mode 
//#define DEBUG        // show debug information 

// define the structure for Account
struct LedgerMaze_PP{
    size_t RANGE_LEN; // the maximum coin value is 2^RANGE_LEN 
    size_t LOG_RANGE_LEN; // this parameter will be used by Bulletproof
    size_t AGG_NUM;    // number of aggregated proofs (for now, we require m to be the power of 2)
    size_t SN_LEN;    // sn length
    size_t TUNNING; 
    size_t DEC_THREAD_NUM; 
    size_t IO_THREAD_NUM; // used by twisted ElGamal

    BIGNUM *BN_MAXIMUM_COINS; 

    EC_POINT *g; //generator
    EC_POINT *h; //h<-Hash(g)
    EC_POINT *u; // used for inside innerproduct statement
    vector<EC_POINT *> vec_g; 
    vector<EC_POINT *> vec_h; // the pp of innerproduct part  
};

struct LedgerMaze_Account{
    string identity;                // id
    Twisted_ElGamal_KP keypair;
    ZK_BALANCE_CT zk_balance;  // NIZK balance
    BIGNUM *pt_balance;             // plaintext balance
    // BIGNUM *m;                      // dangerous (should only be used for speeding up the proof generation)
    // BIGNUM *sn; 
};

struct LedgerMaze_Cheque{
    Twisted_ElGamal_CT C;   //C=Enc(pk,transfer,r)
    EC_POINT *pk;           //recriver's pk
    BIGNUM *transfer;       //transfer value
    BIGNUM *r;              //random number

};
struct pi_sendcheque_pi1
{
    Plaintext_Knowledge_Proof pi_enc;
    Plaintext_Knowledge_Instance pi_enc_instance;
    Bullet_Proof pi_range;
    Bullet_Instance  pi_range_instance;
    /* data */
};

struct pi_sendcheque_pi2
{
    Plaintext_Knowledge_Proof pi_enc;
    Plaintext_Knowledge_Instance pi_enc_instance;
    Bullet_Proof pi_range; 
    Bullet_Instance pi_range_instance;
    /* data */
};

struct pi_sendcheque_pi3
{
    Plaintext_Equality_Proof pi_dualenc; 
    Plaintext_Equality_Instance pi_dualenc_instance;
    /* data */
};
//pi_sendcheque = pi1,pi2,pi3
struct Pi_sendcheque
{
    pi_sendcheque_pi1 pi1;
    pi_sendcheque_pi2 pi2;
    pi_sendcheque_pi3 pi3;
    /* data */
};

struct Pi_mint
{
    DLOG_Equality_Proof pi_random;
    DLOG_Equality_Instance pi_random_instance;
    Bullet_Proof pi_range; 
    Bullet_Instance pi_range_instance;
    /* data */
};

struct pi_refreshcheque_pi1
{
    One_Out_of_N_Proof  pi_oneoutofn1;
    One_Out_of_N_Proof  pi_oneoutofn2;
    One_Out_of_N_Proof  pi_oneoutofn3;

};
struct pi_refreshcheque_pi2
{
    Plaintext_Knowledge_Proof pi_enc;
    Plaintext_Knowledge_Instance pi_enc_instance;
    Bullet_Proof pi_range;  
    Bullet_Instance pi_range_instance;
};
struct Pi_refreshcheque
{
    pi_refreshcheque_pi1 pi1;
    pi_refreshcheque_pi2 pi2;
    /* data */
};



struct LedgerMaze_trans
{
    Pi_mint pi_mint;
    
    Pi_mint pi_redeem;

    Pi_sendcheque pi_sendcheque;

    Pi_mint pi_depositcheque;

    Pi_refreshcheque pi_refreshcheque;
    /* data */
};


// define the structure for confidential transaction
struct LedgerMaze_CTx{
    BIGNUM *sn;                           // serial number: uniquely defines a transaction
    int type;                             // trans type in {mint, redeem, sendcheque,refreshcheque,depositcheque}
    // memo information (padding zero if some trans don't need)
    EC_POINT *pk1;                        // sender = pk1
    EC_POINT *tag;                        //only used in Refreshcheque, tag=Commit(r)=g^-r;
    BIGNUM *value;                        // the transfer value
    Twisted_ElGamal_CT rest_balance;      //ciphertext of rest balance, rest_balance=ISE.Enc(pk_sender,pt_restbalabce,r1)
    Twisted_ElGamal_CT transfer;          //ciphertext of transfer value, transfer=ISE.Enc(pk_receiver,value,r2), only used in Sendcheque
    Hashed_ElGamal_CT  aux;               //only used in Sendcheque

 
    // valid proof
    LedgerMaze_trans proof;
    // Pi_mint pi_mint;

    // Pi_mint pi_redeem;

    // Pi_sendcheque pi_sendcheque;

    // Pi_sendcheque pi_depositcheque;

    // Pi_refreshcheque pi_refreshcheque;


    // // Plaintext_Equality_Proof pi_sendcheque_dualenc;     //proof for NIZK{(r1,r2,v)|C1=r1*PK and C2=r1*G+v*H and C1'=r2*PK and C2=r2*G+v*H}     //NIZKPoK for transfer ciphertext (X1, X2, Y)
    // Plaintext_Equality_Proof pi_dualenc;     //proof for NIZK{(r1,r2,v)|C1=r1*PK and C2=r1*G+v*H and C1'=r2*PK and C2=r2*G+v*H}     //NIZKPoK for transfer ciphertext (X1, X2, Y)
    
    // // Bullet_Proof pi_mint_range;      // aggregated range proof for v , m-v and m lie in the right range 
    // // Bullet_Proof pi_redeem_range;      // aggregated range proof for v , m-v and m lie in the right range 
    // // Bullet_Proof pi_sendcheque_range;      // aggregated range proof for v , m-v and m lie in the right range 
    // // Bullet_Proof pi_refreshcheque_range;      // aggregated range proof for v , m-v and m lie in the right range 
    // Bullet_Proof pi_range;      // aggregated range proof for v , m-v and m lie in the right range 

   
    // // Plaintext_Knowledge_Proof pi_sendcheque_enc; // proof for NIZK{(r,v)|C1=r*PK and C2=r*G+v*H}    //NIZKPoK for refresh ciphertext (X^*, Y^*)
    // // Plaintext_Knowledge_Proof pi_refreshcheque_enc; // proof for NIZK{(r,v)|C1=r*PK and C2=r*G+v*H}    //NIZKPoK for refresh ciphertext (X^*, Y^*)
    // Plaintext_Knowledge_Proof pi_enc; // proof for NIZK{(r,v)|C1=r*PK and C2=r*G+v*H}    //NIZKPoK for refresh ciphertext (X^*, Y^*)
    
    // // DLOG_Equality_Proof pi_mint_random;     // proof for NIZK{(r)|C1=r*PK and C2=r*G+vtran*H} s.t. vtran is public
    // DLOG_Equality_Proof pi_random;    // proof for NIZK{(r)|C1=r*PK and C2=r*G+vtran*H} s.t. vtran is public
    
    // // One_Out_of_N_Proof  pi_oneoutofn1; // proof for one out of N
    // // One_Out_of_N_Proof  pi_oneoutofn2; // proof for one out of N
    // One_Out_of_N_Proof  pi_oneoutofn; // proof for one out of N

    Twisted_ElGamal_SIG sig;            //signature of trans
};










string Get_ctxfilename(LedgerMaze_CTx &newCTx);

void LedgerMaze_PP_print(LedgerMaze_PP &pp);

void LedgerMaze_Account_print(LedgerMaze_Account &Acct);

/* print the details of a confidential transaction */
void LedgerMaze_CTx_print(LedgerMaze_CTx &newCTx);

void LedgerMaze_PP_new(LedgerMaze_PP &pp, size_t RANGE_LEN, size_t AGG_NUM);

void LedgerMaze_PP_free(LedgerMaze_PP &pp);

void LedgerMaze_Account_new(LedgerMaze_Account &newAcct);

void LedgerMaze_Account_free(LedgerMaze_Account &newAcct);

void LedgerMaze_CTx_new(LedgerMaze_CTx &newCTx,int CTtype);

void LedgerMaze_CTx_free(LedgerMaze_CTx &newCTx);

// obtain pp for each building block
void Get_Bullet_PP(LedgerMaze_PP &pp, Bullet_PP &bullet_pp)
{
    bullet_pp.RANGE_LEN = pp.RANGE_LEN; 
    bullet_pp.LOG_RANGE_LEN = pp.LOG_RANGE_LEN;
    bullet_pp.AGG_NUM = pp.AGG_NUM;  

    bullet_pp.g = pp.g; 
    bullet_pp.h = pp.h; 
    bullet_pp.u = pp.u; 
    bullet_pp.vec_g = pp.vec_g; 
    bullet_pp.vec_h = pp.vec_h; 
}

void Get_Gadget_PP(LedgerMaze_PP &pp, Gadget_PP &gadget_pp)
{
    gadget_pp.RANGE_LEN = pp.RANGE_LEN; 
    gadget_pp.LOG_RANGE_LEN = log2(pp.RANGE_LEN);
    
    gadget_pp.TUNNING = pp.TUNNING; 
    gadget_pp.DEC_THREAD_NUM = pp.DEC_THREAD_NUM; 
    gadget_pp.IO_THREAD_NUM = pp.IO_THREAD_NUM; 

    gadget_pp.g = pp.g; 
    gadget_pp.h = pp.h; 
    gadget_pp.u = pp.u; 
    gadget_pp.vec_g.assign(pp.vec_g.begin(), pp.vec_g.begin() + pp.RANGE_LEN);  
    gadget_pp.vec_h.assign(pp.vec_h.begin(), pp.vec_h.begin() + pp.RANGE_LEN); 
}

void Get_Enc_PP(LedgerMaze_PP &pp, Twisted_ElGamal_PP &enc_pp)
{
    enc_pp.MSG_LEN = pp.RANGE_LEN; 
    enc_pp.TUNNING = pp.TUNNING;
    enc_pp.DEC_THREAD_NUM = pp.DEC_THREAD_NUM; 
    enc_pp.IO_THREAD_NUM = pp.IO_THREAD_NUM;  
    enc_pp.BN_MSG_SIZE = pp.BN_MAXIMUM_COINS; 
    enc_pp.g = pp.g; 
    enc_pp.h = pp.h;  
}

void Get_Plaintext_Equality_PP(LedgerMaze_PP &pp, Plaintext_Equality_PP &pteq_pp)
{
    pteq_pp.g = pp.g; 
    pteq_pp.h = pp.h;  
}

void Get_DLOG_Equality_PP(LedgerMaze_PP &pp, DLOG_Equality_PP &dlogeq_pp)
{
    dlogeq_pp.ss_reserve = "dummy";  
}

void Get_Plaintext_Knowledge_PP(LedgerMaze_PP &pp, Plaintext_Knowledge_PP &ptknowledge_pp)
{
    ptknowledge_pp.g = pp.g; 
    ptknowledge_pp.h = pp.h; 
}

void LedgerMaze_PP_serialize(LedgerMaze_PP &pp, string LedgerMaze_pp_file)
{
    ofstream fout; 
    fout.open(LedgerMaze_pp_file, ios::binary); 
    fout.write((char *)(&pp.RANGE_LEN), sizeof(pp.RANGE_LEN));
    fout.write((char *)(&pp.LOG_RANGE_LEN), sizeof(pp.LOG_RANGE_LEN));
    fout.write((char *)(&pp.AGG_NUM), sizeof(pp.AGG_NUM));
    fout.write((char *)(&pp.SN_LEN), sizeof(pp.SN_LEN));
    fout.write((char *)(&pp.DEC_THREAD_NUM), sizeof(pp.DEC_THREAD_NUM));
    fout.write((char *)(&pp.IO_THREAD_NUM), sizeof(pp.IO_THREAD_NUM));
    fout.write((char *)(&pp.TUNNING), sizeof(pp.TUNNING));

    BN_serialize(pp.BN_MAXIMUM_COINS, fout);  
    ECP_serialize(pp.g, fout); 
    ECP_serialize(pp.h, fout);
    ECP_serialize(pp.u, fout); 
    ECP_vec_serialize(pp.vec_g, fout); 
    ECP_vec_serialize(pp.vec_h, fout); 

    fout.close();   
}

void LedgerMaze_PP_deserialize(LedgerMaze_PP &pp, string LedgerMaze_pp_file)
{
    ifstream fin; 
    fin.open(LedgerMaze_pp_file, ios::binary); 
    fin.read((char *)(&pp.RANGE_LEN), sizeof(pp.RANGE_LEN));
    fin.read((char *)(&pp.LOG_RANGE_LEN), sizeof(pp.LOG_RANGE_LEN));
    fin.read((char *)(&pp.AGG_NUM), sizeof(pp.AGG_NUM));
    fin.read((char *)(&pp.SN_LEN), sizeof(pp.SN_LEN));
    fin.read((char *)(&pp.DEC_THREAD_NUM), sizeof(pp.DEC_THREAD_NUM));
    fin.read((char *)(&pp.IO_THREAD_NUM), sizeof(pp.IO_THREAD_NUM));
    fin.read((char *)(&pp.TUNNING), sizeof(pp.TUNNING));

    BN_deserialize(pp.BN_MAXIMUM_COINS, fin);
    ECP_deserialize(pp.g, fin); 
    ECP_deserialize(pp.h, fin);
    ECP_deserialize(pp.u, fin); 
    ECP_vec_deserialize(pp.vec_g, fin); 
    ECP_vec_deserialize(pp.vec_h, fin); 

    fin.close();   
}





// // save CTx into sn.ctx file
// void LedgerMaze_CTx_serialize(LedgerMaze_CTx &newCTx, string LedgerMaze_ctx_file)
// {
//     ofstream fout; 
//     fout.open(LedgerMaze_ctx_file); 
    
//     // save sn
//     BN_serialize(newCTx.sn, fout); 
     
//     // save memo info
//     ECP_serialize(newCTx.pk1, fout); 
//     ECP_serialize(newCTx.pk2, fout); 
//     MR_Twisted_ElGamal_CT_serialize(newCTx.transfer, fout);
//     BN_serialize(newCTx.v, fout);
    
//     // save proofs
//     Plaintext_Equality_Proof_serialize(newCTx.plaintext_equality_proof, fout);
//     Twisted_ElGamal_CT_serialize(newCTx.refresh_sender_updated_balance, fout); 
//     DLOG_Equality_Proof_serialize(newCTx.dlog_equality_proof, fout); 
//     Plaintext_Knowledge_Proof_serialize(newCTx.plaintext_knowledge_proof, fout); 
//     Bullet_Proof_serialize(newCTx.bullet_right_enough_proof, fout); 
//     fout.close();

//     // calculate the size of ctx_file
//     ifstream fin; 
//     fin.open(LedgerMaze_ctx_file, ios::ate | ios::binary);
//     cout << LedgerMaze_ctx_file << " size = " << fin.tellg() << " bytes" << endl;
//     fin.close(); 
// }

// /* recover CTx from ctx file */
// void LedgerMaze_CTx_deserialize(LedgerMaze_CTx &newCTx, string LedgerMaze_ctx_file)
// {
//     // Deserialize_CTx(newCTx, ctx_file); 
//     ifstream fin; 
//     fin.open(LedgerMaze_ctx_file);

//     // recover sn
//     BN_deserialize(newCTx.sn, fin);
    
//     // recover memo
//     ECP_deserialize(newCTx.pk1, fin); 
//     ECP_deserialize(newCTx.pk2, fin); 
//     MR_Twisted_ElGamal_CT_deserialize(newCTx.transfer, fin);
//     BN_deserialize(newCTx.v, fin); 

//     // recover proof
//     Plaintext_Equality_Proof_deserialize(newCTx.plaintext_equality_proof, fin);
//     Twisted_ElGamal_CT_deserialize(newCTx.refresh_sender_updated_balance, fin); 
//     DLOG_Equality_Proof_deserialize(newCTx.dlog_equality_proof, fin); 
//     Plaintext_Knowledge_Proof_deserialize(newCTx.plaintext_knowledge_proof, fin); 
//     Bullet_Proof_deserialize(newCTx.bullet_right_enough_proof, fin); 
//     fin.close(); 
// }

// /* This function implements Setup algorithm of LedgerMaze */
// void LedgerMaze_Setup(LedgerMaze_PP &pp, size_t RANGE_LEN, size_t AGG_NUM, 
//                size_t SN_LEN, size_t DEC_THREAD_NUM, size_t IO_THREAD_NUM, size_t TUNNING);

// void LedgerMaze_Initialize(LedgerMaze_PP &pp);

// /* create an account for input identity */
// void LedgerMaze_Create_Account(LedgerMaze_PP &pp, string identity, BIGNUM *&init_balance, BIGNUM *&sn, 
//                         LedgerMaze_Account &newAcct);

// /* update Account if CTx is valid */
// bool LedgerMaze_Update_Account(LedgerMaze_PP &pp, LedgerMaze_CTx &newCTx, LedgerMaze_Account &Acct_sender, LedgerMaze_Account &Acct_receiver)
// {    
//     Twisted_ElGamal_PP enc_pp; 
//     Get_Enc_PP(pp, enc_pp); 
//     if (EC_POINT_cmp(group, newCTx.pk1, Acct_sender.pk, bn_ctx) 
//         || EC_POINT_cmp(group, newCTx.pk2, Acct_receiver.pk, bn_ctx))
//     {
//         cout << "sender and receiver addresses do not match" << endl;
//         return false;  
//     }
//     else
//     {
//         BN_add(Acct_sender.sn, Acct_receiver.sn, BN_1);

//         Twisted_ElGamal_CT C_out; 
//         C_out.X = newCTx.transfer.X1; 
//         C_out.Y = newCTx.transfer.Y;
//         Twisted_ElGamal_CT C_in; 
//         C_in.X = newCTx.transfer.X2; 
//         C_in.Y = newCTx.transfer.Y;

//         // update sender's balance
//         Twisted_ElGamal_HomoSub(Acct_sender.balance, Acct_sender.balance, C_out); 
//         // update receiver's balance
//         Twisted_ElGamal_HomoAdd(Acct_receiver.balance, Acct_receiver.balance, C_in); 

//         Twisted_ElGamal_Dec(enc_pp, Acct_sender.sk, Acct_sender.balance, Acct_sender.m); 
//         Twisted_ElGamal_Dec(enc_pp, Acct_receiver.sk, Acct_receiver.balance, Acct_receiver.m);

//         LedgerMaze_Account_serialize(Acct_sender, Acct_sender.identity+".account"); 
//         LedgerMaze_Account_serialize(Acct_receiver, Acct_receiver.identity+".account"); 
//         return true; 
//     }
// } 

// /* reveal the balance */ 
// void LedgerMaze_Reveal_Balance(LedgerMaze_PP &pp, LedgerMaze_Account &Acct, BIGNUM *&m)
// {
//     Twisted_ElGamal_PP enc_pp;
//     Get_Enc_PP(pp, enc_pp); 
//     Twisted_ElGamal_Dec(enc_pp, Acct.sk, Acct.balance, m); 
//     //BN_copy(m, Acct.m); 
// }

// // in: pp, Acct, value
// // out: newCT, new_pt_balance,new_zk_balance
// void LedgerMaze_Mint(LedgerMaze_PP &pp, LedgerMaze_Account &Acct, BIGNUM *&value, 
// LedgerMaze_CTx &newCT,BIGNUM *&new_pt_balance,ZK_BALANCE_CT &new_zk_balance);

// /* generate a confidential transaction: pk1 transfers v coins to pk2 */
// void LedgerMaze_Create_CTx(LedgerMaze_PP &pp, LedgerMaze_Account &Acct_sender, BIGNUM *&v, EC_POINT *&pk2, LedgerMaze_CTx &newCTx)
// {
//     #ifdef DEMO
//     cout << "begin to genetate CTx >>>>>>" << endl; 
//     #endif
//     SplitLine_print('-'); 

//     auto start_time = chrono::steady_clock::now(); 
//     BN_copy(newCTx.sn, Acct_sender.sn);
//     EC_POINT_copy(newCTx.pk1, Acct_sender.pk); 
//     EC_POINT_copy(newCTx.pk2, pk2); 

//     Twisted_ElGamal_PP enc_pp;
//     Get_Enc_PP(pp, enc_pp); 

//     BIGNUM *r = BN_new(); 
//     BN_random(r); 

//     BN_copy(newCTx.v, v); 
//     MR_Twisted_ElGamal_Enc(enc_pp, newCTx.pk1, newCTx.pk2, v, r, newCTx.transfer); 

//     EC_POINT_copy(newCTx.sender_balance.X, Acct_sender.balance.X);
//     EC_POINT_copy(newCTx.sender_balance.Y, Acct_sender.balance.Y);

//     #ifdef DEMO
//     cout <<"1. generate memo info of CTx" << endl;  
//     #endif

//     // begin to generate the valid proof for ctx
//     string transcript_str = BN_bn2string(newCTx.sn); 

//     // generate NIZK proof for validity of transfer              
//     Plaintext_Equality_PP pteq_pp; 
//     Get_Plaintext_Equality_PP(pp, pteq_pp);
    
//     Plaintext_Equality_Instance pteq_instance;
//     NIZK_Plaintext_Equality_Instance_new(pteq_instance); 
//     EC_POINT_copy(pteq_instance.pk1, newCTx.pk1); 
//     EC_POINT_copy(pteq_instance.pk2, newCTx.pk2); 
//     EC_POINT_copy(pteq_instance.X1, newCTx.transfer.X1);
//     EC_POINT_copy(pteq_instance.X2, newCTx.transfer.X2);
//     EC_POINT_copy(pteq_instance.Y, newCTx.transfer.Y);
    
//     Plaintext_Equality_Witness pteq_witness; 
//     NIZK_Plaintext_Equality_Witness_new(pteq_witness); 
//     BN_copy(pteq_witness.r, r); 
//     BN_copy(pteq_witness.v, v); 

//     NIZK_Plaintext_Equality_Prove(pteq_pp, pteq_instance, pteq_witness, 
//                                   transcript_str, newCTx.plaintext_equality_proof);

//     #ifdef DEMO
//     cout << "2. generate NIZKPoK for plaintext equality" << endl;  
//     #endif

//     #ifdef DEMO
//     cout << "3. compute updated balance" << endl;  
//     #endif
//     // compute the updated balance

//     Twisted_ElGamal_CT sender_updated_balance; 
//     Twisted_ElGamal_CT_new(sender_updated_balance);
//     EC_POINT_sub(sender_updated_balance.X, newCTx.sender_balance.X, newCTx.transfer.X1); 
//     EC_POINT_sub(sender_updated_balance.Y, newCTx.sender_balance.Y, newCTx.transfer.Y); 

//     #ifdef DEMO
//     cout << "4. compute refreshed updated balance" << endl;  
//     #endif
//     // refresh the updated balance (with random coins r^*)
//     BIGNUM* r_star = BN_new(); 
//     BN_random(r_star);   
//     Twisted_ElGamal_ReRand(enc_pp, Acct_sender.pk, Acct_sender.sk, 
//                            sender_updated_balance, newCTx.refresh_sender_updated_balance, r_star);

//     // generate the NIZK proof for updated balance and fresh updated balance encrypt the same message
//     DLOG_Equality_PP dlogeq_pp; 
//     Get_DLOG_Equality_PP(pp, dlogeq_pp); 
    
//     DLOG_Equality_Instance dlogeq_instance;
//     NIZK_DLOG_Equality_Instance_new(dlogeq_instance); 
//     // g1 = Y-Y^* = g^{r-r^*}    
//     EC_POINT_sub(dlogeq_instance.g1, sender_updated_balance.Y, newCTx.refresh_sender_updated_balance.Y); 
//     // h1 = X-X^* = pk^{r-r^*}
//     EC_POINT_sub(dlogeq_instance.h1, sender_updated_balance.X, newCTx.refresh_sender_updated_balance.X); 
    
//     EC_POINT_copy(dlogeq_instance.g2, enc_pp.g);                         // g2 = g
//     EC_POINT_copy(dlogeq_instance.h2, Acct_sender.pk);                    // h2 = pk  
//     DLOG_Equality_Witness dlogeq_witness; 
//     NIZK_DLOG_Equality_Witness_new(dlogeq_witness); 
//     BN_copy(dlogeq_witness.w, Acct_sender.sk); 

//     NIZK_DLOG_Equality_Prove(dlogeq_pp, dlogeq_instance, dlogeq_witness, transcript_str, newCTx.dlog_equality_proof); 

//     #ifdef DEMO
//     cout << "5. generate NIZKPoK for correct refreshing and authenticate the memo info" << endl;  
//     #endif

//     Plaintext_Knowledge_PP ptke_pp;
//     Get_Plaintext_Knowledge_PP(pp, ptke_pp);

//     Plaintext_Knowledge_Instance ptke_instance; 
//     NIZK_Plaintext_Knowledge_Instance_new(ptke_instance); 
//     EC_POINT_copy(ptke_instance.pk, Acct_sender.pk); 
//     EC_POINT_copy(ptke_instance.X, newCTx.refresh_sender_updated_balance.X); 
//     EC_POINT_copy(ptke_instance.Y, newCTx.refresh_sender_updated_balance.Y); 
    
//     Plaintext_Knowledge_Witness ptke_witness; 
//     NIZK_Plaintext_Knowledge_Witness_new(ptke_witness);
//     BN_copy(ptke_witness.r, r_star); 

//     BN_mod_sub(ptke_witness.v, Acct_sender.m, v, order, bn_ctx); // m = Twisted_ElGamal_Dec(sk1, balance); 

//     NIZK_Plaintext_Knowledge_Prove(ptke_pp, ptke_instance, ptke_witness, 
//                                    transcript_str, newCTx.plaintext_knowledge_proof); 
    
//     #ifdef DEMO
//     cout << "6. generate NIZKPoK for refreshed updated balance" << endl;  
//     #endif

//     // aggregated range proof for v and m-v lie in the right range 
//     Bullet_PP bullet_pp; 
//     Get_Bullet_PP(pp, bullet_pp);
//     Bullet_Instance bullet_instance;
//     Bullet_Instance_new(bullet_pp, bullet_instance); 
//     EC_POINT_copy(bullet_instance.C[0], newCTx.transfer.Y);
//     EC_POINT_copy(bullet_instance.C[1], newCTx.refresh_sender_updated_balance.Y);

//     Bullet_Witness bullet_witness; 
//     Bullet_Witness_new(bullet_pp, bullet_witness); 
//     BN_copy(bullet_witness.r[0], pteq_witness.r); 
//     BN_copy(bullet_witness.r[1], ptke_witness.r); 
//     BN_copy(bullet_witness.v[0], pteq_witness.v);
//     BN_copy(bullet_witness.v[1], ptke_witness.v);

//     Bullet_Prove(bullet_pp, bullet_instance, bullet_witness, transcript_str, newCTx.bullet_right_enough_proof); 

//     #ifdef DEMO
//     cout << "7. generate range proofs for transfer amount and updated balance" << endl;    
//     #endif

//     #ifdef DEMO
//     SplitLine_print('-'); 
//     #endif

//     // #ifndef PREPROCESSING
//     // BN_copy(newCTx.v, v); 
//     // #endif

//     Twisted_ElGamal_CT_free(sender_updated_balance);

//     NIZK_Plaintext_Equality_Instance_free(pteq_instance); 
//     NIZK_Plaintext_Equality_Witness_free(pteq_witness);

//     NIZK_DLOG_Equality_Instance_free(dlogeq_instance);
//     NIZK_DLOG_Equality_Witness_free(dlogeq_witness);

//     NIZK_Plaintext_Knowledge_Instance_free(ptke_instance); 
//     NIZK_Plaintext_Knowledge_Witness_free(ptke_witness); 

//     Bullet_Instance_free(bullet_instance); 
//     Bullet_Witness_free(bullet_witness);

//     auto end_time = chrono::steady_clock::now(); 

//     auto running_time = end_time - start_time;
//     cout << "ctx generation takes time = " 
//     << chrono::duration <double, milli> (running_time).count() << " ms" << endl;
// }

// /* check if the given confidential transaction is valid */ 
// bool LedgerMaze_Verify_CTx(LedgerMaze_PP &pp, LedgerMaze_CTx &newCTx)
// {     
//     #ifdef DEMO
//     cout << "begin to verify CTx >>>>>>" << endl; 
//     #endif

//     auto start_time = chrono::steady_clock::now(); 
    
//     bool Validity; 
//     bool V1, V2, V3, V4; 

//     string transcript_str = BN_bn2string(newCTx.sn); 

//     Plaintext_Equality_PP pteq_pp;
//     Get_Plaintext_Equality_PP(pp, pteq_pp); 

//     Plaintext_Equality_Instance pteq_instance; 
//     NIZK_Plaintext_Equality_Instance_new(pteq_instance); 
//     EC_POINT_copy(pteq_instance.pk1, newCTx.pk1);
//     EC_POINT_copy(pteq_instance.pk2, newCTx.pk2);
//     EC_POINT_copy(pteq_instance.X1, newCTx.transfer.X1);
//     EC_POINT_copy(pteq_instance.X2, newCTx.transfer.X2);
//     EC_POINT_copy(pteq_instance.Y, newCTx.transfer.Y);

//     V1 = NIZK_Plaintext_Equality_Verify(pteq_pp, pteq_instance, transcript_str, newCTx.plaintext_equality_proof);
    
//     NIZK_Plaintext_Equality_Instance_free(pteq_instance);

//     #ifdef DEMO
//     if (V1) cout<< "NIZKPoK for plaintext equality accepts" << endl; 
//     else cout<< "NIZKPoK for plaintext equality rejects" << endl; 
//     #endif

//     // check V2
//     Twisted_ElGamal_PP enc_pp; 
//     Get_Enc_PP(pp, enc_pp); 

//     Twisted_ElGamal_CT updated_sender_balance; 
//     Twisted_ElGamal_CT_new(updated_sender_balance);
//     EC_POINT_sub(updated_sender_balance.X, newCTx.sender_balance.X, newCTx.transfer.X1); 
//     EC_POINT_sub(updated_sender_balance.Y, newCTx.sender_balance.Y, newCTx.transfer.Y); 

//     // generate the NIZK proof for updated balance and fresh updated balance encrypt the same message
//     DLOG_Equality_PP dlogeq_pp; 
//     Get_DLOG_Equality_PP(pp, dlogeq_pp);

//     DLOG_Equality_Instance dlogeq_instance; 
//     NIZK_DLOG_Equality_Instance_new(dlogeq_instance); 

//     EC_POINT_sub(dlogeq_instance.g1, updated_sender_balance.Y, newCTx.refresh_sender_updated_balance.Y); 
//     EC_POINT_sub(dlogeq_instance.h1, updated_sender_balance.X, newCTx.refresh_sender_updated_balance.X); 
//     EC_POINT_copy(dlogeq_instance.g2, enc_pp.g); 
//     EC_POINT_copy(dlogeq_instance.h2, newCTx.pk1);  

//     V2 = NIZK_DLOG_Equality_Verify(dlogeq_pp, dlogeq_instance, transcript_str, newCTx.dlog_equality_proof); 

//     Twisted_ElGamal_CT_free(updated_sender_balance);
//     NIZK_DLOG_Equality_Instance_free(dlogeq_instance); 

//     #ifdef DEMO
//     if (V2) cout<< "NIZKPoK for refreshing correctness accepts and memo info is authenticated" << endl; 
//     else cout<< "NIZKPoK for refreshing correctness rejects or memo info is unauthenticated" << endl; 
//     #endif

//     Plaintext_Knowledge_PP ptke_pp; 
//     Get_Plaintext_Knowledge_PP(pp, ptke_pp);

//     Plaintext_Knowledge_Instance ptke_instance; 
//     NIZK_Plaintext_Knowledge_Instance_new(ptke_instance); 
//     EC_POINT_copy(ptke_instance.pk, newCTx.pk1); 
//     EC_POINT_copy(ptke_instance.X, newCTx.refresh_sender_updated_balance.X); 
//     EC_POINT_copy(ptke_instance.Y, newCTx.refresh_sender_updated_balance.Y); 

//     V3 = NIZK_Plaintext_Knowledge_Verify(ptke_pp, ptke_instance, 
//                                          transcript_str, newCTx.plaintext_knowledge_proof); 

//     NIZK_Plaintext_Knowledge_Instance_free(ptke_instance); 

//     #ifdef DEMO
//     if (V3) cout<< "NIZKPoK for refresh updated balance accepts" << endl; 
//     else cout<< "NIZKPoK for refresh updated balance rejects" << endl; 
//     #endif

//     // aggregated range proof for v and m-v lie in the right range 
//     Bullet_PP bullet_pp; 
//     Get_Bullet_PP(pp, bullet_pp);

//     Bullet_Instance bullet_instance;
//     Bullet_Instance_new(bullet_pp, bullet_instance); 
//     EC_POINT_copy(bullet_instance.C[0], newCTx.transfer.Y);
//     EC_POINT_copy(bullet_instance.C[1], newCTx.refresh_sender_updated_balance.Y);

//     V4 = Bullet_Verify(bullet_pp, bullet_instance, transcript_str, newCTx.bullet_right_enough_proof); 

//     Bullet_Instance_free(bullet_instance); 

//     #ifdef DEMO
//     if (V4) cout<< "range proofs for transfer amount and updated balance accept" << endl; 
//     else cout<< "range proofs for transfer amount and updated balance reject" << endl;   
//     #endif

//     Validity = V1 && V2 && V3 && V4; 

//     string ctx_file  = ECP_ep2string(newCTx.pk1) + "_" + BN_bn2string(newCTx.sn) + ".ctx"; 
//     #ifdef DEMO
//     if (Validity) cout << ctx_file << " is valid <<<<<<" << endl; 
//     else cout << ctx_file << " is invalid <<<<<<" << endl;
//     #endif

//     auto end_time = chrono::steady_clock::now(); 

//     auto running_time = end_time - start_time;
//     cout << "ctx verification takes time = " 
//     << chrono::duration <double, milli> (running_time).count() << " ms" << endl;

//     return Validity; 
// }

// /* check if a ctx is valid and update accounts if so */
// bool LedgerMaze_Miner(LedgerMaze_PP &pp, LedgerMaze_CTx &newCTx, LedgerMaze_Account &Acct_sender, LedgerMaze_Account &Acct_receiver)
// {
//     if(EC_POINT_cmp(group, newCTx.pk1, Acct_sender.pk, bn_ctx) == 1)
//     {
//         cout << "sender does not match CTx" << endl; 
//         return false; 
//     }

//     if(EC_POINT_cmp(group, newCTx.pk2, Acct_receiver.pk, bn_ctx) == 1)
//     {
//         cout << "receiver does not match CTx" << endl; 
//         return false; 
//     }

//     if(LedgerMaze_Verify_CTx(pp, newCTx) == true){
//         LedgerMaze_Update_Account(pp, newCTx, Acct_sender, Acct_receiver);
//         string ctx_file = ECP_ep2string(newCTx.pk1) + "_" + BN_bn2string(newCTx.sn) + ".ctx"; 
//         LedgerMaze_CTx_serialize(newCTx, ctx_file);  
//         cout << Get_ctxfilename(newCTx) << " is recorded on the blockchain" << endl; 
//         return true; 
//     }
//     else{
//         cout << Get_ctxfilename(newCTx) << " is discarded" << endl; 
//         return false; 
//     }
// }

// /* support more policies */

// struct LIMIT_POLICY{
//     size_t RANGE_LEN;  // the transfer limit = 2^RANGE_LEN - 1 
// };

// struct RATE_POLICY{
//     BIGNUM *t1; 
//     BIGNUM *t2;  // the tax rate = t1/t2
// };

// struct OPEN_POLICY{
//     BIGNUM *v;   // the hidden value = v
// }; 

// void RATE_POLICY_new(RATE_POLICY &predicate)
// {
//     predicate.t1 = BN_new(); 
//     predicate.t2 = BN_new(); 
// }

// void RATE_POLICY_free(RATE_POLICY &predicate)
// {
//     BN_free(predicate.t1);
//     BN_free(predicate.t2);
// }

// void OPEN_POLICY_new(OPEN_POLICY &predicate)
// {
//     predicate.v = BN_new(); 
// }

// void OPEN_POLICY_free(OPEN_POLICY &predicate)
// {
//     BN_free(predicate.v); 
// }

// /* generate a NIZK proof for CT = Enc(pk, v; r)  */
// bool LedgerMaze_Justify_open_policy(LedgerMaze_PP &pp, LedgerMaze_Account &Acct_user, LedgerMaze_CTx &doubtCTx, 
//                              OPEN_POLICY &policy, DLOG_Equality_Proof &open_proof)
// {
//     if(EC_POINT_cmp(group, Acct_user.pk, doubtCTx.pk1, bn_ctx) 
//        && EC_POINT_cmp(group, Acct_user.pk, doubtCTx.pk2, bn_ctx)){
//         cout << "the identity of claimer does not match ctx" << endl; 
//         return false; 
//     }

//     auto start_time = chrono::steady_clock::now(); 

//     DLOG_Equality_PP dlogeq_pp;
//     Get_DLOG_Equality_PP(pp, dlogeq_pp); 
//     Twisted_ElGamal_PP enc_pp; 
//     Get_Enc_PP(pp, enc_pp); 

//     DLOG_Equality_Instance dlogeq_instance; 
//     NIZK_DLOG_Equality_Instance_new(dlogeq_instance); 
//     EC_POINT* T = EC_POINT_new(group); 
//     EC_POINT_mul(group, T, NULL, enc_pp.h, policy.v, bn_ctx); 
    
//     EC_POINT_copy(dlogeq_instance.g1, doubtCTx.transfer.Y); 
//     EC_POINT_sub(dlogeq_instance.g1, dlogeq_instance.g1, T); // g1 = g^r h^v - h^v = g^r
//     EC_POINT_copy(dlogeq_instance.g2, enc_pp.g); 
//     if (EC_POINT_cmp(group, Acct_user.pk, doubtCTx.pk1, bn_ctx) == 0)
//     {
//         EC_POINT_copy(dlogeq_instance.h1, doubtCTx.transfer.X1); // pk1^r
//         EC_POINT_copy(dlogeq_instance.h2, doubtCTx.pk1);  
//     }
//     else
//     {
//         EC_POINT_copy(dlogeq_instance.h1, doubtCTx.transfer.X2);  // pk2^r
//         EC_POINT_copy(dlogeq_instance.h2, doubtCTx.pk2);  
//     }
//     DLOG_Equality_Witness dlogeq_witness; 
//     NIZK_DLOG_Equality_Witness_new(dlogeq_witness);
//     BN_copy(dlogeq_witness.w, Acct_user.sk); 

//     string transcript_str = ""; 
//     NIZK_DLOG_Equality_Prove(dlogeq_pp, dlogeq_instance, dlogeq_witness, transcript_str, open_proof); 
    
//     EC_POINT_free(T);
//     NIZK_DLOG_Equality_Instance_free(dlogeq_instance); 
//     NIZK_DLOG_Equality_Witness_free(dlogeq_witness); 
    
//     auto end_time = chrono::steady_clock::now(); 

//     auto running_time = end_time - start_time;
//     cout << "generate NIZK proof for open policy takes time = " 
//     << chrono::duration <double, milli> (running_time).count() << " ms" << endl;

//     return true; 
// } 

// /* check if the proposed NIZK proof PI for open policy is valid */ 
// bool LedgerMaze_Audit_open_policy(LedgerMaze_PP &pp, EC_POINT *&pk, LedgerMaze_CTx &doubtCTx,  
//                            OPEN_POLICY &policy, DLOG_Equality_Proof &open_proof)
// { 
//     if(EC_POINT_cmp(group, pk, doubtCTx.pk2, bn_ctx) && EC_POINT_cmp(group, pk, doubtCTx.pk1, bn_ctx)){
//         cout << "the identity of claimer does not match" << endl; 
//         return false; 
//     }

//     auto start_time = chrono::steady_clock::now(); 

//     DLOG_Equality_PP dlogeq_pp; 
//     Get_DLOG_Equality_PP(pp, dlogeq_pp); 
//     Twisted_ElGamal_PP enc_pp; 
//     Get_Enc_PP(pp, enc_pp); 

//     DLOG_Equality_Instance dlogeq_instance; 
//     NIZK_DLOG_Equality_Instance_new(dlogeq_instance);
//     EC_POINT *T = EC_POINT_new(group); 
//     EC_POINT_mul(group, T, NULL, enc_pp.h, policy.v, bn_ctx);  // T *= v;
//     EC_POINT_sub(dlogeq_instance.g1, doubtCTx.transfer.Y, T);  // g1 = g^r h^v - h^v = g^r
//     EC_POINT_copy(dlogeq_instance.g2, enc_pp.g);
//     if (EC_POINT_cmp(group, pk, doubtCTx.pk1, bn_ctx) == 0)
//     {
//         EC_POINT_copy(dlogeq_instance.h1, doubtCTx.transfer.X1);
//         EC_POINT_copy(dlogeq_instance.h2, doubtCTx.pk1);  
//     }
//     else
//     {
//         EC_POINT_copy(dlogeq_instance.h1, doubtCTx.transfer.X2);
//         EC_POINT_copy(dlogeq_instance.h2, doubtCTx.pk2);  
//     }
//     bool validity;

//     string transcript_str = "";
//     validity = NIZK_DLOG_Equality_Verify(dlogeq_pp, dlogeq_instance, transcript_str, open_proof); 

//     EC_POINT_free(T); 
//     NIZK_DLOG_Equality_Instance_free(dlogeq_instance); 

//     auto end_time = chrono::steady_clock::now(); 

//     auto running_time = end_time - start_time;
//     cout << "verify NIZK proof for open policy takes time = " 
//     << chrono::duration <double, milli> (running_time).count() << " ms" << endl;

//     return validity; 
// }


// /* 
//     generate NIZK proof for rate policy: CT = Enc(pk, v1) && CT = Enc(pk, v2) 
//     v2/v1 = t1/t2
// */
// bool LedgerMaze_Justify_rate_policy(LedgerMaze_PP &pp, LedgerMaze_Account &Acct_user, LedgerMaze_CTx &ctx1, LedgerMaze_CTx &ctx2,  
//                              RATE_POLICY &policy, DLOG_Equality_Proof &rate_proof)
// {
//     if(EC_POINT_cmp(group, Acct_user.pk, ctx1.pk2, bn_ctx) || 
//        EC_POINT_cmp(group, Acct_user.pk, ctx2.pk1, bn_ctx)){
//         cout << "the identity of claimer does not match" << endl; 
//         return false; 
//     }

//     auto start_time = chrono::steady_clock::now(); 
    
//     DLOG_Equality_PP dlogeq_pp;
//     Get_DLOG_Equality_PP(pp, dlogeq_pp); 
//     Twisted_ElGamal_PP enc_pp; 
//     Get_Enc_PP(pp, enc_pp); 

//     DLOG_Equality_Instance dlogeq_instance; 
//     NIZK_DLOG_Equality_Instance_new(dlogeq_instance); 
//     EC_POINT_copy(dlogeq_instance.g1, enc_pp.g);     // g1 = g 
//     EC_POINT_copy(dlogeq_instance.h1, Acct_user.pk); // g2 = pk = g^sk

//     Twisted_ElGamal_CT C_in; Twisted_ElGamal_CT_new(C_in); 
//     EC_POINT_copy(C_in.X, ctx1.transfer.X2); 
//     EC_POINT_copy(C_in.Y, ctx1.transfer.Y); 
//     Twisted_ElGamal_ScalarMul(C_in, C_in, policy.t1); 
    
//     Twisted_ElGamal_CT C_out; Twisted_ElGamal_CT_new(C_out); 
//     EC_POINT_copy(C_out.X, ctx2.transfer.X1); 
//     EC_POINT_copy(C_out.Y, ctx2.transfer.Y); 
//     Twisted_ElGamal_ScalarMul(C_out, C_out, policy.t2); 

//     Twisted_ElGamal_CT C_diff; Twisted_ElGamal_CT_new(C_diff); 
//     Twisted_ElGamal_HomoSub(C_diff, C_in, C_out);  

//     EC_POINT_copy(dlogeq_instance.g2, C_diff.Y); 
//     EC_POINT_copy(dlogeq_instance.h2, C_diff.X); 

//     DLOG_Equality_Witness dlogeq_witness; 
//     NIZK_DLOG_Equality_Witness_new(dlogeq_witness);
//     BN_copy(dlogeq_witness.w, Acct_user.sk); 

//     string transcript_str = ""; 
//     NIZK_DLOG_Equality_Prove(dlogeq_pp, dlogeq_instance, dlogeq_witness, transcript_str, rate_proof); 
    
//     Twisted_ElGamal_CT_free(C_in);
//     Twisted_ElGamal_CT_free(C_out);
//     Twisted_ElGamal_CT_free(C_diff); 

//     NIZK_DLOG_Equality_Instance_free(dlogeq_instance); 
//     NIZK_DLOG_Equality_Witness_free(dlogeq_witness); 
    
//     auto end_time = chrono::steady_clock::now(); 
//     auto running_time = end_time - start_time;
//     cout << "generate NIZK proof for rate policy takes time = " 
//     << chrono::duration <double, milli> (running_time).count() << " ms" << endl;

//     return true; 
// } 

// /* check if the NIZK proof PI for rate policy is valid */
// bool LedgerMaze_Audit_rate_policy(LedgerMaze_PP &pp, EC_POINT *pk, LedgerMaze_CTx &ctx1, LedgerMaze_CTx &ctx2,  
//                            RATE_POLICY &policy, DLOG_Equality_Proof &rate_proof)
// { 
//     if(EC_POINT_cmp(group, pk, ctx1.pk2, bn_ctx) || 
//        EC_POINT_cmp(group, pk, ctx2.pk1, bn_ctx)){
//         cout << "the identity of claimer does not match" << endl; 
//         return false; 
//     }
    
//     auto start_time = chrono::steady_clock::now(); 
    
//     DLOG_Equality_PP dlogeq_pp; 
//     Get_DLOG_Equality_PP(pp, dlogeq_pp); 
//     Twisted_ElGamal_PP enc_pp; 
//     Get_Enc_PP(pp, enc_pp); 

//     DLOG_Equality_Instance dlogeq_instance; 
//     NIZK_DLOG_Equality_Instance_new(dlogeq_instance); 
//     EC_POINT_copy(dlogeq_instance.g1, enc_pp.g);     // g1 = g 
//     EC_POINT_copy(dlogeq_instance.h1, pk); // g2 = pk = g^sk

//     Twisted_ElGamal_CT C_in; Twisted_ElGamal_CT_new(C_in); 
//     EC_POINT_copy(C_in.X, ctx1.transfer.X2); 
//     EC_POINT_copy(C_in.Y, ctx1.transfer.Y); 
//     Twisted_ElGamal_ScalarMul(C_in, C_in, policy.t1); 
    
//     Twisted_ElGamal_CT C_out; Twisted_ElGamal_CT_new(C_out); 
//     EC_POINT_copy(C_out.X, ctx2.transfer.X1); 
//     EC_POINT_copy(C_out.Y, ctx2.transfer.Y); 
//     Twisted_ElGamal_ScalarMul(C_out, C_out, policy.t2); 

//     Twisted_ElGamal_CT C_diff; Twisted_ElGamal_CT_new(C_diff); 
//     Twisted_ElGamal_HomoSub(C_diff, C_in, C_out);  

//     EC_POINT_copy(dlogeq_instance.g2, C_diff.Y); 
//     EC_POINT_copy(dlogeq_instance.h2, C_diff.X); 

//     string transcript_str = ""; 
//     bool validity = NIZK_DLOG_Equality_Verify(dlogeq_pp, dlogeq_instance, transcript_str, rate_proof); 
    
//     Twisted_ElGamal_CT_free(C_in);
//     Twisted_ElGamal_CT_free(C_out);
//     Twisted_ElGamal_CT_free(C_diff); 

//     NIZK_DLOG_Equality_Instance_free(dlogeq_instance); 

//     auto end_time = chrono::steady_clock::now(); 

//     auto running_time = end_time - start_time;
//     cout << "verify NIZK proof for rate policy takes time = " 
//     << chrono::duration <double, milli> (running_time).count() << " ms" << endl;

//     return validity; 
// }

// /*
//     sender prove an encrypted value C = Enc(pk, m; r) lie in the right range 
//     prover knows m and r
// */


// /*  generate a NIZK proof for limit predicate */
// bool LedgerMaze_Justify_limit_policy(LedgerMaze_PP &pp, LedgerMaze_Account &Acct_user, vector<LedgerMaze_CTx> &ctx_set, 
//                               LIMIT_POLICY &policy, Gadget2_Proof &limit_proof)
// {
//     for(auto i = 0; i < ctx_set.size(); i++){
//         if(EC_POINT_cmp(group, Acct_user.pk, ctx_set[i].pk1, bn_ctx) == 1)
//         {
//             cout << "the identity of claimer does not match" << endl; 
//             return false; 
//         }
//     }

//     auto start_time = chrono::steady_clock::now(); 

//     Twisted_ElGamal_PP enc_pp; 
//     Get_Enc_PP(pp, enc_pp); 
//     Twisted_ElGamal_CT CT_sum; 
//     Twisted_ElGamal_CT_new(CT_sum); 
//     Twisted_ElGamal_CT CT_temp; 
//     for(auto i = 0; i < ctx_set.size(); i++)
//     {
//         CT_temp.X = ctx_set[i].transfer.X1; 
//         CT_temp.Y = ctx_set[i].transfer.Y;
//         Twisted_ElGamal_HomoAdd(CT_sum, CT_sum, CT_temp); 
//     } 
 
//     Gadget_PP gadget_pp;
//     Get_Gadget_PP(pp, gadget_pp); 
 
//     Gadget2_Prove(gadget_pp, Acct_user.pk, CT_sum, Acct_user.sk, policy.RANGE_LEN, limit_proof); 
    
//     Twisted_ElGamal_CT_free(CT_sum); 

//     auto end_time = chrono::steady_clock::now(); 

//     auto running_time = end_time - start_time;
//     cout << "generate NIZK proof for limit policy takes time = " 
//     << chrono::duration <double, milli> (running_time).count() << " ms" << endl;

//     return true;
// } 

// /* check if the proposed NIZK proof for limit policy is valid */ 
// bool LedgerMaze_Audit_limit_policy(LedgerMaze_PP &pp, EC_POINT *pk, vector<LedgerMaze_CTx> &ctx_set, 
//                             LIMIT_POLICY &policy, Gadget2_Proof &limit_proof)
// { 
//     for(auto i = 0; i < ctx_set.size(); i++){
//         if(EC_POINT_cmp(group, pk, ctx_set[i].pk1, bn_ctx) == 1)
//         {
//             cout << "the identity of claimer does not match" << endl; 
//             return false; 
//         }
//     }

//     auto start_time = chrono::steady_clock::now(); 

//     Twisted_ElGamal_PP enc_pp; 
//     Get_Enc_PP(pp, enc_pp); 
//     Twisted_ElGamal_CT CT_sum; 
//     Twisted_ElGamal_CT_new(CT_sum); 
//     Twisted_ElGamal_CT CT_temp; 
//     for(auto i = 0; i < ctx_set.size(); i++)
//     {
//         CT_temp.X = ctx_set[i].transfer.X1; 
//         CT_temp.Y = ctx_set[i].transfer.Y;
//         Twisted_ElGamal_HomoAdd(CT_sum, CT_sum, CT_temp); 
//     } 

//     Gadget_PP gadget_pp;
//     Get_Gadget_PP(pp, gadget_pp); 

//     bool validity = Gadget2_Verify(gadget_pp, pk, CT_sum, policy.RANGE_LEN, limit_proof); 
//     Twisted_ElGamal_CT_free(CT_sum); 

//     auto end_time = chrono::steady_clock::now(); 

//     auto running_time = end_time - start_time;
//     cout << "verify NIZK proof for limit policy takes time = " 
//     << chrono::duration <double, milli> (running_time).count() << " ms" << endl;

//     return validity; 
// }

#endif

// // converts sn to a length-32 HEX string with leading zeros
// string sn_to_string(PGC_PP &pp, BIGNUM *&x)
// {
//     stringstream ss; 
//     ss << setfill('0') << setw(pp.SN_LEN) << BN_bn2hex(x);
//     return ss.str();  
// }

// void PGC_memo2string(PGC_CTx &newCTx, string &aux_str)
// {
//     aux_str += BN_bn2string(newCTx.sn); 
//     aux_str += ECP_ep2string(newCTx.sender_balance.X) + ECP_ep2string(newCTx.sender_balance.Y); 
//     aux_str += ECP_ep2string(newCTx.pk1) + ECP_ep2string(newCTx.pk2); 
//     aux_str += ECP_ep2string(newCTx.transfer.X1) + ECP_ep2string(newCTx.transfer.X2) 
//              + ECP_ep2string(newCTx.transfer.Y);
//     aux_str += BN_bn2string(newCTx.v);
// }

void LedgerMaze_Setup(LedgerMaze_PP &pp, size_t RANGE_LEN, size_t AGG_NUM, 
               size_t SN_LEN, size_t DEC_THREAD_NUM, size_t IO_THREAD_NUM, size_t TUNNING);

void LedgerMaze_Create_Account(LedgerMaze_PP &pp, string identity, BIGNUM *&init_balance, BIGNUM *&sn, LedgerMaze_Account &newAcct);

void LedgerMaze_Account_serialize(LedgerMaze_Account &user, string LedgerMaze_account_file);

void LedgerMaze_Initialize(LedgerMaze_PP &pp);

void LedgerMaze_Account_deserialize(LedgerMaze_Account &user, string LedgerMaze_account_file);

void LedgerMaze_Mint(LedgerMaze_PP &pp, LedgerMaze_Account &Acct, BIGNUM *&value,LedgerMaze_CTx &newCT,BIGNUM *&new_pt_balance,ZK_BALANCE_CT &new_zk_balance);