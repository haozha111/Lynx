//
//  main.cpp
//  mytest
//
//  Created by haoliang on 13-11-29.
//  Copyright (c) 2013年 haoliang. All rights reserved.
//

#include <WinSock2.h> 
#include <Windows.h>
#include <tchar.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib ")



#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <math.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <queue>

using namespace std;

#define SERVER_PORT 7862 //服务器监听端口号
#define MAX_QUERY_SIZE 100
//#define MAX_REPLY_SIZE 600
#define BACK_LOG 10

SOCKET client_fd; 
char request[MAX_QUERY_SIZE];   
//char reply[MAX_REPLY_SIZE];
string end_signal = "<##>";
string gap = "<#>";

map<string, double> query_to_cnt;


struct trie_node{
    char c;
    bool terminate;
    list<trie_node*> children;
};

void insert_trienode(trie_node* root, char * vocab, int start, int len){
    if (start == len) {
        root->terminate = true;//we've completely insert a node
    }
    else{
		root->terminate = false;//we've completely insert a node
        char c = vocab[start];
        list<trie_node*>::const_iterator iter = root->children.begin();
        while (iter!= root->children.end()) {
            if ((*iter)->c == c) {
                break;
            }
            iter++;
        }
        if (iter == root->children.end()) {
            //insert a new node
            trie_node* p = new trie_node;
            p->c = c;
            root->children.push_back(p);
            insert_trienode(p, vocab, start + 1, len);
        }
        else{
            insert_trienode(*iter, vocab, start + 1, len);
            
        }
    }
    
}



trie_node* BuildTrieTree(char* vocab_list[], int size){
    if (size <= 0) {
        return NULL;
    }
    
    struct trie_node* root = new trie_node;
    for (int i = 0; i < size; i++) {
        insert_trienode(root, vocab_list[i], 0, strlen(vocab_list[i]));
    }
    
    return root;
}


class compare
{
public:
    bool operator() (const string & a, const string & b) const
    {
        return query_to_cnt[a] < query_to_cnt[b];
    }
};


void find_all_matches_after_firstlocating(trie_node* start_node, string& one_match, priority_queue<string, vector<string>, compare > &p_q, bool go_beyond_space){
    if (start_node->terminate) {
        p_q.push(one_match);
        //cout<<one_match<<endl;
    }
    if ((start_node->terminate == true && start_node->children.size() == 0) || (start_node->terminate == true && start_node->children.size() == 1 && start_node->children.front()->c == ' ' && !go_beyond_space)){
        return;
    }

    list<trie_node*>::const_iterator iter = start_node->children.begin();
    
    while (iter != start_node->children.end()) {
        if ((*iter)->c != ' ') {
            one_match += (*iter)->c;
            find_all_matches_after_firstlocating(*iter, one_match, p_q, go_beyond_space);
            one_match = one_match.substr(0,one_match.length()-1);
        }
        else{
            if (go_beyond_space) {
                one_match += ' ';
                find_all_matches_after_firstlocating(*iter, one_match, p_q, false);
                one_match = one_match.substr(0,one_match.length()-1);
            }
        }
        iter++;
    }
}


list<string> prefix_match(string prefix, trie_node* start_node, int top_k){
    list<string> prefix_wordset;
    priority_queue<string, vector<string>, compare > p_q;
    list<trie_node*>::const_iterator iter = start_node->children.begin();
    
    bool f = true;
    
    for (int i = 0; i < prefix.length(); i++) {
        char c = prefix[i];
        iter = start_node->children.begin();
        while (iter != start_node->children.end()) {
            if ((*iter)->c == c) {
                break;
            }
            iter++;
        }
        if (iter == start_node->children.end()) {
            f = false;
            break;
        }
        else{
            start_node = *iter;
        }
    }
    
    if (f == false) {
        return prefix_wordset;//the prefix matches no vocab!
    }
    else{
        find_all_matches_after_firstlocating(start_node, prefix, p_q, true);
        while (p_q.size() > 0 && top_k--) {
            prefix_wordset.push_back(p_q.top());
            p_q.pop();
        }
        return prefix_wordset;
    }
}

int main(int argc, const char * argv[])
{
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
		perror( "socket error!");  
		exit(1);  
	}   
	my_addr.sin_family=AF_INET; 
	my_addr.sin_port=htons(SERVER_PORT);  
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset( &(my_addr.sin_zero),0,8);  
	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) { 
		perror( "bind error!");
		exit(1);    
	}

    ifstream fin(argv[1]);
    string line;
    char** vocab_list = new char*[4123835];
    int size = 0;
	while (getline(fin,line)) {
		string word, cnt;
		size_t pos = line.find('\t');
		word = line.substr(0,pos);
		cnt = line.substr(pos + 1, line.length());
		query_to_cnt[word] = atof(cnt.c_str());
		vocab_list[size] = new char[100];
		strcpy(vocab_list[size], word.c_str());
		size++;
	}
	fin.close();

	trie_node * root = NULL;
	root = BuildTrieTree(vocab_list, size);

	while(1)
	{
		printf("Prepare listening...\n");
		if (listen(sockfd, BACK_LOG) != -1) {   
			int sin_size = sizeof(struct sockaddr_in);  
			client_fd = accept(sockfd, (struct sockaddr *) &remote_addr, &sin_size); 
			//printf( "received a query: %s\n", inet_ntoa(remote_addr.sin_addr));
			int recvbytes;
			if ((recvbytes=recv(client_fd, request, MAX_QUERY_SIZE, 0)) !=-1) {  
				request[recvbytes] = '\0';
				printf( "Received query: %d,%s\n",client_fd,request);

				string t;
				list<string> match_set = prefix_match(request, root, 10);
				list<string>::const_iterator iter = match_set.begin();
				while (iter != match_set.end()) {
					t = t + *iter + gap;
					//cout<<*iter<<" "<<query_to_cnt[*iter]<<endl;
					iter++;
				}

				//memset(reply,0,MAX_REPLY_SIZE);
				//strcpy(reply, t.c_str());
				send(client_fd, t.c_str(), t.length(), 0);
			}
			closesocket(client_fd);
		}
	}
	
	
   
    
    
    
    /*cout<<"Now enter your query:"<<endl;
    
    
    
    char q[100];
    
    while (cin.getline(q, 100)) {
        prefix += q;
        list<string> match_set = prefix_match(prefix, root, 10);
        list<string>::const_iterator iter = match_set.begin();
        while (iter != match_set.end()) {
            cout<<*iter<<" "<<query_to_cnt[*iter]<<endl;
            iter++;
        }
        if (match_set.size() > 0) {
            cout<<prefix;
        }
        else{
            prefix.clear();
        }
        
    }*/
    
    return 0;
}

