#include <WinSock2.h> 
#include <Windows.h>
#include <tchar.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <map>
#include "boolfunc.h"
#include "generalfunc.h"
#include "vectorspace.h"
#include "SnippetGenerator.h"
#include "word2vec.h"
//#include "../json/json.h"

//#pragma comment(lib, "../json/json_mtd.lib")
#pragma comment(lib, "ws2_32.lib ")

#define SERVER_PORT 7861 //服务器监听端口号
#define MAX_QUERY_SIZE 100
//#define MAX_REPLY_SIZE 600
#define BACK_LOG 10

using namespace std;

SOCKET client_fd; 
char request[MAX_QUERY_SIZE];   
//char reply[MAX_REPLY_SIZE];
string end_signal = "<##>";
string gap = "<#>";

int SIZE_OF_VOCAB = 1086036;//true size + 1
int SIZE_OF_COLLECTION = 205344;
float AVERAGE_DOCUMENT_LENGTH = 492.685367;
unsigned short NUM_OF_WEIGHT = 30;
inv_node** inv_list = new inv_node*[SIZE_OF_VOCAB];
float* idf = new float[SIZE_OF_VOCAB];
doc_info* docs = new doc_info[SIZE_OF_COLLECTION + 1];
map<string, int> mp;//term->index
map<string, token_item> token_list;
map<string, token_item>::const_iterator token_list_iter[27];
float* weight = new float[NUM_OF_WEIGHT + 1];
map<int, list<inv_pos_item> > inv_list_pos;

char * doc_binary_filepath = "C:/Users/JD/Desktop/IR/data/docbinary/";
char * doc_raw_filepath = "C:/Users/JD/Desktop/IR/data/docraw/";
char * vocab_total_filepath = "C:/Users/JD/Desktop/IR/data/vocab.total.txt";
char * invlist_filepath = "C:/Users/JD/Desktop/IR/data/inv_list.txt";
char * token_vocab_filepath = "C:/Users/JD/Desktop/IR/data/token_vocab.txt";
char * doc_info_filepath = "C:/Users/JD/Desktop/IR/data/doc_info.txt";
char * word_vector_filepath = "C:/Users/JD/Desktop/IR/data/wordvec.bin";
char * poslist_filepath = "C:/Users/JD/Desktop/IR/data/position/";

