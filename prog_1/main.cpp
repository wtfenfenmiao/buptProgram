#include "simpleTrie.h"
using namespace std;


//ʾ����nihao��hao��hs��hsr��ģʽ��ƥ��sdmfhsgnshejfgnihaofhsrnihao��ȷ
//����pattern��string����һ��i�ٲ���
//ʾ�����������ģ�����ģ������ǣ�����    ģʽ��ƥ������������������һ���໵���������������ߵ��������������   ��ȷ


int main(int argc,char* argv[]) {

	const char* str = argv[1];
	string pattern = argv[2];
	const char* result = argv[3];	
	//clock_t start, finish;
	//double totaltime;
	//start = clock();
	//������
	simpleTrie* tree = new simpleTrie();
	ifstream patternFile;
	patternFile.open(pattern);
	if (patternFile.is_open())
	{
		string temp;
		while (getline(patternFile, temp))
		{
			tree->Insert(temp);
		}

		tree->ergodicFail();
		//tree->Ergodic();    //�����������ӡ�����bug
		cout << "tree build finish!" << endl;
	}
	else
	{
		cout << "no such patternFile!" << endl;
	}
	patternFile.close();


	//���ң�������
	//ofstream resultFile;
	FILE* resultFile=fopen(result,"w");
	FILE* fp=fopen(str,"r");	
	if (fp==NULL)
	{	
		cout << "no such strFile!" << endl;
	}
	else
	{
		if (resultFile==NULL)
		{
			cout << "no such resultFile!" << endl;
		}
		else
		{
			tree->Find(fp, resultFile);	
			fclose(resultFile);
		}
		fclose(fp);
	}
	
	
	
	//������
	tree->destroy();	
	
	//finish = clock();
	//totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	//cout << "\n�˳��������ʱ��Ϊ" << totaltime << "�룡" << endl;
	//cout << "ƥ�������" << cotline << endl;
	//getchar();
	return 0;
	
	
}

