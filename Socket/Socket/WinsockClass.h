#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

class SocketClass
{
	private:
		SOCKET sock;
		SOCKADDR_IN sockaddr_in;
		int error_info;
	
	public:
		SocketClass(){}
		~SocketClass()
		{
			WSACleanup();
		}

		bool setSocket( const int nrport, const u_int msg, const HWND hwnd );
		bool sendTo(const char* data, const int len);
		char* receiveData(char* data, int len);
		const int socketErrorInfo();
};