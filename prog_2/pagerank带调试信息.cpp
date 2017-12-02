#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>   //ֻ����һ�У�linux��windows
#include<string.h>
#include<vector>
#include<unordered_map>
#include<iostream>
#include<fstream>
#include<algorithm>
#include<string>
using namespace std;
//Gm:ÿһ����һ��������ϵ�������3�У����Ǳ��3��URL����������˭��

int topNum = 10;
double alpha = 0.85;
double epsilon = 1.0;
int dimen = 0;
unordered_map<int, string> BianHaothis;

class TriSparMatrix
{
public:
	unordered_map<int,unordered_map<int,double> > values;   //����Ԫ��ֵ,ǰ�����У��������
	unordered_map<int, double> columnNum;  //ÿһ�з���Ԫ���ܺ�
	double others;
};

void getvalues(TriSparMatrix* A,char* urltxt)
{
	int N = 0;
	ifstream urlread;
	urlread.open(urltxt);
	if (!urlread.is_open())
	{
		cout << "no such url file!" << endl;
		return;
	}
	string line;
	string url;
	int column = 0;
	int row = 0;
        while (getline(urlread, line))
	{
		if (strlen(line.c_str())==0)   //֮ǰ��windows��ֱ��line==""�������ǲ��еģ�linux�¾Ͳ��У�����û�취�жϿ����ǿյģ�ֻ����������Ȼ��֪��Ϊɶ
		{
			break;		
		}
                cout<<line<<endl;
                char *ptemp;
		ptemp = strtok((char*)(line.c_str()), " ");
		url=(string)ptemp;
		cout<<url<<endl;
		ptemp = strtok(NULL, " ");
		column = stoi(ptemp)-1;
		cout<<"columnt:"<<column<<endl;
		BianHaothis.insert(make_pair(column, url));
		//getline(urlread, line);
		++N;
                cout<<N<<endl;
	}
        //getline(urlread, line);
	column=0;
        getchar();
	while (getline(urlread, line))
	{
                cout<<line<<endl;
                cout<<"there"<<endl;
                getchar();
		char *ptemp;
		ptemp = strtok((char*)(line.c_str()), " ");
		column = stoi(ptemp)-1;   //֮ǰ��Ŵ�1��ʼ�ģ����Ǵ�0��ʼ�ģ��ܲ�����...��������
		ptemp = strtok(NULL, " ");
		row = stoi(ptemp)-1;
		cout<<"column:"<<column<<endl;
		cout<<"row:"<<row<<endl;
                getchar();
		if (column > dimen)
		{
			dimen = column;
		}
		if (row > dimen)
		{
			dimen = row;
		}
		A->values[column][row] = 1;
		unordered_map<int, double>::iterator itvalues;
		itvalues = A->columnNum.find(column);
		if (itvalues != A->columnNum.end())
		{
			itvalues->second += 1.0;
		}
		else
			A->columnNum[column] = 1.0;

	}
	unordered_map<int, double>::iterator iterColumnNum;
	for (iterColumnNum=A->columnNum.begin();iterColumnNum!=A->columnNum.end();++iterColumnNum)
	{
		unordered_map<int, double>::iterator iterValues;
		int columnIndex = iterColumnNum->first;
		double columnSum = iterColumnNum->second;
		for (iterValues = A->values[columnIndex].begin(); iterValues != A->values[columnIndex].end(); ++iterValues)
		{
			iterValues->second = alpha / columnSum + (1 - alpha) / N;
		}
	}
	A->others = (1 - alpha) / N;
	urlread.close();
}

void matrixMulti(TriSparMatrix* A, double* pr, double* R)
{
        double Rtemp=0.0;
        for (int j = 0; j < dimen; ++j)
	{
		Rtemp += pr[j];
	}
	Rtemp *= A->others;
	for (int i = 0; i < dimen; ++i)
	{
		R[i]=Rtemp;
		unordered_map<int, double>::iterator iterLie;
		double PRtemp = 0.0;
		for (iterLie = A->values[i].begin(); iterLie != A->values[i].end(); ++iterLie)
		{
			R[i] += pr[iterLie->first] * iterLie->second;
			PRtemp += pr[iterLie->first];
		}
		R[i] -= PRtemp*A->others;
	}
}

double diff(double* R, double* pr, int dimen)
{
	double re = 0;
	for (int i = 0; i < dimen; i++)
	{
		re += abs(R[i] - pr[i]);
	}
	cout << re << endl;
	return re;
}

double* pagerank(char* urltxt) {
	TriSparMatrix* A = new TriSparMatrix;
	getvalues(A,urltxt);
	double* pr = new double[dimen];
	for (int i = 0; i < dimen; i++)
	{
		pr[i] = 1.0;
	}
	double* R = new double[dimen];
	memset(R, 0, dimen * sizeof(double));
	matrixMulti(A, pr, R);
	while (1)
	{
		if (diff(R, pr, dimen) < epsilon)
			break;
		for (int i = 0; i < dimen; i++)
		{
			pr[i] = R[i];
		}
		memset(R, 0, dimen * sizeof(double));
		matrixMulti(A, pr, R);
	}
	delete[] pr;
	pr = NULL;
	delete A;
	A = NULL;
	return R;
}

int main(int argc, char* argv[])
{
	const char* urltxt = argv[1];    //����Ҫ��const char*��д��Ķ��ᱨ��������
	const char* top10txt = argv[2];
	double* prvalue;
	prvalue = pagerank((char*)urltxt);    //�����ӣ�pagerank��new��R��ֱ��������delete prvalue�Ϳ����ͷ���
	FILE* topfile = fopen(top10txt, "a+");
	int* top = new int[topNum];
	top[0] = 1;
	for (int i = 2; i <= topNum; ++i)           //ǰʮ���Ӹߵ�������
	{
		int j;
		for (j = 0; j<i - 1; ++j)
		{
			if (prvalue[i]>prvalue[top[j]])
			{
				break;
			}
		}
		for (int k = i - 1; k >= j + 1; --k)
		{
			top[k] = top[k - 1];
		}
		top[j] = i;
	}

	for (int i = topNum + 1; i<dimen; ++i)
	{
		int j;
		for (j = 0; j<topNum; ++j)
		{
			if (prvalue[i]>prvalue[top[j]])
			{
				break;
			}
		}
		for (int k = topNum - 1; k >= j + 1; --k)
		{
			top[k] = top[k - 1];
		}
		top[j] = i;
	}
	string url;
	for (int l = 0; l<topNum; ++l)
	{
		int Bian = top[l];
		url = BianHaothis[Bian];
		fprintf(topfile, "%s %.15lf\n", url.c_str(), prvalue[Bian]);
	}
	fclose(topfile);
	delete[] prvalue;
	prvalue = NULL;
	delete[] top;
	top = NULL;
	return 0;
}