#include "vectorspace.h"
#include "generalfunc.h"
#include "boolfunc.h"
#include <math.h>
#include <queue>
#include <sstream>
#include <set>

extern int SIZE_OF_VOCAB;
extern int SIZE_OF_COLLECTION;
extern float AVERAGE_DOCUMENT_LENGTH;
extern unsigned short NUM_OF_WEIGHT;
extern inv_node** inv_list;
extern doc_info* docs;
extern map<string, int> mp;
extern float* weight;
extern char * doc_binary_filepath;
extern char * doc_raw_filepath;
extern float* idf;
map<int, int> tmap;

double log2(double x){
	return log(x)/log(2.0);
}

void compute_idf(float * idf){
    float aef;
    for (int i = 1; i < SIZE_OF_VOCAB; i++) {
        if (!inv_list[i]) {
            continue;
        }
        aef = (float)inv_list[i]->total_tf / (float)inv_list[i]->df;
        idf[i] = log10((float)SIZE_OF_COLLECTION/(float)inv_list[i]->df) * aef / (1 + aef);
    }
}

void initialize_docinfo(char * doc_info){
	ifstream fin(doc_info);
	string str, docid, doclength, cnt_terms, title, date, hot, img,url;
	size_t pos1,pos2,pos3,pos4,pos5,pos6,pos7,pos8;
	float fhot;
	while (getline(fin,str)) {
		pos1 = str.find('*');
		docid = str.substr(0,pos1);
		pos2 = str.find('*', pos1 + 1);
		doclength = str.substr(pos1 + 1, pos2 - pos1 - 1);
		pos3 = str.find('*', pos2 + 1);
		cnt_terms = str.substr(pos2 + 1, pos3 - pos2 - 1);
		pos4 = str.find('*', pos3 + 1);
		title = str.substr(pos3 + 1, pos4 - pos3 - 1);
		pos5 = str.find('*',pos4 + 1);
		date = str.substr(pos4 + 1, pos5 - pos4 - 1);
		pos6 = str.find('*',pos5 + 1);
		hot = str.substr(pos5 + 1, pos6 - pos5 - 1);
		pos7 = str.find('*',pos6 + 1);
		img = str.substr(pos6 + 1, pos7 - pos6 - 1);
		pos8 = str.find('*',pos7 + 1);
		url = str.substr(pos7 + 1, pos8 - pos7 - 1);
		istringstream iss(hot);
		iss>>fhot;
		docs[atoi(docid.c_str())].length = atoi(doclength.c_str());
		docs[atoi(docid.c_str())].count_of_diff_term = atoi(cnt_terms.c_str());
		docs[atoi(docid.c_str())].avg_length = (float)docs[atoi(docid.c_str())].length / (float)docs[atoi(docid.c_str())].count_of_diff_term;
		docs[atoi(docid.c_str())].title = title;
		docs[atoi(docid.c_str())].date = date;
		docs[atoi(docid.c_str())].hot = fhot;
		docs[atoi(docid.c_str())].img = img[0] == '1' ? true : false;
		docs[atoi(docid.c_str())].url = url;
	}
	fin.close();
}


void update_tf(doc_info* docs){
	float ritf,lrtf;
	for (int i = 1; i < SIZE_OF_VOCAB; i++) {
		if (!inv_list[i]) {
			continue;
		}
		list<inv_item>::iterator iter = inv_list[i]->data.begin();
		while (iter != inv_list[i]->data.end()) {
			ritf = log2(1 + (float)iter->doc_tf) / log2(1 + (float)docs[iter->doc_id].avg_length);
			lrtf = (float)iter->doc_tf * log2(1 + AVERAGE_DOCUMENT_LENGTH / docs[iter->doc_id].length);
			iter->britf = ritf / (1 + ritf);
			iter->blrtf = lrtf / (1 + lrtf);
			iter++;
		}
	}
}

void update_weight(float* weight){
    for (int i = 1; i <= NUM_OF_WEIGHT; i++) {
        weight[i] = 2.0 / (1 + log2(1.0 + (float)i));
    }
}

bool operator<(doc_score a, doc_score b){
    return a.score < b.score;
}



