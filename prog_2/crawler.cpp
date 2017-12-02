#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
//#include <unistd.h>
#include <evhttp.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/bufferevent.h>


#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

//#include <WinSock2.h>
//#include "queue.h"
#include <iostream>
#include <string>
#include <string.h>
#include <unordered_map>
#include <queue>
using namespace std;

unsigned int RSHash(const char* str, unsigned int length);
unsigned int JSHash(const char* str, unsigned int length);
unsigned int PJWHash(const char* str, unsigned int length);
unsigned int ELFHash(const char* str, unsigned int length);
unsigned int BKDRHash(const char* str, unsigned int length);
unsigned int SDBMHash(const char* str, unsigned int length);
unsigned int DJBHash(const char* str, unsigned int length);
unsigned int DEKHash(const char* str, unsigned int length);
unsigned int APHash(const char* str, unsigned int length);
void thisURLextraction(string & thisHtml, string & thisUrl, int & No);



int countKong = 0;
int con = 0;
int failnum = 0;
int max_conn = 10000;     //epoll没有并发限制，select要1024的限制
int total = 0;
int whil = 0;
//struct event_base *base;
int hashNum = 9;
int countBianHao = 0;   //加编号用的
unsigned long int summery = 0;
unordered_map<string, int> BianHao;
unordered_map<int, vector<string> > resultfile;
//unordered_map<unsigned long int, string> BianHao1;
queue<string> UrlQueue;   //只爬取用的队列
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
	bitMap(unsigned int size)
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
	void initbitMap(unsigned int size)
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
	~bitMap()
	{
		//cout << "bitmap析构" << endl;
		delete[] bitmap;
	}

	//把index这一位置成1，位指的是bit位
	bool bitMapSet(unsigned int index)   //这里index要从0开始，而不是从1开始，然后下边的这个步骤，就是位运算，自己跟着画小方格算一下就明白了......
	{
		int sizeofChar = sizeof(char);
		int addr = index / sizeofChar;    //在第几个char里
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
		int addrOffset = index%sizeofChar;   //在第addr个char中的位置
		unsigned char temp = 0x01 << addrOffset;
		return ((bitmap[addr] & temp) == 0) ? true : false;   //这一位是0，返回true，这一位是1，返回false														  //}
	}
private:
	char* bitmap;
	unsigned int size;
};

class bloomFilter
{
public:
	bloomFilter(int size)
	{
		bitmap.initbitMap(size);
		this->size = size;
		//cout << "bloom初始化" << endl;
	}

	~bloomFilter()
	{
		
	}
	bool ifContainAndInsert(string thisUrl)          //这里用模板类一定是好的，什么都通用，然而我还是先用基本的吧.......
	{
		unsigned int *index;
		index = new unsigned int[hashNum];
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
			index[j] %= size;   //映射到bitmap的大小里
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
			delete[] index;
			return true;   //之前没有，return true
		}
		delete[] index;
		return false;  //之前有了，return false

	}
private:
	bitMap bitmap;
	unsigned int size;
};

bloomFilter* bloom;
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

string getReqUrl(char *url)
{
	char host[500];
	char othPath[800];
	char *pos = strstr(url, "/");    //strstr(str1,str2),判断str2是不是str1的子串，是的话就返回str2在str1中首次出现的地址，不是的话返回NULL
	if (pos == NULL)
	{
		return " ";
	}
	else
	{
		pos += 1;
	}
	sscanf(pos, "%[^/]%s", host, othPath);    //从一个字符串中读进与指定格式相符的数据
	return (string)host;
}


void http_request_done(struct evhttp_request *req, void *arg) {
	if (req == NULL)
	{
		return;     //如果这次请求失败，就什么都不干了
	}
	int resCode = evhttp_request_get_response_code(req);
	string thisUrl;
	string thisHtml;
	thisUrl = evhttp_request_get_uri(req);
	thisUrl = thisUrl.substr(6);
	if (resCode != 200)
	{
		--summery;
		char buf[1024];
		int n;
		while ((n = evbuffer_remove(req->input_buffer, &buf, sizeof(buf) - 1)) > 0)
		{

		}
		if (total > 0)
			--total;
		else
			total = 0;
		return;
	}
	BianHao.insert(make_pair(thisUrl, ++countBianHao));
	char buf[1024];
	int n;
	while ((n = evbuffer_remove(req->input_buffer, &buf, sizeof(buf) - 1)) > 0)
	{
		buf[n] = '\0';
		thisHtml += string(buf);
	}
	thisURLextraction(thisHtml, thisUrl, countBianHao);
	if (total > 0)
		--total;
	else
		total = 0;
	return;
}

void RemoteRequestErrorCallback(enum evhttp_request_error error, void* arg)
{
	//cout << "request failed" << endl;
	//cout << ++failnum << endl;
}

void RemoteConnectionCloseCallback(struct evhttp_connection* connection, void* arg)
{
}



