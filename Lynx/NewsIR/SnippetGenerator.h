#ifndef __test__SnippetGenerator__
#define __test__SnippetGenerator__

#include <iostream>
#include <string>
#include <list>
#include <vector>

using namespace std;

struct sentence_score{
    int sentence_id;
    float score;
};

string snippet_generator(string query,int docid);
float score_one_sentence(list<int> query, int sentence_id, list<int> sentence);
string post_process(vector<string> snippet, list<int> query);

#endif /* defined(__test__SnippetGenerator__) */
