//
//  word2vec.h
//  test
//
//  Created by haoliang on 13-12-7.
//  Copyright (c) 2013年 haoliang. All rights reserved.
//

#ifndef __test__word2vec__
#define __test__word2vec__
#include <list>
using namespace std;
int initialize_word2vec(char* fname);
list<string> similar_words(const char* st1);

#endif /* defined(__test__word2vec__) */
