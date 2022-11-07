#ifndef backend_h
#define backend_h
#include <stdio.h>
#include <stdlib.h>
#include <evhttp.h>
#include <event.h>
#include <string.h>

#include "event2/http.h"
#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/bufferevent_compat.h"
#include "event2/http_struct.h"
#include "event2/http_compat.h"
#include "event2/util.h"
#include "event2/listener.h"


#include "CJSON.hpp"
#include"sqlpaser.hpp"
 
#define RANGE_LEN_BACKEND  32  // set the range to be [0, 2^32-1]
#define AGG_NUM_BACKEND  2
#define BUF_MAX 1024*16





#ifdef __cplusplus
extern "C"
{
#endif

void GetParam(char* text, char* sql_m)
{
	cJSON* json;
	cJSON *sqlvalue,*idvalue;
	sqlvalue = cJSON_CreateObject();
	idvalue = cJSON_CreateObject();
	

	json = cJSON_Parse(text);
	if (!json) { printf("Error before: [%s]\n", cJSON_GetErrorPtr()); }
	else
	{
		sqlvalue = cJSON_GetObjectItem(json,"sql");
		idvalue = cJSON_GetObjectItem(json,"id");
		printf("sql: %s\n",sqlvalue->valuestring);
		printf("id: %d\n",idvalue->valueint);


	}
	cJSON_Delete(json);
}

char* Backend_Findhttpheader(struct evhttp_request* req, struct evkeyvalq* params, const char* query_char)
{
	if (req == NULL || params == NULL || query_char == NULL)
	{
		printf("====line:%d,%s\n", __LINE__, "input params is null.");
		return NULL;
	}

	struct evhttp_uri* decoded = NULL;
	char* query = NULL;
	char* query_result = NULL;
	const char* path;
	const char* uri = evhttp_request_get_uri(req);

	if (uri == NULL)
	{
		printf("====line:%d,evhttp_request_get_uri return null\n", __LINE__);
		return NULL;
	}
	else
	{
		printf("====line:%d,Got a GET request for <%s>\n", __LINE__, uri);
	}

	decoded = evhttp_uri_parse(uri);
	if (!decoded)
	{
		printf("====line:%d,It's not a good URI. Sending BADREQUEST\n", __LINE__);
		evhttp_send_error(req, HTTP_BADREQUEST, 0);
		return NULL;
	}

	path = evhttp_uri_get_path(decoded);
	if (path == NULL)
	{
		path = "/";
	}
	else
	{
		printf("====line:%d,path is:%s\n", __LINE__, path);
	}

	
	query = (char*)evhttp_uri_get_query(decoded);
	if (query == NULL)
	{
		printf("====line:%d,evhttp_uri_get_query return null\n", __LINE__);
		return NULL;
	}

	evhttp_parse_query_str(query, params);
	query_result = (char*)evhttp_find_header(params, query_char);

	return query_result;
}
	
void Backend_Getpostmessage(char* buf, struct evhttp_request* req)
{
	size_t post_size = 0;

	post_size = evbuffer_get_length(req->input_buffer);
	printf("====line:%d,post len:%d\n", __LINE__, (int)post_size);
	if (post_size <= 0)
	{
		printf("====line:%d,post msg is empty!\n", __LINE__);
		return;
	}
	else
	{
		size_t copy_len = post_size > BUF_MAX ? BUF_MAX : post_size;
		// printf("====line:%d,post len:%d, copy_len:%d\n", __LINE__, post_size, copy_len);
		memcpy(buf, evbuffer_pullup(req->input_buffer, -1), copy_len);
		buf[post_size] = '\0';
		// printf("====line:%d,post msg:%s\n", __LINE__, buf);
	}
}

void Backend_CreateAccount(struct evhttp_request* req, void* arg) {

    size_t post_size = 0;



	if (req == NULL)
	{
		printf("input param req is null.");
		return;
	}

	char buf[BUF_MAX] = { 0 };
	Backend_Getpostmessage(buf, req);
	post_size = evbuffer_get_length(req->input_buffer);
	if (buf == NULL)
	{
		printf("get_post_message return null.");
		return;
	}

	//get params
	cJSON* json;
	cJSON *pt_balanceValue,*idValue,*passValue;
	idValue = cJSON_CreateObject();
	pt_balanceValue = cJSON_CreateObject();
	passValue = cJSON_CreateObject();

	json = cJSON_Parse(buf);
	if (!json) { printf("Error before: [%s]\n", cJSON_GetErrorPtr()); }
	else
	{
		idValue = cJSON_GetObjectItem(json,"id");
		pt_balanceValue = cJSON_GetObjectItem(json,"pt_balance");
		passValue = cJSON_GetObjectItem(json,"pass");
	}


	//限定初始化明文范围[0,10000]
	//todo:限定用户名id的唯一性
	BIGNUM *pt_balance = BN_new(); 
	BIGNUM *bn_10000 = BN_new(); 
	BN_set_word(bn_10000,10000);
	BN_asc2bn(&pt_balance,pt_balanceValue->valuestring);
	char retParams[BUF_MAX] = { 0 };
	if(BN_cmp(pt_balance,bn_10000)>0)
	{
		strcat(retParams,"create account fail");
	}
	else
	{
		//call create account function and insert the account data into table acct
		LedgerMaze_PP pp; 
		LedgerMaze_PP_new(pp, RANGE_LEN_BACKEND, AGG_NUM_BACKEND); 
		LedgerMaze_PP_deserialize(pp, "LedgerMaze.pp"); 
		cout<<">>CreateAccount : begin to create a account:"<< idValue->valuestring<<endl; //print hint
		BIGNUM *sn = BN_new(); BN_one(sn); 
		LedgerMaze_Account Acct; 
		LedgerMaze_Account_new(Acct); 
		//////////////////////////////////
    	auto start_time = chrono::steady_clock::now();

		LedgerMaze_Create_Account(pp, idValue->valuestring, pt_balance, sn, Acct); 

    	auto end_time = chrono::steady_clock::now(); // end to count the time
    	auto running_time = end_time - start_time;  
		//////////////////////////////////
		
		string ACCT_file = idValue->valuestring;
		LedgerMaze_Account_serialize(Acct, ACCT_file); //store the acct in the file
		string pass=passValue->valuestring;
		SQL_insert_acct(Acct,pass); //store in database

		cJSON_Delete(json);
		//get return params
		strcat(retParams,"create account success");
		LedgerMaze_Account_print(Acct);
		//free
		LedgerMaze_Account_free(Acct);
		LedgerMaze_PP_free(pp);
		BN_free(sn);

		//////////////////////////////////
		
		cout << "LedgerMaze_Create_Account takes time = " 
        	<< chrono::duration <double, milli> (running_time).count() << " ms" << endl; 
	}
	
	BN_free(pt_balance);
	

	struct evbuffer* retbuff = NULL;
	retbuff = evbuffer_new();
	if (retbuff == NULL)
	{
		printf("====line:%d,%s\n", __LINE__, "retbuff is null.");
		return;
	}
	evbuffer_add(retbuff, retParams, strlen(retParams));
	//evbuffer_add_printf(retbuff,"Receive post request,Thamks for the request!");
	evhttp_send_reply(req, HTTP_OK, "Client", retbuff);
	evbuffer_free(retbuff);

	
}

void Backend_AccountLogin(struct evhttp_request* req, void* arg) {

    size_t post_size = 0;
	if (req == NULL)
	{
		printf("input param req is null.");
		return;
	}

	char buf[BUF_MAX] = { 0 };
	Backend_Getpostmessage(buf, req);
	post_size = evbuffer_get_length(req->input_buffer);
	if (buf == NULL)
	{
		printf("get_post_message return null.");
		return;
	}

	//get params
	cJSON* json;
	cJSON *pt_balanceValue,*idValue,*passValue;
	idValue = cJSON_CreateObject();
	passValue = cJSON_CreateObject();

	json = cJSON_Parse(buf);
	if (!json) { printf("Error before: [%s]\n", cJSON_GetErrorPtr()); }
	else
	{
		idValue = cJSON_GetObjectItem(json,"id");
		passValue = cJSON_GetObjectItem(json,"pass");
	}

	//get acct value if login success
	string id=idValue->valuestring;
	string pass=passValue->valuestring;

	char sqlres[6][256]={0};
	char retParams[BUF_MAX] = { 0 };
	int retpass=SQL_select_acct(sqlres,id,pass);
	if(retpass==1)
	{
		// cJSON* retJson = cJSON_CreateObject();
		// cJSON_AddStringToObject(retJson,"id",sqlres[0]);
		// cJSON_AddStringToObject(retJson,"pt_balance",sqlres[1]);
		// cJSON_AddStringToObject(retJson,"pk",sqlres[2]);
		// cJSON_AddStringToObject(retJson,"sn",sqlres[3]);
		// cJSON_AddStringToObject(retJson,"ct_balance_CT",sqlres[4]);
		// cJSON_AddStringToObject(retJson,"ct_balance_value",sqlres[5]);
		strcat(retParams,"login success");
	}
	if(retpass==-1)
	{
		strcat(retParams,sqlres[0]);
	}
		
	struct evbuffer* retbuff = NULL;
	retbuff = evbuffer_new();
	if (retbuff == NULL)
	{
		printf("====line:%d,%s\n", __LINE__, "retbuff is null.");
		return;
	}
	evbuffer_add(retbuff, retParams, strlen(retParams));
	//evbuffer_add_printf(retbuff,"Receive post request,Thamks for the request!");
	evhttp_send_reply(req, HTTP_OK, "Client", retbuff);
	cJSON_Delete(json);
	evbuffer_free(retbuff);

}

void Backend_Mint(struct evhttp_request* req, void* arg) 
{
    size_t post_size = 0;
	if (req == NULL)
	{
		printf("input param req is null.");
		return;
	}

	char buf[BUF_MAX] = { 0 };
	Backend_Getpostmessage(buf, req);
	post_size = evbuffer_get_length(req->input_buffer);
	if (buf == NULL)
	{
		printf("get_post_message return null.");
		return;
	}

	//get params
	cJSON* json;
	cJSON *idValue,*Value;
	idValue = cJSON_CreateObject();
	Value = cJSON_CreateObject();

	json = cJSON_Parse(buf);
	if (!json) { printf("Error before: [%s]\n", cJSON_GetErrorPtr()); }
	else
	{
		idValue = cJSON_GetObjectItem(json,"id");
		Value = cJSON_GetObjectItem(json,"value");
	}

	//call Mint function
    LedgerMaze_PP pp; 
    LedgerMaze_PP_new(pp, RANGE_LEN_BACKEND , AGG_NUM_BACKEND ); 
    LedgerMaze_PP_deserialize(pp, "LedgerMaze.pp");
	LedgerMaze_CTx ctx_mint;
    LedgerMaze_CTx_new(pp,ctx_mint,MINT);
	LedgerMaze_Account Acct; 
    LedgerMaze_Account_new(Acct);
	LedgerMaze_Account_deserialize(Acct, idValue->valuestring); //read acct data form file
	// SQL_select_acctCT(Acct,idValue->valuestring); //read acct data form database
	BIGNUM *Vtransfer = BN_new(); 
	BN_asc2bn(&Vtransfer,Value->valuestring);
	BIGNUM *new_pt_balance = BN_new();
    ZK_BALANCE_CT new_zk_balance;
    ZK_BALANCE_CT_new(new_zk_balance);

	bool V_mint;
	cout<<">>Mint : begin to mint :"<< idValue->valuestring<<endl; //print hint


	//////////////////////////////////
    	auto start_time = chrono::steady_clock::now();

		int mint_p=LedgerMaze_Mint(pp,Acct,Vtransfer,ctx_mint,new_pt_balance,new_zk_balance);

    	auto end_time = chrono::steady_clock::now(); // end to count the time
    	auto running_time = end_time - start_time;  
		//////////////////////////////////
	

	if(mint_p==-1)
	{
		cout << ">>>>>> This Mint trans transfer value is invalid"<< endl; 
	}
	else
	{
		V_mint=LedgerMaze_Verify_CTx(pp,ctx_mint,Acct.zk_balance.r,new_zk_balance.r,new_zk_balance.v);
		if(V_mint)
		{
			LedgerMaze_Update_Acct(Acct,new_pt_balance,new_zk_balance);
			string ACCT_file = idValue->valuestring;
			LedgerMaze_Account_serialize(Acct, ACCT_file); //store the update acct data in the file
			//get unique CT id=(sn,pk,sig)
			string CT_file=BN_bn2string(ctx_mint.sn);
			CT_file.append(ECP_ep2string(ctx_mint.pk1));
			CT_file.append(ECP_ep2string(ctx_mint.sig.R));
			CT_file.append(BN_bn2string(ctx_mint.sig.z));
			BIGNUM *ct_bn = BN_new();
			Hash_String_to_BN(CT_file,ct_bn);
			LedgerMaze_CT_serialize(ctx_mint, ct_bn); //store the acct data in the file
			// store the simple ct data in the database
			SQL_insert_CT(ctx_mint,ct_bn,NULL);
			// SQL_update_acct(Acct);
			// SQL_insert_CT(ctx_mint,NULL);//store the trans data in the database
			cout << ">>>>>> This Mint trans is valid"<< endl; 

			//////////////////////////////////
			LedgerMaze_CTx_print(ctx_mint);
			cout << "LedgerMaze_Mint takes time = " 
        		<< chrono::duration <double, milli> (running_time).count() << " ms" << endl; 
			BN_free(ct_bn);
		}
		else    
			cout << ">>>>>> This Mint trans is invalid"<< endl;
	}
	


	//get return params
	char retParams[BUF_MAX] = { 0 };
	if(V_mint)
	{
		strcat(retParams,"mint success");
	}
	else if(mint_p==-1)
	{
		strcat(retParams,"mint value error");//false
	}
	else
	{
		strcat(retParams,"mint verify fail");//false
	}
	

	struct evbuffer* retbuff = NULL;
	retbuff = evbuffer_new();
	if (retbuff == NULL)
	{
		printf("====line:%d,%s\n", __LINE__, "retbuff is null.");
		return;
	}
	evbuffer_add(retbuff, retParams, strlen(retParams));
	evhttp_send_reply(req, HTTP_OK, "Client", retbuff);

	//free	
	cJSON_Delete(json);
	LedgerMaze_PP_free(pp);
	LedgerMaze_CTx_free(ctx_mint);
	LedgerMaze_Account_free(Acct);
	ZK_BALANCE_CT_free(new_zk_balance);
	BN_free(Vtransfer);
	BN_free(new_pt_balance);
	evbuffer_free(retbuff);

}

void Backend_Redeem(struct evhttp_request* req, void* arg) {

    size_t post_size = 0;

	if (req == NULL)
	{
		printf("input param req is null.");
		return;
	}

	char buf[BUF_MAX] = { 0 };
	Backend_Getpostmessage(buf, req);
	post_size = evbuffer_get_length(req->input_buffer);
	if (buf == NULL)
	{
		printf("get_post_message return null.");
		return;
	}

	//get params
	cJSON* json;
	cJSON *idValue,*Value;
	idValue = cJSON_CreateObject();
	Value = cJSON_CreateObject();

	json = cJSON_Parse(buf);
	if (!json) { printf("Error before: [%s]\n", cJSON_GetErrorPtr()); }
	else
	{
		idValue = cJSON_GetObjectItem(json,"id");
		Value = cJSON_GetObjectItem(json,"value");
	}

	
	//call redeem function
	LedgerMaze_PP pp; 
    LedgerMaze_PP_new(pp, RANGE_LEN_BACKEND , AGG_NUM_BACKEND ); 
    LedgerMaze_PP_deserialize(pp, "LedgerMaze.pp");
	LedgerMaze_CTx ctx_redeem;
    LedgerMaze_CTx_new(pp,ctx_redeem,REDEEM);
	LedgerMaze_Account Acct; 
    LedgerMaze_Account_new(Acct);
	LedgerMaze_Account_deserialize(Acct, idValue->valuestring); //read acct data
	BIGNUM *Vtransfer = BN_new(); 
	BN_asc2bn(&Vtransfer,Value->valuestring);
	BIGNUM *new_pt_balance = BN_new();
    ZK_BALANCE_CT new_zk_balance;
    ZK_BALANCE_CT_new(new_zk_balance);

	bool V_redeem;
	cout<<">>Redeem : begin to redeem :"<< idValue->valuestring<<endl; //print hint
	//////////////////////////////////
    	auto start_time = chrono::steady_clock::now();

		int redeem_p=LedgerMaze_Redeem(pp,Acct,Vtransfer,ctx_redeem,new_pt_balance,new_zk_balance);

    	auto end_time = chrono::steady_clock::now(); // end to count the time
    	auto running_time = end_time - start_time;  
	//////////////////////////////////
	
	if(redeem_p==-1)
	{
		cout << ">>>>>> This Redeem trans transfer value is invalid"<< endl; 
	}
	else
	{
		V_redeem=LedgerMaze_Verify_CTx(pp,ctx_redeem,Acct.zk_balance.r,new_zk_balance.r,new_zk_balance.v);
		if(V_redeem)
		{
			LedgerMaze_Update_Acct(Acct,new_pt_balance,new_zk_balance);
			string ACCT_file = idValue->valuestring;
			LedgerMaze_Account_serialize(Acct, ACCT_file); //store the update acct data in the file
			//get unique CT id=(sn,pk,sig)
			string CT_file=BN_bn2string(ctx_redeem.sn);
			CT_file.append(ECP_ep2string(ctx_redeem.pk1));
			CT_file.append(ECP_ep2string(ctx_redeem.sig.R));
			CT_file.append(BN_bn2string(ctx_redeem.sig.z));
			BIGNUM *ct_bn=BN_new();
			Hash_String_to_BN(CT_file,ct_bn);
			LedgerMaze_CT_serialize(ctx_redeem, ct_bn); //store the ct data in the file
			SQL_insert_CT(ctx_redeem,ct_bn,NULL);  //store the simple ct data in the database

			cout << ">>>>>> This Redeem trans is valid"<< endl; 

			//////////////////////////////////
			LedgerMaze_CTx_print(ctx_redeem);
			cout << "LedgerMaze_Redeem takes time = " 
        		<< chrono::duration <double, milli> (running_time).count() << " ms" << endl; 
		}
		else    
			cout << ">>>>>> This Redeem trans is invalid"<< endl;
	}

	//get return params
	char retParams[BUF_MAX] = { 0 };
	if(V_redeem)
	{
		strcat(retParams,"redeem success");
	}
	else if(redeem_p==-1)
	{
		strcat(retParams,"redeem value error");//false
	}
	else
	{
		strcat(retParams,"redeem verify fail");//false
	}

	struct evbuffer* retbuff = NULL;
	retbuff = evbuffer_new();
	if (retbuff == NULL)
	{
		printf("====line:%d,%s\n", __LINE__, "retbuff is null.");
		return;
	}
	evbuffer_add(retbuff, retParams, strlen(retParams));
	//evbuffer_add_printf(retbuff,"Receive post request,Thamks for the request!");
	evhttp_send_reply(req, HTTP_OK, "Client", retbuff);

	//free
	cJSON_Delete(json);
	evbuffer_free(retbuff);
	LedgerMaze_PP_free(pp);
	LedgerMaze_CTx_free(ctx_redeem);
	LedgerMaze_Account_free(Acct);
	ZK_BALANCE_CT_free(new_zk_balance);
	BN_free(Vtransfer);
	BN_free(new_pt_balance);

}

void Backend_SendCheque(struct evhttp_request* req, void* arg) {

    size_t post_size = 0;

	if (req == NULL)
	{
		printf("input param req is null.");
		return;
	}

	char buf[BUF_MAX] = { 0 };
	Backend_Getpostmessage(buf, req);
	post_size = evbuffer_get_length(req->input_buffer);
	if (buf == NULL)
	{
		printf("get_post_message return null.");
		return;
	}

	//get params
	cJSON* json;
	cJSON *SenderidValue,*ReceiveridValue, *Value;
	SenderidValue = cJSON_CreateObject();
	ReceiveridValue = cJSON_CreateObject();
	Value = cJSON_CreateObject();

	json = cJSON_Parse(buf);
	if (!json) { printf("Error before: [%s]\n", cJSON_GetErrorPtr()); }
	else
	{
		SenderidValue = cJSON_GetObjectItem(json,"sender_id");
		ReceiveridValue = cJSON_GetObjectItem(json,"receiver_id");
		Value = cJSON_GetObjectItem(json,"value");
	}

	//call sendcheque function
	LedgerMaze_PP pp; 
    LedgerMaze_PP_new(pp, RANGE_LEN_BACKEND , AGG_NUM_BACKEND ); 
    LedgerMaze_PP_deserialize(pp, "LedgerMaze.pp");
	LedgerMaze_CTx ctx_send;
    LedgerMaze_CTx_new(pp,ctx_send,SENDCHEQUE);
	LedgerMaze_Account Acct_sender,Acct_receiver; 
    LedgerMaze_Account_new(Acct_sender);
	LedgerMaze_Account_new(Acct_receiver);
	LedgerMaze_Account_deserialize(Acct_sender, SenderidValue->valuestring); //read sender acct data
	//debug
	cout<<"***sender account info"<<endl;
	LedgerMaze_Account_print(Acct_sender);
	LedgerMaze_Account_deserialize(Acct_receiver, ReceiveridValue->valuestring); //read receiver 
	cout<<"***receiver account info"<<endl;
	LedgerMaze_Account_print(Acct_receiver);
	//acct data
	BIGNUM *Vtransfer = BN_new(); 
	BN_asc2bn(&Vtransfer,Value->valuestring);
	BIGNUM *new_pt_balance = BN_new();
    ZK_BALANCE_CT new_zk_balance;
    ZK_BALANCE_CT_new(new_zk_balance);
	LedgerMaze_Cheque cheque;
    LedgerMaze_Cheque_new(cheque);

	cout<<">>Send : "<< SenderidValue->valuestring <<" begin to send cheque to:"<< ReceiveridValue->valuestring<<endl; //print hint

	//////////////////////////////////
    	auto start_time = chrono::steady_clock::now();

		int send_p=LedgerMaze_SendCheque(pp,Acct_sender,Acct_receiver,Vtransfer,ctx_send,new_pt_balance,new_zk_balance,cheque);

    	auto end_time = chrono::steady_clock::now(); // end to count the time
    	auto running_time = end_time - start_time;  
	//////////////////////////////////

	
	bool V_send;
	if(send_p==-1)
	{
		cout << ">>>>>> This SendCheque trans transfer value is invalid, should in(0,zk_balance.v]"<< endl; 
	}
	else
	{
		V_send=LedgerMaze_Verify_CTx(pp,ctx_send,NULL,NULL,NULL);
		if(V_send)
		{
			LedgerMaze_Update_Acct(Acct_sender,new_pt_balance,new_zk_balance);
			string ACCT_file = SenderidValue->valuestring;
			LedgerMaze_Account_serialize(Acct_sender, ACCT_file); //store the update acct data in the file
			string CT_file=BN_bn2string(ctx_send.sn);
			CT_file.append(ECP_ep2string(ctx_send.pk1));
			CT_file.append(ECP_ep2string(ctx_send.sig.R));
			CT_file.append(BN_bn2string(ctx_send.sig.z));
			BIGNUM *ct_bn=BN_new();
			Hash_String_to_BN(CT_file,ct_bn);
			LedgerMaze_CT_serialize(ctx_send, ct_bn); //store the ct data in the file

			LedgerMaze_CTx_print(ctx_send);
			
			SQL_insert_CT(ctx_send,ct_bn,Acct_receiver.keypair.pk);//store the simple ct data in the database
			cout << ">>>>>> This SendCheque trans is valid"<< endl; 

			//////////////////////////////////
			LedgerMaze_CTx_print(ctx_send);
			cout << "LedgerMaze_SendCheque takes time = " 
        		<< chrono::duration <double, milli> (running_time).count() << " ms" << endl; 
		}
		else    
			cout << ">>>>>> This SendCheque trans is invalid"<< endl;
	}

	//get return params
	char retParams[BUF_MAX] = { 0 };
	if(V_send)
	{
		strcat(retParams,"send cheque success");
	}
	else if(send_p==-1)
	{
		strcat(retParams,"send value error");//false
	}
	else
	{
		strcat(retParams,"send verify fail");//false
	}

	struct evbuffer* retbuff = NULL;
	retbuff = evbuffer_new();
	if (retbuff == NULL)
	{
		printf("====line:%d,%s\n", __LINE__, "retbuff is null.");
		return;
	}
	evbuffer_add(retbuff, retParams, strlen(retParams));
	//evbuffer_add_printf(retbuff,"Receive post request,Thamks for the request!");
	evhttp_send_reply(req, HTTP_OK, "Client", retbuff);

	cJSON_Delete(json);
	evbuffer_free(retbuff);
	LedgerMaze_PP_free(pp);
	LedgerMaze_CTx_free(ctx_send);
	LedgerMaze_Account_free(Acct_sender);
	LedgerMaze_Account_free(Acct_receiver);
	BN_free(Vtransfer);
	BN_free(new_pt_balance);
	ZK_BALANCE_CT_free(new_zk_balance);
	LedgerMaze_Cheque_free(cheque);
}

void Backend_RefreshCheque(struct evhttp_request* req, void* arg) {

    size_t post_size = 0;

	char retParams[BUF_MAX] = { 0 };



	if (req == NULL)
	{
		printf("input param req is null.");
		return;
	}

	char buf[BUF_MAX] = { 0 };
	Backend_Getpostmessage(buf, req);
	post_size = evbuffer_get_length(req->input_buffer);
	if (buf == NULL)
	{
		printf("get_post_message return null.");
		return;
	}

	//get params
	cJSON* json;
	cJSON *hashValue, *idValue;
	idValue = cJSON_CreateObject();
	hashValue = cJSON_CreateObject();

	json = cJSON_Parse(buf);
	if (!json) { printf("Error before: [%s]\n", cJSON_GetErrorPtr()); }
	else
	{
		idValue = cJSON_GetObjectItem(json,"id");
		hashValue = cJSON_GetObjectItem(json,"hash");
	}
	LedgerMaze_PP pp; 
    LedgerMaze_PP_new(pp, RANGE_LEN_BACKEND , AGG_NUM_BACKEND ); 
    LedgerMaze_PP_deserialize(pp, "LedgerMaze.pp");
	LedgerMaze_CTx ctx_send;
    LedgerMaze_CTx_new(pp,ctx_send,SENDCHEQUE);
	LedgerMaze_CT_deserialize(ctx_send,hashValue->valuestring);

	LedgerMaze_CTx_print(ctx_send);

	LedgerMaze_Account Acct_receiver; 
	LedgerMaze_Account_new(Acct_receiver);
	LedgerMaze_Account_deserialize(Acct_receiver, idValue->valuestring); //read receiver acct data

	LedgerMaze_CTx ctx_refresh;
    LedgerMaze_CTx_new(pp,ctx_refresh,REFRESHCHEQUE);


	LedgerMaze_Cheque cheque;
    LedgerMaze_Cheque_new(cheque);

	//////////////////////////////////
    	auto start_time = chrono::steady_clock::now();

		LedgerMaze_RefreshCheque(pp,Acct_receiver,ctx_send,ctx_refresh,cheque);

    	auto end_time = chrono::steady_clock::now(); // end to count the time
    	auto running_time = end_time - start_time;  
	//////////////////////////////////

	

	//LedgerMaze_CTx_print(ctx_refresh);

    
    
    cout << "checkout the trans_RefreshCheque if valid" << endl;
    if(!LedgerMaze_Verify_CTx(pp,ctx_refresh,NULL,NULL,NULL))
    {
        cout << ">>>>>> the trans_sendcheque is invalid!!" << endl;
		strcat(retParams,"refresh fail");
    }
    else
	{
        cout << ">>>>>> the trans_refresh is valid!!" << endl;

		cJSON* retJson = cJSON_CreateObject();

		string chequeHash=BN_bn2string(cheque.r);
		cJSON_AddStringToObject(retJson,"random",chequeHash.c_str());
		chequeHash.append(ECP_ep2string(cheque.pk));
		chequeHash.append(ECP_ep2string(cheque.C.X));
		chequeHash.append(ECP_ep2string(cheque.C.Y));
		chequeHash.append(BN_bn2string(cheque.transfer));
		BIGNUM *chequeHash_bn=BN_new();
		Hash_String_to_BN(chequeHash,chequeHash_bn);
		LedgerMaze_cheque_serialize(cheque,chequeHash_bn);
		chequeHash = BN_bn2string(chequeHash_bn);
		cJSON_AddStringToObject(retJson,"cheque_hash",chequeHash.c_str());
		cJSON_AddStringToObject(retJson,"value",BN_bn2dec(cheque.transfer));
		strcat(retParams,cJSON_Print(retJson));
		cJSON_Delete(retJson);

		string CTPath = hashValue->valuestring;   
			CTPath = CTPath +".ctx"; 
    		if(remove(CTPath.c_str())==0)
    		{
        		//cout<<"delete CT success"<<endl;
    		}
    		else
    		{
        		cout<<"delete CT fail"<<endl;
    		}
		//////////////////////////////////
			LedgerMaze_CT_serialize(ctx_refresh,chequeHash_bn);
			SQL_insert_CT(ctx_refresh,chequeHash_bn,Acct_receiver.keypair.pk);
			LedgerMaze_CTx_print(ctx_refresh);
			cout << "LedgerMaze_RefreshCheque takes time = " 
        		<< chrono::duration <double, milli> (running_time).count() << " ms" << endl; 
	}




	struct evbuffer* retbuff = NULL;
	retbuff = evbuffer_new();
	if (retbuff == NULL)
	{
		printf("====line:%d,%s\n", __LINE__, "retbuff is null.");
		return;
	}
	evbuffer_add(retbuff, retParams, strlen(retParams));
	//evbuffer_add_printf(retbuff,"Receive post request,Thamks for the request!");
	evhttp_send_reply(req, HTTP_OK, "Client", retbuff);
	evbuffer_free(retbuff);

	cJSON_Delete(json);


}

void Backend_DepositCheque(struct evhttp_request* req, void* arg) {

    size_t post_size = 0;

	char retParams[BUF_MAX] = { 0 };
	if (req == NULL)
	{
		printf("input param req is null.");
		return;
	}

	char buf[BUF_MAX] = { 0 };
	Backend_Getpostmessage(buf, req);
	post_size = evbuffer_get_length(req->input_buffer);
	if (buf == NULL)
	{
		printf("get_post_message return null.");
		return;
	}

	//get params
	cJSON* json;
	cJSON *idValue,*chequehashValue;
	idValue = cJSON_CreateObject();
	chequehashValue = cJSON_CreateObject();

	json = cJSON_Parse(buf);
	if (!json) { printf("Error before: [%s]\n", cJSON_GetErrorPtr()); }
	else
	{
		idValue = cJSON_GetObjectItem(json,"id");
		chequehashValue = cJSON_GetObjectItem(json,"cheque_hash");
	}

	//call sendcheque function
	LedgerMaze_PP pp; 
    LedgerMaze_PP_new(pp, RANGE_LEN_BACKEND , AGG_NUM_BACKEND ); 
    LedgerMaze_PP_deserialize(pp, "LedgerMaze.pp");

	LedgerMaze_Account Acct_receiver; 
	LedgerMaze_Account_new(Acct_receiver);
	LedgerMaze_Account_deserialize(Acct_receiver, idValue->valuestring); //read receiver acct data

	LedgerMaze_CTx ctx_deposit;
    LedgerMaze_CTx_new(pp,ctx_deposit,DEPOSITCHEQUE);

	BIGNUM *new_pt_balance = BN_new();
    ZK_BALANCE_CT new_zk_balance;
    ZK_BALANCE_CT_new(new_zk_balance);
	LedgerMaze_Cheque cheque;
    LedgerMaze_Cheque_new(cheque);
	LedgerMaze_cheque_deserialize(cheque,chequehashValue->valuestring);

	SplitLine_print('*');
	cout << "run the trans_DepositCheque" << endl; 

	//////////////////////////////////
    	auto start_time = chrono::steady_clock::now();

		LedgerMaze_Depositcheque(pp,Acct_receiver,cheque,ctx_deposit,new_pt_balance,new_zk_balance);

    	auto end_time = chrono::steady_clock::now(); // end to count the time
    	auto running_time = end_time - start_time;  
	//////////////////////////////////
    

	bool V_send=LedgerMaze_Verify_CTx(pp,ctx_deposit,NULL,NULL,NULL);
		if(V_send)
		{
			
			LedgerMaze_Update_Acct(Acct_receiver,new_pt_balance,new_zk_balance);
			string ACCT_file = idValue->valuestring;
			LedgerMaze_Account_serialize(Acct_receiver, ACCT_file); //store the update acct data in the file

			string CT_file=BN_bn2string(ctx_deposit.sn);
			CT_file.append(ECP_ep2string(ctx_deposit.pk1));
			CT_file.append(ECP_ep2string(ctx_deposit.sig.R));
			CT_file.append(BN_bn2string(ctx_deposit.sig.z));
			BIGNUM *ct_bn=BN_new();
			Hash_String_to_BN(CT_file,ct_bn);
			LedgerMaze_CT_serialize(ctx_deposit, ct_bn); //store the ct data in the file
			
			SQL_insert_CT(ctx_deposit,ct_bn,Acct_receiver.keypair.pk);//store the simple ct data in the database
			cout << ">>>>>> This DepositCheque trans is valid"<< endl; 
			strcat(retParams,"deposit cheque success");

			string chequePath = chequehashValue->valuestring;   
			chequePath = chequePath +".cheque"; 
    		if(remove(chequePath.c_str())==0)
    		{
        		//cout<<"delete depositcheque success"<<endl;
    		}
    		else
    		{
        		cout<<"delete depositcheque fail"<<endl;
    		}
			//////////////////////////////////
			LedgerMaze_CTx_print(ctx_deposit);
			cout << "LedgerMaze_Depositcheque takes time = " 
        		<< chrono::duration <double, milli> (running_time).count() << " ms" << endl; 



		}
		else
		{    
			cout << ">>>>>> This DepositCheque trans is invalid"<< endl;
			strcat(retParams,"Deposit Cheque verify fail");
		}

	//get return params

	struct evbuffer* retbuff = NULL;
	retbuff = evbuffer_new();
	if (retbuff == NULL)
	{
		printf("====line:%d,%s\n", __LINE__, "retbuff is null.");
		return;
	}
	evbuffer_add(retbuff, retParams, strlen(retParams));
	//evbuffer_add_printf(retbuff,"Receive post request,Thamks for the request!");
	evhttp_send_reply(req, HTTP_OK, "Client", retbuff);
	evbuffer_free(retbuff);
	cJSON_Delete(json);

}

void Backend_RequestTrans(struct evhttp_request* req, void* arg) {

    size_t post_size = 0;

	if (req == NULL)
	{
		printf("input param req is null.");
		return;
	}

	char buf[BUF_MAX] = { 0 };
	Backend_Getpostmessage(buf, req);
	post_size = evbuffer_get_length(req->input_buffer);
	if (buf == NULL)
	{
		printf("get_post_message return null.");
		return;
	}

	//get params
	cJSON* json;
	cJSON *idValue,*typeValue;
	idValue = cJSON_CreateObject();
	typeValue = cJSON_CreateObject();

	json = cJSON_Parse(buf);
	if (!json) { printf("Error before: [%s]\n", cJSON_GetErrorPtr()); }
	else
	{
		idValue = cJSON_GetObjectItem(json,"id");
	}
	

	LedgerMaze_Account Acct_receiver; 
	LedgerMaze_Account_new(Acct_receiver);
	LedgerMaze_Account_deserialize(Acct_receiver, idValue->valuestring); //read receiver acct data
	
	//sqlres *res = NULL;
	int num_sendct=0;
	//res = (sqlres*)malloc(sizeof(sqlres)+10*sizeof(str));  //max row num

	char res[20][512] = {0};
	SQL_select_CT_send(res,Acct_receiver.keypair.pk, num_sendct);
	
	LedgerMaze_PP pp; 
    LedgerMaze_PP_new(pp, RANGE_LEN_BACKEND , AGG_NUM_BACKEND ); 
    LedgerMaze_PP_deserialize(pp, "LedgerMaze.pp");
	LedgerMaze_CTx CT;
	LedgerMaze_CTx_new(pp,CT,SENDCHEQUE);

	//get return params
	char retParams[BUF_MAX] = { 0 };
	// 定义 [ ] 数组
	strcat(retParams,"[");

	if (num_sendct != 0)
	{
		for(int i=0;i<num_sendct-1;i++)
		{
			cJSON *retJson = cJSON_CreateObject();
			// 定义 { } 对象
			cout<<"ct_id"<<res[i]<<endl;
			LedgerMaze_CT_deserialize(CT,res[i]);
		
			cJSON_AddStringToObject(retJson,"ct_sn",BN_bn2dec(CT.sn));
			cJSON_AddStringToObject(retJson,"type","SendCheque");
			cJSON_AddStringToObject(retJson,"pk_sender",EC_POINT_point2hex(group,CT.pk1,POINT_CONVERSION_COMPRESSED,bn_ctx));
			cJSON_AddStringToObject(retJson,"ct_hash",res[i]);

			strcat(retParams,cJSON_Print(retJson));
			strcat(retParams,",");
			cJSON_Delete(retJson);

		}
		//the last
		cJSON *retJson = cJSON_CreateObject();
		// 定义 { } 对象
		//cout<<"ct_id"<<res[num_sendct-1]<<endl;
		LedgerMaze_CT_deserialize(CT,res[num_sendct-1]);
		
		cJSON_AddStringToObject(retJson,"ct_sn",BN_bn2dec(CT.sn));
		cJSON_AddStringToObject(retJson,"type","SendCheque");
		cJSON_AddStringToObject(retJson,"pk_sender",EC_POINT_point2hex(group,CT.pk1,POINT_CONVERSION_COMPRESSED,bn_ctx));
		cJSON_AddStringToObject(retJson,"ct_hash",res[num_sendct-1]);

		strcat(retParams,cJSON_Print(retJson));
		cJSON_Delete(retJson);
		/* code */
	}
	
	
	strcat(retParams,"]");

	
	

	struct evbuffer* retbuff = NULL;
	retbuff = evbuffer_new();
	if (retbuff == NULL)
	{
		printf("====line:%d,%s\n", __LINE__, "retbuff is null.");
		return;
	}
	evbuffer_add(retbuff, retParams, strlen(retParams));
	//evbuffer_add_printf(retbuff,"Receive post request,Thamks for the request!");
	evhttp_send_reply(req, HTTP_OK, "Client", retbuff);
	evbuffer_free(retbuff);
	cJSON_Delete(json);

}

void Backend_RequestAccts(struct evhttp_request* req, void* arg) {

    size_t post_size = 0;
	if (req == NULL)
	{
		printf("input param req is null.");
		return;
	}

	char buf[BUF_MAX] = { 0 };
	Backend_Getpostmessage(buf, req);
	post_size = evbuffer_get_length(req->input_buffer);
	if (buf == NULL)
	{
		printf("get_post_message return null.");
		return;
	}

	//get params
	cJSON* json;
	cJSON *idValue,*typeValue;
	idValue = cJSON_CreateObject();

	json = cJSON_Parse(buf);
	if (!json) { printf("Error before: [%s]\n", cJSON_GetErrorPtr()); }
	else
	{
		idValue = cJSON_GetObjectItem(json,"id");
	}
	
	string id=idValue->valuestring;
	//get return params
	char retParams[BUF_MAX] = { 0 };
	if (SQL_ifacctexsit_byid(id)==1)
	{
		LedgerMaze_Account Acct; 
		LedgerMaze_Account_new(Acct);
		LedgerMaze_Account_deserialize(Acct, idValue->valuestring); //read receiver acct data
		//LedgerMaze_Account_print(Acct);
		cJSON *retJson = cJSON_CreateObject();
		cJSON_AddStringToObject(retJson,"id",Acct.identity.c_str());
		cJSON_AddStringToObject(retJson,"pt_balance",BN_bn2dec(Acct.pt_balance));
		cJSON_AddStringToObject(retJson,"pk",EC_POINT_point2hex(group,Acct.keypair.pk,POINT_CONVERSION_COMPRESSED,bn_ctx));
		cJSON_AddStringToObject(retJson,"sn",BN_bn2dec(Acct.zk_balance.sn));
		char ctbuffer[POINT_LEN*4+1]={0};
		Twisted_ElGamal_CT_to_string(Acct.zk_balance.C,ctbuffer);
		cJSON_AddStringToObject(retJson,"ct_balance_CT",ctbuffer);
		cJSON_AddStringToObject(retJson,"ct_balance_value",BN_bn2dec(Acct.zk_balance.v));	

		strcat(retParams,cJSON_Print(retJson));
		
		cJSON_Delete(retJson);
	}
	else
	{
		strcat(retParams,"there is no user");
	}   
	

	
	struct evbuffer* retbuff = NULL;
	retbuff = evbuffer_new();
	if (retbuff == NULL)
	{
		printf("====line:%d,%s\n", __LINE__, "retbuff is null.");
		return;
	}
	evbuffer_add(retbuff, retParams, strlen(retParams));
	//evbuffer_add_printf(retbuff,"Receive post request,Thamks for the request!");
	evhttp_send_reply(req, HTTP_OK, "Client", retbuff);
	evbuffer_free(retbuff);
	cJSON_Delete(json);

}

void Backend_RequestAllTrans(struct evhttp_request* req, void* arg) {

    size_t post_size = 0;

	if (req == NULL)
	{
		printf("input param req is null.");
		return;
	}

	char buf[BUF_MAX] = { 0 };
	Backend_Getpostmessage(buf, req);
	post_size = evbuffer_get_length(req->input_buffer);
	if (buf == NULL)
	{
		printf("get_post_message return null.");
		return;
	}

	//get params
	cJSON* json;
	cJSON *idValue,*typeValue;
	idValue = cJSON_CreateObject();
	typeValue = cJSON_CreateObject();

	json = cJSON_Parse(buf);
	if (!json) { printf("Error before: [%s]\n", cJSON_GetErrorPtr()); }
	else
	{
		idValue = cJSON_GetObjectItem(json,"id");
	}
	

	LedgerMaze_Account Acct_sender; 
	LedgerMaze_Account_new(Acct_sender);
	LedgerMaze_Account_deserialize(Acct_sender, idValue->valuestring); //read receiver acct data
	
	//sqlres *res = NULL;
	int num_ct=0;
	//res = (sqlres*)malloc(sizeof(sqlres)+10*sizeof(str));  //max row num

	char res[20][512] = {0};
	SQL_select_CT(res,Acct_sender.keypair.pk, num_ct);
	
	LedgerMaze_PP pp; 
    LedgerMaze_PP_new(pp, RANGE_LEN_BACKEND , AGG_NUM_BACKEND ); 
    LedgerMaze_PP_deserialize(pp, "LedgerMaze.pp");
	LedgerMaze_CTx CT;
	LedgerMaze_CTx_new(pp,CT,SENDCHEQUE);

	//get return params
	char retParams[BUF_MAX] = { 0 };
	// 定义 [ ] 数组
	strcat(retParams,"[");

	if (num_ct != 0)
	{
		for(int i=0;i<num_ct-1;i++)
		{
			cJSON *retJson = cJSON_CreateObject();
			// 定义 { } 对象
			LedgerMaze_CTx CT;
			LedgerMaze_CTx_new(pp,CT,SENDCHEQUE);
			cout<<"ct_id"<<res[i]<<endl;
			LedgerMaze_CT_deserialize(CT,res[i]);
		
			cJSON_AddStringToObject(retJson,"ct_sn",BN_bn2dec(CT.sn));
			if(BN_cmp(CT.type,BN_0)==0)
			{
				cJSON_AddStringToObject(retJson,"type","Mint");
			}
			else if(BN_cmp(CT.type,BN_1)==0)
			{
				cJSON_AddStringToObject(retJson,"type","Redeem");
			}
			else if(BN_cmp(CT.type,BN_2)==0)
			{
				cJSON_AddStringToObject(retJson,"type","Sendcheque");
			}
			else if(BN_cmp(CT.type,BN_3)==0)
			{
				cJSON_AddStringToObject(retJson,"type","DepositCheque");
			}
			else
			{
				cJSON_AddStringToObject(retJson,"type","RefreshCheque");
			}

			cJSON_AddStringToObject(retJson,"pk_sender",EC_POINT_point2hex(group,CT.pk1,POINT_CONVERSION_COMPRESSED,bn_ctx));
			cJSON_AddStringToObject(retJson,"ct_hash",res[i]);

			strcat(retParams,cJSON_Print(retJson));
			strcat(retParams,",");
			cJSON_Delete(retJson);

		}
		//the last
		cJSON *retJson = cJSON_CreateObject();
		// 定义 { } 对象
		cout<<"ct_id"<<res[num_ct-1]<<endl;
		LedgerMaze_CT_deserialize(CT,res[num_ct-1]);
		cJSON_AddStringToObject(retJson,"ct_sn",BN_bn2dec(CT.sn));
		if(BN_cmp(CT.type,BN_0)==0)
		{
				cJSON_AddStringToObject(retJson,"type","Mint");
		}
		else if(BN_cmp(CT.type,BN_1)==0)
		{
				cJSON_AddStringToObject(retJson,"type","Redeem");
		}
		else if(BN_cmp(CT.type,BN_2)==0)
		{
				cJSON_AddStringToObject(retJson,"type","Sendcheque");
		}
		else if(BN_cmp(CT.type,BN_3)==0)
		{
				cJSON_AddStringToObject(retJson,"type","DepositCheque");
		}
		else
		{
				cJSON_AddStringToObject(retJson,"type","RefreshCheque");
		}

		cJSON_AddStringToObject(retJson,"pk_sender",EC_POINT_point2hex(group,CT.pk1,POINT_CONVERSION_COMPRESSED,bn_ctx));
		cJSON_AddStringToObject(retJson,"ct_hash",res[num_ct-1]);

		strcat(retParams,cJSON_Print(retJson));
		cJSON_Delete(retJson);
		/* code */
	}
	
	
	strcat(retParams,"]");

	
	

	struct evbuffer* retbuff = NULL;
	retbuff = evbuffer_new();
	if (retbuff == NULL)
	{
		printf("====line:%d,%s\n", __LINE__, "retbuff is null.");
		return;
	}
	evbuffer_add(retbuff, retParams, strlen(retParams));
	//evbuffer_add_printf(retbuff,"Receive post request,Thamks for the request!");
	evhttp_send_reply(req, HTTP_OK, "Client", retbuff);
	evbuffer_free(retbuff);
	cJSON_Delete(json);

}


void Backend_start() {

	struct evhttp* http_server = NULL;
	short http_port = 8086;
	const char* http_addr = "0.0.0.0";

	event_init();

	http_server = evhttp_start(http_addr, http_port);
	if (http_server == NULL)
	{
		printf("http server start failed.");
		return ;
	}

	evhttp_set_timeout(http_server, 5);

	evhttp_set_cb(http_server, "/CreateAccount", Backend_CreateAccount, NULL);
	evhttp_set_cb(http_server, "/AccountLogin", Backend_AccountLogin, NULL);
	evhttp_set_cb(http_server, "/Mint", Backend_Mint, NULL);
	evhttp_set_cb(http_server, "/Redeem", Backend_Redeem, NULL);
	evhttp_set_cb(http_server, "/SendCheque", Backend_SendCheque, NULL);
	evhttp_set_cb(http_server, "/RequestTrans", Backend_RequestTrans, NULL);
	evhttp_set_cb(http_server, "/RequestAllTrans", Backend_RequestAllTrans, NULL);
	evhttp_set_cb(http_server, "/RequestAccts", Backend_RequestAccts, NULL);
	
	evhttp_set_cb(http_server, "/RefreshCheque", Backend_RefreshCheque, NULL);
	evhttp_set_cb(http_server, "/DepositCheque", Backend_DepositCheque, NULL);
	// evhttp_set_cb(http_server, "/GetUserinfo", Backend_GetUserinfo, NULL);
	

	event_dispatch();
	evhttp_free(http_server);
}


#ifdef __cplusplus
}
#endif

#endif