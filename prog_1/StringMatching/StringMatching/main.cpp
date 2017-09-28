#include "simpleTrie.h"
using namespace std;


//ʾ����nihao��hao��hs��hsr��ģʽ��ƥ��sdmfhsgnshejfgnihaofhsrnihao��ȷ
//ʾ�����������ģ�����ģ������ǣ�����    ģʽ��ƥ������������������һ���໵���������������ߵ��������������   ��ȷ������λ����ʾ�����ֽ�λ�ã����һ��һ����������


void main(int argc,char* argv[]) {
	string str = argv[1];
	string pattern = argv[2];
	string result = argv[3];	
	clock_t start, finish;
	double totaltime;
	start = clock();
	//������
	simpleTrie* tree = new simpleTrie;
	//tree->Insert("his");
	//tree->Insert("hers");
	//tree->Insert("he");
	//tree->Insert("she");
	//tree->ergodicFail();
	//tree->Ergodic();
	

	ifstream patternFile;
	
	//treeҪdelete���ѽ��delete��
	//cout << pattern << endl;
	patternFile.open(pattern);
	if (patternFile.is_open())
	{
		string temp;
		while (getline(patternFile, temp))
		{
			cout << temp << endl;
			//���ֱ��봦���tip
			//for (int i = 0; i < temp.length();)
			//{
				//���ַ��ı���淶;GB2312�ĺ����Դ���0x80���ֽڿ�ʼ??���ʶ���֣�����
			//	if (temp[i] & 0x80)   //����������ģ�����ռ��λ������߰�λ�Ǵ���0x80�ģ���ô���Ǻ��֡�������λ��һ�£�����λ�Ͷ������ˣ�10000000����ֻ�жϵ�һλ���У�������Ǻ��ֽ������0
			//	{
			//		cout << temp.substr(i, 2) << endl;
			//		i += 2;
			//	}
			//	else
			//	{
			//		cout << temp[i++] << endl;
			//	}
			//}
			
			tree->Insert(temp);
		}

		tree->ergodicFail();
		//tree->Ergodic();    �����������ӡ�����bug��
		cout << "tree build finish!" << endl;
	}
	else
	{
		cout << "no such patternFile!" << endl;
	}
	patternFile.close();


	//���ң�������
	ifstream strFile;
	strFile.open(str);
	ofstream resultFile;
	resultFile.open(result);
	if (strFile.is_open())
	{
		if (resultFile.is_open())
		{
			string temp;
			int cot = 0;
			while (getline(strFile, temp))
			{
				cout <<"��ѯ��"<<cot<<"      " <<temp << endl;
				tree->Find(temp, cot++, resultFile);
			}
		}
		else
		{
			cout << "no such resultFile!" << endl;
		}
		
	}
	else
	{
		cout << "no such strFile!" << endl;
	}
	strFile.close();
	resultFile.close();

	//������
	tree->destroy();	
	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "\n�˳��������ʱ��Ϊ" << totaltime << "�룡" << endl;
	getchar();
}

/*
void main(int argc,char *argv[])
{
	cout << argv[1] << endl;
	getchar();
}
*/