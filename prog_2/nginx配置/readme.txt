���nginx����������֮��ֱ�Ӹ�conf��Ȼ��ֱ�Ӵ򿪣���������Լ���������ip�Ͷ˿ھ��ܷ����ˡ�linux windows������
����exe���Ǽ�Ŀ¼��
    ��nginx��������start nginx
    �ر�nginx��������nginx -s quit
(�����������д򿪹رգ�����᲻��ȫ�Ĵ򿪺͹رգ��������������ȫ��nginx�ĸ��ֽ���......�ر����)
conf���ˣ�
server{
        listen 80;
        server_name 127.0.0.1 news.sohu.com;
        location /{
            root G:\buptpro\page.com\\news.sohu.com;
            autoindex on;
        } 
    }

ע���Ǹ���\\����֮ǰ�õ�\��һֱ����ȥ��error log��������������⡣
Ȼ���Ӧ��Get�������������C++ socket��ģʽ����
"GET http://news.sohu.com/ HTTP/1.1\r\n
 Host:news.sohu.com:80\r\n
 Connection:Close\r\n\r\n";
	
ע�⣬���conf�п�ʼ��һ��worker_connections  1024;��������Ʋ������ģ�������Ĵ�������Ӳ�����