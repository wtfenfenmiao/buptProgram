#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<stdio.h>
#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<unordered_map>
#include<regex>
#include<vector>
#include<queue>
#include<algorithm>
#include<winsock2.h>
#include<map>
using namespace std;
#pragma comment(lib,"ws2_32.lib")    //windows下socket编程要加这个
unsigned int RSHash(const char* str, unsigned int length);
unsigned int JSHash(const char* str, unsigned int length);
unsigned int PJWHash(const char* str, unsigned int length);
unsigned int ELFHash(const char* str, unsigned int length);
unsigned int BKDRHash(const char* str, unsigned int length);
unsigned int SDBMHash(const char* str, unsigned int length);
unsigned int DJBHash(const char* str, unsigned int length);
unsigned int DEKHash(const char* str, unsigned int length);
unsigned int APHash(const char* str, unsigned int length);
char host[500];
char othPath[800];
string allHtml;
SOCKET sock;
int hashNum = 9;
unsigned long int countBianHao = 0;   //加编号用的
unsigned long int summery = 0;
unordered_map<string, unsigned long int> BianHao;
unordered_map<unsigned long int,string> BianHao1;
queue<unsigned long int> UrlQueue;   //只爬取用的队列
unordered_map<string, vector<string> > link;  //存储链接关系
//布隆过滤器实现
//new的格式：
//格式1：指针变量名=new 类型标识符;
//格式2：指针变量名=new 类型标识符（初始值）;
//格式3：指针变量名=new 类型标识符[内存单元个数];

//把char当成8位01串
class bitMap
{
public:
	//没有参数的构造函数
	bitMap()
	{
		bitmap = NULL;
		size = 0;
	}
	//传参是size的构造函数
	bitMap(int size)
	{
		bitmap = NULL;
		bitmap = new char[size];
		if (bitmap == NULL)
		{
			cout << "构造失败！" << endl;
		}
		else
		{
			memset(bitmap, 0x0, size * sizeof(char));
			this->size = size;
		}
	}
	void initbitMap(int size)
	{
		bitmap = NULL;
		bitmap = new char[size];
		if (bitmap == NULL)
		{
			cout << "设置长度失败！" << endl;
		}
		else
		{
			memset(bitmap, 0x0, size * sizeof(char));
			this->size = size;
		}
	}

	//把index这一位置成1，位指的是bit位
	bool bitMapSet(unsigned int index)   //这里index要从0开始，而不是从1开始，然后下边的这个步骤，就是位运算，自己跟着画小方格算一下就明白了......
	{
		int sizeofChar = sizeof(char);
		int addr = index / sizeofChar;    //在第几个char里
										  /*
										  if (addr >= size)
										  {
										  return false;       //没有set成
										  }
										  else
										  {
										  */
		int addrOffset = index%sizeofChar;   //在第addr个char中的位置
		unsigned char temp = 0x01 << addrOffset;
		bitmap[addr] |= temp;
		return true;
		//}	
	}
	//查询index这一位是不是1
	//返回true的情况是这一位是1，返回0的情况是这一位大小超范围了，或者没超范围，这一位是0
	//或者不考虑超范围的问题，返回true这一位是0，返回false这一位是1
	bool bitMapGet(unsigned int index)
	{
		int sizeofChar = sizeof(char);
		int addr = index / sizeofChar;
		/*
		if (addr >= size)   //这个判断是不是有必要？感觉，这个判断没必要，有必要的话也不是这么用的......这个要去看hash函数
		{
		return false;
		}
		else
		{
		*/
		int addrOffset = index%sizeofChar;   //在第addr个char中的位置
		unsigned char temp = 0x01 << addrOffset;
		//return bitmap[addr]&temp==0?false:true;
		return ((bitmap[addr] & temp) == 0) ? true : false;   //这一位是0，返回true，这一位是1，返回false
														  //}
	}
private:
	char* bitmap;
	int size;
};

