#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

class SocketClass
{
	private:
		SOCKET sock;
		char* error_info;
	
	public:
		SocketClass(){}
		~SocketClass()
		{
			closesocket(sock);
			WSACleanup();
		}

		bool setSocket( const int nrport, const u_int msg, const HWND hwnd );
		bool sendTo(const char* data, const int len);
		bool acceptConnect();
		bool receiveData(char* data, int len);
		const char* socketErrorInfo();
};