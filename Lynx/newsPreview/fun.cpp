//
//  fun.cpp
//  LynxPreview
//
//  Created by 张 昊亮 on 13-12-22.
//  Copyright (c) 2013年 张 昊亮. All rights reserved.
//

#include "fun.h"

//extern char * doc_raw_filepath;
extern char * doc_info_filepath;
extern doc_info* docs;

string find_doc_index(int doc_id, string doc_path){//search for which txt file contains the target document
    stringstream ss;
    ss<< (doc_id - 1) / 1000 + 1;
    string result;
    ss>>result;
    result = doc_path + result + ".txt";
    return result;
}

string read_raw_doc_byline(int doc_id,const char* doc_raw_filepath){
    string doc_byline;
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
                doc_byline += str.substr(0, str.length() - 1)+ ". ";
            }
        }
    }
    fin.close();
    return doc_byline;
}


void initialize_docinfo(const char * doc_info){
    ifstream fin(doc_info);
    string str, docid, doclength, cnt_terms, title, date, hot, img;
    size_t pos1,pos2,pos3,pos4,pos5,pos6,pos7;
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
        istringstream iss(hot);
        iss>>fhot;
        docs[atoi(docid.c_str())].length = atoi(doclength.c_str());
        docs[atoi(docid.c_str())].count_of_diff_term = atoi(cnt_terms.c_str());
        docs[atoi(docid.c_str())].avg_length = (float)docs[atoi(docid.c_str())].length / (float)docs[atoi(docid.c_str())].count_of_diff_term;
        docs[atoi(docid.c_str())].title = title;
        docs[atoi(docid.c_str())].date = date;
        docs[atoi(docid.c_str())].hot = fhot;
        docs[atoi(docid.c_str())].img = img[0] == '1' ? true : false;
    }
    fin.close();
}
