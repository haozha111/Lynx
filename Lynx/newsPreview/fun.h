//
//  fun.h
//  LynxPreview
//
//  Created by 张 昊亮 on 13-12-22.
//  Copyright (c) 2013年 张 昊亮. All rights reserved.
//

#ifndef __LynxPreview__fun__
#define __LynxPreview__fun__

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

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

void initialize_docinfo(const char * doc_info);
string read_raw_doc_byline(int doc_id, const char* doc_raw_filepath);


#endif /* defined(__LynxPreview__fun__) */
