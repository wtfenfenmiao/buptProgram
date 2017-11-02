#include "simpleTrie.h"


using namespace std;
long int pos;
long int cotline;
string strTemp;
char strTemp2;
simpleTrieNode* tempFind;
unordered_map<string, simpleTrieNode*>::iterator iterFind;
//树结点构造函数
simpleTrieNode::simpleTrieNode(int number) {
	this->fail = NULL;
	No = number;
}

//树构造函数
simpleTrie::simpleTrie() {    //要给根节点分配内存
	this->root = new simpleTrieNode(0);
	this->count = 0;
}




//插入pattern
void simpleTrie::Insert(string & str1) {
	string str = str1 + '\0';
	simpleTrieNode* tempOut = root;
	char* i = &str[0];
	unordered_map<string, simpleTrieNode*>::iterator iter;	
	strTemp = *i;
	if (*i & 0x80)    //中文字符
	{
		strTemp += *(++i);
	}
	iter = tempOut->g.find(strTemp);
	while (*i != '\0' && iter != tempOut->g.end())        //找到这个pattern在树里开始找不到的地方
	{
		++i;
		tempOut = iter->second;
		strTemp = *i;
		if (*i & 0x80)    //中文字符
		{
			strTemp += *(++i);
		}
		iter = tempOut->g.find(strTemp);
	}
	while (*i != '\0')                                     //把这个pattern后面找不到的地方插到树里
	{
		simpleTrieNode* tempin = new simpleTrieNode(++count);
		tempOut->g.insert(make_pair(strTemp, tempin));
		tempOut = tempin;
		++i;
		strTemp = *i;
		if (*i & 0x80)    //中文
		{
			++i;
			strTemp += *i;		
		}
	}
	tempOut->output.push_back(str1);
}

//遍历构造失效指针
void simpleTrie::ergodicFail() {      //用队列来弄，因为算是一个层序遍历
	queue<simpleTrieNode*> temp;
	unordered_map<string, simpleTrieNode*>::iterator iter;
	root->fail = root;
	for (iter = root->g.begin(); iter != root->g.end(); ++iter)    //根节点下面的第一层，失效指针就是根节点
	{
		iter->second->fail = root;
		temp.push(iter->second);
	}
	while (!temp.empty())         //逐层遍历。这个构造fail的过程就是老师讲的简单树的fail指针构造过程
	{
		bool flag = false;
		simpleTrieNode* tempin = temp.front();
		temp.pop();
		for (iter = tempin->g.begin(); iter != tempin->g.end(); ++iter)
		{
			simpleTrieNode* state = tempin->fail;
			while (state != root)
			{
				unordered_map<string, simpleTrieNode*>::iterator iterTemp = state->g.find(iter->first);
				if (iterTemp == state->g.end())
				{
					state = state->fail;
				}
				else
				{
					iter->second->fail = iterTemp->second;
					
					iter->second->output.insert(iter->second->output.end(), iterTemp->second->output.begin(), iterTemp->second->output.end());    //一个节点的output和它失效节点的output合并,可能不需要，结合查找来看
	
					break;
				}
			} 
			if (state == root)
			{
				unordered_map<string, simpleTrieNode*>::iterator iterTemp = state->g.find(iter->first);

				if (iterTemp == state->g.end())
				{
					iter->second->fail = root;
			
				}
				else
				{
					iter->second->fail = iterTemp->second;
					
				
					iter->second->output.insert(iter->second->output.end(), iterTemp->second->output.begin(), iterTemp->second->output.end());    //一个节点的output和它失效节点的output合并,可能不需要，结合查找来看
					
				
					
				}
			}
			
			temp.push(iter->second);			
		}
	}
}


void simpleTrie::Ergodic() {       //遍历，打印结果，看看对不对
	unordered_map<string, simpleTrieNode*>::iterator iter;
	queue<simpleTrieNode*> temp;
	temp.push(root);
	while (!temp.empty()) {
		simpleTrieNode* tempin = temp.front();
		temp.pop();
		cout << "there!" << endl;
		

		cout << tempin->No << " fail:";
		cout << tempin->fail->No << endl;
		/*输出失效函数*/
		
		

		for (iter = tempin->g.begin(); iter != tempin->g.end(); ++iter)
		{
			cout << tempin->No << ":" << iter->first << ":" << iter->second->No << endl;
			temp.push(iter->second);
		}
		/*输出转移函数*/
	
		if (tempin->output.size() != 0)
		{
			cout << tempin->No << " output:";
			for (auto i : tempin->output)
			{
				cout << i << endl;
			}
		}
		cout << endl;

	}
}


//FILE指针读完一次之后自己会增加，不用再自己加...
void simpleTrie::Find(FILE* fp,FILE* ofile) {    
	cotline = 0;
	tempFind = root;
	pos = 0;
	bool countnum = true;	
	fscanf(fp, "%c", &strTemp2);
	++pos;
	strTemp = strTemp2;
	if (strTemp2 & 0x80)   //汉字     
	{
		fscanf(fp, "%c", &strTemp2);
		++pos;
		strTemp += strTemp2;
	}
	do
	{    
		iterFind = tempFind->g.find(strTemp);
		if (iterFind == tempFind->g.end())
		{
			if (tempFind == root)
			{
				//从头找
				fscanf(fp, "%c", &strTemp2);
				++pos;
				strTemp = strTemp2;
				if (strTemp2 & 0x80)   //汉字       
				{
					fscanf(fp, "%c", &strTemp2);
					++pos;
					strTemp += strTemp2;
				}
				countnum = true;
			}
			else
			{
			    //转到失效函数
				if (countnum)
				{
					if (!tempFind->output.empty())    //输出此节点的output
					{
						for (auto j : tempFind->output)
						{
							
					        fprintf(ofile, "%s %ld\n", j.c_str(), (pos - j.size() - strTemp.size()));
							++cotline;
						}
					}
					countnum = false;
				}		
				tempFind = tempFind->fail;				
			}			
		}
		else
		{
			//接着找
			if (countnum)
			{
				if (!tempFind->output.empty())    //输出此节点的output
				{
					for (auto j : tempFind->output)
					{
						
				        fprintf(ofile, "%s %ld\n", j.c_str(), (pos - j.size() - strTemp.size()));
						++cotline;
					}
				}
				countnum = false;
			}
			tempFind = iterFind->second;
			fscanf(fp, "%c", &strTemp2);
			++pos;
			strTemp = strTemp2;
			if (strTemp2 & 0x80)   //汉字       
			{
				fscanf(fp, "%c", &strTemp2);
				++pos;
				strTemp += strTemp2;
			}
			countnum = true;
		}
	} while (!feof(fp));
	if (countnum)
	{
		if (!tempFind->output.empty())    //输出此节点的output
		{
			for (auto j : tempFind->output)
			{

			
				fprintf(ofile, "%s %ld\n", j.c_str(), (pos - j.size() - strTemp.size()));
				++cotline;			
			}
		}
		countnum = false;

	}
}


void simpleTrie::destroy() {
	queue<simpleTrieNode*> temp;
	temp.push(root);
	while (!temp.empty())
	{
		simpleTrieNode* tempin = temp.front();
		temp.pop();
		unordered_map<string, simpleTrieNode*>::iterator iter;
		for (iter = tempin->g.begin(); iter != tempin->g.end(); ++iter)
		{
			temp.push(iter->second);
		}
		delete tempin;
	}
	cout << "tree destroyed!" << endl;
}
