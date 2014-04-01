#ifndef __test__boolfunc__
#define __test__boolfunc__

#include <map>
#include <vector>
#include <stack>
#include <list>
#include <math.h>
#include <utility>
#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include "vectorspace.h"
#include "generalfunc.h"

using namespace std;

struct TreeNode
{
	string key;
	TreeNode* left;
	TreeNode* right;
};



bool tf_cmp(int, int);
vector<int> intersect(vector<int>, vector<int>);
vector<int> boolean_retrieval_without_pos(string query, int top_k, int len, bool hot, bool date);
map<int, list<pair<int, int> > > positional_intersect(int, int, int);
bool proc_check(int, vector<pair<int, int> > &, int, vector<map<int, list<pair<int, int> > > > &, int);
vector<int> multipositional_intersect(string query, int margins[], int len, int top_k, int length, bool hot, bool date);

#endif /* defined(__test__boolfunc__) */
