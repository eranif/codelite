#include "np_connections_server.h"
#include "named_pipe_server.h"

#ifndef __WXMSW__
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <sys/un.h>
#  include <stdio.h>
#endif

#ifdef __WXMSW__
static PIPE_HANDLE createNamedPipe(const char* pipeName, SECURITY_ATTRIBUTES sa)
{
	return CreateNamedPipe(	pipeName,
	                        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
	                        PIPE_TYPE_BYTE | PIPE_WAIT | PIPE_READMODE_BYTE,
	                        PIPE_UNLIMITED_INSTANCES,
	                        8192, 8192, 0,
	                        &sa);
}

static PACL prepareNamedPipeAcl(SECURITY_DESCRIPTOR* sd, SECURITY_ATTRIBUTES* sa)
{
	DWORD req_acl_size;
	char everyone_buf[32], owner_buf[32];
	PSID sid_everyone, sid_owner;
	SID_IDENTIFIER_AUTHORITY
	siaWorld = SECURITY_WORLD_SID_AUTHORITY,
	           siaCreator = SECURITY_CREATOR_SID_AUTHORITY;
	PACL acl;

	sid_everyone = (PSID)&everyone_buf;
	sid_owner = (PSID)&owner_buf;

	req_acl_size = sizeof(ACL) +
	               (2 * ((sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD)) + GetSidLengthRequired(1)));

	acl = (PACL) malloc(req_acl_size);

	if (acl == NULL) {
		return NULL;
	}

	if (!InitializeSid(sid_everyone, &siaWorld, 1)) {
		goto out_fail;
	}
	*GetSidSubAuthority(sid_everyone, 0) = SECURITY_WORLD_RID;

	if (!InitializeSid(sid_owner, &siaCreator, 1)) {
		goto out_fail;
	}
	*GetSidSubAuthority(sid_owner, 0) = SECURITY_CREATOR_OWNER_RID;

	if (!InitializeAcl(acl, req_acl_size, ACL_REVISION)) {
		goto out_fail;
	}

	if (!AddAccessAllowedAce(acl, ACL_REVISION, FILE_GENERIC_READ | FILE_GENERIC_WRITE, sid_everyone)) {
		goto out_fail;
	}

	if (!AddAccessAllowedAce(acl, ACL_REVISION, FILE_ALL_ACCESS, sid_owner)) {
		goto out_fail;
	}

	if (!InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION)) {
		goto out_fail;
	}

	if (!SetSecurityDescriptorDacl(sd, TRUE, acl, FALSE)) {
		goto out_fail;
	}

	sa->lpSecurityDescriptor = sd;

	return acl;

out_fail:
	free(acl);
	return NULL;

}
#endif

clNamedPipeConnectionsServer::clNamedPipeConnectionsServer(const char* pipeName)
: _listenHandle(INVALID_PIPE_HANDLE)
{
	_pipePath = strdup(pipeName);
}

clNamedPipeConnectionsServer::~clNamedPipeConnectionsServer()
{
	if(_pipePath) {
		free(_pipePath);
		_pipePath = NULL;	
	}
	_listenHandle = INVALID_PIPE_HANDLE;
}

PIPE_HANDLE clNamedPipeConnectionsServer::initNewInstance()
{
#ifdef __WXMSW__
	PACL acl;
	SECURITY_DESCRIPTOR  sd = {0};

	SECURITY_ATTRIBUTES  sa = {0};
	memset(&sa, 0, sizeof(sa));
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	HANDLE hPipe = createNamedPipe(_pipePath, sa);
	if (!hPipe) {
		this->setLastError(NP_SERVER_UNKNOWN_ERROR);
		return INVALID_PIPE_HANDLE;
	}
	return hPipe;
#else
	if(_listenHandle == INVALID_PIPE_HANDLE) {
		
		unlink(_pipePath);
		struct sockaddr_un server;
		
		_listenHandle = socket(AF_UNIX, SOCK_STREAM, 0);
		if (_listenHandle < 0) {
			perror("ERROR: socket");
			return INVALID_PIPE_HANDLE;
		}
		
		server.sun_family = AF_UNIX;
		strcpy(server.sun_path, _pipePath);
		if (bind(_listenHandle, (struct sockaddr *) &server, sizeof(struct sockaddr_un))) {
			perror("ERROR: binding stream socket");
			return INVALID_PIPE_HANDLE;
		}	
	}
	listen(_listenHandle, 10);
	return _listenHandle;
#endif
}

bool clNamedPipeConnectionsServer::shutdown()
{
	if(_pipePath){
		free(_pipePath);
		_pipePath = NULL;	
	}

#ifndef __WXMSW__
	close(_listenHandle);
#endif

	_listenHandle = INVALID_PIPE_HANDLE;
	return true;
}

clNamedPipe *clNamedPipeConnectionsServer::waitForNewConnection( int timeout )
{
	PIPE_HANDLE hConn = this->initNewInstance();

#ifdef __WXMSW__
	OVERLAPPED ov = {0};
	HANDLE ev = CreateEvent(NULL, TRUE, TRUE, NULL);
	ov.hEvent = ev;

	bool fConnected = ConnectNamedPipe(hConn, &ov);
	if (fConnected != 0) {
		this->setLastError(NP_SERVER_UNKNOWN_ERROR);
		return NULL;
	}

	switch (GetLastError()) {

		// The overlapped connection in progress.
	case ERROR_IO_PENDING: {
		DWORD res = WaitForSingleObject(ov.hEvent, timeout) ;
		switch (res) {
		case WAIT_OBJECT_0 : {
			clNamedPipeServer *conn = new clNamedPipeServer(_pipePath);
			conn->setHandle(hConn);
			return conn;
		}
		case WAIT_TIMEOUT : {
			this->setLastError(NP_SERVER_TIMEOUT);
			return NULL;
		}
		default: {
			this->setLastError(NP_SERVER_UNKNOWN_ERROR);
			return NULL;
		}

		}
	}

	case ERROR_PIPE_CONNECTED: {
		clNamedPipeServer *conn = new clNamedPipeServer(_pipePath);
		conn->setHandle(hConn);
		return conn;
	}
	// If an error occurs during the connect operation...
	default: {
		this->setLastError(NP_SERVER_UNKNOWN_ERROR);
		return NULL;
	}
	}
#else
	// accept new connection
	if(hConn != INVALID_PIPE_HANDLE){
		PIPE_HANDLE fd = ::accept(hConn, 0, 0);
		if(fd > 0){
			clNamedPipeServer *conn = new clNamedPipeServer(_pipePath);
			conn->setHandle(fd);
			return conn;
		} else {
			perror("ERROR: accept");
			return NULL;
		}
		
	}
	return NULL;
#endif
}
