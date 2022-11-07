#include "../depends/oneoutofnproofs/src/one_out_of_n_protocol.hpp"
#include "../depends/twisted_elgamal/twisted_elgamal.hpp"


void Oneoutoft_test()
{

    One_Out_of_N_Proof_Instance instance;
    One_Out_of_N_Proof_Instance_new(instance);

    One_Out_of_N_Proof proof;
    One_Out_of_N_Proof_new(proof,instance);

    One_Out_of_N_Proof_Prove(256,instance,proof);

    if(One_Out_of_N_Proof_Verify(proof,instance) == true)
    {
        printf("true!\n");
    }
    else
        printf("false!\n");

    One_Out_of_N_Proof_free(proof);

    
    //Bignum to Scalar
    secp_primitives::Scalar r;

    BIGNUM* a = BN_new();
    BN_random(a);
    std::string a_hex(BN_bn2hex(a));
    r.SetHex(a_hex);


    

    //EC_POINT to GroupElement
    EC_POINT* G = EC_POINT_new(group);
    EC_POINT* R = EC_POINT_new(group);

    ECP_random(G);


    EC_POINT_add(group,R,G,G,bn_ctx);
    EC_POINT_add(group,R,R,G,bn_ctx);

    ECP_print(R);

  
    //EC_POINT to GroupElement and Dl
    BIGNUM* X = BN_new();
    BIGNUM* Y = BN_new();
    EC_POINT_get_affine_coordinates_GFp(group,G,X,Y,bn_ctx);

    char* x = BN_bn2hex(X);
    char* y = BN_bn2hex(Y);

    secp_primitives::GroupElement g((char*)x,(char*)y,16);
    secp_primitives::GroupElement t;
    t.randomize();
    secp_primitives::Scalar zero(unsigned(3));
    t = g*zero;

    cout << t.GetHex()<< endl;
    cout << t.isMember()<< endl;

    string tmp("12AB");

    cout << tmp << endl;




    

    
    
    
    


}

int main()
{ 
    global_initialize(NID_secp256k1); 

    // cout<<BN_cmp(BN_1,BN_0)<<endl;
    EC_POINT* res;
    EC_POINT* Vcheque_r2;
    BIGNUM* Vcheque_r2_bn;
    Vcheque_r2_bn = BN_new();
    BIGNUM* Vcheque;
    Vcheque = BN_new();
    BN_random(Vcheque);
    BIGNUM* r2;
    r2 = BN_new();
    BN_random(r2);
    Vcheque_r2 = EC_POINT_new(group);
    res = EC_POINT_new(group);

    EC_POINT_set_affine_coordinates(group,Vcheque_r2,r2,Vcheque,bn_ctx);
    BN_print(r2);
    BN_print(Vcheque);

    EC_POINT_point2bn(group,Vcheque_r2,POINT_CONVERSION_UNCOMPRESSED,Vcheque_r2_bn,bn_ctx);
    BN_print(Vcheque_r2_bn);

    EC_POINT_bn2point(group,Vcheque_r2_bn,res,bn_ctx);
    ECP_print(res);

    BIGNUM* a;
    a = BN_new();
    BIGNUM* b;
    b = BN_new();

    EC_POINT_get_affine_coordinates(group,res,a,b,bn_ctx);

    BN_print(a);
    BN_print(b);
   


    //Oneoutoft_test();
    global_finalize();
}