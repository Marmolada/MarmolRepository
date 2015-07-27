#include "WinsockClass.h"

bool SocketClass::setSocket( const int nrport, const u_int msg, const HWND hwnd )
{
	WSADATA wsaData;
	if( WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		error_info = WSAGetLastError();
		WSACleanup();
		return false;
	}

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sock == INVALID_SOCKET)
	{
		error_info = WSAGetLastError();
		WSACleanup();
		return false;
	}
	
	sockaddr_in.sin_family = AF_INET;
	sockaddr_in.sin_port = htons(nrport);
	sockaddr_in.sin_addr.s_addr = inet_addr("127.0.0.1");

	////////////////////////////////////////////////////////////////////////////////////////
	//// istotne instrukcje - celem setsockopt() z parametrem SO_REUSEADDR
	//// jest umozliwienie zbindowania wielu socketow dla pojedyñczego adresu fizycznego
	BOOL bOptVal = TRUE;
    int bOptLen = sizeof (BOOL);
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&bOptVal, bOptLen) == SOCKET_ERROR)
	{
		error_info = WSAGetLastError();
		closesocket(sock);
		WSACleanup();
		return false;
	}
	/////////////////////////////////////////////////////////////////////////////////////////

	if(bind(sock, (SOCKADDR*)&sockaddr_in, sizeof(sockaddr_in)) == INVALID_SOCKET)
	{
		error_info = WSAGetLastError();
		closesocket(sock);
		WSACleanup();
		return false;
	}

	if(WSAAsyncSelect(sock, hwnd, msg, FD_READ) == SOCKET_ERROR)
	{
		error_info = WSAGetLastError();
		closesocket(sock);
		WSACleanup();
		return false;
	}
	return true;
}

bool SocketClass::sendTo( const char* data, const int len )
{
	if(sendto(sock, data, len+1, 0, (SOCKADDR*)&sockaddr_in, sizeof(sockaddr_in)) > 0)
		return true;
	return false;
}

char* SocketClass::receiveData(char* data, int len)
{
	recv(sock, data, len-1, 0);
	return data;
}

const int SocketClass::socketErrorInfo()
{
	return error_info;
}