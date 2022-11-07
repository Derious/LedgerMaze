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

#define MINT 0
#define REDEEM 1
#define SENDCHEQUE 2
#define DEPOSITCHEQUE 3
#define REFRESHCHEQUE 4

#define DEMO           // demo mode 

//#define DEBUG        // show debug information 
void generate_random_instance_witness(DLOG_Equality_PP &pp, 
                                      DLOG_Equality_Instance &instance, 
                                      DLOG_Equality_Witness &witness, 
                                      bool flag)
{
    // generate a true statement (false with overwhelming probability)
    SplitLine_print('-'); 
    if (flag == true){
        cout << ">>> generate a DDH tuple" << endl;
    }
    else{
        cout << ">>> generate a random tuple" << endl; 
    } 
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
    One_Out_of_N_Proof_Instance pi_1outoft1__instance;
    One_Out_of_N_Proof_Instance pi_1outoft2__instance;
    One_Out_of_N_Proof_Instance pi_1outoft3__instance;
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

    Pi_sendcheque pi_depositcheque;

    Pi_refreshcheque pi_refreshcheque;
    /* data */
};


// define the structure for confidential transaction
struct LedgerMaze_CTx{
    BIGNUM *sn;                           // serial number: uniquely defines a transaction
    BIGNUM *type;                             // trans type in {mint, redeem, sendcheque,refreshcheque,depositcheque}
    EC_POINT *pk1;                        // sender = pk1
    EC_POINT *tag;                        //only used in Refreshcheque, tag=Commit(r)=g^-r;
    BIGNUM *value;                        // the transfer value
    Twisted_ElGamal_CT rest_balance;      //ciphertext of rest balance, rest_balance=ISE.Enc(pk_sender,pt_restbalabce,r1)
    Twisted_ElGamal_CT transfer;          //ciphertext of transfer value, transfer=ISE.Enc(pk_receiver,value,r2), only used in Sendcheque
    Hashed_ElGamal_CT  aux;               //only used in Sendcheque
    // valid proof
    LedgerMaze_trans proof;
    Twisted_ElGamal_SIG sig;            //signature of trans
};

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
void LedgerMaze_Account_serialize(LedgerMaze_Account &user, string LedgerMaze_account_file)
{
    ofstream fout; 
    char identity[BN_LEN] = {0};
    strcpy(identity,user.identity.c_str());
    fout.open(LedgerMaze_account_file, ios::binary);
    fout.write((char *)(identity), sizeof(identity));

    ECP_serialize(user.keypair.pk, fout);              
    BN_serialize(user.keypair.sk, fout);
    ZK_BALANCE_CT_serialize(user.zk_balance,fout);
    BN_serialize(user.pt_balance, fout);
    fout.close();  
};
void LedgerMaze_Account_deserialize(LedgerMaze_Account &user, string LedgerMaze_account_file)
{
    ifstream fin; 
    char identity[BN_LEN] = {0};
    fin.open(LedgerMaze_account_file, ios::binary);
    fin.read(identity, sizeof(identity));
    user.identity.append(identity);

    ECP_deserialize(user.keypair.pk, fin);              
    BN_deserialize(user.keypair.sk, fin);   
    ZK_BALANCE_CT_deserialize(user.zk_balance, fin);
    BN_deserialize(user.pt_balance, fin);
    fin.close();  
};
// template<typename T>
// string to_string(T value) {
//   std::ostringstream os;
//   os << value;
//   return os.str;
// }
void LedgerMaze_cheque_serialize(LedgerMaze_Cheque &cheque, BIGNUM *cheque_bn)
{
    ofstream fout; 
    string LedgerMaze_cheque_file=BN_bn2string(cheque_bn)+".cheque";
    fout.open(LedgerMaze_cheque_file, ios::binary);
    
    Twisted_ElGamal_CT_serialize(cheque.C,fout);
    ECP_serialize(cheque.pk,fout);
    BN_serialize(cheque.r,fout);
    BN_serialize(cheque.transfer,fout);

    fout.close();  

}

void LedgerMaze_cheque_deserialize(LedgerMaze_Cheque &cheque, string LedgerMaze_cheque_file)
{
    ifstream fin; 
    fin.open(LedgerMaze_cheque_file+".cheque", ios::binary);

    Twisted_ElGamal_CT_deserialize(cheque.C,fin);
    ECP_deserialize(cheque.pk,fin);
    BN_deserialize(cheque.r,fin);
    BN_deserialize(cheque.transfer,fin);

    fin.close();   

}


void LedgerMaze_CT_serialize(LedgerMaze_CTx &ct, BIGNUM *ct_bn)
{
    ofstream fout; 
    string LedgerMaze_ct_file=BN_bn2string(ct_bn)+".ctx";
   
    fout.open(LedgerMaze_ct_file, ios::binary);

    BN_serialize(ct.sn,fout);
    BN_serialize(ct.type,fout);
    
    ECP_serialize(ct.pk1, fout);  
    BN_serialize(ct.value,fout);
    ECP_serialize(ct.tag, fout);
    Twisted_ElGamal_CT_serialize(ct.rest_balance,fout);
    Twisted_ElGamal_CT_serialize(ct.transfer,fout);
    Hashed_ElGamal_CT_serialize(ct.aux,fout);
    Twisted_ElGamal_SIG_serialize(ct.sig,fout);

    fout.close();  

}




