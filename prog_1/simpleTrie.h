#ifndef SIMPLETRIE_H 
#define SIMPLETRIE_H

#include<string>              //h�ļ���˵��Ӧ�ð�����Щ��զ����
#include<vector>
#include<unordered_map>
#include<queue>
#include<iostream>
#include<fstream>
#include<time.h>



struct simpleTrieNode {                       //������union�����ǻ�������   //C����typedef�������±��������±���Ҫ����struct�ؼ��֣����鷳������C++��Ͳ��ã�ֱ��struct���У�����Ͳ��ô�struct�ؼ�����
	std::unordered_map<std::string, simpleTrieNode*> g;    //ת����ָ��
	simpleTrieNode* fail;//ʧЧ����ָ��
	std::vector<std::string> output;
	int No;
	simpleTrieNode(int number);    //���캯��
};


struct simpleTrie {   
	simpleTrieNode* root;     //���ڵ�
	int count;           //��ǰ�������
	/*
	long int nodeNo;         //��ǰ�ڵ������
	//std::vector< std::vector<int> > edge;
	std::map<int,std::map<char,int> > g;         //ת����    �⼸����������զ������������
	std::vector<int> f;                 //ʧЧ����
	std::map<int, std::vector<std::string> > output; //�������
	*/
	simpleTrie();        //���캯��
	void Insert(std::string & i);       //����ڵ�,����ת����ָ��
	//void Export();       //������������ļ�����һ���õõ������ܲ�����
	void Find(std::FILE* fp,FILE* ofile);       //���Һ���,���Ҵ˴���ģʽ���ֵ�λ�ã����д���
	void ergodicFail();       //��������������ʧЧ����ָ��
	void Ergodic();     //������������������������Ĺ������
	void destroy();    //���ڴ��ͷŵ�
}; 
extern long int pos;
extern std::string strTemp;
extern char strTemp2;
extern simpleTrieNode* tempFind;
extern std::unordered_map<std::string, simpleTrieNode*>::iterator iterFind;
extern long int cotline;
#endif