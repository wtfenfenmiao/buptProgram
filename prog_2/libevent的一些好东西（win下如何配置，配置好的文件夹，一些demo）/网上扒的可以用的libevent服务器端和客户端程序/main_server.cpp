// ConsoleApplication5.cpp : 定义控制台应用程序的入口点。  
//  

//#include "stdafx.h"  
#include <event2/listener.h>  
#include <event2/event.h>  
#include <event2/event_struct.h>  
#include <event2/bufferevent.h>  
#include <event2/buffer.h>  
#include <signal.h>  

#pragma comment(lib, "libevent.lib")  
#pragma comment(lib, "libevent_core.lib")  
#pragma comment(lib, "libevent_extras.lib")  
#pragma comment(lib, "ws2_32.lib")  

static const char MESSAGE[] = "hello new connection!";

void read_cb(struct bufferevent *bev, void *ctx)
{
	struct evbuffer* input = bufferevent_get_input(bev);
	char buffer[1024] = { 0 };
	int n = 0;
	while ((n = evbuffer_remove(input, buffer, sizeof(buffer))) > 0)
	{
		printf("read_cb:%s\n", buffer);
	}

	char* msg = "我是服务器发来的信息";
	bufferevent_write(bev, msg, strlen(msg));
}

void write_cb(struct bufferevent *bev, void *ctx)
{
	struct evbuffer* output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0)
	{
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
	/* None of the other events can happen here, since we haven't enabled
	* timeouts */
	bufferevent_free(bev);
}

void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sa, int socklen, void *user_data)
{
	struct event_base* base = (struct event_base*)user_data;

	struct bufferevent* bev = NULL;
	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

	bufferevent_setcb(bev, read_cb, write_cb, event_cb, NULL);

	bufferevent_enable(bev, EV_WRITE | EV_READ);

	//bufferevent_disable(bev, EV_READ);  

	bufferevent_write(bev, MESSAGE, strlen(MESSAGE));
}

void signal_cb(evutil_socket_t fd, short events, void *user_data)
{
	struct event_base* base = (struct event_base*)user_data;

	struct timeval delay = { 2, 0 };

	event_base_loopexit(base, &delay);
}

int main(int argc, char* argv[])
{
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);

	/*struct event_config* cfg = event_config_new();
	event_config_set_flag(cfg, EVENT_BASE_FLAG_STARTUP_IOCP);
	struct event_base* base = event_base_new_with_config(cfg);
	event_config_free(cfg);*/

	struct event_base* base = event_base_new();
	//const char* method = event_base_get_method(base);  

	struct sockaddr_in listen_addr = { 0 };
	memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_addr.s_addr = htonl(0x7f000001);
	//inet_pton(AF_INET, "127.0.0.1", &listen_addr.sin_addr.s_addr);  
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_port = htons(8981);

	struct evconnlistener* listener = evconnlistener_new_bind(base,
		listener_cb,
		(void*)base,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
		-1,
		(const sockaddr*)&listen_addr,
		sizeof(listen_addr));

	//struct event * signal_event = evsignal_new(base, SIGINT, signal_cb, (void*)base);  
	//event_add(signal_event, NULL);  

	event_base_dispatch(base);

	evconnlistener_free(listener);

	//event_free(signal_event);  

	event_base_free(base);

	WSACleanup();

	return 0;
}