class bloomFilter
{
public:
	bloomFilter(int size)
	{
		bitmap.initbitMap(size);
		this->size = size;
		cout << "bloom初始化" << endl;
	}
	bool ifContainAndInsert(string thisUrl)          //这里用模板类一定是好的，什么都通用，然而我还是先用基本的吧.......
	{
		//cout << "bloom判断是否存在" << endl;
		unsigned int *index;
		index = new unsigned int[hashNum];
		//找委员的ppt看一看，怎么才能不这么彪的写9遍......
		char* htmlTemp = (char*)thisUrl.c_str();
		int length = thisUrl.length();
		index[0] = RSHash(htmlTemp, length);
		index[1] = JSHash(htmlTemp, length);    //这以上是进行hash
		index[2] = PJWHash(htmlTemp, length);
		index[3] = ELFHash(htmlTemp, length);
		index[4] = BKDRHash(htmlTemp, length);
		index[5] = SDBMHash(htmlTemp, length);
		index[6] = DJBHash(htmlTemp, length);
		index[7] = DEKHash(htmlTemp, length);
		index[8] = APHash(htmlTemp, length);
		
		for (int j = 0; j < hashNum; ++j)
		{
			//cout << j << ":" << endl;
			//cout << index[j] << endl;
			index[j] %= size;   //映射到bitmap的大小里
			//cout << size << endl;
			//cout << index[j] << endl;
		}
		
		int i = 0;
		while (i < hashNum)
		{
			if (bitmap.bitMapGet(index[i]))     //如果发现这一位是0了，就break
			{
				break;
			}
			++i;
		}
		if (i < hashNum)    //提前break了，证明找到了是0的位
		{
			while (i < hashNum)
			{
				bitmap.bitMapSet(index[i]);
				++i;
			}
			return true;   //之前没有，return true
		}
		return false;  //之前有了，return false

	}
private:
	bitMap bitmap;
	int size;
};
bloomFilter bloom(10000000);    //bloom过滤器的位数，这个就是100000*8

unsigned int RSHash(const char* str, unsigned int length)
{
	unsigned int b = 378551;
	unsigned int a = 63689;
	unsigned int hash = 0;
	unsigned int i = 0;

	for (i = 0; i < length; ++str, ++i)
	{
		hash = hash * a + (*str);
		a = a * b;
	}
	return hash;
}

unsigned int JSHash(const char* str, unsigned int length)
{
	unsigned int hash = 1315423911;
	unsigned int i = 0;

	for (i = 0; i < length; ++str, ++i)
	{
		hash ^= ((hash << 5) + (*str) + (hash >> 2));
	}

	return hash;
}

