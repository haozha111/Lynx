#include "boolfunc.h"

using namespace std;

extern inv_node** inv_list;
extern int firstletter_pos[26];
extern doc_info* docs;
extern char * poslist_filepath;
extern map<int, list<inv_pos_item> > inv_list_pos;
extern map<string, token_item> token_list;
extern int SIZE_OF_COLLECTION;

stack<char> opt;
stack<TreeNode*> subexp;
stack<char> bracket;


bool tf_cmp(int w1, int w2){//sort by increasing term frequency
	if (inv_list[w1]->total_tf < inv_list[w2]->total_tf) {
		return true;
	}
	else
		return false;
}

vector<int> intersect(vector<int> post1, vector<int> post2){//interset two posting lists
	vector<int> answer;
	vector<int>::const_iterator iter1 = post1.begin();
	vector<int>::const_iterator iter2 = post2.begin();//set pointers to begin pos
	while (iter1 != post1.end() && iter2 != post2.end()) {
		if (*iter1 == *iter2) {
			answer.push_back(*iter1);
			iter1++;
			iter2++;
		}
		else if (*iter1 < *iter2){
			iter1++;
		}
		else{
			iter2++;
		}
	}
	return answer;
}

vector<int> combine(vector<int> post1, vector<int> post2){//combine two posting lists
	set<int> result;
	vector<int>::const_iterator iter = post1.begin();
	while (iter != post1.end()) {
		result.insert(*iter);
		iter++;
	}

	iter = post2.begin();
	while (iter != post2.end()) {
		result.insert(*iter);
		iter++;
	}
	vector<int> answer;
	set<int>::const_iterator iters = result.begin();
	while (iters != result.end()) {
		answer.push_back(*iters);
		iters++;
	}
	return answer;
}



vector<int> get_post_list(int term_id, bool flag){
	vector<int> post_list;
	map<int, int> tmp;
	list<inv_item>::const_iterator iter = inv_list[term_id]->data.begin();
	while (iter != inv_list[term_id]->data.end()) {
		post_list.push_back(iter->doc_id);
		tmp[iter->doc_id] = 1;
		iter++;
	}
	if (flag) {
		return post_list;
	}
	else{
		vector<int> exclude_list;
		for (int i = 1; i <= SIZE_OF_COLLECTION; i++) {
			if (tmp.count(i) == 0) {
				exclude_list.push_back(i);
			}
		}
		return exclude_list;
	}
}





bool hot_cmp(int a, int b){
	return docs[a].hot > docs[b].hot;
}

bool date_cmp(int a, int b){
	return strcmp(docs[a].date.c_str(), docs[b].date.c_str()) > 0;
}



void fetch_pos_list(int term){
	int start_pos_doc = inv_list[term]->offset / 200000 + 1;
	bool f = true;
	for (int doc = start_pos_doc; f ; doc++) {
		stringstream iss;
		iss<<doc;
		string docname, str;
		iss>>docname;
		docname += ".dat";
		ifstream fin((poslist_filepath + docname).c_str());
		while (getline(fin,str)) {
			istringstream is(str);
			string term_id;
			is>>term_id;
			if (atoi(term_id.c_str()) > term) {
				f = false;
				break;
			}
			if (atoi(term_id.c_str()) == term) {
				string doc_id, pos;
				is>>doc_id;
				list<unsigned short> tpos;
				while (is>>pos) {
					tpos.push_back(atoi(pos.c_str()));
				}
				inv_pos_item tmp;
				tmp.doc_id = atoi(doc_id.c_str());
				tmp.pos_list = tpos;
				inv_list_pos[term].push_back(tmp);
			}
		}
		fin.close();
	}

}

map<int, list<pair<int, int> > > positional_intersect(int p1, int p2, int k){//k-proximity intersection
	pair<int, int > pr;
	map<int, list<pair<int, int> > > answer;
	list<inv_pos_item>::const_iterator iter1 = inv_list_pos[p1].begin();
	list<inv_pos_item>::const_iterator iter2 = inv_list_pos[p2].begin();
	list<unsigned short>::const_iterator pp1,pp2;
	while (iter1 != inv_list_pos[p1].end() && iter2 != inv_list_pos[p2].end()) {
		if (iter1->doc_id == iter2->doc_id) {

			pp1 = iter1->pos_list.begin();
			for (; pp1 != iter1->pos_list.end(); pp1++) {
				for (pp2 = iter2->pos_list.begin(); pp2 != iter2->pos_list.end(); pp2++) {
					if (abs(*pp1 - *pp2) <= k) {
						pr = make_pair(*pp1, *pp2);
						answer[iter1->doc_id].push_back(pr);
					}
					else if (*pp2 > *pp1){
						break;
					}
				}
			}
			iter1++;
			iter2++;
		}
		else if (iter1->doc_id < iter2->doc_id){
			iter1++;
		}
		else {
			iter2++;
		}
	}
	return answer;

}

