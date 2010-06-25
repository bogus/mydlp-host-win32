#ifndef __THREAD_H__
#define __THREAD_H__
/////////////////////////////////////////////////////////////////////////////////
#include "Windows.h"
struct IRunnable {
	virtual unsigned long run() = 0;
	virtual void stop() = 0;
};
class ThreadException {
public:
	ThreadException(DWORD lastError) {
		HLOCAL msgBuffer;
		::FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			0, lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPSTR)&msgBuffer, 0,	NULL);

		Message = (LPSTR)msgBuffer;
		::LocalFree(msgBuffer);
	}
	ThreadException(const std::string &msg) { Message = msg; }
	std::string Message;
};

class Thread {
public:
    Thread(IRunnable *ptr=0) {
		_runnable = ptr;
		_started = false;
		_threadHandle = 0;
	}
	~Thread() {
		if(_threadHandle != 0)
			::CloseHandle(_threadHandle);
	}
	void start(IRunnable *ptr=0) {
		if(_started)
			throw ThreadException("Thread already started.");

		if(!_started && _threadHandle != 0)			::CloseHandle(_threadHandle);
		if(ptr != 0)
			_runnable = ptr;
		if(_runnable == 0)			throw ThreadException("An object implementing the IRunnable interface required.");
		DWORD threadID=0;
		_threadHandle = ::CreateThread(0, 0, ThreadProc, this, 0, &threadID);
		if(_threadHandle == 0)
			throw ThreadException(::GetLastError());

		::Sleep(0);
	}

	void stop() {
		checkAlive();
		_runnable->stop();
	}
    void suspend() {
		checkAlive();
		checkThreadHandle();
		if(::SuspendThread(_threadHandle) == -1)
 		    throw ThreadException(::GetLastError());    }
    void resume() {
		checkAlive();
		checkThreadHandle();
		if(::ResumeThread(_threadHandle) == -1)
 		    throw ThreadException(::GetLastError());
    }
	void join(unsigned long timeOut=INFINITE) {
		checkThreadHandle();
		if(isAlive()) {
			DWORD waitResult = ::WaitForSingleObject(_threadHandle, timeOut);
			if(waitResult == WAIT_FAILED)
				throw ThreadException(::GetLastError());
		}
	}
    bool isAlive() { return _started; }
protected:
	bool _started;
	HANDLE _threadHandle;
	IRunnable *_runnable;
	unsigned long run() {
		_started = true;
        unsigned long threadExitCode = _runnable->run();
		_started = false;
		return threadExitCode;
	}

	void checkThreadHandle() {
		if(_threadHandle == 0)
			throw ThreadException("Thread not yet created, call the start() method.");
	}
	void checkAlive() {
		if(!isAlive())
			throw ThreadException("No Thread alive.");
	}

	static unsigned long __stdcall ThreadProc(void* ptr) {
		return ((Thread *)ptr)->run();
	}
};

///////////////////////////////////////////////////////////////////////////////
#endif // __THREAD_H__