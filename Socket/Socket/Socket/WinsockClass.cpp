#include "WinsockClass.h"

bool SocketClass::setSocket( const int nrport, const u_int msg, const HWND hwnd )
{
	WSADATA wsaData;
	SOCKADDR_IN target;

	if( WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		error_info = "WSAStartup";
		WSACleanup();
		return false;
	}

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == INVALID_SOCKET)
	{
		error_info = "Socket creating";
		WSACleanup();
		return false;
	}
	
	target.sin_family = AF_INET;
	target.sin_port = htons(nrport);
	target.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(connect(sock, (SOCKADDR*)&target, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		closesocket(sock);
		
		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(sock == INVALID_SOCKET)
		{
			error_info = "Socket creating";
			WSACleanup();
			return false;
		} 

		error_info = "Connect";

		SOCKADDR_IN sock_in;
		sock_in.sin_family = AF_INET;
		sock_in.sin_port = htons(nrport);
		sock_in.sin_addr.s_addr = htonl (INADDR_ANY);

		if(bind(sock, (LPSOCKADDR)&sock_in, sizeof(sock_in)) == SOCKET_ERROR)
		{
			error_info = "Bind";
			WSACleanup();
			return false;
		}
		listen(sock, SOMAXCONN);
	}
	
	WSAAsyncSelect(sock, hwnd, msg, FD_READ | FD_CONNECT | FD_CLOSE | FD_ACCEPT);
	return true;
}

bool SocketClass::acceptConnect()
{
	SOCKADDR_IN from;
	int len = sizeof(sockaddr_in);

	SOCKET tempsock = accept(sock, (struct sockaddr*)&from, &len);

	sock = tempsock;
	return true;
}

bool SocketClass::sendTo( const char* data, const int len )
{
	if(send(sock, data, len+1, 0) > 0)
		return true;
	return false;
}

bool SocketClass::receiveData(char* data, int len)
{
	if(recv(sock, data, len-1, 0) > 0)
		return true;
	return false;
}

const char* SocketClass::socketErrorInfo()
{
	return error_info;
}