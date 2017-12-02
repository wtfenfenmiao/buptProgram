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
#include <WinSock2.h>
#include "queue.h"
#include <iostream>
#include <string>
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
int max_conn = 1023;     //这个数不能大于1000，不知道为什么
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
		//cout << "bloom析构" << endl;
		//bitmap.~bitMap();
	}
	bool ifContainAndInsert(string thisUrl)          //这里用模板类一定是好的，什么都通用，然而我还是先用基本的吧.......
	{
		//cout << "判断：" << thisUrl << endl;
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
								//cout <<j<<":"<< index[j] << endl;
		}
		//getchar();
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
		//cout << "空的！" << endl;
		//cout << url << endl;
		//getchar();
		return " ";
	}
	else
	{
		pos += 1;
	}
	sscanf(pos, "%[^/]%s", host, othPath);    //从一个字符串中读进与指定格式相符的数据
	return (string)host;
}
/*
//目前不需要
pair<string, string> getReqUrl(char *url)
{
pair<string, string> re;
re.first = "";
re.second = "";
char *pos = strstr(url, "http://");    //strstr(str1,str2),判断str2是不是str1的字串，是的话就返回str2在str1中首次出现的地址，不是的话返回NULL
if (pos == NULL)
{
return re;
}
else
{
pos += 7;
}
sscanf(pos, "%[^/]%s", host, othPath);    //从一个字符串中读进与指定格式相符的数据
re.first = host;
re.second = othPath;
return re;
}
*/
//和解析配套的socket连接，目前不需要


void http_request_done(struct evhttp_request *req, void *arg) {
	//cout << "have?" << endl;
	//getchar();

	//cout << "total1:" << total << endl;
	if (req == NULL)
	{
		//evhttp_request_free(req);
		//--total;
		//cout << "have1?" << endl;
		return;     //如果这次请求失败，就什么都不干了
	}
	/*
	if (req)
	{
	cout << "这里连接了req2" << endl;
	cout << req << endl;
	getchar();
	}
	if (req->input_buffer)
	{
	cout << "这里有buffer2" << endl;
	cout << req->input_buffer << endl;
	getchar();
	}

	if (req->evcon == NULL)
	{
	cout << "free3" << endl;
	//getchar();
	}
	cout << "have2?" << endl;
	*/
	//cout << "there8" << endl;
	int resCode = evhttp_request_get_response_code(req);
	//cout << "状态码：" << resCode << endl;
	string thisUrl;
	string thisHtml;
	thisUrl = evhttp_request_get_uri(req);
	thisUrl = thisUrl.substr(6);

	//cout << "simThisUrl:" << thisUrl << endl;
	//unordered_map<string, unsigned long int>::iterator iter1 = BianHao.find(thisUrl);
	//unsigned long int numtemp = iter1->second;
	if (resCode != 200)
	{
		//cout << "测试1" << endl;
		--summery;
		//BianHao.erase(iter1);
		//BianHao1.erase(BianHao1.find(numtemp));
		//cout << "要退出！" << endl;
		//event_base_loopexit((struct event_base*)arg, NULL);
		//evhttp_request_free(req);
		char buf[1024];
		int n;
		while ((n = evbuffer_remove(req->input_buffer, &buf, sizeof(buf) - 1)) > 0)
		{

		}
		/*
		cout << "have3?" << endl;
		if (req->evcon == NULL)
		{
		cout << "free1" << endl;
		//getchar();
		}
		if (req->evcon)
		{
		evhttp_connection_free(req->evcon);
		}

		if (req->evcon == NULL)
		{
		cout << "free" << endl;
		//getchar();
		}
		//getchar();
		//delete req;
		//req = NULL;
		*/
		if (total > 0)
			--total;
		else
			total = 0;
		return;
	}
	//getchar();
	//struct evkeyvalq* headers = evhttp_request_get_input_headers(remote_rsp);
	//struct evkeyval* header;
	//header = headers->tqh_first;
	//TAILQ_FOREACH(header, headers, next)
	//{
	//	fprintf(stderr, "< %s: %s\n", header->key, header->value);
	//}
	//fprintf(stderr, "< \n");
	//cout << "have4?" << endl;
	BianHao.insert(make_pair(thisUrl, ++countBianHao));
	//BianHao1.insert(make_pair(countBianHao, thisUrl));
	char buf[1024];
	int n;
	while ((n = evbuffer_remove(req->input_buffer, &buf, sizeof(buf) - 1)) > 0)
	{
		buf[n] = '\0';
		thisHtml += string(buf);
		//printf("%s", buf);
	}
	//cout << "have5?" << endl;
	//int s = evbuffer_remove(req->input_buffer, &buf, sizeof(buf) - 1);
	//buf[s] = '\0';
	//printf("%s", buf);
	// terminate event_base_dispatch()
	//event_base_loopbreak((struct event_base *)arg);
	//cout << "simthisHtml:" << thisHtml << endl;
	//getchar();

	thisURLextraction(thisHtml, thisUrl, countBianHao);
	//delete req;
	//req == NULL;
	/*
	if (req->evcon == NULL)
	{
	cout << "free1" << endl;
	//getchar();
	}
	if (req->evcon)
	{
	evhttp_connection_free(req->evcon);
	}
	if (req->evcon == NULL)
	{
	cout << "free" << endl;
	//getchar();
	}
	*/
	if (total > 0)
		--total;
	else
		total = 0;
	//cout << "have6?" << endl;
	//evhttp_request_free(req);
	//cout << "队列大小：" << UrlQueue.size() << endl;

	return;
}