void LedgerMaze_CT_deserialize(LedgerMaze_CTx &CT, string LedgerMaze_CT_file)
{
    ifstream fin; 
    fin.open(LedgerMaze_CT_file+".ctx", ios::binary);

    if(!fin.is_open())
    {
         return;
    }

    BN_deserialize(CT.sn,fin);
    BN_deserialize(CT.type,fin);
    ECP_deserialize(CT.pk1,fin);
    BN_deserialize(CT.value,fin);
    ECP_deserialize(CT.tag,fin);
    Twisted_ElGamal_CT_deserialize(CT.rest_balance,fin);
    Twisted_ElGamal_CT_deserialize(CT.transfer,fin);
    Hashed_ElGamal_CT_deserialize(CT.aux,fin);
    Twisted_ElGamal_SIG_deserialize(CT.sig,fin);

    fin.close();  
}
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
    BN_print_dec(Acct.pt_balance,"pt_balance");
    // BN_print(Acct.pt_balance, "pt_balance");   
    // cout << "pt_balance "<< BN_bn2dec(Acct.pt_balance) << endl;
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
    // printf("type=%d\n",newCTx.type);
    BN_print(newCTx.type,"type");
    BN_print(newCTx.sn,"sn");
    ECP_print(newCTx.pk1, "pk1"); 
    cout << endl;  
    cout << "rest_balance/refresh value >>>" << endl;
    Twisted_ElGamal_CT_print(newCTx.rest_balance);
    cout << endl; 
    if(BN_cmp(newCTx.type,BN_0)==0)//mint
    {
            cout << "pi_mint_pi_random >>>" << endl; 
            DLOG_Equality_Proof_print(newCTx.proof.pi_mint.pi_random);
            cout << "pi_mint_pi_range >>>" << endl; 
            Bullet_Proof_print(newCTx.proof.pi_mint.pi_range);
            cout << endl; 
    }
    if(BN_cmp(newCTx.type,BN_1)==0)//redeem
    {
            cout << "pi_redeem_pi_random >>>" << endl; 
            DLOG_Equality_Proof_print(newCTx.proof.pi_redeem.pi_random);
            cout << "pi_redeem_pi_range >>>" << endl; 
            Bullet_Proof_print(newCTx.proof.pi_redeem.pi_range);
            cout << endl;  
    }
    if(BN_cmp(newCTx.type,BN_2)==0)//SENDCHEQUE
    {
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
    }
    if(BN_cmp(newCTx.type,BN_3)==0)//DEPOSITCHEQUE
    {
            cout << "transfer >>>" << endl;
            Twisted_ElGamal_CT_print(newCTx.transfer);
            cout << "aux >>>" << endl;
            Hashed_ElGamal_CT_print(newCTx.aux);
            cout << "pi1>>>" << endl; 
            cout << "pi_depositcheque.pi1.pi_enc >>>" << endl; 
            Plaintext_Knowledge_Proof_print(newCTx.proof.pi_depositcheque.pi1.pi_enc);
            cout << "pi_depositcheque.pi1.pi_range >>>" << endl; 
            Bullet_Proof_print(newCTx.proof.pi_depositcheque.pi1.pi_range);
            cout << "pi2>>>" << endl; 
            cout << "pi_depositcheque.pi2.pi_enc >>>" << endl; 
            Plaintext_Knowledge_Proof_print(newCTx.proof.pi_depositcheque.pi2.pi_enc);
            cout << "pi_depositcheque.pi2.pi_range >>>" << endl; 
            Bullet_Proof_print(newCTx.proof.pi_depositcheque.pi2.pi_range);
            cout << "pi3>>>" << endl; 
            cout << "pi_depositcheque.pi3.pi_dualenc >>>" << endl; 
            Plaintext_Equality_Proof_print(newCTx.proof.pi_depositcheque.pi3.pi_dualenc);
            cout << endl; 
    }
    if(BN_cmp(newCTx.type,BN_4)==0)//REFRESHCHEQUE
    {
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
    newCTx.type = BN_new(); 
    Twisted_ElGamal_CT_new(newCTx.rest_balance);
    Twisted_ElGamal_CT_new(newCTx.transfer);
    Hashed_ElGamal_CT_new(newCTx.aux);
    Twisted_ElGamal_SIG_new(newCTx.sig);
    // One_Out_of_N_Proof_Instance instance;
    // One_Out_of_N_Proof_Instance_new(instance);

    Bullet_PP bullet_pp; 
    Get_Bullet_PP(pp, bullet_pp);
    if(CTtype==MINT)
    {
            BN_copy(newCTx.type,BN_0);
            NIZK_DLOG_Equality_Proof_new(newCTx.proof.pi_mint.pi_random);
            NIZK_DLOG_Equality_Instance_new(newCTx.proof.pi_mint.pi_random_instance);
            Bullet_Proof_new(newCTx.proof.pi_mint.pi_range);
            Bullet_Instance_new(bullet_pp,newCTx.proof.pi_mint.pi_range_instance);

    }
    if(CTtype==REDEEM)
    {
            BN_copy(newCTx.type,BN_1);
            NIZK_DLOG_Equality_Proof_new(newCTx.proof.pi_redeem.pi_random);
            NIZK_DLOG_Equality_Instance_new(newCTx.proof.pi_redeem.pi_random_instance);
            Bullet_Proof_new(newCTx.proof.pi_redeem.pi_range);
            Bullet_Instance_new(bullet_pp,newCTx.proof.pi_redeem.pi_range_instance);

    }
    if(CTtype==SENDCHEQUE)
    {
            BN_copy(newCTx.type,BN_2);
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
    }
    if(CTtype==DEPOSITCHEQUE)
    {
            BN_copy(newCTx.type,BN_3);
            NIZK_Plaintext_Knowledge_Proof_new(newCTx.proof.pi_depositcheque.pi1.pi_enc);
            NIZK_Plaintext_Knowledge_Instance_new(newCTx.proof.pi_depositcheque.pi1.pi_enc_instance);
            Bullet_Proof_new(newCTx.proof.pi_depositcheque.pi1.pi_range);
            Bullet_Instance_new(bullet_pp,newCTx.proof.pi_depositcheque.pi1.pi_range_instance);
            NIZK_Plaintext_Knowledge_Proof_new(newCTx.proof.pi_depositcheque.pi2.pi_enc);
            NIZK_Plaintext_Knowledge_Instance_new(newCTx.proof.pi_depositcheque.pi2.pi_enc_instance);
            Bullet_Proof_new(newCTx.proof.pi_depositcheque.pi2.pi_range);
            Bullet_Instance_new(bullet_pp,newCTx.proof.pi_depositcheque.pi2.pi_range_instance);
            NIZK_Plaintext_Equality_Proof_new(newCTx.proof.pi_depositcheque.pi3.pi_dualenc);
            NIZK_Plaintext_Equality_Instance_new(newCTx.proof.pi_depositcheque.pi3.pi_dualenc_instance);
    }
    if(CTtype==REFRESHCHEQUE)
    {
            BN_copy(newCTx.type,BN_4);
            One_Out_of_N_Proof_Instance_new(newCTx.proof.pi_refreshcheque.pi1.pi_1outoft1__instance);
            One_Out_of_N_Proof_Instance_new(newCTx.proof.pi_refreshcheque.pi1.pi_1outoft2__instance);
            One_Out_of_N_Proof_Instance_new(newCTx.proof.pi_refreshcheque.pi1.pi_1outoft3__instance);
            One_Out_of_N_Proof_new(newCTx.proof.pi_refreshcheque.pi1.pi_oneoutofn1,newCTx.proof.pi_refreshcheque.pi1.pi_1outoft1__instance);
            One_Out_of_N_Proof_new(newCTx.proof.pi_refreshcheque.pi1.pi_oneoutofn2,newCTx.proof.pi_refreshcheque.pi1.pi_1outoft2__instance);
            One_Out_of_N_Proof_new(newCTx.proof.pi_refreshcheque.pi1.pi_oneoutofn3,newCTx.proof.pi_refreshcheque.pi1.pi_1outoft3__instance);
            NIZK_Plaintext_Knowledge_Proof_new(newCTx.proof.pi_refreshcheque.pi2.pi_enc);
            NIZK_Plaintext_Knowledge_Instance_new(newCTx.proof.pi_refreshcheque.pi2.pi_enc_instance);
            Bullet_Proof_new(newCTx.proof.pi_refreshcheque.pi2.pi_range);
            Bullet_Instance_new(bullet_pp,newCTx.proof.pi_refreshcheque.pi2.pi_range_instance);
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

    if (BN_cmp(newCTx.type,BN_0)==0)
    {
            NIZK_DLOG_Equality_Proof_free(newCTx.proof.pi_mint.pi_random);
            NIZK_DLOG_Equality_Instance_free(newCTx.proof.pi_mint.pi_random_instance);
            Bullet_Proof_free(newCTx.proof.pi_mint.pi_range);
            Bullet_Instance_free(newCTx.proof.pi_mint.pi_range_instance);
    }
    else if (BN_cmp(newCTx.type,BN_1)==0)
    {
            NIZK_DLOG_Equality_Proof_free(newCTx.proof.pi_redeem.pi_random);
            NIZK_DLOG_Equality_Instance_free(newCTx.proof.pi_redeem.pi_random_instance);
            Bullet_Proof_free(newCTx.proof.pi_redeem.pi_range);
            Bullet_Instance_free(newCTx.proof.pi_redeem.pi_range_instance);
    }
    else if (BN_cmp(newCTx.type,BN_2)==0)
    {
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
    }
    else if (BN_cmp(newCTx.type,BN_3)==0)
    {
            NIZK_Plaintext_Knowledge_Proof_free(newCTx.proof.pi_depositcheque.pi1.pi_enc);
            NIZK_Plaintext_Knowledge_Instance_free(newCTx.proof.pi_depositcheque.pi1.pi_enc_instance);
            Bullet_Proof_free(newCTx.proof.pi_depositcheque.pi1.pi_range);
            Bullet_Instance_free(newCTx.proof.pi_depositcheque.pi1.pi_range_instance);
            NIZK_Plaintext_Knowledge_Proof_free(newCTx.proof.pi_depositcheque.pi2.pi_enc);
            NIZK_Plaintext_Knowledge_Instance_free(newCTx.proof.pi_depositcheque.pi2.pi_enc_instance);
            Bullet_Proof_free(newCTx.proof.pi_depositcheque.pi2.pi_range);
            Bullet_Instance_free(newCTx.proof.pi_depositcheque.pi2.pi_range_instance);
            NIZK_Plaintext_Equality_Proof_free(newCTx.proof.pi_depositcheque.pi3.pi_dualenc);
            NIZK_Plaintext_Equality_Instance_free(newCTx.proof.pi_depositcheque.pi3.pi_dualenc_instance);
    }
    else if (BN_cmp(newCTx.type,BN_4)==0)
    {
            One_Out_of_N_Proof_Instance_free(newCTx.proof.pi_refreshcheque.pi1.pi_1outoft1__instance);
            One_Out_of_N_Proof_Instance_free(newCTx.proof.pi_refreshcheque.pi1.pi_1outoft2__instance);
            One_Out_of_N_Proof_Instance_free(newCTx.proof.pi_refreshcheque.pi1.pi_1outoft3__instance);
            One_Out_of_N_Proof_free(newCTx.proof.pi_refreshcheque.pi1.pi_oneoutofn1);
            One_Out_of_N_Proof_free(newCTx.proof.pi_refreshcheque.pi1.pi_oneoutofn2);
            One_Out_of_N_Proof_free(newCTx.proof.pi_refreshcheque.pi1.pi_oneoutofn3);
            // One_Out_of_N_Proof_Instance_free()
            NIZK_Plaintext_Knowledge_Proof_new(newCTx.proof.pi_refreshcheque.pi2.pi_enc);
            NIZK_Plaintext_Knowledge_Instance_free(newCTx.proof.pi_refreshcheque.pi2.pi_enc_instance);
            Bullet_Proof_free(newCTx.proof.pi_refreshcheque.pi2.pi_range);
            Bullet_Instance_free(newCTx.proof.pi_refreshcheque.pi2.pi_range_instance);
    }
    else    ;
    

    BN_free(newCTx.type);
    
}

void LedgerMaze_Cheque_new(LedgerMaze_Cheque &cheque)
{
    Twisted_ElGamal_CT_new(cheque.C);   //C=Enc(pk,transfer,r)
    cheque.pk = EC_POINT_new(group);            //recriver's pk
    cheque.transfer = BN_new();       //transfer value
    cheque.r = BN_new();              //random number

}

void LedgerMaze_Cheque_free(LedgerMaze_Cheque &cheque)
{
    Twisted_ElGamal_CT_free(cheque.C);
    EC_POINT_free(cheque.pk);
    BN_free(cheque.transfer);
    BN_free(cheque.r);
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
    BIGNUM *v = BN_new(); 
    BN_set_word(v,0);
    Hash_String_to_BN(newAcct.identity, r); 
    BN_copy(newAcct.zk_balance.r, r); 
    BN_copy(newAcct.zk_balance.v, v);
    Twisted_ElGamal_Enc(enc_pp, newAcct.zk_balance.pk, BN_0,r,newAcct.zk_balance.C);
    

    #ifdef DEMO
    cout << identity << "'s account creation succeeds" << endl;
    ECP_print(newAcct.zk_balance.pk, "pk"); 
    cout << identity << "'s initial pt_balance = "<< endl; 
    BN_print_dec(init_balance); 
    cout << identity << "'s initial zk_balance = "<< endl; 
    Twisted_ElGamal_CT_print(newAcct.zk_balance.C);
    cout << identity << "'s initial zk_balance(real value) = "<< endl; 
    BN_print_dec(newAcct.zk_balance.v); 
    SplitLine_print('-'); 
    #endif 
}

void LedgerMaze_Commit(LedgerMaze_PP &pp, EC_POINT* tag, BIGNUM* r)
{
    //get the k^-r
    EC_POINT* k = EC_POINT_new(group);

    EC_POINT_copy(k,pp.h);

    EC_POINT_invert(group,k,bn_ctx);

    EC_POINT_mul(group,tag,NULL,k,r,bn_ctx);
}






void LedgerMaze_BIGNUMtoScalar(secp_primitives::Scalar &r, BIGNUM* a)
{
    unsigned char bin_temp[BN_LEN*2+1] = {0};
    BN_bn2bin(a,bin_temp);
    secp_primitives::Scalar temp(bin_temp);
    r = temp;
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
void test_nizk_dlog_equality(LedgerMaze_PP &pp, DLOG_Equality_Instance &instance, 
                              DLOG_Equality_Witness &witness, 
                              string &transcript_str, 
                              DLOG_Equality_Proof &proof)
{
    cout << "begin the test of dlog equality proof (standard version) >>>" << endl; 
    
    DLOG_Equality_PP pp1;
    NIZK_DLOG_Equality_PP_new(pp1);  
    NIZK_DLOG_Equality_Setup(pp1);
    // DLOG_Equality_Instance instance; 
    // NIZK_DLOG_Equality_Instance_new(instance); 
    // DLOG_Equality_Witness witness; 
    // NIZK_DLOG_Equality_Witness_new(witness); 
    // DLOG_Equality_Proof proof; 
    // NIZK_DLOG_Equality_Proof_new(proof); 
    string str_pro="";
    string str_ver="";
    // generate_random_instance_witness(pp1, instance, witness, true);
    NIZK_DLOG_Equality_Prove(pp1, instance, witness, str_pro, proof); 
    NIZK_DLOG_Equality_Verify(pp1, instance, str_ver, proof);

    NIZK_DLOG_Equality_PP_free(pp1); 
    NIZK_DLOG_Equality_Instance_free(instance);
    NIZK_DLOG_Equality_Witness_free(witness);
    NIZK_DLOG_Equality_Proof_free(proof);
}

void LedgerMaze_nizk_plaintext_equality(bool flag, Plaintext_Equality_Instance &instance, Plaintext_Equality_Proof &proof, Plaintext_Equality_Witness &witness)
{
    //cout << "begin the test of NIZKPoK for plaintext equality >>>" << endl; 

    Plaintext_Equality_PP pi3pp;
    NIZK_Plaintext_Equality_PP_new(pi3pp);    
    NIZK_Plaintext_Equality_Setup(pi3pp);
    string transcript_str; 
    transcript_str = ""; 
    NIZK_Plaintext_Equality_Prove(pi3pp, instance, witness, transcript_str, proof); 

    NIZK_Plaintext_Equality_PP_free(pi3pp); 

}






//output the update zk_balance and pt_balance,but don't update them in the Acct until verify success
int LedgerMaze_Mint(LedgerMaze_PP &pp, LedgerMaze_Account &Acct, BIGNUM *&value,LedgerMaze_CTx &newCT,BIGNUM *&new_pt_balance,ZK_BALANCE_CT &new_zk_balance)
{
    #ifdef DEMO
    cout << "begin to Mint and genetate CTx >>>>>>" << endl; 
    #endif
    //SplitLine_print('-'); 

    if(BN_cmp(value,BN_0)==0||BN_cmp(value, Acct.pt_balance)>0)
    {
        cout << "the mint transfer value should in (0,pt_balance]" << endl;
        return -1;
    }
    Twisted_ElGamal_PP enc_pp;
    Get_Enc_PP(pp, enc_pp);
    //set CT fixed value
    EC_POINT_copy(newCT.pk1,Acct.keypair.pk);
    BN_copy(newCT.value, value);
    
    //3. pt_balance*=pt_balance-Vtransfer
    BN_sub(new_pt_balance,Acct.pt_balance,value); 


    BIGNUM *r = BN_new(); //4. r*
    BN_random(r); 

    BIGNUM *new_realzk_balance = BN_new(); 
    BN_add(new_realzk_balance,Acct.zk_balance.v,value);//5. Vu*=Vu+Vtransfer
    //6. CVu*=ISE.Enc(pkU,Vu*;r*);
    Twisted_ElGamal_Enc(enc_pp,Acct.keypair.pk,new_realzk_balance,r,newCT.rest_balance);

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

    //g1=g,h1=g^w_r=(g^w_r*h^v)/h^v
    EC_POINT *v_H= EC_POINT_new(group);
    EC_POINT_copy(newCT.proof.pi_mint.pi_random_instance.g1,enc_pp.g);
    EC_POINT_mul(group,v_H,NULL,enc_pp.h,value,bn_ctx); //bn=h^value
    EC_POINT_sub(newCT.proof.pi_mint.pi_random_instance.h1, new_zk_balance.C.Y, Acct.zk_balance.C.Y);//h1=g^w_r*h^v
    EC_POINT_sub(newCT.proof.pi_mint.pi_random_instance.h1, newCT.proof.pi_mint.pi_random_instance.h1,v_H);//h1=g^w_r
    
    // BN_print(value,"***transfer_value");
    // BN_print(new_zk_balance.v,"***new_zk_balance_value");
    // BN_print(Acct.zk_balance.v,"***Acct.zk_balance_value");
    // ECP_print(newCT.proof.pi_mint.pi_random_instance.h1,"***compute h1");
    // EC_POINT *tmp= EC_POINT_new(group);
    // EC_POINT_mul(group,tmp,NULL,enc_pp.g,w_r,bn_ctx); 
    // ECP_print(tmp,"***actual h1");

    //g2=pk,h2 = pk^w_r
    EC_POINT_copy(newCT.proof.pi_mint.pi_random_instance.g2,Acct.keypair.pk);
    EC_POINT_sub(newCT.proof.pi_mint.pi_random_instance.h2, new_zk_balance.C.X, Acct.zk_balance.C.X);

    // ECP_print(newCT.proof.pi_mint.pi_random_instance.h2,"***compute h2");
    // EC_POINT_mul(group,tmp,NULL,Acct.keypair.pk,w_r,bn_ctx); 
    // ECP_print(tmp,"***actual h2");

    DLOG_Equality_Witness dlogeq_witness; 
    NIZK_DLOG_Equality_Witness_new(dlogeq_witness); 
    BN_copy(dlogeq_witness.w, w_r); 

    // BN_print(dlogeq_witness.w,"w_r");
    string str_pro = "";
    // string str_ver = "";
    NIZK_DLOG_Equality_Prove(dlogeq_pp, newCT.proof.pi_mint.pi_random_instance, dlogeq_witness, str_pro, newCT.proof.pi_mint.pi_random);
    // cout<<"***P_V"<<NIZK_DLOG_Equality_Verify(dlogeq_pp,newCT.proof.pi_mint.pi_random_instance,str_ver,newCT.proof.pi_mint.pi_random)<<endl;


    // aggregated range proof for new_realzk_balance lie in the right range 
    Bullet_PP bullet_pp; 
    Get_Bullet_PP(pp, bullet_pp);
    EC_POINT_copy(newCT.proof.pi_mint.pi_range_instance.C[0], newCT.rest_balance.Y);
    Bullet_Witness bullet_witness; 
    Bullet_Witness_new(bullet_pp, bullet_witness);
    BN_copy(bullet_witness.r[0], r);
    BN_copy(bullet_witness.v[0], new_realzk_balance);
    Bullet_Prove(bullet_pp, newCT.proof.pi_mint.pi_range_instance, bullet_witness, str_pro, newCT.proof.pi_mint.pi_range);

    //convert to memo=(pkU,sn,Vtransfer,CVu*)
    std::string memo(ECP_ep2string(newCT.pk1));
    memo.append(BN_bn2hex((const BIGNUM*)newCT.sn));
    memo.append(BN_bn2hex((const BIGNUM*)value)); 
    char buffer[POINT_LEN*4+1] = {0};
    Twisted_ElGamal_CT_to_string(newCT.rest_balance,buffer);
    memo.append(reinterpret_cast<const char*>(buffer));
    
    BIGNUM *memo_bn=BN_new();
    Hash_String_to_BN(memo,memo_bn);
    //cout << "step 10" << endl;
    //10. sigma=ISE.Sign(skU,memo)
    Twisted_ElGamal_Sig(enc_pp,Acct.keypair.sk,memo_bn,newCT.sig);
    //cout << "step 11" << endl;

    
    BN_free(r);
    BN_free(w_r);
    BN_free(memo_bn);
    BN_free(new_realzk_balance);
    // Twisted_ElGamal_PP_free(enc_pp);
    // NIZK_DLOG_Equality_PP_free(dlogeq_pp);
    // EC_POINT_free(v_H);
    // NIZK_DLOG_Equality_Witness_free(dlogeq_witness);
    // // Bullet_PP_free(bullet_pp);
    // Bullet_Witness_free(bullet_witness);
    // cout<<"***P_VSIG"<<Twisted_ElGamal_Ver(enc_pp,Acct.keypair.pk,memo_bn,newCT.sig)<<endl;
    return 0;
}

int LedgerMaze_Redeem(LedgerMaze_PP &pp, LedgerMaze_Account &Acct, BIGNUM *&value,LedgerMaze_CTx &newCT,BIGNUM *&new_pt_balance,ZK_BALANCE_CT &new_zk_balance)
{
    #ifdef DEMO
    cout << "begin to Redeem and genetate CTx >>>>>>" << endl; 
    #endif
    //SplitLine_print('-'); 
    //2.
    if(BN_cmp(value,BN_0)==0||BN_cmp(value,Acct.zk_balance.v)>0)
    {
        cout << "the redeem transfer value should in (0,zk_balance.v]" << endl;
        return -1;
    }
    Twisted_ElGamal_PP enc_pp;
    Get_Enc_PP(pp, enc_pp);

    //set CT fixed value
    EC_POINT_copy(newCT.pk1,Acct.keypair.pk);
    BN_copy(newCT.value, value);

    //3. pt_balance*=pt_balance+Vtransfer
    BN_add(new_pt_balance,Acct.pt_balance,value);

    //4. r*
    BIGNUM *r = BN_new(); 
    BN_random(r); 

    //5. Vu*=Vu-Vtransfer
    BIGNUM *new_realzk_balance = BN_new(); 
    BN_sub(new_realzk_balance,Acct.zk_balance.v,value);

    //6. CVu*=ISE.Enc(pkU,Vu*;r*);
    Twisted_ElGamal_Enc(enc_pp,Acct.keypair.pk,new_realzk_balance,r,newCT.rest_balance);


    //7. sn++
    BN_add(newCT.sn,Acct.zk_balance.sn,BN_1);

    //8. set zk_balance*
    Twisted_ElGamal_CT_copy(new_zk_balance.C,newCT.rest_balance);
    EC_POINT_copy(new_zk_balance.pk,Acct.keypair.pk);
    BN_copy(new_zk_balance.r,r);
    BN_copy(new_zk_balance.sn,newCT.sn);   //set sn++
    BN_copy(new_zk_balance.v,new_realzk_balance);

    //9. generate pi_redeem
    BIGNUM *w_r = BN_new();
    BN_mod_sub(w_r, r, Acct.zk_balance.r, order, bn_ctx); //w_r=r*-r mod n;
    // cout<< "P_C1****************" << endl;
    // BN_print(w_r,"w_r");
    // generate NIZK proof for validity of new zk_balance and old zk_balance
    DLOG_Equality_PP dlogeq_pp; 
    NIZK_DLOG_Equality_PP_new(dlogeq_pp);
    Get_DLOG_Equality_PP(pp, dlogeq_pp);
    //g1=g,h1=g^w_r=(g^w_r*h^v)/h^v
    EC_POINT *v_H= EC_POINT_new(group);
    EC_POINT_copy(newCT.proof.pi_redeem.pi_random_instance.g1,enc_pp.g);
    EC_POINT_mul(group,v_H,NULL,enc_pp.h,value,bn_ctx); //bn=h^v
    EC_POINT_sub(newCT.proof.pi_redeem.pi_random_instance.h1, new_zk_balance.C.Y, Acct.zk_balance.C.Y);//h1=g^w_r*h^(-v)    
    EC_POINT_add(group,newCT.proof.pi_redeem.pi_random_instance.h1, newCT.proof.pi_redeem.pi_random_instance.h1,v_H,bn_ctx);//h1=g^w_r

    // ECP_print(newCT.proof.pi_redeem.pi_random_instance.h1,"compute h1");
    // EC_POINT *tmp= EC_POINT_new(group);
    // EC_POINT_mul(group,tmp,NULL,enc_pp.g,w_r,bn_ctx); 
    // ECP_print(tmp,"actual h1");
    // cout<<"--------------9.2-------"<<endl;
    //g2=pk,h2 = pk^w_r
    EC_POINT_copy(newCT.proof.pi_redeem.pi_random_instance.g2,Acct.keypair.pk);
    EC_POINT_sub(newCT.proof.pi_redeem.pi_random_instance.h2, new_zk_balance.C.X, Acct.zk_balance.C.X);
    
    // ECP_print(newCT.proof.pi_redeem.pi_random_instance.h2,"compute h2");
    // EC_POINT_mul(group,tmp,NULL,Acct.keypair.pk,w_r,bn_ctx); 
    // ECP_print(tmp,"actual h2");

    DLOG_Equality_Witness dlogeq_witness; 
    NIZK_DLOG_Equality_Witness_new(dlogeq_witness); 
    BN_copy(dlogeq_witness.w, w_r); 
    string transcript_str = "";
    NIZK_DLOG_Equality_Prove(dlogeq_pp, newCT.proof.pi_redeem.pi_random_instance, dlogeq_witness, transcript_str, newCT.proof.pi_redeem.pi_random);
    // string transcript_str_v = "";
    // cout<<"******P_V"<<NIZK_DLOG_Equality_Verify(dlogeq_pp,newCT.proof.pi_redeem.pi_random_instance,transcript_str_v,newCT.proof.pi_redeem.pi_random)<<endl;
    // aggregated range proof for new_realzk_balance lie in the right range 
    //newCv.Y=g^r*h^new_realzk_balance
    Bullet_PP bullet_pp; 
    Get_Bullet_PP(pp, bullet_pp);
    EC_POINT_copy(newCT.proof.pi_redeem.pi_range_instance.C[0], newCT.rest_balance.Y);
    Bullet_Witness bullet_witness; 
    Bullet_Witness_new(bullet_pp, bullet_witness);
    BN_copy(bullet_witness.r[0], r);
    BN_copy(bullet_witness.v[0], new_realzk_balance);
    Bullet_Prove(bullet_pp, newCT.proof.pi_redeem.pi_range_instance, bullet_witness, transcript_str, newCT.proof.pi_redeem.pi_range);
    //convert to memo=(pkU,sn,Vtransfer,CVu*)
    std::string memo(ECP_ep2string(newCT.pk1));
    memo.append(BN_bn2hex((const BIGNUM*)newCT.sn));
    memo.append(BN_bn2hex((const BIGNUM*)value)); 
    char buffer[POINT_LEN*4+1] = {0};
    Twisted_ElGamal_CT_to_string(newCT.rest_balance,buffer);
    memo.append(reinterpret_cast<const char*>(buffer));

    BIGNUM *memo_bn = BN_new();
    // Hash_String_to_BN(memo,memo_bn);
    // BN_print(memo_bn,"$$$P_memo_bn");

    //10. sigma=ISE.Sign(skU,memo)
    Twisted_ElGamal_Sig(enc_pp,Acct.keypair.sk,memo_bn,newCT.sig);

    // #ifdef DEMO
    //     cout << "the context of CTx >>>>>>" << endl; 
    //     LedgerMaze_CTx_print(newCT);
    //     cout << "the context of zk >>>>>>" << endl; 
    //     ZK_BALANCE_CT_print(new_zk_balance);
    //     BN_print(new_pt_balance,"new_pt_balance");
    // #endif

    return 0;

}


//output the update zk_balance and pt_balance,but don't update them in the Acct until verify success
//SendCheque 
int LedgerMaze_SendCheque(LedgerMaze_PP &pp, LedgerMaze_Account &AcctU, LedgerMaze_Account &AcctR, BIGNUM *&Vcheque, 
LedgerMaze_CTx &newCT,BIGNUM *&new_pt_balance,ZK_BALANCE_CT &new_zk_balance, LedgerMaze_Cheque &ChequeR)
{

    #ifdef DEMO
    cout << "begin to SendCheque and genetate CTx >>>>>>" << endl; 
    #endif
    //SplitLine_print('-'); 
    if(BN_cmp(Vcheque,BN_0)==0||BN_cmp(Vcheque,AcctU.zk_balance.v)>0)
    {
        cout << "the send value should in (0,zk_balance.v]" << endl;
        return -1;
    }


    BIGNUM *r, *r1, *r2, *Vnew;
    string transcript_str; 
    r = BN_new();
    r1 = BN_new();
    r2 = BN_new();
    Vnew = BN_new();
    BN_copy(r,AcctU.zk_balance.r);
    BN_random(r1);
    BN_random(r2);

    BN_sub(Vnew,AcctU.zk_balance.v,Vcheque);

    //set new_pt_balance as old AcctU.pt_balance
    BN_copy(new_pt_balance,AcctU.pt_balance);

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
 
    BN_add(newCT.sn,AcctU.zk_balance.sn,BN_1); //7. sn++
    //set new_zk_balance
    Twisted_ElGamal_CT_copy(new_zk_balance.C,newCv);
    EC_POINT_copy(new_zk_balance.pk,AcctU.zk_balance.pk);
    BN_copy(new_zk_balance.r,r1);
    BN_copy(new_zk_balance.sn,newCT.sn);
    BN_copy(new_zk_balance.v,Vnew);

    //set new_pt_balance
    BN_copy(new_pt_balance,AcctU.pt_balance);

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
    MR_Twisted_ElGamal_CT MR_CT; 
    MR_Twisted_ElGamal_CT_new(MR_CT); 

    Plaintext_Equality_Witness PI3_dualenc_witness; 
    NIZK_Plaintext_Equality_Witness_new(PI3_dualenc_witness); 
    BN_copy(PI3_dualenc_witness.v,Vcheque);
    BN_copy(PI3_dualenc_witness.r,r2);

    MR_Twisted_ElGamal_Enc(enc_pp, AcctU.keypair.pk, AcctR.keypair.pk, PI3_dualenc_witness.v, PI3_dualenc_witness.r, MR_CT); 
    EC_POINT_copy(newCT.proof.pi_sendcheque.pi3.pi_dualenc_instance.pk1, AcctU.keypair.pk); 
    EC_POINT_copy(newCT.proof.pi_sendcheque.pi3.pi_dualenc_instance.pk2, AcctR.keypair.pk); 
    EC_POINT_copy(newCT.proof.pi_sendcheque.pi3.pi_dualenc_instance.X1, MR_CT.X1); 
    EC_POINT_copy(newCT.proof.pi_sendcheque.pi3.pi_dualenc_instance.X2, MR_CT.X2); 
    EC_POINT_copy(newCT.proof.pi_sendcheque.pi3.pi_dualenc_instance.Y, MR_CT.Y); 

    MR_Twisted_ElGamal_CT_free(MR_CT); 

    LedgerMaze_nizk_plaintext_equality(true,newCT.proof.pi_sendcheque.pi3.pi_dualenc_instance,newCT.proof.pi_sendcheque.pi3.pi_dualenc,PI3_dualenc_witness);

    //SIGN
    //convert to memo
    std::string memo(ECP_ep2string(AcctU.keypair.pk));
    memo.append(BN_bn2hex((const BIGNUM*)new_zk_balance.sn));
    char buffer[POINT_LEN*4+1] = {0};
    Twisted_ElGamal_CT_to_string(Cv_cheque,buffer);
    memo.append(reinterpret_cast<const char*>(buffer));

    char buffer1[POINT_LEN*4+1] = {0};
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
    memo_bn = BN_new();
    Hash_String_to_BN(memo,memo_bn);

    Twisted_ElGamal_Sig(enc_pp,AcctU.keypair.sk,memo_bn,newCT.sig);
    BN_free(memo_bn);

    EC_POINT_copy(newCT.pk1,AcctU.keypair.pk);
    Twisted_ElGamal_CT_copy(newCT.transfer,Cv_cheque);
    Twisted_ElGamal_CT_copy(newCT.rest_balance,newCv);

    return 0;
}

void LedgerMaze_RefreshCheque(LedgerMaze_PP &pp, LedgerMaze_Account &AcctR, LedgerMaze_CTx &sendCT,
LedgerMaze_CTx &refreshCT, LedgerMaze_Cheque &newChequeR)
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

    BN_free(Vcheque_r_bn);
    EC_POINT_free(Vcheque_r);



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
    LedgerMaze_Commit(pp,TAG,r);


    //compute 1 out of t proof
    //get commit C_l = C(Rl,l)-CR_new
    Twisted_ElGamal_CT C_l;
    Twisted_ElGamal_CT_new(C_l); 
    Twisted_ElGamal_CT_copy(C_l,sendCT.transfer);

    secp_primitives::GroupElement g_element;
    LedgerMaze_EC_POINTtoGroupElement(g_element,pp.g);
    secp_primitives::GroupElement h_element;
    LedgerMaze_EC_POINTtoGroupElement(h_element,pp.h);
    secp_primitives::GroupElement pk_elem;
    LedgerMaze_EC_POINTtoGroupElement(pk_elem,AcctR.keypair.pk);
    secp_primitives::GroupElement pkk_elem;




    //get commit A_l = C_l1/CR_new1 and get pi_1outoft_1
    EC_POINT* A_l = EC_POINT_new(group);
    EC_POINT_sub(A_l,C_l.X,CR_new.X);

    

    secp_primitives::GroupElement Al_element;
    LedgerMaze_EC_POINTtoGroupElement(Al_element,A_l);

    secp_primitives::Scalar r_1_scalar;
    BIGNUM* r_1 = BN_new();
    BN_mod_sub(r_1,r,r_new,order,bn_ctx);

    LedgerMaze_BIGNUMtoScalar(r_1_scalar,r_1);//get r-r_new mod order

    BN_free(r_1);




    One_Out_of_N_Proof_Prove(256,refreshCT.proof.pi_refreshcheque.pi1.pi_1outoft1__instance,refreshCT.proof.pi_refreshcheque.pi1.pi_oneoutofn1,pk_elem,h_element,Al_element,r_1_scalar);


    

    //get commit B_l = C_l2/CR_new2 and get pi_1outoft_2
    EC_POINT* B_l = EC_POINT_new(group);
    EC_POINT_sub(B_l,C_l.Y,CR_new.Y);

    secp_primitives::GroupElement Bl_element;
    LedgerMaze_EC_POINTtoGroupElement(Bl_element,B_l);
    EC_POINT_free(B_l);

    One_Out_of_N_Proof_Prove(256,refreshCT.proof.pi_refreshcheque.pi1.pi_1outoft2__instance,refreshCT.proof.pi_refreshcheque.pi1.pi_oneoutofn2,g_element,h_element,Bl_element,r_1_scalar);


    //get commit D_l = C_l1/TAG and get pi_1outoft_3
    EC_POINT* D_l = EC_POINT_new(group);
    EC_POINT_sub(D_l,C_l.X,TAG);


    

    secp_primitives::GroupElement Dl_element;
    LedgerMaze_EC_POINTtoGroupElement(Dl_element,D_l);

    EC_POINT_free(D_l);

    secp_primitives::Scalar r_2_scalar;
    secp_primitives::Scalar r_2_scalar_inv;


    LedgerMaze_BIGNUMtoScalar(r_2_scalar,r);


    r_2_scalar_inv = r_2_scalar.inverse();
    pkk_elem = Dl_element*r_2_scalar_inv;

    One_Out_of_N_Proof_Prove(256,refreshCT.proof.pi_refreshcheque.pi1.pi_1outoft3__instance,refreshCT.proof.pi_refreshcheque.pi1.pi_oneoutofn3,pkk_elem,h_element,Dl_element,r_2_scalar);


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
    BIGNUM *memo_bn = BN_new();

    std::string memo(ECP_ep2string(AcctR.keypair.pk));
    memo.append(ECP_ep2string(TAG));
    char buffer[POINT_LEN*4+1] = {0};
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
    Hash_String_to_BN(memo,memo_bn);
    Twisted_ElGamal_Sig(enc_pp,AcctR.keypair.sk,memo_bn,refreshCT.sig);
    BN_free(memo_bn);


    //get Trans_refersh
    EC_POINT_copy(refreshCT.pk1,AcctR.keypair.pk);
    Twisted_ElGamal_CT_copy(refreshCT.transfer,CR_new);
    EC_POINT_copy(refreshCT.tag,TAG);;


}



void LedgerMaze_Depositcheque(LedgerMaze_PP &pp, LedgerMaze_Account &AcctR, LedgerMaze_Cheque &ChequeR, 
LedgerMaze_CTx &newCT,BIGNUM *&new_pt_balance,ZK_BALANCE_CT &new_zk_balance)
{
    BIGNUM* VR_new = BN_new();
    BIGNUM* r_new = BN_new();
    BN_mod_add(VR_new,AcctR.zk_balance.v,ChequeR.transfer,order,bn_ctx);
    BN_random(r_new);

    //Cv_cheque=ISE.Enc(pk_R,VR_new,r_new);
    Twisted_ElGamal_PP enc_pp;
    Get_Enc_PP(pp, enc_pp);

    Twisted_ElGamal_CT CR_new;
    Twisted_ElGamal_CT_new(CR_new);   
    Twisted_ElGamal_Enc(enc_pp,AcctR.keypair.pk,VR_new,r_new,CR_new);

    //get PIdepositcheque
    string transcript_str = "";

    //Pi1.pi_enc
    Plaintext_Knowledge_PP PI1_enc_pp; 
    NIZK_Plaintext_Knowledge_PP_new(PI1_enc_pp); 
    NIZK_Plaintext_Knowledge_Setup(PI1_enc_pp);
    // Plaintext_Knowledge_Instance PI1_enc_instance;
    // NIZK_Plaintext_Knowledge_Instance_new(PI1_enc_instance);  
    EC_POINT_copy(newCT.proof.pi_depositcheque.pi1.pi_enc_instance.pk,AcctR.keypair.pk);
    EC_POINT_copy(newCT.proof.pi_depositcheque.pi1.pi_enc_instance.X,CR_new.X);
    EC_POINT_copy(newCT.proof.pi_depositcheque.pi1.pi_enc_instance.Y,CR_new.Y);
    Plaintext_Knowledge_Witness PI1_enc_witness; 
    NIZK_Plaintext_Knowledge_Witness_new(PI1_enc_witness); 
    BN_copy(PI1_enc_witness.r,r_new);
    BN_copy(PI1_enc_witness.v,VR_new);
    transcript_str = ""; 
    NIZK_Plaintext_Knowledge_Prove(PI1_enc_pp,newCT.proof.pi_depositcheque.pi1.pi_enc_instance,PI1_enc_witness,transcript_str,newCT.proof.pi_depositcheque.pi1.pi_enc);
    
    //Pi1.pi_range
    Bullet_PP PI1_range_pp; 
    Bullet_PP_new(PI1_range_pp,pp.RANGE_LEN,pp.AGG_NUM);
    Get_Bullet_PP(pp, PI1_range_pp);
    // Bullet_Instance PI1_range_instance;
    // Bullet_Instance_new(PI1_range_pp, PI1_range_instance); 
    EC_POINT_copy(newCT.proof.pi_depositcheque.pi1.pi_range_instance.C[0], CR_new.Y);
    Bullet_Witness PI1_range_witness; 
    Bullet_Witness_new(PI1_range_pp, PI1_range_witness);
    BN_copy(PI1_range_witness.r[0], r_new);
    BN_copy(PI1_range_witness.v[0], VR_new);
    transcript_str = "";
    Bullet_Prove(PI1_range_pp,newCT.proof.pi_depositcheque.pi1.pi_range_instance,PI1_range_witness,transcript_str,newCT.proof.pi_depositcheque.pi1.pi_range);

    //Pi2.pi_enc
    Plaintext_Knowledge_PP PI2_enc_pp; 
    NIZK_Plaintext_Knowledge_PP_new(PI2_enc_pp); 
    NIZK_Plaintext_Knowledge_Setup(PI2_enc_pp);
    // Plaintext_Knowledge_Instance PI2_enc_instance;
    // NIZK_Plaintext_Knowledge_Instance_new(PI2_enc_instance);  
    EC_POINT_copy(newCT.proof.pi_depositcheque.pi2.pi_enc_instance.pk,AcctR.keypair.pk);
    EC_POINT_copy(newCT.proof.pi_depositcheque.pi2.pi_enc_instance.X,ChequeR.C.X);
    EC_POINT_copy(newCT.proof.pi_depositcheque.pi2.pi_enc_instance.Y,ChequeR.C.Y);
    Plaintext_Knowledge_Witness PI2_enc_witness; 
    NIZK_Plaintext_Knowledge_Witness_new(PI2_enc_witness); 
    BN_copy(PI2_enc_witness.r,ChequeR.r);
    BN_copy(PI2_enc_witness.v,ChequeR.transfer);
    transcript_str = ""; 
    NIZK_Plaintext_Knowledge_Prove(PI2_enc_pp,newCT.proof.pi_depositcheque.pi2.pi_enc_instance,PI2_enc_witness,transcript_str,newCT.proof.pi_depositcheque.pi2.pi_enc);

    //Pi2.pi_range
    Bullet_PP PI2_range_pp; 
    Bullet_PP_new(PI2_range_pp,pp.RANGE_LEN,pp.AGG_NUM);
    Get_Bullet_PP(pp, PI2_range_pp);
    // Bullet_Instance PI2_range_instance;
    // Bullet_Instance_new(PI2_range_pp, PI2_range_instance); 
    EC_POINT_copy(newCT.proof.pi_depositcheque.pi2.pi_range_instance.C[0], ChequeR.C.Y);
    Bullet_Witness PI2_range_witness; 
    Bullet_Witness_new(PI2_range_pp, PI2_range_witness);
    BN_copy(PI2_range_witness.r[0], ChequeR.r);
    BN_copy(PI2_range_witness.v[0], ChequeR.transfer);
    transcript_str = "";
    Bullet_Prove(PI2_range_pp,newCT.proof.pi_depositcheque.pi2.pi_range_instance,PI2_range_witness,transcript_str,newCT.proof.pi_depositcheque.pi2.pi_range);


    //Pi3.pi_dualenc
    MR_Twisted_ElGamal_CT MR_CT; 
    MR_Twisted_ElGamal_CT_new(MR_CT); 

    Plaintext_Equality_Witness PI3_dualenc_witness; 
    NIZK_Plaintext_Equality_Witness_new(PI3_dualenc_witness); 
    BN_copy(PI3_dualenc_witness.v,ChequeR.transfer);
    BN_copy(PI3_dualenc_witness.r,ChequeR.r);

    MR_Twisted_ElGamal_Enc(enc_pp, AcctR.keypair.pk, AcctR.keypair.pk, PI3_dualenc_witness.v, PI3_dualenc_witness.r, MR_CT); 
    EC_POINT_copy(newCT.proof.pi_depositcheque.pi3.pi_dualenc_instance.pk1, AcctR.keypair.pk); 
    EC_POINT_copy(newCT.proof.pi_depositcheque.pi3.pi_dualenc_instance.pk2, AcctR.keypair.pk); 
    EC_POINT_copy(newCT.proof.pi_depositcheque.pi3.pi_dualenc_instance.X1, MR_CT.X1); 
    EC_POINT_copy(newCT.proof.pi_depositcheque.pi3.pi_dualenc_instance.X2, MR_CT.X2); 
    EC_POINT_copy(newCT.proof.pi_depositcheque.pi3.pi_dualenc_instance.Y, MR_CT.Y); 

    MR_Twisted_ElGamal_CT_free(MR_CT); 

    LedgerMaze_nizk_plaintext_equality(true,newCT.proof.pi_depositcheque.pi3.pi_dualenc_instance,newCT.proof.pi_depositcheque.pi3.pi_dualenc,PI3_dualenc_witness);

    //SIGN
    //convert to memo
    BN_add(newCT.sn,AcctR.zk_balance.sn,BN_1); //7. sn++
    


    std::string memo(ECP_ep2string(AcctR.keypair.pk));
    memo.append(BN_bn2hex((const BIGNUM*)newCT.sn));
    char buffer[POINT_LEN*4+1] = {0};
    Twisted_ElGamal_CT_to_string(ChequeR.C,buffer);
    memo.append(reinterpret_cast<const char*>(buffer));

    char buffer1[POINT_LEN*4+1] = {0};
    Twisted_ElGamal_CT_to_string(CR_new,buffer1);
    memo.append(reinterpret_cast<const char*>(buffer1));

    BIGNUM *memo_bn;
    memo_bn = BN_new();
    Hash_String_to_BN(memo,memo_bn);

    Twisted_ElGamal_Sig(enc_pp,AcctR.keypair.sk,memo_bn,newCT.sig);
    BN_free(memo_bn);

    //get newzk_balance
    Twisted_ElGamal_CT_copy(new_zk_balance.C,CR_new);
    EC_POINT_copy(new_zk_balance.pk,AcctR.keypair.pk);
    BN_copy(new_zk_balance.v,VR_new);
    BN_copy(new_zk_balance.sn,newCT.sn);
    BN_copy(new_zk_balance.r,r_new);

    BN_copy(new_pt_balance,AcctR.pt_balance);


    //get new CT
    EC_POINT_copy(newCT.pk1,AcctR.keypair.pk);
    Twisted_ElGamal_CT_copy(newCT.rest_balance,CR_new);
    Twisted_ElGamal_CT_copy(newCT.transfer,ChequeR.C);

}


bool LedgerMaze_Verify_Mint_CTx(LedgerMaze_PP &pp ,LedgerMaze_CTx &newCT, BIGNUM *r,BIGNUM *rstar, BIGNUM *Vustar)
{
        #ifdef DEMO
        cout << "begin to verify Mint CTx >>>>>>" << endl; 
        #endif

        bool V,V_random,V_range,V_sign;
        string transcript_str = "";
        Twisted_ElGamal_PP enc_pp;
        Get_Enc_PP(pp, enc_pp);
        // 1. check if value in (0,pt_balance]
        // if(BN_cmp(newCT.value,BN_0)==0||BN_cmp(newCT.value, Acct.pt_balance)>0)
        // {
        //     cout << "the mint transfer value should in (0,pt_balance]" << endl;
        // }

        // 2. verify pi_mint 
        // pi_random
        DLOG_Equality_PP dlogeq_pp; 
        NIZK_DLOG_Equality_PP_new(dlogeq_pp);
        Get_DLOG_Equality_PP(pp, dlogeq_pp);
        DLOG_Equality_Witness dlogeq_witness; 
        NIZK_DLOG_Equality_Witness_new(dlogeq_witness);
        BN_mod_sub(dlogeq_witness.w, rstar, r, order, bn_ctx);  //w_r=r*-r mod n;
        // cout<< "****************" << endl;
        // BN_print(dlogeq_witness.w,"w_r");
        V_random=NIZK_DLOG_Equality_Verify(dlogeq_pp,newCT.proof.pi_mint.pi_random_instance,transcript_str,newCT.proof.pi_mint.pi_random);
        #ifdef DEMO
        if (V_random) 
        cout<< "NIZKPoK for V_random accepts" << endl; 
        else cout<< "NIZKPoK for V_random rejects" << endl; 
        #endif
        //pi_range
        Bullet_PP bullet_pp; 
        Get_Bullet_PP(pp, bullet_pp);
        Bullet_Witness bullet_witness; 
        Bullet_Witness_new(bullet_pp, bullet_witness);
        BN_copy(bullet_witness.r[0], rstar);
        BN_copy(bullet_witness.v[0], Vustar);
        V_range=Bullet_Verify(bullet_pp,newCT.proof.pi_mint.pi_range_instance,transcript_str,newCT.proof.pi_mint.pi_range);
        #ifdef DEMO
        if (V_range) 
        cout<< "NIZKPoK for V_range accepts" << endl; 
        else cout<< "NIZKPoK for V_range rejects" << endl; 
        #endif
        
        //convert to memo=(pkU,sn,Vtransfer,CVu*)
        std::string memo(ECP_ep2string(newCT.pk1));
        memo.append(BN_bn2hex((const BIGNUM*)newCT.sn));
        memo.append(BN_bn2hex((const BIGNUM*)newCT.value));
        char buffer[POINT_LEN*4+1] = {0};
        Twisted_ElGamal_CT_to_string(newCT.rest_balance,buffer);
        memo.append(reinterpret_cast<const char*>(buffer)); 

        BIGNUM *memo_bn=BN_new();
        Hash_String_to_BN(memo,memo_bn);
        //3. verify sigma
        V_sign=Twisted_ElGamal_Ver(enc_pp,newCT.pk1,memo_bn,newCT.sig);
        #ifdef DEMO
        if (V_sign) 
        cout<< "Signature accepts" << endl; 
        else cout<< "Signature rejects" << endl; 
        #endif
        BN_free(memo_bn);
        
        V=V_random&&V_range&&V_sign;

        string ctx_file  = ECP_ep2string(newCT.pk1) + "_" + BN_bn2string(newCT.sn) + ".ctx"; 

        // #ifdef DEMO
        // if (V) cout << ctx_file << " is valid <<<<<<" << endl; 
        // else cout << ctx_file << " is invalid <<<<<<" << endl;
        // #endif

        return  V;


}
bool LedgerMaze_Verify_Redeem_CTx(LedgerMaze_PP &pp ,LedgerMaze_CTx &newCT, BIGNUM *r,BIGNUM *rstar, BIGNUM *Vustar)
{
    #ifdef DEMO
        cout << "begin to verify Redeem CTx >>>>>>" << endl; 
    #endif

    bool V,V_random,V_range,V_sign;
    string transcript_str = "";
    Twisted_ElGamal_PP enc_pp;
    Get_Enc_PP(pp, enc_pp);

    // 2. verify pi_mint 
    // pi_random
    DLOG_Equality_PP dlogeq_pp; 
    NIZK_DLOG_Equality_PP_new(dlogeq_pp);
    Get_DLOG_Equality_PP(pp, dlogeq_pp);
    DLOG_Equality_Witness dlogeq_witness; 
    NIZK_DLOG_Equality_Witness_new(dlogeq_witness);
    BN_mod_sub(dlogeq_witness.w, rstar, r, order, bn_ctx);  //w_r=r*-r mod n;
    // cout<< "V_C1****************" << endl;
    // BN_print(dlogeq_witness.w,"w_r");
    V_random=NIZK_DLOG_Equality_Verify(dlogeq_pp,newCT.proof.pi_redeem.pi_random_instance,transcript_str,newCT.proof.pi_redeem.pi_random);
    #ifdef DEMO
    if (V_random) 
        cout<< "NIZKPoK for V_random accepts" << endl; 
    else cout<< "NIZKPoK for V_random rejects" << endl; 
    #endif
    //pi_range
    Bullet_PP bullet_pp; 
    Get_Bullet_PP(pp, bullet_pp);
    Bullet_Witness bullet_witness; 
    Bullet_Witness_new(bullet_pp, bullet_witness);
    BN_copy(bullet_witness.r[0], rstar);
    BN_copy(bullet_witness.v[0], Vustar);
    V_range=Bullet_Verify(bullet_pp,newCT.proof.pi_redeem.pi_range_instance,transcript_str,newCT.proof.pi_redeem.pi_range);
    #ifdef DEMO
    if (V_range) 
    cout<< "NIZKPoK for V_range accepts" << endl; 
    else cout<< "NIZKPoK for V_range rejects" << endl; 
    #endif

    //convert to memo=(pkU,sn,Vtransfer,CVu*)
    std::string memo(ECP_ep2string(newCT.pk1));
    memo.append(BN_bn2hex((const BIGNUM*)newCT.sn));
    memo.append(BN_bn2hex((const BIGNUM*)newCT.value)); 
    char buffer[POINT_LEN*4+1]={0};
    Twisted_ElGamal_CT_to_string(newCT.rest_balance,buffer);
    memo.append(reinterpret_cast<const char*>(buffer));
    BIGNUM *memo_bn=BN_new();
    // Hash_String_to_BN(memo,memo_bn);
    // BN_print(memo_bn,"$$$V_memo_bn");
    //3. verify sigma
    V_sign=Twisted_ElGamal_Ver(enc_pp,newCT.pk1,memo_bn,newCT.sig);
    #ifdef DEMO
    if (V_sign) 
    cout<< "Signature accepts" << endl; 
    else cout<< "Signature rejects" << endl; 
    #endif

    BN_free(memo_bn);
        
    V=V_random&&V_range&&V_sign;

    string ctx_file  = ECP_ep2string(newCT.pk1) + "_" + BN_bn2string(newCT.sn) + ".ctx"; 

    // #ifdef DEMO
    // if (V) cout << ctx_file << " is valid <<<<<<" << endl; 
    // else cout << ctx_file << " is invalid <<<<<<" << endl;
    // #endif

    return  V;

}
bool LedgerMaze_Verify_SendCheque_CTx(LedgerMaze_PP &pp, LedgerMaze_CTx &newCT)
{
    Twisted_ElGamal_PP enc_pp;
    Twisted_ElGamal_PP_new(enc_pp);
    Get_Enc_PP(pp, enc_pp);

    std::string memo(ECP_ep2string(newCT.pk1));
    memo.append(BN_bn2hex((const BIGNUM*)newCT.sn));
    char buffer[POINT_LEN*4+1] = {0};
    Twisted_ElGamal_CT_to_string(newCT.transfer,buffer);
    memo.append(reinterpret_cast<const char*>(buffer));
    char buffer1[POINT_LEN*4+1] = {0};
    Twisted_ElGamal_CT_to_string(newCT.rest_balance,buffer1);
    memo.append(reinterpret_cast<const char*>(buffer1));

    BIGNUM *memo_bn;
    memo_bn = BN_new();
    Hash_String_to_BN(memo,memo_bn);

    if(!Twisted_ElGamal_Ver(enc_pp,newCT.pk1,memo_bn,newCT.sig)){
        cout << "SignatureU fail!>>>" << endl;
        
        return false;
    }
    BN_free(memo_bn);

    bool V1, V2, V3, V4, V5, Validity; // variables for checking results

    string transcript_str; 

    Plaintext_Knowledge_PP PI1_enc_pp; 
    NIZK_Plaintext_Knowledge_PP_new(PI1_enc_pp); 
    NIZK_Plaintext_Knowledge_Setup(PI1_enc_pp);
    transcript_str = "";
    V1 = NIZK_Plaintext_Knowledge_Verify(PI1_enc_pp, newCT.proof.pi_sendcheque.pi1.pi_enc_instance, transcript_str, newCT.proof.pi_sendcheque.pi1.pi_enc); 
    //pi2 range
    Bullet_PP PI1_range_pp; 
    Bullet_PP_new(PI1_range_pp,pp.RANGE_LEN,pp.AGG_NUM);
    Get_Bullet_PP(pp, PI1_range_pp);
    transcript_str = "";
    V2 = Bullet_Verify(PI1_range_pp,newCT.proof.pi_sendcheque.pi1.pi_range_instance,transcript_str,newCT.proof.pi_sendcheque.pi1.pi_range);

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

    #ifdef DEMO

        cout << V1 << V2 << V3 << V4 << V5 <<endl;

    #endif

    return Validity;


}
bool LedgerMaze_Verify_DepositCheque_CTx(LedgerMaze_PP &pp, LedgerMaze_CTx &newCT)
{

    Twisted_ElGamal_PP enc_pp;
    Twisted_ElGamal_PP_new(enc_pp);
    Get_Enc_PP(pp, enc_pp);

    std::string memo(ECP_ep2string(newCT.pk1));
    memo.append(BN_bn2hex((const BIGNUM*)newCT.sn));
    char buffer[POINT_LEN*4+1] = {0};
    Twisted_ElGamal_CT_to_string(newCT.transfer,buffer);
    memo.append(reinterpret_cast<const char*>(buffer));
    char buffer1[POINT_LEN*4+1] = {0};
    Twisted_ElGamal_CT_to_string(newCT.rest_balance,buffer1);
    memo.append(reinterpret_cast<const char*>(buffer1));

    

    BIGNUM *memo_bn;
    memo_bn = BN_new();
    Hash_String_to_BN(memo,memo_bn);

    if(!Twisted_ElGamal_Ver(enc_pp,newCT.pk1,memo_bn,newCT.sig)){
        cout << "SignatureU fail!>>>" << endl;
        
        return false;
    }
    BN_free(memo_bn);

    bool V1, V2, V3, V4, V5, Validity; // variables for checking results

    string transcript_str; 

    Plaintext_Knowledge_PP PI1_enc_pp; 
    NIZK_Plaintext_Knowledge_PP_new(PI1_enc_pp); 
    NIZK_Plaintext_Knowledge_Setup(PI1_enc_pp);
    transcript_str = "";
    V1 = NIZK_Plaintext_Knowledge_Verify(PI1_enc_pp, newCT.proof.pi_depositcheque.pi1.pi_enc_instance, transcript_str, newCT.proof.pi_depositcheque.pi1.pi_enc); 
    //pi2 range
    Bullet_PP PI1_range_pp; 
    Bullet_PP_new(PI1_range_pp,pp.RANGE_LEN,pp.AGG_NUM);
    Get_Bullet_PP(pp, PI1_range_pp);
    transcript_str = "";
    V2 = Bullet_Verify(PI1_range_pp,newCT.proof.pi_depositcheque.pi1.pi_range_instance,transcript_str,newCT.proof.pi_depositcheque.pi1.pi_range);

    //verify pi2 enc
    

    Plaintext_Knowledge_PP PI2_enc_pp; 
    NIZK_Plaintext_Knowledge_PP_new(PI2_enc_pp); 
    NIZK_Plaintext_Knowledge_Setup(PI2_enc_pp);
    transcript_str = "";
    V3 = NIZK_Plaintext_Knowledge_Verify(PI2_enc_pp, newCT.proof.pi_depositcheque.pi2.pi_enc_instance, transcript_str, newCT.proof.pi_depositcheque.pi2.pi_enc); 
    //pi2 range
    Bullet_PP PI2_range_pp; 
    Bullet_PP_new(PI2_range_pp,pp.RANGE_LEN,pp.AGG_NUM);
    Get_Bullet_PP(pp, PI2_range_pp);
    transcript_str = "";
    V4 = Bullet_Verify(PI2_range_pp,newCT.proof.pi_depositcheque.pi2.pi_range_instance,transcript_str,newCT.proof.pi_depositcheque.pi2.pi_range);

    //pi3 dualenc
    Plaintext_Equality_PP PI3_dualenc_pp;
    NIZK_Plaintext_Equality_PP_new(PI3_dualenc_pp);    
    NIZK_Plaintext_Equality_Setup(PI3_dualenc_pp);
    transcript_str = "";
    V5 = NIZK_Plaintext_Equality_Verify(PI3_dualenc_pp,newCT.proof.pi_depositcheque.pi3.pi_dualenc_instance,transcript_str,newCT.proof.pi_depositcheque.pi3.pi_dualenc);

    Validity = V1 && V2 && V3 && V4 && V5;

    #ifdef DEMO

        cout << V1 << V2 << V3 << V4 << V5 <<endl;

    #endif

    return Validity;

}
bool LedgerMaze_Verify_RefreshCheque_CTx(LedgerMaze_PP &pp, LedgerMaze_CTx &newCT)
{
    ///////////////////////////////////////////////////////验证TAG！！！！！
    Twisted_ElGamal_PP enc_pp;
    Get_Enc_PP(pp, enc_pp);

    BIGNUM *memo_bn = BN_new();

    std::string memo(ECP_ep2string(newCT.pk1));
    memo.append(ECP_ep2string(newCT.tag));
    char buffer[POINT_LEN*4+1] = {0};
    Twisted_ElGamal_CT_to_string(newCT.transfer,buffer);
    memo.append(reinterpret_cast<const char*>(buffer));
    Hash_String_to_BN(memo,memo_bn);

    if(!Twisted_ElGamal_Ver(enc_pp,newCT.pk1,memo_bn,newCT.sig)){
        cout << "SignatureU fail!>>>" << endl;
        
        return false;
    }

    bool V1, V2, V3, V4, V5, Validity; // variables for checking results
    //verify 1 out of t proof

    V1 = One_Out_of_N_Proof_Verify(newCT.proof.pi_refreshcheque.pi1.pi_oneoutofn1,newCT.proof.pi_refreshcheque.pi1.pi_1outoft1__instance);

    V2 = One_Out_of_N_Proof_Verify(newCT.proof.pi_refreshcheque.pi1.pi_oneoutofn2,newCT.proof.pi_refreshcheque.pi1.pi_1outoft2__instance);

    V3 = One_Out_of_N_Proof_Verify(newCT.proof.pi_refreshcheque.pi1.pi_oneoutofn3,newCT.proof.pi_refreshcheque.pi1.pi_1outoft3__instance);

    //verify pi2
    string transcript_str; 
    Plaintext_Knowledge_PP Pi2pp; 
    NIZK_Plaintext_Knowledge_PP_new(Pi2pp); 
    NIZK_Plaintext_Knowledge_Setup(Pi2pp);
    transcript_str = "";
    V4 = NIZK_Plaintext_Knowledge_Verify(Pi2pp,newCT.proof.pi_refreshcheque.pi2.pi_enc_instance,transcript_str,newCT.proof.pi_refreshcheque.pi2.pi_enc);

    Bullet_PP PI2_range_pp; 
    Bullet_PP_new(PI2_range_pp,pp.RANGE_LEN,pp.AGG_NUM);
    Get_Bullet_PP(pp, PI2_range_pp);
    transcript_str = "";
    V5 = Bullet_Verify(PI2_range_pp,newCT.proof.pi_refreshcheque.pi2.pi_range_instance,transcript_str,newCT.proof.pi_refreshcheque.pi2.pi_range);

    Validity = V1 && V2 && V3 && V4 && V5;

    #ifdef DEMO

        cout << V1 << V2 << V3 << V4 << V5 <<endl;

    #endif

    return Validity;

}

bool LedgerMaze_Verify_CTx(LedgerMaze_PP &pp, LedgerMaze_CTx &newCT, BIGNUM *r,BIGNUM *rstar, BIGNUM *Vustar)
{
    bool V;
    if(BN_cmp(newCT.type,BN_0)==0)
    {
        V=LedgerMaze_Verify_Mint_CTx(pp, newCT,r,rstar,Vustar);
    }
    if(BN_cmp(newCT.type,BN_1)==0)
    {
        V=LedgerMaze_Verify_Redeem_CTx(pp, newCT, r,rstar,Vustar);
    }
    if(BN_cmp(newCT.type,BN_2)==0)
    {
        V=LedgerMaze_Verify_SendCheque_CTx(pp,newCT);
    }
    if(BN_cmp(newCT.type,BN_3)==0)
    {
        V=LedgerMaze_Verify_DepositCheque_CTx(pp,newCT);
    }
    if(BN_cmp(newCT.type,BN_4)==0)
    {
        V=LedgerMaze_Verify_RefreshCheque_CTx(pp,newCT);
    }
   
    return V;
}


void LedgerMaze_Update_Acct(LedgerMaze_Account &Acct, BIGNUM *new_pt_balance,ZK_BALANCE_CT &new_zk_balance)
{
    #ifdef DEMO
        cout << "begin to update Acct >>>>>>" << endl; 
    #endif

    BN_copy(Acct.pt_balance,new_pt_balance);
    ZK_BALANCE_CT_copy(Acct.zk_balance,new_zk_balance);

    #ifdef DEMO
        cout << "updated Acct context>>>>>>" << endl; 
        LedgerMaze_Account_print(Acct);
    #endif

}


#endif

