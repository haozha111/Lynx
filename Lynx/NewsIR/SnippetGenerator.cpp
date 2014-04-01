#include "SnippetGenerator.h"
#include "vectorspace.h"
#include "generalfunc.h"
#include <map>
#include <queue>
#include <sstream>
#include <algorithm>
extern char * doc_binary_filepath;
extern map<string, int> mp;


float score_one_sentence(list<int> query, int sentence_id, list<int> sentence){
    //return a weighted score of the given sentence. apply the methods rendered by Turpin et al.
    float headingline_score = 0;
    if (sentence_id == 1 ) {
        headingline_score = 1.5;
    }
    else if (sentence_id == 2) {
        headingline_score = 1;
    }
    
    float c = 0;
    query.unique();
    int * max_sequence = new int[sentence.size()];//-1 denotes not query term, 1 otherwise
    int size = 0;
    list<int>::const_iterator iter = sentence.begin();
    while (iter != sentence.end()) {
        bool f = false;
        list<int>::const_iterator iters = query.begin();
        while (iters != query.end()) {
            if (*iter == *iters) {
                f = true;
                break;
            }
            iters++;
        }
        if (f) {
            max_sequence[size++] = 1;
            c++;
        }
        else{
            max_sequence[size++] = -1;
        }
        iter++;
    }
    
    float d = 0;
    list<int>::const_iterator iters = query.begin();
    while (iters != query.end()) {
        bool f = false;
        list<int>::const_iterator iter = sentence.begin();
        while (iter != sentence.end()) {
            if (*iter == *iters) {
                f = true;
                break;
            }
            iter++;
        }
        if (f) {
            d++;
        }
        iters++;
    }
    
    //identify longest contiguous run of query terms in sentence
    
    
    
    int* b = new int[sentence.size()];
    b[0] = max_sequence[0];
    float max_sum = b[0];
    for (int i = 1;  i < sentence.size(); i++) {
        b[i] = b[i-1] > 0 ? b[i-1] + max_sequence[i] : max_sequence[i];
        if (b[i] > max_sum) {
            max_sum = b[i];
        }
    }
    
    float final_score = 0.2 * headingline_score + 0.4 * c + 0.2 * d + 0.2 * max_sum;
    return final_score;
}

bool operator < (sentence_score a, sentence_score b){
    return a.score < b.score;
}


string post_process(vector<string> snippet, list<int> query){
    string snippet_modified;
    vector<string> tmp[2];
    int cnt[2] = {0,0};
    int k = 16;
    
    if (snippet.size() == 1) {
        istringstream iss(snippet[0]);
        string s;
        while (iss>>s) {
            tmp[0].push_back(s);
        }
        vector<string>::iterator iter = tmp[0].begin();
        while (iter != tmp[0].end() && k--) {
            snippet_modified += *iter + ' ';
            iter++;
        }
        if (iter != tmp[0].end()) {
            snippet_modified += " ...";
        }
        return snippet_modified;
        
    }
    
    for (int i = 0; i < 2; i++) {
        istringstream iss(snippet[i]);
        string s;
        while (iss>>s) {
            cnt[i]++;
            tmp[i].push_back(s);
        }
    }
    
    if (cnt[0] + cnt[1] <= 26) {
        return snippet[0] + ' ' + snippet[1];
    }
    
    vector<string>::iterator iter = tmp[0].begin();
    while (iter != tmp[0].end() && k--) {
        snippet_modified += *iter + ' ';
        iter++;
    }
    if (iter != tmp[0].end()) {
        snippet_modified += " ...";
    }
    
    int* span = new int[tmp[1].size()];
    memset(span, 0, sizeof(span));
    for (int i = 0; i < tmp[1].size(); i++)
    {
        string stmp = tmp[1][i];
        transform(stmp.begin(), stmp.end(), stmp.begin(), ::tolower);
        list<int>::const_iterator it = query.begin();
        while (it != query.end()) {
            if (*it == mp[stmp]) {//the occurence of query word
                span[i] = 1;
                break;
            }
            it++;
        }
    }

    map<int, int> mtmp;
    int count = 0;
    for (int i = 0; i < tmp[1].size(); i++) {
        int j = i + 11;
        if (j > tmp[1].size() - 1) {
            j = tmp[1].size() - 1;
        }
        count = 0;
        for (int k = i; k <= j; k++) {
            if (span[k] == 1) {
                count++;
            }
        }
        mtmp[i] = count;
    }
    
    map<int, int>::const_iterator itermap = mtmp.begin();
    int maxi = itermap->first, maxcount = itermap->second;
    while (itermap != mtmp.end()) {
        if (itermap->second > maxcount) {
            maxcount = itermap->second;
            maxi = itermap->first;
        }
        itermap++;
    }
    
    k = 12;
    for (int i = maxi; i < tmp[1].size() && k; k--, i++) {
        snippet_modified += tmp[1][i] + ' ';
    }
    
    snippet_modified.erase(snippet_modified.length() - 1, 1);
    snippet_modified += " ...";
    return snippet_modified;
    
}

string snippet_generator(string query_1,int docid){
    list<int> query = token_to_index(query_1);
    if (query.size() == 0) {
        return NULL;
    }
    list<int> doc_line = read_doc_byline(docid);
    priority_queue<sentence_score, vector<sentence_score> > score;
    list<int>::const_iterator iter = doc_line.begin();
    int sentence_id  = 0;
    list<int> sentence;
    
    while (iter != doc_line.end()) {
        if (*iter == 0) {
            if (sentence.size() > 0) {
                sentence_score tmp;
                tmp.sentence_id = sentence_id;
                tmp.score = score_one_sentence(query, sentence_id, sentence);
                score.push(tmp);
            }
            sentence_id++;
            sentence.clear();
        }
        else{
            sentence.push_back(*iter);
        }
        iter++;
    }
    if (sentence.size() > 0) {
        sentence_score tmp;
        tmp.sentence_id = sentence_id;
        tmp.score = score_one_sentence(query, sentence_id, sentence);
        score.push(tmp);
    }
    
    list<string> raw_doc_line = read_raw_doc_byline(docid);
    
    //in case if the # lines in docbinary and docraw do not match
    if (raw_doc_line.size() != sentence_id) {
        string s = raw_doc_line.front();
        istringstream iss(s);
        s.clear();
        int cnt = 26;
        string snippet;
        while (iss>>s && cnt--) {
            snippet += s + ' ';
        }
        snippet += ' ...';
        return snippet;
    }
    
    vector<string> tmp;
    list<string>::const_iterator iters = raw_doc_line.begin();
    while (iters != raw_doc_line.end()) {
        tmp.push_back(*iters);
        iters++;
    }
    
    
    int cnt = 2;//return #cnt highest score sentences
    vector<string> snippet;
    while (score.size() > 0 && cnt--) {
        sentence_id = score.top().sentence_id;
        score.pop();
        string str = tmp[sentence_id - 1];
        snippet.push_back(str.erase(str.length() - 1, 1));
    }
        
    string snippet_modified = post_process(snippet, query);
    return snippet_modified;
    
}
