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
int max_conn = 10000;     //epollû�в������ƣ�selectҪ1024������
int total = 0;
int whil = 0;
//struct event_base *base;
int hashNum = 9;
int countBianHao = 0;   //�ӱ���õ�
unsigned long int summery = 0;
unordered_map<string, int> BianHao;
unordered_map<int, vector<string> > resultfile;
//unordered_map<unsigned long int, string> BianHao1;
queue<string> UrlQueue;   //ֻ��ȡ�õĶ���
						  //��¡������ʵ��
						  //new�ĸ�ʽ��
						  //��ʽ1��ָ�������=new ���ͱ�ʶ��;
						  //��ʽ2��ָ�������=new ���ͱ�ʶ������ʼֵ��;
						  //��ʽ3��ָ�������=new ���ͱ�ʶ��[�ڴ浥Ԫ����];

						  //��char����8λ01��
class bitMap
{
public:
	//û�в����Ĺ��캯��
	bitMap()
	{
		bitmap = NULL;
		size = 0;
	}
	//������size�Ĺ��캯��
	bitMap(unsigned int size)
	{
		bitmap = NULL;
		bitmap = new char[size];
		if (bitmap == NULL)
		{
			cout << "����ʧ�ܣ�" << endl;
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
			cout << "���ó���ʧ�ܣ�" << endl;
		}
		else
		{
			memset(bitmap, 0x0, size * sizeof(char));
			this->size = size;
		}
	}
	~bitMap()
	{
		//cout << "bitmap����" << endl;
		delete[] bitmap;
	}

	//��index��һλ�ó�1��λָ����bitλ
	bool bitMapSet(unsigned int index)   //����indexҪ��0��ʼ�������Ǵ�1��ʼ��Ȼ���±ߵ�������裬����λ���㣬�Լ����Ż�С������һ�¾�������......
	{
		int sizeofChar = sizeof(char);
		int addr = index / sizeofChar;    //�ڵڼ���char��
		int addrOffset = index%sizeofChar;   //�ڵ�addr��char�е�λ��
		unsigned char temp = 0x01 << addrOffset;
		bitmap[addr] |= temp;
		return true;
		//}	
	}
	//��ѯindex��һλ�ǲ���1
	//����true���������һλ��1������0���������һλ��С����Χ�ˣ�����û����Χ����һλ��0
	//���߲����ǳ���Χ�����⣬����true��һλ��0������false��һλ��1
	bool bitMapGet(unsigned int index)
	{
		int sizeofChar = sizeof(char);
		int addr = index / sizeofChar;
		int addrOffset = index%sizeofChar;   //�ڵ�addr��char�е�λ��
		unsigned char temp = 0x01 << addrOffset;
		return ((bitmap[addr] & temp) == 0) ? true : false;   //��һλ��0������true����һλ��1������false														  //}
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
		//cout << "bloom��ʼ��" << endl;
	}

	~bloomFilter()
	{
		
	}
	bool ifContainAndInsert(string thisUrl)          //������ģ����һ���Ǻõģ�ʲô��ͨ�ã�Ȼ���һ������û����İ�.......
	{
		unsigned int *index;
		index = new unsigned int[hashNum];
		char* htmlTemp = (char*)thisUrl.c_str();
		int length = thisUrl.length();
		index[0] = RSHash(htmlTemp, length);
		index[1] = JSHash(htmlTemp, length);    //�������ǽ���hash
		index[2] = PJWHash(htmlTemp, length);
		index[3] = ELFHash(htmlTemp, length);
		index[4] = BKDRHash(htmlTemp, length);
		index[5] = SDBMHash(htmlTemp, length);
		index[6] = DJBHash(htmlTemp, length);
		index[7] = DEKHash(htmlTemp, length);
		index[8] = APHash(htmlTemp, length);

		for (int j = 0; j < hashNum; ++j)
		{
			index[j] %= size;   //ӳ�䵽bitmap�Ĵ�С��
		}
		int i = 0;
		while (i < hashNum)
		{
			if (bitmap.bitMapGet(index[i]))     //���������һλ��0�ˣ���break
			{
				break;
			}
			++i;
		}
		if (i < hashNum)    //��ǰbreak�ˣ�֤���ҵ�����0��λ
		{
			while (i < hashNum)
			{
				bitmap.bitMapSet(index[i]);
				++i;
			}
			delete[] index;
			return true;   //֮ǰû�У�return true
		}
		delete[] index;
		return false;  //֮ǰ���ˣ�return false

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
	char *pos = strstr(url, "/");    //strstr(str1,str2),�ж�str2�ǲ���str1���Ӵ����ǵĻ��ͷ���str2��str1���״γ��ֵĵ�ַ�����ǵĻ�����NULL
	if (pos == NULL)
	{
		return " ";
	}
	else
	{
		pos += 1;
	}
	sscanf(pos, "%[^/]%s", host, othPath);    //��һ���ַ����ж�����ָ����ʽ���������
	return (string)host;
}


void http_request_done(struct evhttp_request *req, void *arg) {
	if (req == NULL)
	{
		return;     //����������ʧ�ܣ���ʲô��������
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



//URL��ȡ����DFA.switch����ʵ��
//����Ǹ���һ��Htmlҳ�棬���г�ȡURL
void thisURLextraction(string & thisHtml, string & thisUrl, int & No)
{
	string hostt;
	int state = 0;
	int i = 0;
	int len = thisHtml.length();
	//cout << "���ȣ�" << len << endl;
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
				bool flag = bloom->ifContainAndInsert(tempUrl);  //flag��true���������Url֮ǰû�й������ǲ�������URL������û��
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
	bloom = new bloomFilter(10000000);    //bloom��������λ�����������10000000*8
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
			struct evhttp_request* req = evhttp_request_new(http_request_done, base);  //���ʳ�ͻ��˭��ͻ�ˣ�
			evhttp_request_set_error_cb(req, RemoteRequestErrorCallback);   
			evhttp_connection_set_closecb(conn, RemoteConnectionCloseCallback, base);
			evhttp_connection_set_timeout(conn, 600);
			evhttp_add_header(req->output_headers, "Host", "news.sohu.com");
			evhttp_add_header(req->output_headers, "Connection", "close");
			evhttp_make_request(conn, req, EVHTTP_REQ_GET, ("http:/" + teUrl).c_str());
			countKong = 0;
			++total;
		}
		event_base_loop(base, EVLOOP_NONBLOCK);  //��仰��������֪ͨbase�����¼������ģ�֮ǰһֱ�ڼ��¼�����û��ִ�У���仰���Ǹ���base����ȥִ����Щ�¼�ȥ����callback������
	}
	delete bloom;
	linkExtract(urltxt);
	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	return 0;
}