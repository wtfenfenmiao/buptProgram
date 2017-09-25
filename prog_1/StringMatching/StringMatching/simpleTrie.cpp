#include "simpleTrie.h"


using namespace std;
//没用到内存分配，先跑通，内存优化一定用分配的，自动在栈上你在堆上，差的还挺多


simpleTrieNode::simpleTrieNode(int number) {
	//cout << "there2" << endl;
	this->fail = NULL;
	No = number;
	//cout << "there3" << endl;
}


simpleTrie::simpleTrie() {    //要给根节点分配内存
	//cout << "there1" << endl;
	this->root = new simpleTrieNode(0);
	this->count = 0;
	//cout << "there4" << endl;

}



//数据很大，所以g函数不能放到图里，得用树来存
void simpleTrie::Insert(string str) {      //目前还没有0的状态
	bool flag = false;
	simpleTrieNode* tempOut = root;
	int i = 0;
	while (i < str.length())
	{
		//cout << "字符:" << str[i] << endl;
		map<char, simpleTrieNode*>::iterator iter;
		iter = tempOut->g.find(str[i]);
		if (iter == tempOut->g.end())
		{
			flag = true;
			for (int j = i; j < str.length(); j++)
			{
				//cout << "字符2" << str[j] << endl;
				simpleTrieNode* tempin = new simpleTrieNode(++count);
				tempOut->g.insert(map<char, simpleTrieNode*>::value_type(str[j], tempin));
				tempOut = tempin;
			}
			tempOut->output.push_back(str);
			break;
		}
		else
		{
			tempOut = iter->second;
			i++;
		}
	}
	if (!flag)
	{
		tempOut->output.push_back(str);
	}
}

void simpleTrie::ergodicFail() {      //用队列来弄，因为算是一个层序遍历
	queue<simpleTrieNode*> temp;
	map<char, simpleTrieNode*>::iterator iter;
	for (iter = root->g.begin(); iter != root->g.end(); iter++)    //根节点下面的第一层，失效指针就是根节点
	{
		iter->second->fail = root;
		temp.push(iter->second);
		//cout << "num:" << iter->second->No << endl;
	}
	while (!temp.empty())         //逐层搞
	{
		simpleTrieNode* tempin = temp.front();
		temp.pop();
		for (iter = tempin->g.begin(); iter != tempin->g.end(); iter++)
		{
			//cout << "num:" << iter->second->No << endl;
			simpleTrieNode* state = tempin->fail;
			map<char, simpleTrieNode*>::iterator iterTemp = state->g.find(iter->first);
			if (iterTemp == state->g.end())
			{
				iter->second->fail = root;
			}
			else
			{
				iter->second->fail = iterTemp->second;
				iter->second->output.insert(iter->second->output.end(), iterTemp->second->output.begin(), iterTemp->second->output.end());    //一个节点的output和它失效节点的output合并
			}
			temp.push(iter->second);
		}
	}
}


void simpleTrie::Ergodic() {       //遍历，打印结果，看看对不对
	map<char, simpleTrieNode*>::iterator iter;
	queue<simpleTrieNode*> temp;
	temp.push(root);
	while (!temp.empty()) {
		simpleTrieNode* tempin = temp.front();
		temp.pop();
		
		/*输出失效函数*/
		cout << tempin->No << " fail:";
		if (tempin->fail == NULL)
		{
			cout << "NULL" << endl;
		}
		else
		{
			cout << tempin->fail->No << endl;
		}
		
		/*输出转移函数*/
		for (iter = tempin->g.begin(); iter != tempin->g.end(); iter++)
		{
			cout << tempin->No << ":" << iter->first << ":" << iter->second->No << endl;
			temp.push(iter->second);
		}
		/*输出output*/
		if (tempin->output.size() != 0)
		{
			cout << tempin->No << " output:";
			for (auto i : tempin->output)
			{
				cout << i << endl;
			}
		}

	}
}

void simpleTrie::Find(string i) {

}
/*
void simpleTrie::getG(string str) {     //这个insert的过程，构造转向函数g
	if (g.empty())
	{
		g[nodeNo++].insert(map<int, char>::value_type(str[0], nodeNo));
		for (int i = 1; i < str.length();i++) {
			g[nodeNo++].insert(map<int,char>::value_type(str[i], nodeNo));
			g[0].insert(map<int, char>::value_type(str[i], 0));
		}
		output[nodeNo].push_back(str);                //第一种输出函数
	}
	else
	{
		int j = 0;
		int m = 0;
		while (j < str.length())
		{
			map<char, int>::iterator iter;
			iter = g[m].find(str[j]);
			if (iter == g[m].end())    //新来的字符串的第一个字符没有和0接过，而且之前也没有出现过
			{
				g[m].insert(map<int, char>::value_type(str[j], ++nodeNo));
				for (int i = j+1; i < str.length(); i++) {
					g[nodeNo++].insert(map<int, char>::value_type(str[i], nodeNo));
					g[0].insert(map<int, char>::value_type(str[i], 0));   //如果已经插入过了，比如前边his，is都有0到0的了，后面来了this，is已经插入过0到0的了，这个就会插入失败，觉得没事，如果说有事情就改成判断
				}
				output[nodeNo].push_back(str);
			}
			else if (iter->second == 0)    //和0接过，但是是0到0的循环，比如前面的字符串是his，这回来了is，i的情况。这个第二次循环开始就没用了，因为不存在1接字符再接0，只有0接字符再接0
			{
				iter->second = (++nodeNo);
				for (int i = j+1; i < str.length(); i++) {
					g[nodeNo++].insert(map<int, char>::value_type(str[i], nodeNo));
					g[0].insert(map<int, char>::value_type(str[i], 0));
				}
				output[nodeNo].push_back(str);
			}
			else
			{
				m = iter->second;
				j++;
			}
		}
	}
}
*/