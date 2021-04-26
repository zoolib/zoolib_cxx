// Copyright (c) 2008-2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ZThread_h__
#define __ZooLib_ZThread_h__ 1
#include "zconfig.h"

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "zoolib/Compat_NonCopyable.h"
#include "zoolib/ZCONFIG_SPI.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ZThread, essential types and functions

namespace ZThread {

typedef std::chrono::duration<double,std::ratio<1>> Duration;

template <class T>
void sStart_T(void(*iProc)(T), T&& iParam)
	{ std::thread(iProc, std::move(iParam)).detach(); }

inline void sSleep(double iDuration)
	{ std::this_thread::sleep_for(Duration(iDuration)); }

#if ZCONFIG_SPI_Enabled(Win)
	typedef unsigned int ID;
#else
	typedef pthread_t ID;
#endif

ID sID();

void sSetName(const char* iName);

} // namespace ZThread

// =================================================================================================
#pragma mark - ZCnd

class ZCnd : public std::condition_variable_any
	{
public:
	typedef ZThread::Duration Duration;

	inline void Wait(std::mutex& iMtx) { this->wait(iMtx); }

	inline bool WaitFor(std::mutex& iMtx, double iTimeout)
		{ return std::cv_status::no_timeout == this->wait_for(iMtx, Duration(iTimeout)); }

	inline bool WaitUntil(std::mutex& iMtx, double iDeadline)
		{
		return std::cv_status::no_timeout == this->wait_until(iMtx,
			std::chrono::time_point<std::chrono::steady_clock, Duration>(Duration(iDeadline)));
		}

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
#pragma mark - ZAcqMtx

class ZAcqMtx : NonCopyable
	{
public:
	inline ZAcqMtx(std::mutex& iMtx) : fMtx(iMtx) { fMtx.lock(); }
	inline ~ZAcqMtx() { fMtx.unlock(); }

private:
	std::mutex& fMtx;
	};

// =================================================================================================
#pragma mark - ZRelMtx

class ZRelMtx : NonCopyable
	{
public:
	inline ZRelMtx(std::mutex& iMtx) : fMtx(iMtx) { fMtx.unlock(); }
	inline ~ZRelMtx() { fMtx.lock(); }

private:
	std::mutex& fMtx;
	};

} // namespace ZooLib

#endif // __ZooLib_ZThread_h__
