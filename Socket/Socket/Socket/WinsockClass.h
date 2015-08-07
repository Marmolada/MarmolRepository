#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

enum Status {SERVER = 0, CLIENT = 1};

class SocketClass
{
	private:
		SOCKET sock;
		SOCKET connect_sock;
		Status status;
		int error_info;

		HWND hwnd;
		u_int msg;
	
	public:
		SocketClass(){}
		~SocketClass(){}

		bool setSocket( const int nrport, const u_int msg, const HWND hwnd );
		bool resetSocket( const int nrport );
		bool acceptConnect();
		bool cleanSocket();

		bool sendData( const char* data, const int len );
		bool receiveData( char* data, int len );

		const int socketErrorInfo();
};