vector<int> fast_cosine_score(int* query , int query_len , float * idf, int top_k){
    priority_queue<doc_score , vector<doc_score> > score;
    map<int, float> accumulated_score;
    doc_score tmp;
    
    for (int i = 0; i < query_len; i++) {
        //wtq is set to be binary
        list<inv_item>::const_iterator iter = inv_list[query[i]]->data.begin();
        while (iter != inv_list[query[i]]->data.end()) {
            if (accumulated_score.count(iter->doc_id) == 0) {
                accumulated_score[iter->doc_id] = (weight[query_len] * iter->britf + ( 1- weight[query_len]) * iter->blrtf) * idf[query[i]];
            }
            else{
                accumulated_score[iter->doc_id] += (weight[query_len] * iter->britf + ( 1- weight[query_len]) * iter->blrtf) * idf[query[i]];
            }
            iter++;
        }
    }
    int result_size = top_k < accumulated_score.size() ? top_k : accumulated_score.size();
    map<int, float>::const_iterator iters = accumulated_score.begin();
    while (iters != accumulated_score.end()) {
        tmp.docid = iters->first;
        tmp.score = iters->second;
        score.push(tmp);
        iters++;
    }
    
    vector<int> result;
    
    while (result_size--) {
        result.push_back(score.top().docid);
        score.pop();
    }
    
    return result;
}


vector<int> do_one_search(string query_string, float* idf, int top_k, int length){
    list<int> q = token_to_index(query_string);
    vector<int> res;

    if (q.size() == 0)  {
        return res;
    }
    list<int>::const_iterator iter = q.begin();
    int *query = new int[q.size()];
    int k = 0;
    while (iter != q.end()) {
        query[k++] = *iter;
        iter++;
    }
    vector<int> result = fast_cosine_score(query, q.size(), idf, top_k);
    for (int i = top_k - length; top_k - length < result.size() && i < top_k && i < result.size(); ++i) {
        res.push_back(result[i]);
    }
    return res;
}


list<int> more_like_this_proc(int* query , int query_len , float * idf, int top_k){
    priority_queue<doc_score , vector<doc_score> > score;
    map<int, float> accumulated_score;
    doc_score tmp;
    
    float weight_doc = 2.0 / (1 + log2(1.0 + (float)query_len));

    for (int i = 0; i < query_len; i++) {
        //wtq is set to be binary
        list<inv_item>::const_iterator iter = inv_list[query[i]]->data.begin();
        while (iter != inv_list[query[i]]->data.end()) {
            if (accumulated_score.count(iter->doc_id) == 0) {
                accumulated_score[iter->doc_id] = (weight_doc * iter->britf + ( 1- weight_doc) * iter->blrtf) * idf[query[i]];
            }
            else{
                accumulated_score[iter->doc_id] += (weight_doc * iter->britf + ( 1- weight_doc) * iter->blrtf) * idf[query[i]];
            }
            iter++;
        }
    }
    int result_size = top_k < accumulated_score.size() ? top_k : accumulated_score.size();
    map<int, float>::const_iterator iters = accumulated_score.begin();
    while (iters != accumulated_score.end()) {
        tmp.docid = iters->first;
        tmp.score = iters->second;
        score.push(tmp);
        iters++;
    }
    
    list<int> result;
    
    while (result_size--) {
        result.push_back(score.top().docid);
        score.pop();
    }
    
    return result;
}

string find_doc_index(int doc_id, string doc_path){//search for which txt file contains the target document
    stringstream ss;
    ss<< (doc_id - 1) / 1000 + 1;
    string result;
    ss>>result;
    result = doc_path + result + ".txt";
    return result;
}

bool cmp_morelikethis(int a, int b){
    return tmap[a] > tmap[b];
}

bool cmp_idf(int a, int b){
    return idf[a] > idf[b];
}

