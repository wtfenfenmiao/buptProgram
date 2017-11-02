#include "simpleTrie.h"


using namespace std;
long int pos;
long int cotline;
string strTemp;
char strTemp2;
simpleTrieNode* tempFind;
unordered_map<string, simpleTrieNode*>::iterator iterFind;
//����㹹�캯��
simpleTrieNode::simpleTrieNode(int number) {
	this->fail = NULL;
	No = number;
}

//�����캯��
simpleTrie::simpleTrie() {    //Ҫ�����ڵ�����ڴ�
	this->root = new simpleTrieNode(0);
	this->count = 0;
}




//����pattern
void simpleTrie::Insert(string & str1) {
	string str = str1 + '\0';
	simpleTrieNode* tempOut = root;
	char* i = &str[0];
	unordered_map<string, simpleTrieNode*>::iterator iter;	
	strTemp = *i;
	if (*i & 0x80)    //�����ַ�
	{
		strTemp += *(++i);
	}
	iter = tempOut->g.find(strTemp);
	while (*i != '\0' && iter != tempOut->g.end())        //�ҵ����pattern�����￪ʼ�Ҳ����ĵط�
	{
		++i;
		tempOut = iter->second;
		strTemp = *i;
		if (*i & 0x80)    //�����ַ�
		{
			strTemp += *(++i);
		}
		iter = tempOut->g.find(strTemp);
	}
	while (*i != '\0')                                     //�����pattern�����Ҳ����ĵط��嵽����
	{
		simpleTrieNode* tempin = new simpleTrieNode(++count);
		tempOut->g.insert(make_pair(strTemp, tempin));
		tempOut = tempin;
		++i;
		strTemp = *i;
		if (*i & 0x80)    //����
		{
			++i;
			strTemp += *i;		
		}
	}
	tempOut->output.push_back(str1);
}

//��������ʧЧָ��
void simpleTrie::ergodicFail() {      //�ö�����Ū����Ϊ����һ���������
	queue<simpleTrieNode*> temp;
	unordered_map<string, simpleTrieNode*>::iterator iter;
	root->fail = root;
	for (iter = root->g.begin(); iter != root->g.end(); ++iter)    //���ڵ�����ĵ�һ�㣬ʧЧָ����Ǹ��ڵ�
	{
		iter->second->fail = root;
		temp.push(iter->second);
	}
	while (!temp.empty())         //���������������fail�Ĺ��̾�����ʦ���ļ�����failָ�빹�����
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
					
					iter->second->output.insert(iter->second->output.end(), iterTemp->second->output.begin(), iterTemp->second->output.end());    //һ���ڵ��output����ʧЧ�ڵ��output�ϲ�,���ܲ���Ҫ����ϲ�������
	
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
					
				
					iter->second->output.insert(iter->second->output.end(), iterTemp->second->output.begin(), iterTemp->second->output.end());    //һ���ڵ��output����ʧЧ�ڵ��output�ϲ�,���ܲ���Ҫ����ϲ�������
					
				
					
				}
			}
			
			temp.push(iter->second);			
		}
	}
}


void simpleTrie::Ergodic() {       //��������ӡ����������Բ���
	unordered_map<string, simpleTrieNode*>::iterator iter;
	queue<simpleTrieNode*> temp;
	temp.push(root);
	while (!temp.empty()) {
		simpleTrieNode* tempin = temp.front();
		temp.pop();
		cout << "there!" << endl;
		

		cout << tempin->No << " fail:";
		cout << tempin->fail->No << endl;
		/*���ʧЧ����*/
		
		

		for (iter = tempin->g.begin(); iter != tempin->g.end(); ++iter)
		{
			cout << tempin->No << ":" << iter->first << ":" << iter->second->No << endl;
			temp.push(iter->second);
		}
		/*���ת�ƺ���*/
	
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


//FILEָ�����һ��֮���Լ������ӣ��������Լ���...
void simpleTrie::Find(FILE* fp,FILE* ofile) {    
	cotline = 0;
	tempFind = root;
	pos = 0;
	bool countnum = true;	
	fscanf(fp, "%c", &strTemp2);
	++pos;
	strTemp = strTemp2;
	if (strTemp2 & 0x80)   //����     
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
				//��ͷ��
				fscanf(fp, "%c", &strTemp2);
				++pos;
				strTemp = strTemp2;
				if (strTemp2 & 0x80)   //����       
				{
					fscanf(fp, "%c", &strTemp2);
					++pos;
					strTemp += strTemp2;
				}
				countnum = true;
			}
			else
			{
			    //ת��ʧЧ����
				if (countnum)
				{
					if (!tempFind->output.empty())    //����˽ڵ��output
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
			//������
			if (countnum)
			{
				if (!tempFind->output.empty())    //����˽ڵ��output
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
			if (strTemp2 & 0x80)   //����       
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
		if (!tempFind->output.empty())    //����˽ڵ��output
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
