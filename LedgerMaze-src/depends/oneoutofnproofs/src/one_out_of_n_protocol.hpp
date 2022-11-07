#include <chrono>
#include<iostream>
#include "../src/params.h"
#include "../src/sigmaplus_prover.h"
#include "../src/sigmaplus_verifier.h"





// typedef sigma::SigmaPlusProof<secp_primitives::Scalar,secp_primitives::GroupElement> One_Out_of_N_Proof;

// structure of proof 
struct One_Out_of_N_Proof
{
    std::vector<secp_primitives::GroupElement> commits;
    sigma::SigmaPlusProof<secp_primitives::Scalar,secp_primitives::GroupElement>* proof;

};

struct One_Out_of_N_Proof_Instance
{
    int n;
    int m;
    secp_primitives::GroupElement g;
    std::vector<secp_primitives::GroupElement> h_gens;

};
// new sigma::SigmaPlusProof<secp_primitives::Scalar,secp_primitives::GroupElement>(n, m);



void One_Out_of_N_Proof_Instance_new(One_Out_of_N_Proof_Instance &instance)
{
    auto params = sigma::Params::get_default();
    instance.n = params->get_n();
    instance.m = params->get_m();
    
    instance.g.randomize();
    instance.h_gens.resize(instance.n * instance.m);
    for(int i = 0; i < instance.n * instance.m; ++i ){
        instance.h_gens[i].randomize();
    }
}

void One_Out_of_N_Proof_Instance_free(One_Out_of_N_Proof_Instance &instance)
{

}

void One_Out_of_N_Proof_new(One_Out_of_N_Proof &proof, One_Out_of_N_Proof_Instance &instance)
{

    proof.proof = new sigma::SigmaPlusProof<secp_primitives::Scalar,secp_primitives::GroupElement>(instance.n, instance.m);
    
}

void One_Out_of_N_Proof_free(One_Out_of_N_Proof &proof)
{
    delete proof.proof;
    
}

void One_Out_of_N_Proof_Prove(int N, One_Out_of_N_Proof_Instance &instance, One_Out_of_N_Proof &proof, secp_primitives::GroupElement g, secp_primitives::GroupElement h, secp_primitives::GroupElement c, secp_primitives::Scalar r)
{

    int n = instance.n;
    int m = instance.m;
    int index = 0;

    

    instance.g = h;
    instance.h_gens[0] = g;


    sigma::SigmaPlusProver<secp_primitives::Scalar,secp_primitives::GroupElement> prover(instance.g, instance.h_gens, n, m);

    for(int i = 0; i < N; ++i){
        if(i == index){
            
            proof.commits.push_back(c);

        }
        else{
            proof.commits.push_back(secp_primitives::GroupElement());
            proof.commits[i].randomize();
        }
    }
    // sigma::SigmaPlusProof<secp_primitives::Scalar,secp_primitives::GroupElement> proof(n,m);

    
    prover.proof(proof.commits, index, r, true, *proof.proof);


}

void One_Out_of_N_Proof_Prove(int N, One_Out_of_N_Proof_Instance &instance, One_Out_of_N_Proof &proof)
{

    int n = instance.n;
    int m = instance.m;
    int index = 0;

    secp_primitives::GroupElement g = instance.g;

    std::vector<secp_primitives::GroupElement> h_gens = instance.h_gens;

    secp_primitives::Scalar r;
    r.randomize();
    sigma::SigmaPlusProver<secp_primitives::Scalar,secp_primitives::GroupElement> prover(g,h_gens, n, m);

    for(int i = 0; i < N; ++i){
        if(i == index){
            secp_primitives::GroupElement c;
            secp_primitives::Scalar zero(unsigned(0));
            c = sigma::SigmaPrimitives<secp_primitives::Scalar,secp_primitives::GroupElement>::commit(g, zero, h_gens[0], r);
            proof.commits.push_back(c);

        }
        else{
            proof.commits.push_back(secp_primitives::GroupElement());
            proof.commits[i].randomize();
        }
    }
    // sigma::SigmaPlusProof<secp_primitives::Scalar,secp_primitives::GroupElement> proof(n,m);

    
    prover.proof(proof.commits, index, r, true, *proof.proof);


}

void One_Out_of_N_Proof_Prove_withR(int &N, secp_primitives::Scalar &r, One_Out_of_N_Proof_Instance &instance, One_Out_of_N_Proof &proof)
{

    int n = instance.n;
    int m = instance.m;
    int index = 0;

    secp_primitives::GroupElement g = instance.g;

    std::vector<secp_primitives::GroupElement> h_gens = instance.h_gens;

    sigma::SigmaPlusProver<secp_primitives::Scalar,secp_primitives::GroupElement> prover(g,h_gens, n, m);

    for(int i = 0; i < N; ++i){
        if(i == index){
            secp_primitives::GroupElement c;
            secp_primitives::Scalar zero(unsigned(0));
            c = sigma::SigmaPrimitives<secp_primitives::Scalar,secp_primitives::GroupElement>::commit(g, zero, h_gens[0], r);
            proof.commits.push_back(c);

        }
        else{
            proof.commits.push_back(secp_primitives::GroupElement());
            proof.commits[i].randomize();
        }
    }
    // sigma::SigmaPlusProof<secp_primitives::Scalar,secp_primitives::GroupElement> proof(n,m);
    prover.proof(proof.commits, index, r, true, *proof.proof);


}

bool One_Out_of_N_Proof_Verify(One_Out_of_N_Proof &proof,One_Out_of_N_Proof_Instance &instance)
{
    int n = instance.n;
    int m = instance.m;
    int index = 0;

    secp_primitives::GroupElement g = instance.g;

    std::vector<secp_primitives::GroupElement> h_gens = instance.h_gens;
    sigma::SigmaPlusVerifier<secp_primitives::Scalar,secp_primitives::GroupElement> verifier(g, h_gens, n, m);

    return verifier.verify(proof.commits, *proof.proof, true);

}


void One_Out_of_N_Proof_print(One_Out_of_N_Proof &proof)
{
    
    

}

void One_Out_of_N_Prove(One_Out_of_N_Proof &proof)
{
    
    

}