unsigned int PJWHash(const char* str, unsigned int length)
{
	const unsigned int BitsInUnsignedInt = (unsigned int)(sizeof(unsigned int) * 8);
	const unsigned int ThreeQuarters = (unsigned int)((BitsInUnsignedInt * 3) / 4);
	const unsigned int OneEighth = (unsigned int)(BitsInUnsignedInt / 8);
	const unsigned int HighBits =
		(unsigned int)(0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
	unsigned int hash = 0;
	unsigned int test = 0;
	unsigned int i = 0;

	for (i = 0; i < length; ++str, ++i)
	{
		hash = (hash << OneEighth) + (*str);

		if ((test = hash & HighBits) != 0)
		{
			hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
		}
	}

	return hash;
}

unsigned int ELFHash(const char* str, unsigned int length)
{
	unsigned int hash = 0;
	unsigned int x = 0;
	unsigned int i = 0;

	for (i = 0; i < length; ++str, ++i)
	{
		hash = (hash << 4) + (*str);

		if ((x = hash & 0xF0000000L) != 0)
		{
			hash ^= (x >> 24);
		}

		hash &= ~x;
	}

	return hash;
}

unsigned int BKDRHash(const char* str, unsigned int length)
{
	unsigned int seed = 131; /* 31 131 1313 13131 131313 etc.. */
	unsigned int hash = 0;
	unsigned int i = 0;

	for (i = 0; i < length; ++str, ++i)
	{
		hash = (hash * seed) + (*str);
	}

	return hash;
}

unsigned int SDBMHash(const char* str, unsigned int length)
{
	unsigned int hash = 0;
	unsigned int i = 0;

	for (i = 0; i < length; ++str, ++i)
	{
		hash = (*str) + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}

unsigned int DJBHash(const char* str, unsigned int length)
{
	unsigned int hash = 5381;
	unsigned int i = 0;

	for (i = 0; i < length; ++str, ++i)
	{
		hash = ((hash << 5) + hash) + (*str);
	}

	return hash;
}

unsigned int DEKHash(const char* str, unsigned int length)
{
	unsigned int hash = length;
	unsigned int i = 0;

	for (i = 0; i < length; ++str, ++i)
	{
		hash = ((hash << 5) ^ (hash >> 27)) ^ (*str);
	}

	return hash;
}

unsigned int APHash(const char* str, unsigned int length)
{
	unsigned int hash = 0xAAAAAAAA;
	unsigned int i = 0;

	for (i = 0; i < length; ++str, ++i)
	{
		hash ^= ((i & 1) == 0) ? ((hash << 7) ^ (*str) * (hash >> 3)) :
			(~((hash << 11) + ((*str) ^ (hash >> 5))));
	}

	return hash;
}


//目前不需要
bool analyUrl(char *url)
{
	char *pos = strstr(url, "http://");    //strstr(str1,str2),判断str2是不是str1的字串，是的话就返回str2在str1中首次出现的地址，不是的话返回NULL
	if (pos == NULL)
	{
		return false;
	}
	else
	{
		pos += 7;
	}
	sscanf(pos, "%[^/]%s", host, othPath);    //从一个字符串中读进与指定格式相符的数据
	cout << "host:" << host << "    repath:" << othPath << endl;
	return true;
}

//和解析配套的socket连接，目前不需要
void preConnect()
{
	WSADATA wd;
	WSAStartup(MAKEWORD(2, 2), &wd); //windows下socket编程要加这个
	sock = socket(AF_INET, SOCK_STREAM, 0); //AF_INET（PF和AF是一样的）是ipv4，sock_stream是面向连接的，所以后面那个0的位置其实也不用填，就是tcp
	if (sock == INVALID_SOCKET)
	{
		cout << "建立socket失败！错误码：" << WSAGetLastError() << endl;
		return;
	}
	sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));   //sa所有字节都是0
	sa.sin_family = AF_INET;       
	int n = bind(sock, (sockaddr*)&sa, sizeof(sa));
	if (n == SOCKET_ERROR)
	{
		cout << "bind函数失败！错误码：" << WSAGetLastError() << endl;
		return;
	}
	struct hostent *p = gethostbyname(host);
	if (p == NULL)
	{
		cout << "主机无法解析出ip！错误码：" << WSAGetLastError() << endl;
		return;
	}
	sa.sin_port = htons(80);
	char* ipTemp = inet_ntoa(*(struct in_addr*)p->h_addr_list[0]);
	cout<<ipTemp<<endl;    //C++输出常量字符串的首地址时，实际输出这个字符串
	sa.sin_addr.S_un.S_addr = inet_addr(ipTemp);
	
	n = connect(sock, (sockaddr*)&sa, sizeof(sa));
	if (n == SOCKET_ERROR)
	{
		cout << "connect函数失败！错误码：" << WSAGetLastError() << endl;
		return;
	}
	//构建Get请求
	string reqInfo = "GET " + (string)othPath + " HTTP/1.1\r\nHost: " + (string)host + "\r\nConnection:Close\r\n\r\n";
	if (SOCKET_ERROR == send(sock, reqInfo.c_str(), reqInfo.size(), 0))
	{
		cout << "send error!错误码： " << WSAGetLastError() << endl;
		closesocket(sock);
		return;
	}
}

//目前用的，ip直接写成127.0.0.1

void preConnectWithIp()
{
	WSADATA wd;
	WSAStartup(MAKEWORD(2, 2), &wd); //windows下socket编程要加这个
	sock = socket(AF_INET, SOCK_STREAM, 0); //AF_INET（PF和AF是一样的）是ipv4，sock_stream是面向连接的，所以后面那个0的位置其实也不用填，就是tcp
	if (sock == INVALID_SOCKET)
	{
		cout << "建立socket失败！错误码：" << WSAGetLastError() << endl;
		return;
	}
	sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));   //sa所有字节都是0
	sa.sin_family = AF_INET;
	int n = bind(sock, (sockaddr*)&sa, sizeof(sa));
	if (n == SOCKET_ERROR)
	{
		cout << "bind函数失败！错误码：" << WSAGetLastError() << endl;
		return;
	}
	sa.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	sa.sin_port = htons(80);
	n = connect(sock, (sockaddr*)&sa, sizeof(sa));
	if (n == SOCKET_ERROR)
	{
		cout << "connect函数失败！错误码：" << WSAGetLastError() << endl;
		return;
	}
	//cout << "there" << endl;
}

