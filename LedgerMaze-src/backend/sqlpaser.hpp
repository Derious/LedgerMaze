#include <mysql/mysql.h>
#include "../LedgerMaze/LedgerMaze.hpp"
#define SQL_MAXLENG 512
#define PROOF_MAX  1024

// template<typename T>
// string to_string(T value) {
//   std::ostringstream os;
//   os << value;
//   return os.str;
// }

typedef char str[512];
struct sqlres
{
	int len;
	str res[];
	/* data */
};
int SQL_insert_acct(LedgerMaze_Account &acct,string &pass)
{

    MYSQL *conn_ptr;  
    conn_ptr = mysql_init(NULL); 
	if(!conn_ptr)  
    {  
      fprintf(stderr,"mysql_init failed\n");  
        return 0;  
    }  
    conn_ptr = mysql_real_connect(conn_ptr,"127.0.0.1","zck","12345678","LedgerMaze",0,NULL,0); 

	BIGNUM *passvalue = BN_new();
	Hash_String_to_BN(pass,passvalue);
    char sql_m[2*SQL_MAXLENG] ={0};
    strcat(sql_m,"insert into account values('");
    strcat(sql_m, acct.identity.c_str());
	strcat(sql_m, "','");
	strcat(sql_m, EC_POINT_point2hex(group, acct.keypair.pk, POINT_CONVERSION_COMPRESSED, bn_ctx));
	strcat(sql_m, "','");
	strcat(sql_m, BN_bn2hex(acct.keypair.sk));
	strcat(sql_m, "','");
	strcat(sql_m,BN_bn2dec(acct.pt_balance));
	strcat(sql_m, "','");
	strcat(sql_m, BN_bn2hex(acct.zk_balance.sn));
	strcat(sql_m, "','");
	char ctbuffer[POINT_LEN*4+1] = {0};
	Twisted_ElGamal_CT_to_string(acct.zk_balance.C,ctbuffer);
	strcat(sql_m, ctbuffer);
	strcat(sql_m, "','");
	strcat(sql_m, BN_bn2dec(acct.zk_balance.v));
	strcat(sql_m, "','");
	strcat(sql_m, BN_bn2hex(acct.zk_balance.r));
	strcat(sql_m, "','");
	strcat(sql_m, BN_bn2hex(passvalue));
	strcat(sql_m, "')");

	BN_free(passvalue);
	//cout<<"sql="<<sql_m<<endl;//debug

	if(mysql_real_query(conn_ptr,sql_m,strlen(sql_m)))
	{
		printf("insert query failed!\n");
		mysql_close(conn_ptr); 
		return -1;  
    }
    mysql_close(conn_ptr); 
	return 0;
}
int SQL_update_acct(LedgerMaze_Account &acct)
{
    MYSQL *conn_ptr;  
    conn_ptr = mysql_init(NULL); 
	if(!conn_ptr)  
    {  
      fprintf(stderr,"mysql_init failed\n");  
        return 0;  
    }  
    conn_ptr = mysql_real_connect(conn_ptr,"127.0.0.1","zck","12345678","LedgerMaze",0,NULL,0); 

    char sql_m[2*SQL_MAXLENG] ={0};
    strcat(sql_m,"update account set pt_balance='");
    strcat(sql_m, BN_bn2hex(acct.pt_balance));
	strcat(sql_m, "',");
    strcat(sql_m,"sn='");
    strcat(sql_m, BN_bn2hex(acct.zk_balance.sn));
    strcat(sql_m, "',");
    strcat(sql_m,"ct_value='");
    char ctbuffer[POINT_LEN*4+1] = {0};
    Twisted_ElGamal_CT_to_string(acct.zk_balance.C,ctbuffer);
    strcat(sql_m, ctbuffer);
    strcat(sql_m, "',");
    strcat(sql_m,"value='");
    strcat(sql_m, BN_bn2hex(acct.zk_balance.v));
    strcat(sql_m, "',");
    strcat(sql_m,"random='");
    strcat(sql_m, BN_bn2hex(acct.zk_balance.r));
    strcat(sql_m, "' where id ='");
    strcat(sql_m, acct.identity.c_str());
    strcat(sql_m, "'");
	//cout<<"sql="<<sql_m<<endl;//debug
	if(mysql_real_query(conn_ptr,sql_m,strlen(sql_m)))
	{
		printf("update query failed!\n");
		mysql_close(conn_ptr); 
		return -1;  
    }
    mysql_close(conn_ptr); 
	return 0;
}

