#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

enum Status {SERVER = 0, CLIENT = 1};

class SocketClass
{
	private:
		SOCKET sock;
		Status status;
		int error_info;
	
	public:
		SocketClass(){}
		~SocketClass()
		{
			closesocket(sock);
			WSACleanup();
		}

		bool setSocket( const int nrport, const u_int msg, const HWND hwnd );
		bool resetSocket(  const int nrport, const u_int msg, const HWND hwnd );
		bool acceptConnect();

		bool sendData( const char* data, const int len );
		bool receiveData( char* data, int len );

		const int socketErrorInfo();
};