//获取爬的这页的文档
//thisUrl是当前要获取html的目标url
string getHtml(string & thisUrl)
{
	//构建Get请求
	string tempHtml = "";
	string reqInfo = "GET http:/"+thisUrl+" HTTP/1.1\r\nHost:news.sohu.com:80\r\nConnection:close\r\n\r\n";
	//cout << reqInfo << endl;
	//这个是说明下，可以往下爬别的URL reqInfo = "GET http://news.sohu.com/20151105/n425315527.shtml HTTP/1.1\r\nHost:news.sohu.com:80\r\nConnection:Close\r\n\r\n";
	//const char* h=reqInfo.c_str();
	//cout << reqInfo.size() << endl;
	if (SOCKET_ERROR == send(sock, reqInfo.c_str(), reqInfo.size(), 0))
	{
		cout << "send error!错误码： " << WSAGetLastError() << endl;
		//closesocket(sock);
		return tempHtml;
	}
	//cout << "there??" << endl;
	int n;
	char buf[1024];
	while ((n = recv(sock, buf, sizeof(buf) - 1, 0)) > 0)
	{
		buf[n] = '\0';
		tempHtml += string(buf);
	}
	//cout << "there2" << endl;
	//这里，如果这个链接不存在，在map里删掉，并且不returnHtml
	if (tempHtml == "")
	{
		--summery;
		unordered_map<string, unsigned long int>::iterator iter1 = BianHao.find(thisUrl);
		unsigned long int numtemp = iter1->second;
		BianHao.erase(iter1);
		BianHao1.erase(BianHao1.find(numtemp));
		cout << "此链接不存在" << endl;
		return "";
	}
	string stateNum = tempHtml.substr(9, 3);
	//cout << "状态码：" << stateNum << endl;
	if (stateNum != "200")
	{
		--summery;
		unordered_map<string, unsigned long int>::iterator iter1 = BianHao.find(thisUrl);
		unsigned long int numtemp = iter1->second;
		BianHao.erase(iter1);
		BianHao1.erase(BianHao1.find(numtemp));
		cout << "此链接不存在" << endl;
		cout << "状态码：" << stateNum << endl;
		/*
		if (BianHao.find(thisUrl) == BianHao.end())
		{
			cout << "找不到了！" << endl;
		}
		
		if (BianHao1.find(numtemp) == BianHao1.end())
		{
			cout << "找不到了1!" << endl;
		}
		*/
		//cout << tempHtml << endl;
	    return "";
	}
	  
														  //写入文件链接关系
	return tempHtml;
}


//URL提取，用DFA.switch就能实现
//这个是给定一个Html页面，进行抽取URL
void thisURLextraction(string & thisHtml,string & thisUrl)
{
	if (thisHtml == "")    //之前的URL无效，这里就不抽取了
	{
		return;
	}
	vector<string> vectorInit;
	unsigned int thisUrlNum = BianHao.find(thisUrl)->second;
	link.insert(make_pair(thisUrl, vectorInit));
	
	int state = 0;
	int i = 0;
	int len = thisHtml.length();
	//cout << "长度：" << len << endl;
	string tempUrl = ""; 
	while (i < len)
	{
		//getchar();
		char input = thisHtml[i];
		//cout << input << endl;
		switch (state)
		{
		case 0:
			//cout << "case 0" << endl;
			switch (input)
			{
			case '<':state = 1; break;
			default:state = 0; break;
			}
			break;
		case 1:
			//cout << "case 1" << endl;
			switch (input)
			{
			case 'a':state = 2; break;
			default:state = 0; break;
			}
			break;
		case 2:
			//cout << "case 2" << endl;
			switch (input)
			{
			case 'h':state = 3; break;
			case '>':state = 0; break;
			default:state = 2; break;
			}
			break;
		case 3:
			//cout << "case 3" << endl;
			switch (input)
			{
			case 'r':state = 4; break;
			case '>':state = 0; break;
			default:state = 2; break;
			}
			break;
		case 4:
			//cout << "case 4" << endl;
			switch (input)
			{
			case 'e':state = 5; break;
			case '>':state = 0; break;
			default:state = 2; break;
			}
			break;
		case 5:
			//cout << "case 5" << endl;
			switch (input)
			{
			case 'f':state = 6; break;
			case '>':state = 0; break;
			default:state = 2; break;
			}
			break;
		case 6:
			//cout << "case 6" << endl;
			switch (input)
			{
			case '=':state = 7; break;
			case '>':state = 0; break;
			default:state = 2; break;
			}
			break;
		case 7:
			//cout << "case 7" << endl;
			switch (input)
			{
			case ' ':state = 7; break;
			case '"':state = 10; break;
			default:state = 0; break;
			}
			break;
		case 10:
			//cout << "case 10" << endl;
			switch (input)
			{
			case '/':state = 8; break;
			case '"' || '>' || '#':state = 0; break;
			default:state = 10; break;
			}
			break;
		case 8:
			//cout << "case 8" << endl;
			switch (input)
			{
			case '"':state = 9; break;
			case '>':state = 0; tempUrl = ""; break;
			default:state = 8; tempUrl += input; break;
			}
			break;
		case 9:
			//cout << "case 9" << endl;
			cout << "匹配！" << endl;      //这里，一边匹配，一边加链接关系，一边布隆过滤器去重
			cout << tempUrl << endl;
			bool flag=bloom.ifContainAndInsert(tempUrl);  //flag是true，就是这个Url之前没有过，但是不检查这个URL到底有没有
			//cout << "flag:" << flag << endl;
			
			if (!flag)
			{
				cout << "已存在！" << endl;
				cout <<"queue大小：" <<UrlQueue.size() << endl;
				cout << "计数大小：" << summery << endl;
				getchar();
			}
			
			if (flag)
			{
				++summery;
				//cout << "there1" << endl;
				    //这个Url之前没有过，加入队列，这个队列只爬取用，防止爬重了	
				BianHao.insert(make_pair(tempUrl, ++countBianHao));
				BianHao1.insert(make_pair(countBianHao, tempUrl));
				UrlQueue.push(countBianHao);
				//cout << "there2" << endl;
			}	
			//.push_back(tempUrl);
			state = 0;
			tempUrl = "";
			break;
		}
		i++;
	}	
}