bool proc_check(int id, vector<pair<int, int> > &pl, int len, vector<map<int, list<pair<int, int> > > > &candidates, int docid){//len = sizeof candidates
	list<pair<int, int> >::const_iterator iter;
	for (iter = candidates[id][docid].begin(); iter != candidates[id][docid].end(); iter++) {
		pl.push_back(*iter);

		if (id == len - 1) {
			bool flag = true;
			for (int k = 1; k < len; k++) {
				if (pl[k].first != pl[k - 1].second) {
					flag = false;
					break;
				}
			}
			if (flag) {
				pl.erase(pl.end() - 1);
				return true;
			}
			else{
				pl.erase(pl.end() - 1);
			}
		}
		else{
			bool flag = proc_check(id + 1, pl, len, candidates, docid);
			if (flag) {
				pl.erase(pl.end() - 1);
				return true;
			}
			else{
				pl.erase(pl.end() - 1);
			}

		}

	}
	return false;
}

vector<int> multipositional_intersect(string query, int margins[], int len, int top_k, int length, bool hot, bool date){//len = sizeof margins
	vector<map<int, list<pair<int, int> > > > candidates;//take positinal_intersect two by two
	vector<int> result;
	list<int> que = token_to_index(query);
	if (que.size() == 0) {
		return result;
	}
	int * queries = new int[que.size()];
	list<int>::const_iterator iter_ = que.begin();
	int size = 0;
	while (iter_ != que.end()) {
		queries[size++] = *iter_;
		fetch_pos_list(*iter_);
		iter_++;
	}

	for (int i = 0; i < len; i++) {
		candidates.push_back(positional_intersect(queries[i], queries[i + 1], margins[i]));
	}
	//take intersection of all candidates
	map<int, list<pair<int, int> > >::const_iterator iter = candidates[0].begin();
	list<int> res;
	while (iter != candidates[0].end()) {
		vector<map<int, list<pair<int, int> > > >::const_iterator iter2 = candidates.begin();
		iter2++;
		bool flag = true;
		while (iter2 != candidates.end()) {
			if (iter2->count(iter->first) == 0) {
				flag = false;
				break;
			}
			iter2++;
		}
		if (flag) {
			res.push_back(iter->first);
		}
		iter++;
	}

	//filter out phrases
	list<int>::const_iterator iter_res = res.begin();
	vector<pair<int, int> > pl;
	vector<int> answer;
	while (iter_res != res.end()) {
		pl.clear();
		bool m = proc_check(0, pl, len, candidates, *iter_res);
		if (m) {
			answer.push_back(*iter_res);
		}
		iter_res++;
	}
	for (int i = top_k - length; top_k - length < answer.size() && i < top_k && i < answer.size(); ++i) {
		result.push_back(answer[i]);
	}

	if (hot) {
		sort(result.begin(), result.end(), hot_cmp);
	}

	if (date) {
		sort(result.begin(), result.end(), date_cmp);
	}

	//release pos_list
	inv_list_pos.clear();

	return result;

}

bool TestBrackets(string exp,int len)
{
	for(int i=0;i<len;i++)
	{
		char ch=exp[i];
		if(ch=='(')
		{
			bracket.push(ch);
		}else
		{
			if(ch==')')
			{
				if(!bracket.empty())
				{
					bracket.pop();
				}else
				{
					return false;
				}
			}
		}
	}
	if(bracket.empty())
	{
		return true;
	}else
	{
		return false;
	}
}

