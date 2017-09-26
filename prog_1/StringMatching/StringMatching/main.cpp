#include<fstream>
#include "simpleTrie.h"
using namespace std;


//示例：nihao，hao，hs，hsr的模式串匹配sdmfhsgnshejfgnihaofhsrnihao正确
//示例：他是它的，他其的，的他是，他是    模式串匹配他是它的主人他是一个嗷嗷的他是主人他其的走的他的他是他其的   正确，但是位置显示的是字节位置，输出一下一看就明白了
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
	
	//tree要delete，把结点delete了
	
	ifile.open("test.txt");
	if (ifile.is_open())
	{
		string temp;
		while (getline(ifile, temp))
		{
			cout << temp << endl;
			/*汉字编码处理的tip
			for (int i = 0; i < temp.length();)
			{
				//看字符的编码规范;GB2312的汉字以大于0x80的字节开始??如何识别汉字？？？
				if (temp[i] & 0x80)   //这个是这样的，汉字占两位，如果高八位是大于0x80的，那么就是汉字。这样按位与一下，低七位就都被盖了（10000000），只判断第一位就行，如果不是汉字结果就是0
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
	tree->Find("他是它的主人他是一个嗷嗷的他是主人他其的走的他的他是他其的");
	tree->destroy();
	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "\n此程序的运行时间为" << totaltime << "秒！" << endl;
	getchar();
	return 0;
}