int _tmain(int argc, _TCHAR *argv[])
{
	initialize_word2vec(word_vector_filepath);
	initialize_invlist(vocab_total_filepath, invlist_filepath);
	compute_idf(idf);
	initialize_docinfo(doc_info_filepath);
	update_tf(docs);
	update_weight(weight);
	initialize_token(token_vocab_filepath);
	printf("Initialize done.\n");
    
	SOCKET sockfd; //sock_fd：监听socket；client_fd：数据传输socket    
	struct sockaddr_in my_addr; // 本机地址信息  
	struct sockaddr_in remote_addr; // 客户端地址信息  
	char szMsg[]="hello";  
	WORD wVersionRequested;  
	WSADATA wsaData;  
	int err;   
	wVersionRequested = MAKEWORD(2,0);   
	err = WSAStartup(wVersionRequested,&wsaData);  
	if (0 != err) {   
		cout<<"Socket failed";    
		return 0;    
	}   
	if (LOBYTE(wsaData.wVersion)!= 2 || HIBYTE(wsaData.wVersion) != 0) {  
		WSACleanup();   
		return 0;   
	}   
	sockfd = socket(AF_INET,SOCK_STREAM,0);  
	if (INVALID_SOCKET == sockfd) {   
		cout<<"Soket Create Failed";  
		return 0;  
	}   
	//sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {   
		perror( "socket创建出错！");   
		exit(1);  
	}   
	my_addr.sin_family=AF_INET; 
	my_addr.sin_port=htons(SERVER_PORT);  
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset( &(my_addr.sin_zero),0,8);  
	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) { 
		perror( "bind出错！");   
		exit(1);    
	}

	while(1) {
		printf("Prepare listening...\n");
		if (listen(sockfd, BACK_LOG) != -1) {   
			int sin_size = sizeof(struct sockaddr_in);  
			client_fd = accept(sockfd, (struct sockaddr *) &remote_addr, &sin_size); 
			//printf( "received a query: %s\n", inet_ntoa(remote_addr.sin_addr));
			int recvbytes;
			if ((recvbytes=recv(client_fd, request, MAX_QUERY_SIZE, 0)) !=-1) {  
				request[recvbytes] = '\0';
				printf( "Received query: %d,%s\n",client_fd,request);
				string reply;
				string str,correct,imgID,page,query,model,sortby;
				//bool phraseQuery = false;
				vector<int> res;
				res.clear();
				str = request;

				size_t first,second,third;
				first = str.find("<#>");
				second = str.find("<#>",first+3);
				//third = str.find("<#>",second+3);
				page = str.substr(0,first);
				query = str.substr(first+3,second-first-3);
				model = str.substr(second+3,str.length()-second-3);
				//sortby = str.substr(third+3,str.length()-third-3);
				cout << page << " " << query << " " << model << endl;

				transform(query.begin(), query.end(), query.begin(), ::tolower);

				
				//cout << "send: correct..." <<endl;
				//cout << t << endl;
				//memset(reply,0,MAX_REPLY_SIZE);
				//strcpy(reply, t.c_str());
				//send(client_fd, t.c_str(), t.length(), 0);

				if(model == "vector") {

					//cout << "corret query..." <<endl;
					if(corret_query(query, correct)){

						for (int i = correct.length() - 1;correct[i] == ' ';i--)
						{
							correct.erase(i,1);
						}

						reply = correct + "<edit_correct>";
						query = correct;
					}
					else{
						reply = "<edit_correct>";
					}
					//cout << "do one search..." <<endl;
					res = do_one_search(query, idf , 10*atoi(page.c_str()), 10);
				} 
				else if(model == "boolTime") 
				{
					reply = "<edit_correct>";
					res = boolean_retrieval_without_pos(query, 10*atoi(page.c_str()), 10, false, true);
					 while (query.find("&") != string::npos) {
                        query = query.replace(query.find("&"), 1, " ");
                     }
                     while (query.find("|") != string::npos) {
                        query = query.replace(query.find("|"), 1, " ");
                     }
                      while (query.find("(") != string::npos) {
                        query = query.replace(query.find("("), 1, " ");
                     }
                      while (query.find(")") != string::npos) {
                        query = query.replace(query.find(")"), 1, " ");
                     }
                      while (query.find("-") != string::npos) {
                        query = query.replace(query.find("-"), 1, " ");
                     }
				}
				else if(model == "boolPop" )
				{
					reply = "<edit_correct>";
					res = boolean_retrieval_without_pos(query, 10*atoi(page.c_str()), 10, true, false);
					while (query.find("&") != string::npos) {
                        query = query.replace(query.find("&"), 1, " ");
                     }
                     while (query.find("|") != string::npos) {
                        query = query.replace(query.find("|"), 1, " ");
                     }
                      while (query.find("(") != string::npos) {
                        query = query.replace(query.find("("), 1, " ");
                     }
                      while (query.find(")") != string::npos) {
                        query = query.replace(query.find(")"), 1, " ");
                     }
                      while (query.find("-") != string::npos) {
                        query = query.replace(query.find("-"), 1, " ");
                     }
				}
				else if(model == "kProxTime" )
				{
					reply = "<edit_correct>";
					istringstream iss(query);
					string query1,stmp;
					int f = 1;
					vector<int> tvec;
					while (iss>>stmp)
					{
						if (f)
						{
							query1 += stmp + " ";
							f = 0;
						}
						else{
							tvec.push_back(atoi(stmp.c_str()));
							f = 1;
						}
					}
					int * margins = new int(tvec.size());
					for (int i = 0; i < tvec.size();i++)
					{
						margins[i] = tvec[i];
					}

					res = multipositional_intersect(query1, margins, tvec.size(), 10*atoi(page.c_str()), 10, false, true);
				}
				else if(model == "kProxPop")
				{
					reply = "<edit_correct>";
					istringstream iss(query);
					string query1,stmp;
					int f = 1;
					vector<int> tvec;
					while (iss>>stmp)
					{
						if (f)
						{
							query1 += stmp + " ";
							f = 0;
						}
						else{
							tvec.push_back(atoi(stmp.c_str()));
							f = 1;
						}
					}
					int * margins = new int(tvec.size());
					for (int i = 0; i < tvec.size();i++)
					{
						margins[i] = tvec[i];
					}

					res = multipositional_intersect(query1, margins, tvec.size(), 10*atoi(page.c_str()), 10, true, false);
				}

				if(res.empty()) {

					reply += "No result...";
					reply += end_signal;
					//cout << reply <<endl;
					//memset(reply,0,MAX_REPLY_SIZE);
					//strcpy(reply, t.c_str());
					//send(client_fd, t.c_str(), t.length(), 0);

				} else {

					vector<int>::const_iterator iter = res.begin();
					while (iter != res.end()) {

						stringstream ss;
						ss << *iter;
						ss >> imgID;
						reply += imgID + gap;

						string title = docs[*iter].title;
						if(title.length() > 70) {
							title = title.substr(0,67);
							title += "...";
						}

						if(!docs[*iter].img)
						{
							imgID = "0";
						}

						reply += title + gap + docs[*iter].url + gap + docs[*iter].date + gap + imgID + gap;
						reply += snippet_generator(query, *iter) + end_signal;
						//cout << "send: news..." <<endl;
						//cout << t << endl;
						//cout << "****************************************************************" << endl;

						//memset(reply,0,MAX_REPLY_SIZE);
						//strcpy(reply, t.c_str());
						//send(client_fd, t.c_str(), t.length(), 0);

						iter++;
					}
					
					reply += "<similar_words>";
					if(model=="vector")
					{
						list<string> s = similar_words(query.c_str());
						//cout <<"similar words size:"<< s.size() << endl;
						list<string>::const_iterator iter_sim = s.begin();
						while (iter_sim != s.end()) {
							//cout << (*iter_sim) << endl;
							reply += (*iter_sim) + "<#>";
							iter_sim++;
						}
					}
					
					//cout << "send: similar..." <<endl;
					//cout << reply << endl;
					//memset(reply,0,MAX_REPLY_SIZE);
					//strcpy(reply, t.c_str());
					send(client_fd, reply.c_str(), reply.length(), 0);
					
				}
			}  
			shutdown(client_fd,2);
		}
	}
    
/*
	char st1[100];

	while (cin.getline(st1, 100)) {
		list<char*> s = similar_words(st1);
		list<char*>::const_iterator iter = s.begin();
		while (iter != s.end()) {
			cout<<*iter<<endl;
			iter++;
		}
	}*/
    
    /*
    string str;
    while (cin>>str) {
        transform(str.begin(), str.end(), str.begin(), ::tolower);
        if (token_list.count(str) == 0) {
            edit_correct(str);

        }
    }
    */
  
   /* initialize_word2vec(word_vector_filepath);
    char st1[100];
    
    while (cin.getline(st1, 100)) {
        int r = similar_words(st1);
    }*/
    
   
   
    
    return 0;
}