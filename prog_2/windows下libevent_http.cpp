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
int max_conn = 1023;     //��������ܴ���1000����֪��Ϊʲô
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
										  /*
										  if (addr >= size)
										  {
										  return false;       //û��set��
										  }
										  else
										  {
										  */
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
		/*
		if (addr >= size)   //����ж��ǲ����б�Ҫ���о�������ж�û��Ҫ���б�Ҫ�Ļ�Ҳ������ô�õ�......���Ҫȥ��hash����
		{
		return false;
		}
		else
		{
		*/
		int addrOffset = index%sizeofChar;   //�ڵ�addr��char�е�λ��
		unsigned char temp = 0x01 << addrOffset;
		//return bitmap[addr]&temp==0?false:true;
		return ((bitmap[addr] & temp) == 0) ? true : false;   //��һλ��0������true����һλ��1������false
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
		//cout << "bloom��ʼ��" << endl;
	}

	~bloomFilter()
	{
		//cout << "bloom����" << endl;
		//bitmap.~bitMap();
	}
	bool ifContainAndInsert(string thisUrl)          //������ģ����һ���Ǻõģ�ʲô��ͨ�ã�Ȼ���һ������û����İ�.......
	{
		//cout << "�жϣ�" << thisUrl << endl;
		//cout << "bloom�ж��Ƿ����" << endl;
		unsigned int *index;
		index = new unsigned int[hashNum];
		//��ίԱ��ppt��һ������ô���ܲ���ô���д9��......
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
			//cout << j << ":" << endl;
			//cout << index[j] << endl;
			index[j] %= size;   //ӳ�䵽bitmap�Ĵ�С��
								//cout << size << endl;
								//cout <<j<<":"<< index[j] << endl;
		}
		//getchar();
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
		//cout << "�յģ�" << endl;
		//cout << url << endl;
		//getchar();
		return " ";
	}
	else
	{
		pos += 1;
	}
	sscanf(pos, "%[^/]%s", host, othPath);    //��һ���ַ����ж�����ָ����ʽ���������
	return (string)host;
}
/*
//Ŀǰ����Ҫ
pair<string, string> getReqUrl(char *url)
{
pair<string, string> re;
re.first = "";
re.second = "";
char *pos = strstr(url, "http://");    //strstr(str1,str2),�ж�str2�ǲ���str1���ִ����ǵĻ��ͷ���str2��str1���״γ��ֵĵ�ַ�����ǵĻ�����NULL
if (pos == NULL)
{
return re;
}
else
{
pos += 7;
}
sscanf(pos, "%[^/]%s", host, othPath);    //��һ���ַ����ж�����ָ����ʽ���������
re.first = host;
re.second = othPath;
return re;
}
*/
//�ͽ������׵�socket���ӣ�Ŀǰ����Ҫ