list<int> extract_query_more_like_this(vector<int> & doc, int threshold){
    vector<int>::const_iterator iter = doc.begin();
    
    tmap.clear();
    while (iter != doc.end()) {
        if (tmap.count(*iter) == 0) {
            tmap[*iter] = 1;
        }
        else{
            tmap[*iter] += 1;
        }
        iter++;
    }
    
    list<int> tmp;
    iter = doc.begin();
    while (iter != doc.end()) {
        tmp.push_back(*iter);
        iter++;
    }
    tmp.unique();
    doc.clear();
    
    list<int>::const_iterator iterl = tmp.begin();
    while (iterl != tmp.end()) {
        doc.push_back(*iterl);
        iterl++;
    }
    sort(doc.begin(),doc.end(), cmp_morelikethis);//sort the doc term as decreasing tf order
    
    
    iter = doc.begin();
    int cnt = 10;//choose 10 significant tf largest term
    set<int> qterms;
    
    while (iter != doc.end()) {
        if (inv_list[*iter]->total_tf <= threshold) {
            if (cnt--){
                qterms.insert(*iter);
            }
            else{
                break;
            }
        }
        iter++;
    }
    
    sort(doc.begin(), doc.end(), cmp_idf);//sort the doc term as decreasing idf order
    
    cnt = 10;//choose 10 significant idf largest term
    
    iter = doc.begin();
    while (iter != doc.end()) {
        if (inv_list[*iter]->total_tf <= threshold) {
            if (qterms.count(*iter) == 0){
                if (cnt--) {
                    qterms.insert(*iter);
                }
                else{
                    break;
                }
                
            }
        }
        iter++;
    }
    
    set<int>::const_iterator iters = qterms.begin();
    list<int> query;

    while (iters != qterms.end() ) {
        query.push_back(*iters);
        iters++;
    }
    
    return query;
}

list<int> read_doc_byline(int doc_id){
    list<int> doc_byline;
    string fname = find_doc_index(doc_id,  doc_binary_filepath);
    stringstream ss;
    ss<<doc_id;
    string target_id;
    ss>>target_id;
    ifstream fin(fname.c_str());
    string str;
    bool start = false;
    
    while (getline(fin, str)) {
        //str.pop_back();
        if (str[0] == 'D') {
            size_t pos = str.find(' ');
            string id = str.substr(pos + 1, str.length() - pos - 1);
            if (start) {
                start = false;
                break;
            }
            if (id == target_id) {
                doc_byline.clear();
                start = true;
                continue;
            }
        }
        else{
            if (start) {
                istringstream iss(str);
                string substr;
                doc_byline.push_back(0);//manifest the beginning of a new line
                while (iss >> substr) {
                    doc_byline.push_back(atoi(substr.c_str()));
                }
            }
        }
    }
    fin.close();
    return doc_byline;
}

list<string> read_raw_doc_byline(int doc_id){
    list<string> doc_byline;
    string fname = find_doc_index(doc_id, doc_raw_filepath);
    stringstream ss;
    ss<<doc_id;
    string target_id;
    ss>>target_id;
    ifstream fin(fname.c_str());
    string str;
    bool start = false;
    
    while (getline(fin, str)) {
        //str.pop_back();
        size_t pos = str.find(' ');
        if (str.substr(0,pos) == "DOCID:") {
            string id = str.substr(pos + 1, str.length() - pos - 1);
            if (start) {
                start = false;
                break;
            }
            if (id == target_id) {
                doc_byline.clear();
                start = true;
                continue;
            }
        }
        else{
            if (start) {
                doc_byline.push_back(str);
            }
        }
    }
    fin.close();
    return doc_byline;
}


vector<int> read_doc(int doc_id){//used for computing more like this page
    string fname = find_doc_index(doc_id,doc_binary_filepath);
    stringstream ss;
    ss<<doc_id;
    string target_id;
    ss>>target_id;
    ifstream fin(fname.c_str());
    string str;
    vector<int> doc;
    bool start = false;
    
    while (getline(fin, str)) {
        //str.pop_back();
        if (str[0] == 'D') {
            size_t pos = str.find(' ');
            string id = str.substr(pos + 1, str.length() - pos - 1);
            if (start) {
                start = false;
                break;
            }
            if (id == target_id) {
                doc.clear();
                start = true;
                continue;
            }
        }
        else{
            if (start) {
                istringstream iss(str);
                string substr;
                while (iss >> substr) {
                    doc.push_back(atoi(substr.c_str()));
                }
            }
        }
    }
    fin.close();
    return doc;
}

list<int> more_like_this(int doc_id, float* idf, int top_k){
    vector<int> doc = read_doc(doc_id);
    list<int> query1 = extract_query_more_like_this(doc, 365767);
    int * query = new int[query1.size()];
    list<int>::const_iterator iter = query1.begin();
    for (int i = 0; i < query1.size(); i++) {
        query[i] = *iter++;
    }
    
    list<int> result = more_like_this_proc(query, query1.size(), idf, top_k);
    return result;
    
    
}