int main()
{
	
	/*
	
	char tmp[800];
	strcpy(tmp, beginUrl.c_str());
	analyUrl(tmp);
	preConnect();
	*/
	
    //allHtml=getHtml(beginUrl);
	// fout("test.txt");
	//fout << allHtml;
	//cout << allHtml << endl;
	/*
	allHtml = 

		"<div class = \"pic\"  ><a href=\"http://pic.news.sohu.com/group-694656.shtml\" target = \"_blank\"  ><img alt = \"\" src = \"http://i0.itc.cn/20151105/34e3_7a90a96e_06e9_bb3c_d1f2_fd16ebc3c018_1.jpg\" width = 340 height = 235 filter = \"1\">< / a>< / div>\
        <a this href=\"http://www.baidu.com/wthelloworld!\"></a>";
	*/	
	//getchar();
	//thisURLextraction(allHtml);
	//getchar();

	
	clock_t start, finish;
	double totaltime;
	start = clock();
	string beginUrl = "/news.sohu.com/";
	++summery;
	bloom.ifContainAndInsert(beginUrl);
	BianHao.insert(make_pair(beginUrl, ++countBianHao));
	BianHao1.insert(make_pair(countBianHao, beginUrl));
	UrlQueue.push(countBianHao);
	while (!UrlQueue.empty())
	{
		cout  <<summery << endl;
		preConnectWithIp();
		//cout << "countBianHao:"<<countBianHao << endl;
		//cout << "爬取队列大小："<<UrlQueue.size() << endl;
		//cout << "BianHao大小：" << BianHao.size() << endl;
		string teUrl = BianHao1.find(UrlQueue.front())->second;
		cout << teUrl << endl;
		UrlQueue.pop();
		getchar();
		string teHtml = getHtml(teUrl);
		//cout << "there?" << endl;
		
		//cout << teHtml << endl;
		//getchar();
		thisURLextraction(teHtml, teUrl);
		//getchar();
		closesocket(sock);
		
		/*
		cout << "输出link" << endl;
		unordered_map<string, vector<string> >::iterator iter;
		for (iter = link.begin(); iter != link.end(); iter++)
		{
			cout << "输出"<<iter->first << ":"<<endl;
			for (auto u : iter->second)
			{
				cout << u << endl;
				//写文件，可能会需要去重
			}

		}
		*/
		unordered_map<string, unsigned long int>::iterator it;
		for (it = BianHao.begin(); it != BianHao.end(); it++)
		{
			cout << it->first << "    :    " << it->second << endl;
		}
		cout << BianHao.size() << endl;
	}
	/*
	unordered_map<string, vector<string> >::iterator iter;
	FILE* resultFile = fopen("url.txt", "w");
	for (iter = link.begin(); iter != link.end(); iter++)
	{
		for (auto u:iter->second)
		{
			fprintf(resultFile, "%s %ld %ld\n", (char*)(iter->first.c_str()),BianHao.find(iter->first)->second, BianHao.find(u)->second);
			//写文件，可能会需要去重
		}
		
	}
	*/
	cout << "总共找到：" << summery << endl;
	cout << BianHao.size() << endl;
	cout << BianHao1.size() << endl;
	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "\n此程序的运行时间为" << totaltime << "秒！" << endl;
	getchar();
	return 0;
}