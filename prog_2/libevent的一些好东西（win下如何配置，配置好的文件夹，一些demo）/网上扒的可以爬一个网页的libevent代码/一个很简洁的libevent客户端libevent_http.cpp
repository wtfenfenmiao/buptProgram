#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
//#include <unistd.h>
#include <evhttp.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/bufferevent.h>
#include <iostream>
#include <WinSock2.h>

using namespace std;


void http_request_done(struct evhttp_request *req, void *arg) {
	fprintf(stderr, "HTTP/1.1 %d %s\n", evhttp_request_get_response_code(req), evhttp_request_get_response_code_line(req)); //这一行可获得状态码
	cout << "there" << endl;
	getchar();
	//struct evkeyvalq* headers = evhttp_request_get_input_headers(remote_rsp);
	//struct evkeyval* header;
	//header = headers->tqh_first;
	//TAILQ_FOREACH(header, headers, next)
	//{
	//	fprintf(stderr, "< %s: %s\n", header->key, header->value);
	//}
	//fprintf(stderr, "< \n");
	char buf[1024];
	int n;
	while ((n = evbuffer_remove(req->input_buffer, &buf, sizeof(buf)-1)) > 0)
	{
		buf[n] = '\0';
		printf("%s", buf);
	}
	//int s = evbuffer_remove(req->input_buffer, &buf, sizeof(buf) - 1);
	//buf[s] = '\0';
	//printf("%s", buf);
	// terminate event_base_dispatch()
	//event_base_loopbreak((struct event_base *)arg);
}

int main() {
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);

	struct event_base *base;
	struct evhttp_connection *conn;
	struct evhttp_request *req;

	base = event_base_new();
	conn = evhttp_connection_base_new(base, NULL, "127.0.0.1", 80);
	req = evhttp_request_new(http_request_done, base);

	evhttp_add_header(req->output_headers, "Host", "news.sohu.com");
	evhttp_add_header(req->output_headers, "Connection", "close");

	evhttp_make_request(conn, req, EVHTTP_REQ_GET, "/");
	evhttp_connection_set_timeout(req->evcon, 600);
	event_base_dispatch(base);
	getchar();
	return 0;
}