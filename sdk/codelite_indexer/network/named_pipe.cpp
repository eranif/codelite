//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : named_pipe.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "named_pipe.h"

#ifndef __WXMSW__
#  include <sys/types.h>
#  include <sys/unistd.h>
#  include <sys/socket.h>
#  include <sys/time.h>
#endif

#ifdef __WXMSW__
class HandleLocker
{
	HANDLE m_event;
public:
	HandleLocker(HANDLE event) : m_event(event) {}
	~HandleLocker() {
		if(m_event != INVALID_PIPE_HANDLE)
			CloseHandle(m_event);
	}
};
#endif

clNamedPipe::clNamedPipe(const char* pipePath)
		: _pipeHandle(INVALID_PIPE_HANDLE)
		, _lastError(ZNP_OK)
{
	setPipePath(pipePath);
}

clNamedPipe::~clNamedPipe()
{

}

bool clNamedPipe::write( const void* data, size_t dataLength, size_t *written, long timeToLive )
{
#ifdef __WXMSW__
	_lastError = ZNP_OK;

	OVERLAPPED ov = {0};
	ov.hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

	HandleLocker locker(ov.hEvent);

	DWORD actualWrote;
	if (!WriteFile(_pipeHandle, data, (DWORD)dataLength, &actualWrote, &ov)) {
		*written = static_cast<int>(actualWrote);
		int err = GetLastError();
		if (err == ERROR_IO_PENDING) {
			if (WaitForSingleObject(ov.hEvent, timeToLive) == WAIT_OBJECT_0) {
				//data is ready
				if (!GetOverlappedResult(_pipeHandle, &ov, &actualWrote, FALSE)) {
					setLastError(ZNP_WRITE_ERROR);
					return false;
				}
				*written = static_cast<int>(actualWrote);
			} else {
				// the wait exited on the timeout
				setLastError(ZNP_TIMEOUT);
				CancelIo(_pipeHandle);
				return false;
			}
		} else {
			this->setLastError(ZNP_WRITE_ERROR);
			return false;
		}
	}

	*written = static_cast<int>(actualWrote);
	return true;
#else
	int dataSent(0);
	dataSent = ::write(_pipeHandle, data, dataLength);
	if (dataSent < 0) {
		setLastError(ZNP_WRITE_ERROR);
		return false;
	}
	
	*written = dataSent;
	return true;
	
#endif
}

bool clNamedPipe::read( void* data, size_t dataLength,  size_t *read, long timeToLive )
{
#ifdef __WXMSW__
	_lastError = ZNP_OK;

	OVERLAPPED ov = {0};
	ov.hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	HandleLocker locker( ov.hEvent );

	//start an asynch read on the named pipe
	if (!ReadFile(_pipeHandle, data, (DWORD)dataLength, (LPDWORD) read, &ov)) {
		int err = GetLastError();
		if (err == ERROR_IO_PENDING) {
			DWORD waitRes;
			//wait for the asynch result
			waitRes = WaitForSingleObject(ov.hEvent, timeToLive);
			if ( waitRes == WAIT_OBJECT_0) {
				//data is ready
				if (!GetOverlappedResult(_pipeHandle, &ov, (LPDWORD) read, FALSE)) {
					this->setLastError(ZNP_READ_ERROR);
					return false;
				}
			} else {
				// the wait exited on the timeout
				this->setLastError(ZNP_TIMEOUT);
				CancelIo(_pipeHandle);
				return false;
			}
		} else {
			this->setLastError(ZNP_READ_ERROR);
			return false;
		}
	}

	return true;
#else
	fd_set rset;
	FD_ZERO(&rset);
	FD_SET(_pipeHandle, &rset);

	struct timeval tv;
	struct timeval *ptv = NULL;

	if (timeToLive > 0) {
		tv.tv_sec = timeToLive / 1000;
		tv.tv_usec = (timeToLive % 1000)*1000;
		ptv = &tv;
	}

	int rc = select(_pipeHandle+1, &rset, 0, 0, ptv);
	if (rc == 0) {
		// timeout
		setLastError(ZNP_TIMEOUT);
		return false;
		
	} else if(rc < 0){
		// error
		setLastError(ZNP_UNKNOWN);
		return false;
		
	} else {
		// read the data
		int dataRead (0);
		dataRead = ::read(_pipeHandle, data, dataLength);
		if(dataRead < 0){
			setLastError(ZNP_READ_ERROR);
			return false;

		} else if(dataRead == 0) {
			setLastError(ZNP_CONN_CLOSED);
			return false;
		}
		
		*read = dataRead;
		return true;
	}
#endif
}