//used in login res
int SQL_select_acct(char res[6][256],string &id,string &pass)
{
	unsigned int num_fields;
	MYSQL_ROW rows;  
	MYSQL_RES *result;
	MYSQL *conn_ptr;
	conn_ptr = mysql_init(NULL);  
	if(!conn_ptr)  
    {  
      	fprintf(stderr,"mysql_init failed\n");  
        return 0;  
    }  
	conn_ptr = mysql_real_connect(conn_ptr,"127.0.0.1","zck","12345678","LedgerMaze",0,NULL,0); 

	char sql_m[2*SQL_MAXLENG] ={0};
	strcat(sql_m,"select * from account where id = '");
	strcat(sql_m,id.c_str());
	strcat(sql_m,"'");
	cout<<"sql"<<sql_m<<endl;//debug
	if(mysql_real_query(conn_ptr,sql_m,strlen(sql_m)))
	{
		printf("select query failed!\n");
		return 0;  
    }
    result = mysql_store_result(conn_ptr);
	num_fields = mysql_num_fields(result);
    //printf("num_fields:%x \n",num_fields);//debug
	rows=mysql_fetch_row(result);
	BIGNUM *pass_bn=BN_new();
	Hash_String_to_BN(pass,pass_bn);
	if(strcmp(BN_bn2hex(pass_bn),rows[8])==0)
	{
		// printf("%s \n",rows[0]);
		// printf("%s \n",rows[3]);
		// printf("%s \n",rows[1]);
		// printf("%s \n",rows[4]);
		// printf("%s \n",rows[5]);
		// printf("%s \n",rows[6]);
	

		strcpy(res[0],rows[0]);
		strcpy(res[1],rows[3]);
		strcpy(res[2],rows[1]);
		strcpy(res[3],rows[4]);
		strcpy(res[4],rows[5]);
		strcpy(res[5],rows[6]);
		mysql_free_result(result); 
        mysql_close(conn_ptr); 
		BN_free(pass_bn);
		return 1;
	}
	
	else
	{
		strcat(res[0],"login fail");
		mysql_free_result(result); 
        mysql_close(conn_ptr); 
		BN_free(pass_bn);
		return -1;
	}

}

//used in login res
int SQL_ifacctexsit_byid(string &id)
{
	unsigned int num_fields;
	MYSQL_ROW rows;  
	MYSQL_RES *result;
	MYSQL *conn_ptr;
	conn_ptr = mysql_init(NULL);  
	if(!conn_ptr)  
    {  
      	fprintf(stderr,"mysql_init failed\n");  
        return 0;  
    }  
	conn_ptr = mysql_real_connect(conn_ptr,"127.0.0.1","zck","12345678","LedgerMaze",0,NULL,0); 

	char sql_m[2*SQL_MAXLENG] ={0};
	strcat(sql_m,"select * from account where id = '");
	strcat(sql_m,id.c_str());
	strcat(sql_m,"'");
	//cout<<"sql"<<sql_m<<endl;//debug
	if(mysql_real_query(conn_ptr,sql_m,strlen(sql_m)))
	{
		printf("select query failed!\n");
		return 0;  
    }
    result = mysql_store_result(conn_ptr);

	if (result)
	{
		num_fields = mysql_num_fields(result);
    	//printf("num_fields:%x \n",num_fields);//debug
		// rows=mysql_fetch_row(result);
		// printf("%s \n",rows[0]);
		// printf("%s \n",rows[3]);
		// printf("%s \n",rows[1]);
		// printf("%s \n",rows[4]);
		// printf("%s \n",rows[5]);
		// printf("%s \n",rows[6]);
	

		// strcpy(res[0],rows[0]);
		// strcpy(res[1],rows[3]);
		// strcpy(res[2],rows[1]);
		// strcpy(res[3],rows[4]);
		// strcpy(res[4],rows[5]);
		// strcpy(res[5],rows[6]);
		mysql_free_result(result); 
        mysql_close(conn_ptr); 
		return 1;
	}	
	else
	{
		// strcat(res[0],"there is no user");
		mysql_free_result(result); 
        mysql_close(conn_ptr); 
		return -1;
	}

}

