#include <windows.h>
#include <thread>
#include <mutex>
#include <vector>

typedef enum COMStatus
{
	S_Unknown = 0,
	S_Started = 1,
	S_Init = 2
};

class COMClass
{
	private:
		HANDLE hCom;
		HANDLE hThread;
		HANDLE hThreadStarted;
		HANDLE hThreadTerm;
		HANDLE hDataRcvd;
		HANDLE hReplyRcvd;
		HANDLE hConnEstabl;
		std::vector < HWND > hControls; // will let us change the properties of passed controls
		bool isConnected;

		COMStatus comStatus;
		std::mutex mtxLocker;
		std::string strBuffer;

	public:
		COMClass();
		~COMClass() {};

		// opening port, setting it and running thread
		bool Init(std::string portName, DWORD dwBaudRate, BYTE byParity, BYTE byStopBits, BYTE byByteSize);
		bool Term();
		void cleanHandle(HANDLE& hHandle);

		// function to pass the class instance to the new thread,
		// need to be static or we wouldnt be able to pass it to _beginthread!
		static unsigned _stdcall funcThread(void* cParam);

		// few functions to make class useful (reading buffor, sending data, ...)
		std::string readData();
		bool sendData( const char* paramData, DWORD paramSize );
		void* getDataEvent(bool param);
		bool isConnectionEstablished();
		void addControl( HWND& control );
		void COMClass::modifyControls( bool param );
		void updateCoordinates();
		void COMClass::test();
};