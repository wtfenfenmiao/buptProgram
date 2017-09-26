#ifndef SIMPLETRIE_H 
#define SIMPLETRIE_H

#include<string>              //h文件中说不应该包含这些，咋整？
#include<vector>
#include<map>
#include<queue>
#include<iostream>
#include<fstream>
#include<time.h>



struct simpleTrieNode {                       //可能用union，但是还不会用   //C里用typedef，否则下边声明个新变量要带着struct关键字，很麻烦，但是C++里就不用，直接struct就行，下面就不用带struct关键字了
	std::map<std::string, simpleTrieNode*> g;    //转向函数指针
	simpleTrieNode* fail;//失效函数指针
	std::vector<std::string> output;
	int No;//最后应该删了的，一个编号
	simpleTrieNode(int number);    //构造函数
};


struct simpleTrie {   
	simpleTrieNode* root;     //根节点
	int count;           //当前最大结点编号
	/*
	long int nodeNo;         //当前节点最大编号
	//std::vector< std::vector<int> > edge;
	std::map<int,std::map<char,int> > g;         //转向函数    这几个函数想想咋整，这样不对
	std::vector<int> f;                 //失效函数
	std::map<int, std::vector<std::string> > output; //输出函数
	*/
	simpleTrie();        //构造函数
	void Insert(std::string i);       //插入节点,构成转向函数指针
	//void Export();       //导出这棵树成文件，不一定用得到，可能测试用
	void Find(std::string & i,int line,std::ofstream & ofile);         //查找函数,查找此串中模式出现的位置，还有次数
	void ergodicFail();       //遍历函数，构成失效函数指针
	void Ergodic();     //遍历函数，用来测试这棵树的构建结果
	void destroy();    //把内存释放掉
}; 


#endif