void RemoteReadCallback(struct evhttp_request* remote_rsp, void* arg)
{
	cout << "there1" << endl;
	//event_base_loopexit((struct event_base*)arg, NULL);    
	//cout << "退出了" << endl;
	//getchar();
}

int ReadHeaderDoneCallback(struct evhttp_request* remote_rsp, void* arg)
{
	cout << "there2" << endl;

	return 0;
}

void ReadChunkCallback(struct evhttp_request* remote_rsp, void* arg)
{
	cout << "there3" << endl;
	getchar();
	string thisUrl;
	thisUrl = evhttp_request_get_uri(remote_rsp);
	cout << "readchunkThisUrl:" << thisUrl << endl;

	//cout << "readchunkThisHost:" << thisHost << endl;
	//thisUrl = thisHost + thisUrl;
	//cout << "readChunk中thisUrl:" <<thisUrl<<endl;
	getchar();



	/*
	cout << "编号：" << BianHao.find(urlname)->second << endl;
	string name = to_string(BianHao.find(urlname)->second) + ".txt";
	FILE* resultFile = fopen(name.c_str(), "a+");   //附加，否则写一次清一次
	*/
	char buf[4096];
	struct evbuffer* evbuf = evhttp_request_get_input_buffer(remote_rsp);
	int n = 0;
	while ((n = evbuffer_remove(evbuf, buf, 4096)) > 0)
	{
		fwrite(buf, n, 1, stdout);
		//fwrite(buf, n, 1, resultFile);        //这个东西，很快！
		cout << "1个！" << endl;
		getchar();
	}

	//cout <<"chunk中thisHtml:" <<thisHtml << endl;
	getchar();
	//cout << thisHtml.size() << endl;
	getchar();
	//thisURLextraction(thisHtml, thisUrl);
	//fclose(resultFile);
}

void RemoteRequestErrorCallback(enum evhttp_request_error error, void* arg)
{
	//cout << "there4" << endl;
	//cout << error << endl;    //明天从这开始查！
	cout << "request failed" << endl;
	cout << ++failnum << endl;
	//cout << con << endl;
	//getchar();
	//event_base_loopexit((struct event_base*)arg, NULL);
}

void RemoteConnectionCloseCallback(struct evhttp_connection* connection, void* arg)
{
	//cout << "关了" << endl;
	//cout << connection << endl;
	//delete connection;
	//connection = NULL;
	//if (connection == NULL)
	//{
	//cout << "end" << endl;
	//}
	//getchar();
	//evhttp_connection_free(connection);
	//cout << "there5" << endl;
	//fprintf(stderr, "remote connection closed\n");
	//event_base_loopexit((struct event_base*)arg, NULL);
}