//return the unique CT file name
int SQL_select_CT_send(char res[10][512] ,EC_POINT *pk_receiver,int &num)
{
    unsigned int num_rows;
	MYSQL_ROW rows;  
	MYSQL_RES *result;
	MYSQL *conn_ptr;
	conn_ptr = mysql_init(NULL);  
	if(!conn_ptr)  
    {  
      	fprintf(stderr,"mysql_init failed\n");  
        return 0;  
    }  
	conn_ptr = mysql_real_connect(conn_ptr,"127.0.0.1","zck","12345678","LedgerMaze",0,NULL,0); 

	char sql_m[2*SQL_MAXLENG] ={0};
	strcat(sql_m,"select ct_id from confidential_transaction where type='2' and pk_receiver='");
	strcat(sql_m,ECP_ep2string(pk_receiver).c_str());
	strcat(sql_m,"'");


    //cout<<"sql"<<sql_m<<endl;//debug
	if(mysql_real_query(conn_ptr,sql_m,strlen(sql_m)))
	{
		printf("select CT query failed!\n");
		return 0;  
    }
    result = mysql_store_result(conn_ptr);

	
	num_rows =mysql_num_rows(result);
	int i = 0;
	while( (rows = mysql_fetch_row(result)) ){

		strcat(res[i],rows[0]);
		//cout<<"--"<<rows[0]<<endl;
		//cout<<"--"<<res[i]<<endl;
		i++;

	}
	num=num_rows;

	mysql_free_result(result); 
    mysql_close(conn_ptr); 
    return 1;

}

int SQL_select_CT(char res[10][512] ,EC_POINT *pk_sender,int &num)
{
    unsigned int num_rows;
	MYSQL_ROW rows;  
	MYSQL_RES *result;
	MYSQL *conn_ptr;
	conn_ptr = mysql_init(NULL);  
	if(!conn_ptr)  
    {  
      	fprintf(stderr,"mysql_init failed\n");  
        return 0;  
    }  
	conn_ptr = mysql_real_connect(conn_ptr,"127.0.0.1","zck","12345678","LedgerMaze",0,NULL,0); 

	char sql_m[2*SQL_MAXLENG] ={0};
	strcat(sql_m,"select ct_id from confidential_transaction where pk_sender='");
	strcat(sql_m,ECP_ep2string(pk_sender).c_str());
	strcat(sql_m,"'");
	strcat(sql_m,"or pk_receiver='");
	strcat(sql_m,ECP_ep2string(pk_sender).c_str());
	strcat(sql_m,"'");


    cout<<"sql"<<sql_m<<endl;//debug
	if(mysql_real_query(conn_ptr,sql_m,strlen(sql_m)))
	{
		printf("select CT query failed!\n");
		return 0;  
    }
    result = mysql_store_result(conn_ptr);

	
	num_rows =mysql_num_rows(result);
	int i = 0;
	while( (rows = mysql_fetch_row(result)) ){

		strcat(res[i],rows[0]);
		// cout<<"--"<<rows[0]<<endl;
		// cout<<"--"<<res[i]<<endl;
		i++;

	}
	num=num_rows;

	mysql_free_result(result); 
    mysql_close(conn_ptr); 
    return 1;



}

int SQL_insert_CT(LedgerMaze_CTx &CT,BIGNUM *ct_bn,EC_POINT *pk_receiver)
{
	MYSQL *conn_ptr;  
    conn_ptr = mysql_init(NULL); 
	if(!conn_ptr)  
    {  
      fprintf(stderr,"mysql_init failed\n");  
        return 0;  
    }  
    conn_ptr = mysql_real_connect(conn_ptr,"127.0.0.1","zck","12345678","LedgerMaze",0,NULL,0); 


    char sql_m[2*SQL_MAXLENG] ={0};
    strcat(sql_m,"insert into confidential_transaction values('");
    strcat(sql_m, BN_bn2dec(CT.sn));
	strcat(sql_m, "','");
	strcat(sql_m, BN_bn2dec(CT.type));
	strcat(sql_m, "','");
    strcat(sql_m, BN_bn2hex(ct_bn));
	if(BN_cmp(CT.type,BN_2)==0)
	{
		strcat(sql_m, "','");
		strcat(sql_m, ECP_ep2string(pk_receiver).c_str());
	}
	else
	{
		strcat(sql_m, "','");
		strcat(sql_m, "NULL");
	}
	strcat(sql_m, "','");
    strcat(sql_m, ECP_ep2string(CT.pk1).c_str());
	strcat(sql_m, "')");

    //cout<<"sql="<<sql_m<<endl;//debug
    if(mysql_real_query(conn_ptr,sql_m,strlen(sql_m)))
	{
		printf("insert ct query failed!\n");
		mysql_close(conn_ptr); 
		return -1;  
    }

    mysql_close(conn_ptr); 
	return 0;
}