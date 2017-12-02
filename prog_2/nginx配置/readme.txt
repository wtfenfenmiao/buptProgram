这个nginx，下载下来之后，直接改conf，然后直接打开，在浏览器以及程序里用ip和端口就能访问了。linux windows都能用
在有exe的那级目录：
    打开nginx服务器：start nginx
    关闭nginx服务器：nginx -s quit
(就是用命令行打开关闭，否则会不完全的打开和关闭，用任务管理器看全是nginx的各种进程......特别想哭)
conf加了：
server{
        listen 80;
        server_name 127.0.0.1 news.sohu.com;
        location /{
            root G:\buptpro\page.com\\news.sohu.com;
            autoindex on;
        } 
    }

注意那个“\\”，之前用的\，一直报错，去看error log，发现了这个问题。
然后对应的Get请求（最最基础的C++ socket的模式）：
"GET http://news.sohu.com/ HTTP/1.1\r\n
 Host:news.sohu.com:80\r\n
 Connection:Close\r\n\r\n";
	
注意，这个conf中开始有一个worker_connections  1024;这个是限制并发数的，把这个改大可以增加并发数