#include "simpleTrie.h"
using namespace std;


//示例：nihao，hao，hs，hsr的模式串匹配sdmfhsgnshejfgnihaofhsrnihao正确
//上面pattern和string各加一个i再测试
//示例：他是它的，他其的，的他是，他是    模式串匹配他是它的主人他是一个嗷嗷的他是主人他其的走的他的他是他其的   正确


int main(int argc,char* argv[]) {

	const char* str = argv[1];
	string pattern = argv[2];
	const char* result = argv[3];	
	//clock_t start, finish;
	//double totaltime;
	//start = clock();
	//生成树
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
		//tree->Ergodic();    //这个是用来打印结果查bug
		cout << "tree build finish!" << endl;
	}
	else
	{
		cout << "no such patternFile!" << endl;
	}
	patternFile.close();


	//查找，输出结果
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
	
	
	
	//销毁树
	tree->destroy();	
	
	//finish = clock();
	//totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	//cout << "\n此程序的运行时间为" << totaltime << "秒！" << endl;
	//cout << "匹配次数：" << cotline << endl;
	//getchar();
	return 0;
	
	
}

