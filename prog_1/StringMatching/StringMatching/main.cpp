#include "simpleTrie.h"
using namespace std;


//示例：nihao，hao，hs，hsr的模式串匹配sdmfhsgnshejfgnihaofhsrnihao正确
//示例：他是它的，他其的，的他是，他是    模式串匹配他是它的主人他是一个嗷嗷的他是主人他其的走的他的他是他其的   正确，但是位置显示的是字节位置，输出一下一看就明白了


void main(int argc,char* argv[]) {
	string str = argv[1];
	string pattern = argv[2];
	string result = argv[3];	
	clock_t start, finish;
	double totaltime;
	start = clock();
	//生成树
	simpleTrie* tree = new simpleTrie;
	//tree->Insert("his");
	//tree->Insert("hers");
	//tree->Insert("he");
	//tree->Insert("she");
	//tree->ergodicFail();
	//tree->Ergodic();
	

	ifstream patternFile;
	
	//tree要delete，把结点delete了
	//cout << pattern << endl;
	patternFile.open(pattern);
	if (patternFile.is_open())
	{
		string temp;
		while (getline(patternFile, temp))
		{
			cout << temp << endl;
			//汉字编码处理的tip
			//for (int i = 0; i < temp.length();)
			//{
				//看字符的编码规范;GB2312的汉字以大于0x80的字节开始??如何识别汉字？？？
			//	if (temp[i] & 0x80)   //这个是这样的，汉字占两位，如果高八位是大于0x80的，那么就是汉字。这样按位与一下，低七位就都被盖了（10000000），只判断第一位就行，如果不是汉字结果就是0
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
		//tree->Ergodic();    这个是用来打印结果查bug的
		cout << "tree build finish!" << endl;
	}
	else
	{
		cout << "no such patternFile!" << endl;
	}
	patternFile.close();


	//查找，输出结果
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
				cout <<"查询："<<cot<<"      " <<temp << endl;
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

	//销毁树
	tree->destroy();	
	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "\n此程序的运行时间为" << totaltime << "秒！" << endl;
	getchar();
}

/*
void main(int argc,char *argv[])
{
	cout << argv[1] << endl;
	getchar();
}
*/