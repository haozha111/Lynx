//  Copyright 2013 Google Inc. All Rights Reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include "word2vec.h"
#include "generalfunc.h"


const long long max_size = 2000;         // max length of strings
const long long N = 30;                  // number of closest words that will be shown
const long long max_w = 50;              // max length of vocabulary entries

char bestw[N][max_size];
char file_name[max_size], st[100][max_size];
float dist, len, bestd[N], vec[max_size];
long long words, size, a, b, c, d, cn, bi[100];
char ch;
float *M;
char *vocab;
extern map<string, token_item> token_list;


int initialize_word2vec(char* fname){
    FILE *f;
    strcpy(file_name, fname);
    f = fopen(file_name, "rb");
    
    fscanf(f, "%lld", &words);
    fscanf(f, "%lld", &size);
    vocab = (char *)malloc((long long)words * max_w * sizeof(char));
    M = (float *)malloc((long long)words * (long long)size * sizeof(float));
    if (M == NULL) {
        printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words * size * sizeof(float) / 1048576, words, size);
        return -1;
    }
    for (b = 0; b < words; b++) {
        fscanf(f, "%s%c", &vocab[b * max_w], &ch);
        for (a = 0; a < size; a++) fread(&M[a + b * size], sizeof(float), 1, f);
        len = 0;
        for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
        len = sqrt(len);
        for (a = 0; a < size; a++) M[a + b * size] /= len;
    }
    fclose(f);
    for (a = 0; a < N; a++) bestd[a] = 0;
    for (a = 0; a < N; a++) bestw[a][0] = 0;
	return 1;
}

bool cmp(string a , string b){
    return token_list[a].total_count > token_list[b].total_count;
}

list<string> similar_words(const char* st1) {
    cn = 0;
    b = 0;
    c = 0;
    list<string> res;
    while (1) {
        st[cn][b] = st1[c];
        b++;
        c++;
        st[cn][b] = 0;
        if (st1[c] == 0) break;
        if (st1[c] == ' ') {
            cn++;
            b = 0;
            c++;
        }
    }
    cn++;
    for (a = 0; a < cn; a++) {
        for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st[a])) break;
        if (b == words) b = -1;
        bi[a] = b;
        if (b == -1) {
            return res;
        }
    }
    for (a = 0; a < size; a++) vec[a] = 0;
    for (b = 0; b < cn; b++) {
        if (bi[b] == -1) continue;
        for (a = 0; a < size; a++) vec[a] += M[a + bi[b] * size];
    }
    len = 0;
    for (a = 0; a < size; a++) len += vec[a] * vec[a];
    len = sqrt(len);
    for (a = 0; a < size; a++) vec[a] /= len;
    for (a = 0; a < N; a++) bestd[a] = 0;
    for (a = 0; a < N; a++) bestw[a][0] = 0;
    for (c = 0; c < words; c++) {
        a = 0;
        for (b = 0; b < cn; b++) if (bi[b] == c) a = 1;
        if (a == 1) continue;
        dist = 0;
        for (a = 0; a < size; a++) dist += vec[a] * M[a + c * size];
        for (a = 0; a < N; a++) {
            if (dist > bestd[a]) {
                for (d = N - 1; d > a; d--) {
                    bestd[d] = bestd[d - 1];
                    strcpy(bestw[d], bestw[d - 1]);
                }
                bestd[a] = dist;
                strcpy(bestw[a], &vocab[c * max_w]);
                break;
            }
        }
    }
    
    vector<string> tmp;
    for (a = 0; a < N; a++) {
        tmp.push_back(bestw[a]);
    }
    sort(tmp.begin(), tmp.end(), cmp);
    for (a = 0; a < 6; a++){
        res.push_back(tmp[a]);
    }
    
    return res;
}
