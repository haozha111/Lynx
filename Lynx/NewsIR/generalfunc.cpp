#include <iostream>
#include <sstream>
#include <ctype.h>
#include "generalfunc.h"

extern map<string, token_item> token_list;
extern inv_node** inv_list;
extern map<string, int> mp;
extern map<string, token_item>::const_iterator token_list_iter[27];


int edit_distance(const char* w1,const char* w2, int s1, int s2){
    int matrix[20][20] = {0};
    
    for (int i = 0; i <= s1; i++) {
        matrix[i][0] = i;
    }
    for (int j = 0; j <= s2; j++) {
        matrix[0][j] = j;
    }
    for (int i = 1; i <= s1; i++) {
        for (int j = 1; j <= s2; j++) {
            if (w1[i - 1] == w2[j - 1]) {
                int tmp[3];
                tmp[0] = matrix[i - 1][j] + 1;
                tmp[1] = matrix[i][j - 1] + 1;
                tmp[2] = matrix[i - 1][j - 1];
                
                sort(tmp, tmp + 3);
                
                matrix[i][j] = tmp[0];
            }
            else{
                int tmp[3];
                tmp[0] = matrix[i - 1][j] + 1;
                tmp[1] = matrix[i][j - 1] + 1;
                tmp[2] = matrix[i - 1][j - 1] + 1;
                
                sort(tmp, tmp + 3);
                matrix[i][j] = tmp[0];
            }
        }
    }
    return matrix[s1][s2];
    
}

bool cmp_edit_dist(edit_dist a, edit_dist b){
    if (a.dist < b.dist) {
        return true;
    }
    else if (a.dist == b.dist && a.freq > b.freq){
        return true;
    }
    else{
        return false;
    }
    
}


string edit_correct(string word_to_be_checked){
    vector<edit_dist> p;
    edit_dist tmp;
    map<string, token_item>::const_iterator iter1 = token_list_iter[word_to_be_checked[0] - 'a'];
    map<string, token_item>::const_iterator iter2 = token_list_iter[word_to_be_checked[0] - 'a' + 1];

    while (iter1 != iter2) {
        tmp.str = iter1->first;
        tmp.dist = edit_distance(word_to_be_checked.c_str(), iter1->first.c_str(), word_to_be_checked.length(), iter1->first.length());
        tmp.freq = iter1->second.total_count;
        p.push_back(tmp);
        iter1++;
    }
    
    sort(p.begin(), p.end(), cmp_edit_dist);
    string result = p[0].str == word_to_be_checked ? p[1].str : p[0].str;
    return result;
}


void initialize_token(char * token_vocab_file){
    ifstream fin(token_vocab_file);
    string str,token,cnt,idx;
    while (getline(fin,str)) {
        istringstream iss(str);
        iss>>token>>cnt>>idx;
        if (token_list.count(token) == 0) {
            token_item tmp;
            tmp.total_count = atoi(cnt.c_str());
            tmp.index = atoi(idx.c_str());
            token_list[token] = tmp;
        }
        
    }
    
    map<string, token_item>::const_iterator iter = token_list.begin();
    int k = 0;
    while (iter != token_list.end()) {
        if (iter->first.length() == 1 && isalpha(iter->first[0])) {
            token_list_iter[k++] = iter;
        }
        iter++;
    }
    token_list_iter[26] = token_list.end();
    fin.close();
    
}

list<int> token_to_index(string query){
    list<int> result;
    istringstream iss(query);
    string s;
    while (iss>>s) {
        if (token_list.count(s) != 0 &&  inv_list[token_list[s].index] != NULL) {
            result.push_back(token_list[s].index);
        }
    }
    return result;
    
}


bool corret_query(string query, string & correct){
    istringstream iss(query);
    string s;
    bool f = true;
    while (iss>>s) {
        if (token_list.count(s) != 0 &&  inv_list[token_list[s].index] != NULL) {
            correct += s + ' ';
        }
        else{
            f = false;
            correct += edit_correct(s) + ' ';
        }
    }
    return !f;
}



void initialize_invlist(char* vocab_file, char* invlist_file){
	ifstream in(vocab_file);
	string str, word, cnt ,tmp, docid;
	int index = 1;
	while (getline(in,str)) {
		//cout << str << endl;
		istringstream iss(str);
		iss>>word>>cnt;
		if (atoi(cnt.c_str()) > 200) {
			mp[word] = index++;
		}
		else{
			index++;
		}

	}
	in.close();

	//initialization
	ifstream fin(invlist_file);
	index = 0;
	inv_item inv_tmp;
	int df = 0, offset = 0;
	bool flag = true;
	while (getline(fin,str)) {
		if (str[0] == 'I'){
			index++;
			inv_list[index] = NULL;
			if (df) {
				inv_list[index - 1]->df = df;
				df = 0;
			}
			istringstream iss(str);
			iss>>tmp>>word>>cnt;
			if (atoi(cnt.c_str()) < 200) {
				flag = false;
				continue;

			}
			else{
				flag = true;
				inv_list[index] = new inv_node;
				inv_list[index]->total_tf = atoi(cnt.c_str());
				inv_list[index]->offset = offset;//offset counts from 0
			}

		}
		else{
			offset++;
			if (flag == false) {
				continue;
			}
			df++;
			istringstream iss(str);
			iss>>docid>>cnt;
			inv_tmp.doc_id = atoi(docid.c_str());
			inv_tmp.doc_tf = atoi(cnt.c_str());
			inv_list[index]->data.push_back(inv_tmp);
		}
	}
	fin.close();
	return;
}