//URL提取，用DFA.switch就能实现
//这个是给定一个Html页面，进行抽取URL
void thisURLextraction(string & thisHtml, string & thisUrl, int & No)
{
	string hostt;
	int state = 0;
	int i = 0;
	int len = thisHtml.length();
	//cout << "长度：" << len << endl;
	string tempUrl = "";
	while (i < len)
	{
		char input = thisHtml[i];
		switch (state)
		{
		case 0:
			switch (input)
			{
			case '<':state = 1; break;
			default:state = 0; break;
			}
			break;
		case 1:
			switch (input)
			{
			case 'a':state = 2; break;
			default:state = 0; break;
			}
			break;
		case 2:
			switch (input)
			{
			case 'h':state = 3; break;
			case '>':state = 0; break;
			default:state = 2; break;
			}
			break;
		case 3:
			switch (input)
			{
			case 'r':state = 4; break;
			case '>':state = 0; break;
			default:state = 2; break;
			}
			break;
		case 4:
			switch (input)
			{
			case 'e':state = 5; break;
			case '>':state = 0; break;
			default:state = 2; break;
			}
			break;
		case 5:
			switch (input)
			{
			case 'f':state = 6; break;
			case '>':state = 0; break;
			default:state = 2; break;
			}
			break;
		case 6:
			switch (input)
			{
			case '=':state = 7; break;
			case '>':state = 0; break;
			default:state = 2; break;
			}
			break;
		case 7:
			switch (input)
			{
			case ' ':state = 7; break;
			case '"':state = 10; break;
			default:state = 0; break;
			}
			break;
		case 10:
			switch (input)
			{
			case '/':state = 8; break;
			case '"' || '>' || '#':state = 0; break;
			default:state = 10; break;
			}
			break;
		case 8:
			switch (input)
			{
			case '"':state = 9; break;
			case '>':state = 0; tempUrl = ""; break;
			case '\n':state = 8; break;
			default:state = 8; tempUrl += input; break;
			}
			break;
		case 9:
			resultfile[No].push_back(tempUrl);  
			hostt = getReqUrl((char*)tempUrl.c_str());   
			if (hostt == "news.sohu.com")
			{
				bool flag = bloom->ifContainAndInsert(tempUrl);  //flag是true，就是这个Url之前没有过，但是不检查这个URL到底有没有
				if (flag)
				{
					++summery;
					UrlQueue.push(tempUrl);
				}
			}
			state = 0;
			tempUrl = "";
			break;
		}
		i++;
	}
}


void linkExtract(string urltxt)
{
	FILE* re = fopen(urltxt.c_str(), "a+");
	unordered_map<string, int>::iterator itLink;
	unordered_map<string, int>::iterator itEach;
    for (itLink = BianHao.begin(); itLink != BianHao.end(); itLink++)
	{
		string thisUrl = itLink->first;
		int thisNo = itLink->second;
		fprintf(re, "%s %ld\n", thisUrl.c_str(), thisNo);
	}
    fprintf(re,"\n");
	for (itLink = BianHao.begin(); itLink != BianHao.end(); itLink++)
	{

		bloomFilter* bloomtemp = new bloomFilter(10000000);
		string thisUrl = itLink->first;
		int thisNo = itLink->second;
		vector<string> tempU;
		tempU = resultfile[thisNo];
		for (int i = 0; i<tempU.size(); i++)
		{
			bool flag = bloomtemp->ifContainAndInsert(tempU[i]);
			if (flag)
			{
				itEach = BianHao.find(tempU[i]);
				if (itEach != BianHao.end())
				{
					fprintf(re, "%ld %ld\n", thisNo, itEach->second);
				}
			}
		}
		delete bloomtemp;
	}
	fclose(re);
}






int main(int argc, char* argv[]) {
	const char* ip = argv[1];
	int port = stoi(argv[2]);
	string urltxt = argv[3];
	bloom = new bloomFilter(10000000);    //bloom过滤器的位数，这个就是10000000*8
	clock_t start, finish;
	double totaltime;
	start = clock();
	//WSADATA wsa_data;
	//WSAStartup(MAKEWORD(2, 2), &wsa_data);
	struct event_base *base;
	base = event_base_new();
	string beginUrl = "/news.sohu.com/";
	UrlQueue.push(beginUrl);
	++summery;
	bloom->ifContainAndInsert(beginUrl);
	struct evhttp_connection* conn;
	conn = evhttp_connection_base_new(base, NULL, ip, port);
	struct evhttp_request *req;
	string teUrl = UrlQueue.front();
	UrlQueue.pop();
	req = evhttp_request_new(http_request_done, base);
	evhttp_request_set_error_cb(req, RemoteRequestErrorCallback);
	evhttp_connection_set_closecb(conn, RemoteConnectionCloseCallback, base);
	evhttp_add_header(req->output_headers, "Host", "news.sohu.com");
	evhttp_add_header(req->output_headers, "Connection", "close");
	evhttp_make_request(conn, req, EVHTTP_REQ_GET, ("http:/" + teUrl).c_str());
	++total;
	event_base_dispatch(base);
	event_base_loop(base, EVLOOP_NONBLOCK);

	sleep(2);
	//getchar();

	while (1)
	{
		if (UrlQueue.empty())
		{   
			++countKong;
			if (countKong == 10)
				break;
			sleep(1);
		}
		while (total<max_conn)
		{
			if (UrlQueue.empty())
				break;
			string teUrl = UrlQueue.front();
			UrlQueue.pop();
			struct evhttp_connection* conn = evhttp_connection_base_new(base, NULL, ip, port);
			con++;
			struct evhttp_request* req = evhttp_request_new(http_request_done, base);  //访问冲突？谁冲突了？
			evhttp_request_set_error_cb(req, RemoteRequestErrorCallback);   
			evhttp_connection_set_closecb(conn, RemoteConnectionCloseCallback, base);
			evhttp_connection_set_timeout(conn, 600);
			evhttp_add_header(req->output_headers, "Host", "news.sohu.com");
			evhttp_add_header(req->output_headers, "Connection", "close");
			evhttp_make_request(conn, req, EVHTTP_REQ_GET, ("http:/" + teUrl).c_str());
			countKong = 0;
			++total;
		}
		event_base_loop(base, EVLOOP_NONBLOCK);  //这句话就是用来通知base有新事件到来的，之前一直在加事件但是没有执行，这句话就是告诉base可以去执行这些事件去调用callback函数了
	}
	delete bloom;
	linkExtract(urltxt);
	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	return 0;
}