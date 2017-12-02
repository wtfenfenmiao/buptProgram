// client.cpp : 定义控制台应用程序的入口点。  
//  



#include <event2/listener.h>  
#include <event2/event.h>  
#include <event2/event_struct.h>  
#include <event2/bufferevent.h>  
#include <event2/buffer.h>  

#pragma comment(lib, "libevent.lib")  
#pragma comment(lib, "libevent_core.lib")  
#pragma comment(lib, "libevent_extras.lib")  
#pragma comment(lib, "ws2_32.lib")  


void read_cb(struct bufferevent *bev, void *ctx)
{
	char* msg = "林子大了什么鸟都有";
	struct evbuffer* input = bufferevent_get_input(bev);
	struct evbuffer* output = bufferevent_get_output(bev);

	char buffer[1024] = { 0 };
	int n = 0;
	while ((n = evbuffer_remove(input, buffer, sizeof(buffer))) > 0)
	{
		printf("read_cb:%s\n", buffer);
	}

	//bufferevent_write(bev, msg, strlen(msg));  

	evbuffer_add(output, msg, strlen(msg));
}


void write_cb(struct bufferevent *bev, void *ctx)
{
	struct evbuffer *output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0)
	{
		//printf("flushed answer\n");  
		//bufferevent_free(bev);  
	}
}

void event_cb(struct bufferevent *bev, short what, void *ctx)
{
	if (what & BEV_EVENT_EOF)
	{
		printf("Connection closed.\n");
	}
	else if (what & BEV_EVENT_ERROR)
	{
		char sz[100] = { 0 };
		strerror_s(sz, 100, errno);
		printf("Got an error on the connection: %s\n",
			sz);/*XXX win32*/
	}
	else if (what & BEV_EVENT_TIMEOUT)
	{
		printf("Connection timeout.\n");
	}
	else if (what & BEV_EVENT_CONNECTED)
	{
		printf("Connect succeed\n");
		//客户端链接成功后，给服务器发送第一条消息    
		char *reply_msg = "I receive a message from client";
		bufferevent_write(bev, reply_msg, strlen(reply_msg));
		return;
	}
	/* None of the other events can happen here, since we haven't enabled
	* timeouts */
	bufferevent_free(bev);
}


int main(int argc, char* argv[])      //之前是_TCHAR，说未定义标识符，这里可以在项目->属性里改字符集，现在的字符集是多字节字符集，就不想改这个字符集了，所以改成了char
{
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);

	struct sockaddr_in addr = { 0 };
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(0x7f000001);
	//inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);  
	addr.sin_port = htons(8981);

	struct event_base* base = event_base_new();

	bufferevent* bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);

	bufferevent_setcb(bev, read_cb, write_cb, event_cb, NULL);

	int i = bufferevent_socket_connect(bev, (const sockaddr*)&addr, sizeof(addr));

	i = bufferevent_enable(bev, EV_READ | EV_WRITE);

	i = event_base_dispatch(base);

	//bufferevent_free(bev);  

	event_base_free(base);

	return 0;
}