void http_request_done(struct evhttp_request *req, void *arg) {
	//cout << "have?" << endl;
	//getchar();

	//cout << "total1:" << total << endl;
	if (req == NULL)
	{
		//evhttp_request_free(req);
		//--total;
		//cout << "have1?" << endl;
		return;     //����������ʧ�ܣ���ʲô��������
	}
	/*
	if (req)
	{
	cout << "����������req2" << endl;
	cout << req << endl;
	getchar();
	}
	if (req->input_buffer)
	{
	cout << "������buffer2" << endl;
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
	//cout << "״̬�룺" << resCode << endl;
	string thisUrl;
	string thisHtml;
	thisUrl = evhttp_request_get_uri(req);
	thisUrl = thisUrl.substr(6);

	//cout << "simThisUrl:" << thisUrl << endl;
	//unordered_map<string, unsigned long int>::iterator iter1 = BianHao.find(thisUrl);
	//unsigned long int numtemp = iter1->second;
	if (resCode != 200)
	{
		//cout << "����1" << endl;
		--summery;
		//BianHao.erase(iter1);
		//BianHao1.erase(BianHao1.find(numtemp));
		//cout << "Ҫ�˳���" << endl;
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
	//cout << "���д�С��" << UrlQueue.size() << endl;

	return;
}

void RemoteReadCallback(struct evhttp_request* remote_rsp, void* arg)
{
	cout << "there1" << endl;
	//event_base_loopexit((struct event_base*)arg, NULL);    
	//cout << "�˳���" << endl;
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
	//cout << "readChunk��thisUrl:" <<thisUrl<<endl;
	getchar();



	/*
	cout << "��ţ�" << BianHao.find(urlname)->second << endl;
	string name = to_string(BianHao.find(urlname)->second) + ".txt";
	FILE* resultFile = fopen(name.c_str(), "a+");   //���ӣ�����дһ����һ��
	*/
	char buf[4096];
	struct evbuffer* evbuf = evhttp_request_get_input_buffer(remote_rsp);
	int n = 0;
	while ((n = evbuffer_remove(evbuf, buf, 4096)) > 0)
	{
		fwrite(buf, n, 1, stdout);
		//fwrite(buf, n, 1, resultFile);        //����������ܿ죡
		cout << "1����" << endl;
		getchar();
	}

	//cout <<"chunk��thisHtml:" <<thisHtml << endl;
	getchar();
	//cout << thisHtml.size() << endl;
	getchar();
	//thisURLextraction(thisHtml, thisUrl);
	//fclose(resultFile);
}

void RemoteRequestErrorCallback(enum evhttp_request_error error, void* arg)
{
	//cout << "there4" << endl;
	//cout << error << endl;    //������⿪ʼ�飡
	cout << "request failed" << endl;
	cout << ++failnum << endl;
	//cout << con << endl;
	//getchar();
	//event_base_loopexit((struct event_base*)arg, NULL);
}

