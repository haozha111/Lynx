//
//  main.cpp
//  LynxPreview
//
//  Created by 张 昊亮 on 13-12-22.
//  Copyright (c) 2013年 张 昊亮. All rights reserved.
//
#include <WinSock2.h> 
#include <Windows.h>
#include <tchar.h>

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include "fun.h"

#pragma comment(lib, "ws2_32.lib ")

#define SERVER_PORT 7863 //服务器监听端口号
#define MAX_QUERY_SIZE 10
//#define MAX_REPLY_SIZE 600
#define BACK_LOG 10

using namespace std;

SOCKET client_fd; 
char request[MAX_QUERY_SIZE];   
//char reply[MAX_REPLY_SIZE];
string end_signal = "<##>";
string gap = "<#>";

//char * doc_raw_filepath = "C:/Users/JD/Desktop/IR/data/docraw/";
//char * doc_info_filepath = "C:/Users/JD/Desktop/IR/data/doc_info.txt";

int SIZE_OF_COLLECTION = 205344;

doc_info* docs = new doc_info[SIZE_OF_COLLECTION + 1];




int main(int argc, const char * argv[])
{

    int doc_id;
    string doc_trim;

    initialize_docinfo(argv[2]);

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

				string t;
				doc_id = atoi(request);
				string doc = read_raw_doc_byline(doc_id,argv[1]);
				doc_trim.clear();
				int cnt = 0;
				for (int i = 0; i < doc.length(); i++) {
					if (doc[i] == ' ') {
						cnt++;
						if (cnt == 300) {
							if (i != doc.length() - 1) {
								doc_trim += "...";
							}
							break;
						}
					}
					if ((doc[i] >= 65 && doc[i] <= 122) || doc[i] == ' ' ) {
						doc_trim += doc[i];
					}
					else
					{
						doc_trim += " ";
					}
				}
				doc_trim = "  " + doc_trim;
				doc_info d = docs[doc_id];
				t = d.title;
				t += gap + doc_trim + gap + "img/"+request+".jpg";
				//cout << t << endl;
				send(client_fd, t.c_str(), t.length(), 0);
			}
			closesocket(client_fd);
		}
	}

	/*
    while (cin>>doc_id) {
        string doc = read_raw_doc_byline(doc_id);
        doc_trim.clear();
        int cnt = 0;
        for (int i = 0; i < doc.length(); i++) {
            if (doc[i] == ' ') {
                cnt++;
                if (cnt == 300) {
                    if (i != doc.length() - 1) {
                        doc_trim += "...";
                    }
                    break;
                }
            }
            doc_trim += doc[i];
        }
        doc_trim = "  " + doc_trim;
        cout<<docs[doc_id].title<<endl;
        cout<<doc_trim<<endl;
    }
    */
    return 0;
}

