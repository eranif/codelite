//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : clindexerprotocol.h
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

#ifndef __clindexerprotocol__
#define __clindexerprotocol__

#include "named_pipe.h"
#include "cl_indexer_reply.h"
#include "cl_indexer_request.h"


class clIndexerProtocol {

public:
	clIndexerProtocol();
	~clIndexerProtocol();

	/**
	 * @brief send request to the server.
	 * @param conn connection to use. The connection should already be connected
	 * @param req request to send
	 * @return true on success, false otherwise
	 */
	static bool SendRequest  (clNamedPipe *conn, clIndexerRequest &req  );
	/**
	 * @brief convert reply to binary and send it to the caller using named pipe.
	 * @param conn [input] named pipe to use
	 * @param reply [input] reply to send
	 * @return true on success, false otherwise
	 */
	static bool SendReply    (clNamedPipe *conn, clIndexerReply   &reply);
	/**
	 * @brief read request from the named pipe. If any error occurred, return false and abort the connection
	 * @param conn [input] named pipe to use for reading the request
	 * @param req [output] holds the received request. Should be used only if this function
	 *        returns true
	 * @return true on success, false otherwise
	 */
	static bool ReadRequest  (clNamedPipe *conn, clIndexerRequest &req  );
	/**
	 * @brief read reply from the server.
	 * @param conn connection to use
	 * @param reply [output]
	 * @return true on success, false otherwise
	 */
	static bool ReadReply    (clNamedPipe *conn, clIndexerReply   &reply);

};
#endif // __clindexerprotocol__
