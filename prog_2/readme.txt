���������������Լ���ȡ���ӹ�ϵ�ĳ���ƽ̨��һ�����Ǹɵ�һ���¡����г����ʱ��ip�Ͷ˿ڿ�ʵ�ʣ�������ε�ip��ʵ���ҷ������ģ��˿���80
#crawler:linux��û�д�ӡ��Ϣ��
#libevent_http:windows���д�ӡ��Ϣ�ģ�Ӧ��û���д�Ļ��ʹ�crawler��windows���ƣ�crawlerһ��û�д�
����Ϊpagerank�ĳ��򣬶���linux�µģ���windows��Ĳ������Ծ�ֻ����linux�µ�

pagerankShouLian:linux�´�ӡ����ֵ
��������Ϣ��pagerank��linux�´���ӡ��Ϣ�ģ������pagerank�汾�����е��ϣ����������pagerankShouLianЧ�ʸ�

��ΰѳ����windows�Ƶ�linux��
    �����cpp:
	ȥ����
	    #include <WinSock2.h>
	    include "queue.h"
	    WSADATA wsa_data;
	    WSAStartup(MAKEWORD(2, 2), &wsa_data);
        ��ӣ�
    	    linux����������ص���صģ�
	    #include <unistd.h>
	    #include <arpa/inet.h>
	    #include <sys/socket.h>
	    #include <netinet/in.h>
            ��strstrһЩ����ֵֹĺ�����صģ�
	    #include <string.h>
	���ģ�
            Sleep�ĳ�sleep.Sleep��1000��1s��Сsleep��1��һ��
	�Ǹ�#include "queue.h"ҲҪȥ�����ڷ�������û�����
    pagerank��cpp��
        ֻ����#include <unistd.h>��#include <string.h>���У���Ϊ���漰�����̵ģ����Ժܺøĵģ��е�ǲ��巴���ܺøģ����Ե�ʱ���Լ����ԣ�
��linux�ϱ��뷽����
    g++ -std=c++11 crawler.cpp -o crawler -levent
    g++ -std=c++11 pagerank.cpp -o pagerank
    g++ -std=c++11 pagerankShouLian.cpp -o pagerank

pagerank��linux��ֻ������linux���ɵ�url.txt��������windows�����ɵ�url.txt!����Ϊlinux�º�windows�µĻ��з��ǲ�һ���ģ�

ע�⣺nginx�Ĳ���������Ҳ����Ҫ�����ô�һ��������һ��