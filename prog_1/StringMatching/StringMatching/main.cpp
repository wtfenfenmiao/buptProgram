#include<fstream>
#include "simpleTrie.h"
using namespace std;


//ʾ����nihao��hao��hs��hsr��ģʽ��ƥ��sdmfhsgnshejfgnihaofhsrnihao��ȷ
//ʾ�����������ģ�����ģ������ǣ�����    ģʽ��ƥ������������������һ���໵���������������ߵ��������������   ��ȷ������λ����ʾ�����ֽ�λ�ã����һ��һ����������
int main() {
	//cout << "hello world!" << endl;
	clock_t start, finish;
	double totaltime;
	start = clock();

	simpleTrie* tree = new simpleTrie;
	/*tree->Insert("his");
	tree->Insert("hers");
	tree->Insert("he");
	tree->Insert("she");
	tree->ergodicFail();
	tree->Ergodic();
	*/

	ifstream ifile;
	
	//treeҪdelete���ѽ��delete��
	
	ifile.open("test.txt");
	if (ifile.is_open())
	{
		string temp;
		while (getline(ifile, temp))
		{
			cout << temp << endl;
			/*���ֱ��봦���tip
			for (int i = 0; i < temp.length();)
			{
				//���ַ��ı���淶;GB2312�ĺ����Դ���0x80���ֽڿ�ʼ??���ʶ���֣�����
				if (temp[i] & 0x80)   //����������ģ�����ռ��λ������߰�λ�Ǵ���0x80�ģ���ô���Ǻ��֡�������λ��һ�£�����λ�Ͷ������ˣ�10000000����ֻ�жϵ�һλ���У�������Ǻ��ֽ������0
				{
					cout << temp.substr(i, 2) << endl;
					i += 2;
				}
				else
				{
					cout << temp[i++] << endl;
				}
			}
			*/
			tree->Insert(temp);
		}

		tree->ergodicFail();
		//tree->Ergodic();
		cout << "tree build finish!" << endl;
	}
	else
	{
		cout << "no such file!" << endl;
	}
	ifile.close();
	tree->Find("����������������һ���໵���������������ߵ��������������");
	tree->destroy();
	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "\n�˳��������ʱ��Ϊ" << totaltime << "�룡" << endl;
	getchar();
	return 0;
}