//URL提取，用DFA.switch就能实现
//这个是给定一个Html页面，进行抽取URL
void thisURLextraction(string & thisHtml, string & thisUrl, int & No)
{
	//int thisco = 0;
	//string name = "resultfile/" + to_string(No) + ".txt";
	string hostt;
	//cout << "name:"<<name << endl;
	//FILE* resultFile = fopen(name.c_str(), "a+");
	/*
	if (resultFile == NULL)
	{
	cout << "no such strFile!" << endl;
	getchar();
	}
	*/
	/*
	if (thisHtml == "")    //之前的URL无效，这里就不抽取了
	{
	return;
	}
	*/
	/*这三句干啥的已经不知道了
	//vector<string> vectorInit;
	//unsigned int thisUrlNum = BianHao.find(thisUrl)->second;
	//link.insert(make_pair(thisUrl, vectorInit));
	*/
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
			case '\n':state = 8; break;
			default:state = 8; tempUrl += input; break;
			}
			break;
		case 9:
			//cout << "case 9" << endl;
			//cout << "匹配！" << endl;      //这里，一边匹配，一边加链接关系，一边布隆过滤器去重
			//++thisco;
			//cout << tempUrl << endl;
			//fprintf(resultFile, "%s\n", tempUrl.c_str());
			resultfile[No].push_back(tempUrl);  //?行吗
												//getchar();
												//这个if后加的，看看能不能快一点
			hostt = getReqUrl((char*)tempUrl.c_str());   //这里有bug
														 //cout << tempUrl << endl;
														 //cout << hostt << endl;
			if (hostt == "news.sohu.com")
			{


				bool flag = bloom->ifContainAndInsert(tempUrl);  //flag是true，就是这个Url之前没有过，但是不检查这个URL到底有没有
																 //cout << "flag:" << flag << endl;
																 /*
																 if (!flag)
																 {
																 cout << "已存在！" << endl;
																 cout <<"queue大小：" <<UrlQueue.size() << endl;
																 cout << "计数大小：" << summery << endl;
																 getchar();
																 }
																 */
				if (flag)
				{
					//cout << "此URL插入！" << endl;
					++summery;
					//cout << "there23333" << endl;
					//getchar();
					//这个Url之前没有过，加入队列，这个队列只爬取用，防止爬重了	
					//BianHao.insert(make_pair(tempUrl, ++countBianHao));
					//BianHao1.insert(make_pair(countBianHao, tempUrl));
					UrlQueue.push(tempUrl);
					//cout << "there2" << endl;
				}
				//.push_back(tempUrl);
			}
			state = 0;
			tempUrl = "";
			break;
		}
		i++;
	}
	//cout <<"这个URL的链接总数："<< thisco << endl;
	//fclose(resultFile);
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
	//int testsum = 0;
	bloom = new bloomFilter(10000000);    //bloom过滤器的位数，这个就是10000000*8
	cout << "test there" << endl;
	clock_t start, finish;
	double totaltime;
	start = clock();
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);
	struct event_base *base;
	base = event_base_new();
	string beginUrl = "/news.sohu.com/";
	//BianHao.insert(make_pair(beginUrl, ++countBianHao));
	//BianHao1.insert(make_pair(countBianHao, beginUrl));
	UrlQueue.push(beginUrl);
	++summery;
	bloom->ifContainAndInsert(beginUrl);



	struct evhttp_connection* conn;
	conn = evhttp_connection_base_new(base, NULL, ip, port);
	struct evhttp_request *req;
	string teUrl = UrlQueue.front();
	//cout << teUrl << endl;
	UrlQueue.pop();

	req = evhttp_request_new(http_request_done, base);
	evhttp_request_set_error_cb(req, RemoteRequestErrorCallback);
	evhttp_connection_set_closecb(conn, RemoteConnectionCloseCallback, base);
	evhttp_add_header(req->output_headers, "Host", "news.sohu.com");
	evhttp_add_header(req->output_headers, "Connection", "close");
	evhttp_make_request(conn, req, EVHTTP_REQ_GET, ("http:/" + teUrl).c_str());
	/*
	if (req->evcon)
	{
	cout << "这里连接了" << endl;
	cout << req->evcon << endl;
	getchar();
	}
	if (req)
	{
	cout << "这里连接了req" << endl;
	cout << req << endl;
	getchar();
	}
	if (req->input_buffer)
	{
	cout << "这里有buffer" << endl;
	cout << req->input_buffer << endl;
	getchar();
	}
	*/
	++total;
	event_base_dispatch(base);
	event_base_loop(base, EVLOOP_NONBLOCK);
	//evhttp_connection_free(conn);
	//evhttp_request_free(req);

	//cout << "total3:" << total << endl;
	//getchar();
	//event_base_loop(base, EVLOOP_NONBLOCK);

	Sleep(2000);
	cout << "wt1" << endl;
	//getchar();

	while (1)
	{
		cout << BianHao.size() << endl;
		//Sleep(1000);
		if (UrlQueue.empty())
		{   
			++countKong;
			cout << "countKong:"<<countKong << endl;
			if (countKong == 10)
			{
				cout << "要停！" << endl;
				getchar();
				break;		
			}
				
			//cout << "空的！" << endl;
			//cout << "这里启动！" << endl;
			//event_base_loop(base, EVLOOP_NONBLOCK);
			Sleep(1000);
			//if (UrlQueue.empty())
			//{
				//cout << "还是空的！" << endl;
				//break;
			//}
		}
		
		//cout << ++whil << endl;
		//cout << "wt2" << endl;
		//cout << "total:" << total << endl;
		//getchar();
		//if (coo == 3)
		//break;
		while (total<max_conn)
		{
			//cout << "total there:" << total << endl;
			if (UrlQueue.empty())
				break;
			
			//cout << summery << endl;
			//getchar();
			string teUrl = UrlQueue.front();
			//cout <<"注册：" <<teUrl << endl;
			UrlQueue.pop();
			//struct evhttp_connection *conn = evhttp_connection_new("127.0.0.1", 80);
			//evhttp_connection_set_base(conn, base);
			//if (conn)
			//{
			//cout << "释放" << endl;
			//evhttp_connection_free(conn);
			//}
			struct evhttp_connection* conn = evhttp_connection_base_new(base, NULL, ip, port);
			con++;
			struct evhttp_request* req = evhttp_request_new(http_request_done, base);  //访问冲突？谁冲突了？
			evhttp_request_set_error_cb(req, RemoteRequestErrorCallback);   //void evhttp_request_set_error_cb(struct evhttp_request *,
																			//evhttp_request_set_header_cb(req, ReadHeaderDoneCallback);
																			//void(*)(enum evhttp_request_error, void *));
			evhttp_connection_set_closecb(conn, RemoteConnectionCloseCallback, base);
			evhttp_connection_set_timeout(conn, 600);
			evhttp_add_header(req->output_headers, "Host", "news.sohu.com");
			evhttp_add_header(req->output_headers, "Connection", "close");
			evhttp_make_request(conn, req, EVHTTP_REQ_GET, ("http:/" + teUrl).c_str());
			//cout << testsum++ << endl;
			//cout << "req成功？" << evhttp_make_request(conn, req, EVHTTP_REQ_GET, ("http:/" + teUrl).c_str()) << endl;
			/*
			if (req->evcon)
			{
			cout << "这里连接了1" << endl;
			cout << req->evcon << endl;
			getchar();
			}
			if (req)
			{
			cout << "这里连接了req1" << endl;
			cout << req << endl;
			getchar();
			}
			*/
			++total;
			countKong = 0;
			//event_base_loop()函数运行 event_base 直到其中没有已经注册的事件为止
			//cout << "total2:" << total << endl;
			//cout << "queue size:" << UrlQueue.size() << endl;
		}
		//cout << "启动" << endl;
		//Sleep(1000);
		cout << "total:"<<total << endl;
		//if (total != 0)
		//{
			//countKong = 0;
		//}
		
		
		//cout << "启动" << endl;
		event_base_loop(base, EVLOOP_NONBLOCK);//这句话就是用来通知base有新事件到来的，之前一直在加事件但是没有执行，这句话就是告诉base可以去执行这些事件去调用callback函数了
		//Sleep(1000);
		//cout << "之后" << endl;
		//cout << "wait:" << endl;
	}

	cout << "空" << endl;
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

	//event_base_dispatch(base);

	cout << "总共找到：" << summery << endl;
	cout << BianHao.size() << endl;
	//cout << BianHao1.size() << endl;
	delete bloom;
	linkExtract(urltxt);
	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "\n此程序的运行时间为" << totaltime << "秒！" << endl;
	getchar();
	getchar();
	return 0;
}