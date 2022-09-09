#include <cstdio>
#include <vector>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory.h>
#include <arpa/inet.h>
#include <time.h>
#include <thread>
#include <unistd.h>
#include <fstream>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>
#include <systemd/sd-daemon.h>
using namespace std;

bool stop = false;
bool socket_ok = false;
time_t lastTick;
char logFile[512];

void log(char* message)
{
	try
	{
		FILE *fp = fopen(logFile, "a+, ccs=utf-8");
		time_t now = time(NULL);
		tm* t = gmtime(&now);
		if (fp)
		{
			fprintf(fp, "%d-%02d-%02d %02d:%02d:%02d %s\n", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec,message);
			//fwrite(message, 1, strlen(message), fp);
			fflush(fp);
			fclose(fp);
		}
		else
		{
			printf("Open log file Error:%d\n", errno);
		}

		printf("%d-%02d-%02d %02d:%02d:%02d %s\n", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, message);
	}
	catch (const std::exception&)
	{

	}
}

void logMsg(const char *format, ...)
{
	char newFormat[1024];
	memset(newFormat, 0, 1024);
	char err[] = "Info: ";
	memcpy(newFormat, err, 6);
	memcpy(newFormat + 6, format, strlen(format));
	va_list a;
	va_start(a, format);
	char buf[1024];
	memset(buf, 0, 1024);
	vsprintf(buf, newFormat, a);
	va_end(a);
	log(buf);
}

void logError(const char* format, ...)
{
	char newFormat[1024];
	memset(newFormat, 0, 1024);
	char err[] = "Error: ";
	memcpy(newFormat, err, 7);
	memcpy(newFormat + 7, format, strlen(format));
	va_list a;
	va_start(a, format);
	char buf[1024];
	memset(buf, 0, 1024);
	vsprintf(buf, newFormat, a);
	va_end(a);
	log(buf);
}

int s;

bool udpListener(int port)
{
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == -1)
	{
		logError("Fail to init socket. ErrorNo:%d\n", errno);
		return false;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	// 将网卡IP地址绑定到接收地址, 
	// 绑定到INADDR_ANY意味着从所有网卡接收,不指定具体的网卡
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	int len = sizeof(addr);            //绑定udp套接字
	int result = ::bind(s, (struct sockaddr *)&addr, len);
	if (result < 0)
	{
		sd_notifyf(0, "STATUS=Failed to create socket: %s\n"
			"ERRNO=%i", strerror(errno), errno);
		logError("Socket bind failure. ErrorNo: %d\n", errno);
		return false;
	}

	logMsg("Socket created. Begin listening at %d.\n", port);
	socket_ok = true;
	char udpbuf[256];
	struct sockaddr_in client_address;				//客户端地址
													//SOCKET的队列
	fd_set readfds, testfds;

	//初始化文件描述符集合,udp套接字加入集合,同时将较大的文件描述符存入maxfd	
	FD_ZERO(&readfds);
	FD_SET(s, &readfds);

	while (!stop)
	{
		testfds = readfds;
		timeval timeout{ 0,500 }; //超时时间 100ms
		int result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, NULL);
		if (result < 1)
		{			
			continue;
		}

		if (FD_ISSET(s, &testfds) > 0)
		{
			int client_len = sizeof(struct sockaddr);
			size_t recLen = recvfrom(s, udpbuf, 256, 0, (struct sockaddr *)&client_address, (socklen_t*)&client_len);
			if (recLen > 0)
			{
				lastTick = time(NULL);
			}
		}
	}
	
	return true;
}

void createLogName()
{
	time_t now = time(NULL);
	tm* t = gmtime(&now);
	memset(logFile, 0, 512);
	sprintf(logFile, "/var/log/CCService/CCWakeup-%d-%02d-%02d.%02d%02d%02d.log", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}

thread* listen_thread = NULL;
void term(int)
{
	sd_notify(0, "STOPPING=1");
	stop = true;
	if (listen_thread != NULL)
	{
		listen_thread->detach();
		delete listen_thread;
	}

	if (socket_ok)
	{
		shutdown(s, 2);
		close(s);
	}
	logMsg("Exit.\n");
}

int main()
{
	createLogName();
	lastTick = time(NULL);
	logMsg("Start up, Current Time is: %ld!\n", lastTick);
	listen_thread = new thread(&udpListener, 54321);

	struct sigaction act;
	act.sa_handler = term;
	sigaction(SIGTERM, &act, NULL);

	sd_notifyf(0, "READY=1\n"
		"STATUS=Starting monitor CC Service status...\n"
		"MAINPID=%lu", (unsigned long)getpid());
	sleep(10); //wait until cservice startup or system boot up
	while (!stop)
	{
		if (socket_ok)
		{
			time_t now = time(NULL);
			if (now - lastTick > 5)// 5 second
			{
				// not running
				logMsg("ccservice is stoped. I will try to start it.\n");

				logMsg("Exec Command \"service ccservice stop\".\n");
				system("service ccservice stop");
				//execlp("service", "ccservice", "start");
				logMsg("Exec Command \"service ccservice start\".\n");
				system("service ccservice start");

				lastTick = now;
			}
		}
		sleep(1);
	}

	return 0;
}

