#include<fstream>
#include "simpleTrie.h"
using namespace std;

int main() {
	//cout << "hello world!" << endl;
	
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
			//tree->Insert(temp);
		}
		//tree->ergodicFail();
		//tree->Ergodic();
	}
	else
	{
		cout << "no such file!" << endl;
	}




	ifile.close();
	
	getchar();
	return 0;
}