TreeNode* InfixExpressionToBinaryTree(string exp,int len)
{
	if(!TestBrackets(exp,len))
	{
		return NULL;
	}
	string tmp;
	for(int i=0;i<len;i++)
	{
		char ch=exp[i];
		if(ch == '(')
		{
			opt.push(ch);
		}
		else if(ch == '&' || ch == '|')
		{
			if(opt.size()==0)
			{
				opt.push(ch);
			}else
			{
				if(opt.top()=='(')
				{
					opt.push(ch);
				}else
				{
					while((!opt.empty())&&opt.top()!='(')
					{
						char op=opt.top();
						opt.pop();
						TreeNode* p1=subexp.top();
						subexp.pop();
						TreeNode* p2=subexp.top();
						subexp.pop();
						TreeNode* newNode=new TreeNode;
						newNode->key=op;
						newNode->left=p2;
						newNode->right=p1;
						subexp.push(newNode);
					}
					opt.push(ch);
				}
			}
		}
		else if(ch==')')
		{
			while((!opt.empty())&&opt.top()!='(')
			{
				char op=opt.top();
				opt.pop();
				TreeNode* p1=subexp.top();
				subexp.pop();
				TreeNode* p2=subexp.top();
				subexp.pop();
				TreeNode* newNode=new TreeNode;
				newNode->key=op;
				newNode->left=p2;
				newNode->right=p1;
				subexp.push(newNode);
			}
			opt.pop();
		}
		else{
			tmp.clear();
			if (ch == '-') {
				tmp += ch;
				i++;
			}
			while ((exp[i] >=65 && exp[i] <= 122)) {//isalpha
				tmp += exp[i];
				i++;
			}
			i--;

			TreeNode* p=new TreeNode;
			p->key=tmp;
			p->left=NULL;
			p->right=NULL;
			subexp.push(p);

		}

	}
	while(!opt.empty())
	{
		char op=opt.top();
		opt.pop();
		TreeNode* p1=subexp.top();
		subexp.pop();
		TreeNode* p2=subexp.top();
		subexp.pop();
		TreeNode* newNode=new TreeNode;
		newNode->key=op;
		newNode->left=p2;
		newNode->right=p1;
		subexp.push(newNode);
	}
	return subexp.top();
}


void PostVisitor(TreeNode* ROOT, vector<string> & post_string)
{
	if(ROOT->left!=NULL)
	{
		PostVisitor(ROOT->left, post_string);
	}
	if(ROOT->right!=NULL)
	{
		PostVisitor(ROOT->right, post_string);
	}
	post_string.push_back(ROOT->key);
}



vector<int> boolean_retrieval_without_pos(string query, int top_k, int len, bool hot, bool date){
	vector<int> result;

	if (!TestBrackets(query, (int)query.length())) {return result;}

	TreeNode* ROOT=InfixExpressionToBinaryTree(query, (int)query.length());
	vector<string> post_string;


	if(ROOT!=NULL) {PostVisitor(ROOT, post_string);}
    //cout<<post_string.size()<<endl;

	while(!subexp.empty())
	{
		subexp.pop();
	}

	stack<vector<int> > opnd;
	vector<string>::const_iterator iter = post_string.begin();
	vector<int> set1, set2;
	while (iter != post_string.end()) {
		if (*iter != "&" && *iter != "|") {
			if ((*iter)[0] == '-'){
				string s = (*iter).substr(1,(*iter).length() - 1);
				//cout<<s<<endl;
				opnd.push(get_post_list(token_list[s].index, false));
			}
			else{
				opnd.push(get_post_list(token_list[*iter].index, true));
			}
		}
		if (*iter == "&") {
			vector<int> a = opnd.top();
			opnd.pop();
			vector<int> b = opnd.top();
			opnd.pop();
			vector<int> c = intersect(a, b);
			opnd.push(c);
		}
		if (*iter == "|") {
			vector<int> a = opnd.top();
			opnd.pop();
			vector<int> b = opnd.top();
			opnd.pop();
			vector<int> c = combine(a, b);
			opnd.push(c);

		}
		iter++;
	}

	vector<int> all = opnd.top();
	for (int i = top_k - len; top_k - len < all.size() && i < top_k && i < all.size(); ++i) {
		result.push_back(all[i]);
		//cout<<all[i]<<endl;
	}

	if (hot) {
		sort(result.begin(), result.end(), hot_cmp);
	}

	if (date) {
		sort(result.begin(), result.end(), date_cmp);
	}



	cout<<"done"<<endl;
	return result;
}





