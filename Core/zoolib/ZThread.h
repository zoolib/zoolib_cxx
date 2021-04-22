// Copyright (c) 2008-2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ZThread_h__
#define __ZooLib_ZThread_h__ 1
#include "zconfig.h"

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/ZThread_T.h" // For ZSem_T

namespace ZooLib {

// =================================================================================================
#pragma mark - ZThread, essential types and functions

namespace ZThread {

typedef std::chrono::duration<double,std::ratio<1>> Duration;

inline void sSleep(double iDuration)
	{ std::this_thread::sleep_for(Duration(iDuration)); }

#if ZCONFIG_SPI_Enabled(Win)
	typedef unsigned int ID;
#else
	typedef pthread_t ID;
#endif

ID sID();

void sSetName(const char* iName);

template <class T>
void sStart_T(void(*iProc)(T), T&& iParam)
	{
	std::thread theThread(iProc, std::move(iParam));
	theThread.detach();
	}

} // namespace ZThread

// =================================================================================================
#pragma mark - ZCnd

class ZCnd : public std::condition_variable_any
	{
public:
	inline void Wait(std::mutex& iMtx)
		{ this->wait(iMtx); }

	inline bool WaitFor(std::mutex& iMtx, double iTimeout)
		{
		return std::cv_status::no_timeout == this->wait_for(iMtx, ZThread::Duration(iTimeout));
		}

	bool WaitUntil(std::mutex& iMtx, double iDeadline);
//	inline bool WaitUntil(std::mutex& iMtx, double iDeadline)
//		{
//		return std::cv_status::no_timeout == this->wait_until(iMtx,
//			std::chrono::time_point<Duration>(Duration(iDeadline)));
//		}

	inline void Signal() { this->notify_one(); }

	inline void Broadcast() { this->notify_all(); }	
	};

// =================================================================================================
#pragma mark - ZMtx

class ZMtx : public std::mutex
	{
public:
	inline void Acquire() { this->lock(); }
	inline void Release() { this->unlock(); }
	};

// =================================================================================================
#pragma mark - ZSem

typedef ZSem_T<ZMtx, ZCnd> ZSem;

// =================================================================================================
#pragma mark - ZAcqMtx, ZRelMtx

typedef ZAcquirer_T<ZMtx> ZAcqMtx;

typedef ZReleaser_T<ZMtx> ZRelMtx;


} // namespace ZooLib

#endif // __ZooLib_Thread_h__
