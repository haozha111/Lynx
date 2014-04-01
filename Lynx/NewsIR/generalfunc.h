#ifndef __test__generalfunc__
#define __test__generalfunc__

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include <list>

using namespace std;
struct edit_dist{
    string str;
    unsigned short dist;
    int freq;
};

struct token_item{
    int total_count;//total occurence of a token
    int index;//index of corresponding term
};

struct inv_item{
    int doc_id;
    unsigned short doc_tf;
    float britf;
    float blrtf;
};

struct inv_node{
	int total_tf;
	int df;//document frequency of term
	list<inv_item> data;
	int offset;
};

struct inv_pos_item{
	int doc_id;
	list<unsigned short> pos_list;
};


int edit_distance(const char*, const char*, int, int);
string edit_correct(string);
void initialize_token(char*);
bool cmp_edit_dist(edit_dist, edit_dist);
bool corret_query(string query, string & correct);
list<int> token_to_index(string);
void initialize_invlist(char*, char*);


#endif /* defined(__test__generalfunc__) */
