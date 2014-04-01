#ifndef __test__vectorspace__
#define __test__vectorspace__

#include <iostream>
#include <string>
#include <list>
#include <vector>

using namespace std;

struct doc_info{
    string url;
    unsigned short length;
    unsigned short count_of_diff_term;
    float avg_length;
    string title;
    string date;
    float hot;
	bool img;
};

struct doc_score{
    int docid;
    float score;
};

double log2(double x);
void compute_idf(float *);
void initialize_docinfo(char*);
void update_tf(doc_info* docs);
void update_weight(float*);
vector<int> do_one_search(string, float*, int, int);
vector<int> fast_cosine_score(int*, int, float*, int);
list<int> more_like_this_proc(int*, int, float *, int);
string find_doc_index(int,string);//search for which txt file contains the target document
list<int> more_like_this(int, float*, int);
list<int> extract_query_more_like_this(vector<int> &, int);
bool cmp_morelikethis(int, int);
bool cmp_idf(int, int);
vector<int> read_doc(int doc_id);
list<int> read_doc_byline(int doc_id);
list<string> read_raw_doc_byline(int doc_id);


#endif /* defined(__test__vectorspace__) */