void RemoteConnectionCloseCallback(struct evhttp_connection* connection, void* arg)
{
	//cout << "����" << endl;
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



//URL��ȡ����DFA.switch����ʵ��
//����Ǹ���һ��Htmlҳ�棬���г�ȡURL
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
	if (thisHtml == "")    //֮ǰ��URL��Ч������Ͳ���ȡ��
	{
	return;
	}
	*/
	/*�������ɶ���Ѿ���֪����
	//vector<string> vectorInit;
	//unsigned int thisUrlNum = BianHao.find(thisUrl)->second;
	//link.insert(make_pair(thisUrl, vectorInit));
	*/
	int state = 0;
	int i = 0;
	int len = thisHtml.length();
	//cout << "���ȣ�" << len << endl;
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
			//cout << "ƥ�䣡" << endl;      //���һ��ƥ�䣬һ�߼����ӹ�ϵ��һ�߲�¡������ȥ��
			//++thisco;
			//cout << tempUrl << endl;
			//fprintf(resultFile, "%s\n", tempUrl.c_str());
			resultfile[No].push_back(tempUrl);  //?����
												//getchar();
												//���if��ӵģ������ܲ��ܿ�һ��
			hostt = getReqUrl((char*)tempUrl.c_str());   //������bug
														 //cout << tempUrl << endl;
														 //cout << hostt << endl;
			if (hostt == "news.sohu.com")
			{


				bool flag = bloom->ifContainAndInsert(tempUrl);  //flag��true���������Url֮ǰû�й������ǲ�������URL������û��
																 //cout << "flag:" << flag << endl;
																 /*
																 if (!flag)
																 {
																 cout << "�Ѵ��ڣ�" << endl;
																 cout <<"queue��С��" <<UrlQueue.size() << endl;
																 cout << "������С��" << summery << endl;
																 getchar();
																 }
																 */
				if (flag)
				{
					//cout << "��URL���룡" << endl;
					++summery;
					//cout << "there23333" << endl;
					//getchar();
					//���Url֮ǰû�й���������У��������ֻ��ȡ�ã���ֹ������	
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
	//cout <<"���URL������������"<< thisco << endl;
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
	bloom = new bloomFilter(10000000);    //bloom��������λ�����������10000000*8
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
	cout << "����������" << endl;
	cout << req->evcon << endl;
	getchar();
	}
	if (req)
	{
	cout << "����������req" << endl;
	cout << req << endl;
	getchar();
	}
	if (req->input_buffer)
	{
	cout << "������buffer" << endl;
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
				cout << "Ҫͣ��" << endl;
				getchar();
				break;		
			}
				
			//cout << "�յģ�" << endl;
			//cout << "����������" << endl;
			//event_base_loop(base, EVLOOP_NONBLOCK);
			Sleep(1000);
			//if (UrlQueue.empty())
			//{
				//cout << "���ǿյģ�" << endl;
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
			//cout <<"ע�᣺" <<teUrl << endl;
			UrlQueue.pop();
			//struct evhttp_connection *conn = evhttp_connection_new("127.0.0.1", 80);
			//evhttp_connection_set_base(conn, base);
			//if (conn)
			//{
			//cout << "�ͷ�" << endl;
			//evhttp_connection_free(conn);
			//}
			struct evhttp_connection* conn = evhttp_connection_base_new(base, NULL, ip, port);
			con++;
			struct evhttp_request* req = evhttp_request_new(http_request_done, base);  //���ʳ�ͻ��˭��ͻ�ˣ�
			evhttp_request_set_error_cb(req, RemoteRequestErrorCallback);   //void evhttp_request_set_error_cb(struct evhttp_request *,
																			//evhttp_request_set_header_cb(req, ReadHeaderDoneCallback);
																			//void(*)(enum evhttp_request_error, void *));
			evhttp_connection_set_closecb(conn, RemoteConnectionCloseCallback, base);
			evhttp_connection_set_timeout(conn, 600);
			evhttp_add_header(req->output_headers, "Host", "news.sohu.com");
			evhttp_add_header(req->output_headers, "Connection", "close");
			evhttp_make_request(conn, req, EVHTTP_REQ_GET, ("http:/" + teUrl).c_str());
			//cout << testsum++ << endl;
			//cout << "req�ɹ���" << evhttp_make_request(conn, req, EVHTTP_REQ_GET, ("http:/" + teUrl).c_str()) << endl;
			/*
			if (req->evcon)
			{
			cout << "����������1" << endl;
			cout << req->evcon << endl;
			getchar();
			}
			if (req)
			{
			cout << "����������req1" << endl;
			cout << req << endl;
			getchar();
			}
			*/
			++total;
			countKong = 0;
			//event_base_loop()�������� event_base ֱ������û���Ѿ�ע����¼�Ϊֹ
			//cout << "total2:" << total << endl;
			//cout << "queue size:" << UrlQueue.size() << endl;
		}
		//cout << "����" << endl;
		//Sleep(1000);
		cout << "total:"<<total << endl;
		//if (total != 0)
		//{
			//countKong = 0;
		//}
		
		
		//cout << "����" << endl;
		event_base_loop(base, EVLOOP_NONBLOCK);//��仰��������֪ͨbase�����¼������ģ�֮ǰһֱ�ڼ��¼�����û��ִ�У���仰���Ǹ���base����ȥִ����Щ�¼�ȥ����callback������
		//Sleep(1000);
		//cout << "֮��" << endl;
		//cout << "wait:" << endl;
	}

	cout << "��" << endl;
	/*
	unordered_map<string, vector<string> >::iterator iter;
	FILE* resultFile = fopen("url.txt", "w");
	for (iter = link.begin(); iter != link.end(); iter++)
	{
	for (auto u:iter->second)
	{
	fprintf(resultFile, "%s %ld %ld\n", (char*)(iter->first.c_str()),BianHao.find(iter->first)->second, BianHao.find(u)->second);
	//д�ļ������ܻ���Ҫȥ��
	}

	}
	*/

	//event_base_dispatch(base);

	cout << "�ܹ��ҵ���" << summery << endl;
	cout << BianHao.size() << endl;
	//cout << BianHao1.size() << endl;
	delete bloom;
	linkExtract(urltxt);
	finish = clock();
	totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "\n�˳��������ʱ��Ϊ" << totaltime << "�룡" << endl;
	getchar();
	getchar();
	return 0;
}