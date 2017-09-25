#include "simpleTrie.h"


using namespace std;
//û�õ��ڴ���䣬����ͨ���ڴ��Ż�һ���÷���ģ��Զ���ջ�����ڶ��ϣ���Ļ�ͦ��


simpleTrieNode::simpleTrieNode(int number) {
	//cout << "there2" << endl;
	this->fail = NULL;
	No = number;
	//cout << "there3" << endl;
}


simpleTrie::simpleTrie() {    //Ҫ�����ڵ�����ڴ�
	//cout << "there1" << endl;
	this->root = new simpleTrieNode(0);
	this->count = 0;
	//cout << "there4" << endl;

}



//���ݺܴ�����g�������ܷŵ�ͼ�����������
void simpleTrie::Insert(string str) {      //Ŀǰ��û��0��״̬
	bool flag = false;
	simpleTrieNode* tempOut = root;
	int i = 0;
	while (i < str.length())
	{
		//cout << "�ַ�:" << str[i] << endl;
		map<char, simpleTrieNode*>::iterator iter;
		iter = tempOut->g.find(str[i]);
		if (iter == tempOut->g.end())
		{
			flag = true;
			for (int j = i; j < str.length(); j++)
			{
				//cout << "�ַ�2" << str[j] << endl;
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

void simpleTrie::ergodicFail() {      //�ö�����Ū����Ϊ����һ���������
	queue<simpleTrieNode*> temp;
	map<char, simpleTrieNode*>::iterator iter;
	for (iter = root->g.begin(); iter != root->g.end(); iter++)    //���ڵ�����ĵ�һ�㣬ʧЧָ����Ǹ��ڵ�
	{
		iter->second->fail = root;
		temp.push(iter->second);
		//cout << "num:" << iter->second->No << endl;
	}
	while (!temp.empty())         //����
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
				iter->second->output.insert(iter->second->output.end(), iterTemp->second->output.begin(), iterTemp->second->output.end());    //һ���ڵ��output����ʧЧ�ڵ��output�ϲ�
			}
			temp.push(iter->second);
		}
	}
}


void simpleTrie::Ergodic() {       //��������ӡ����������Բ���
	map<char, simpleTrieNode*>::iterator iter;
	queue<simpleTrieNode*> temp;
	temp.push(root);
	while (!temp.empty()) {
		simpleTrieNode* tempin = temp.front();
		temp.pop();
		
		/*���ʧЧ����*/
		cout << tempin->No << " fail:";
		if (tempin->fail == NULL)
		{
			cout << "NULL" << endl;
		}
		else
		{
			cout << tempin->fail->No << endl;
		}
		
		/*���ת�ƺ���*/
		for (iter = tempin->g.begin(); iter != tempin->g.end(); iter++)
		{
			cout << tempin->No << ":" << iter->first << ":" << iter->second->No << endl;
			temp.push(iter->second);
		}
		/*���output*/
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
void simpleTrie::getG(string str) {     //���insert�Ĺ��̣�����ת����g
	if (g.empty())
	{
		g[nodeNo++].insert(map<int, char>::value_type(str[0], nodeNo));
		for (int i = 1; i < str.length();i++) {
			g[nodeNo++].insert(map<int,char>::value_type(str[i], nodeNo));
			g[0].insert(map<int, char>::value_type(str[i], 0));
		}
		output[nodeNo].push_back(str);                //��һ���������
	}
	else
	{
		int j = 0;
		int m = 0;
		while (j < str.length())
		{
			map<char, int>::iterator iter;
			iter = g[m].find(str[j]);
			if (iter == g[m].end())    //�������ַ����ĵ�һ���ַ�û�к�0�ӹ�������֮ǰҲû�г��ֹ�
			{
				g[m].insert(map<int, char>::value_type(str[j], ++nodeNo));
				for (int i = j+1; i < str.length(); i++) {
					g[nodeNo++].insert(map<int, char>::value_type(str[i], nodeNo));
					g[0].insert(map<int, char>::value_type(str[i], 0));   //����Ѿ�������ˣ�����ǰ��his��is����0��0���ˣ���������this��is�Ѿ������0��0���ˣ�����ͻ����ʧ�ܣ�����û�£����˵������͸ĳ��ж�
				}
				output[nodeNo].push_back(str);
			}
			else if (iter->second == 0)    //��0�ӹ���������0��0��ѭ��������ǰ����ַ�����his���������is��i�����������ڶ���ѭ����ʼ��û���ˣ���Ϊ������1���ַ��ٽ�0��ֻ��0���ַ��ٽ�0
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