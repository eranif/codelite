//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : clindexerprotocol.cpp
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

#include "clindexerprotocol.h"
#include <memory>
#include <stdio.h>
#include <sstream>

#define ACK_MAGIC 1975

class CharDeleter
{
    char* m_ptr;

public:
    CharDeleter(char* p)
        : m_ptr(p)
    {
    }
    ~CharDeleter()
    {
        if(m_ptr) {
            delete[] m_ptr;
            m_ptr = NULL;
        }
    }
};

clIndexerProtocol::clIndexerProtocol() {}

clIndexerProtocol::~clIndexerProtocol() {}

bool clIndexerProtocol::ReadReply(clNamedPipe* conn, clIndexerReply& reply, std::string& errmsg)
{
    // first we read sizeof(size_t) to get the actual data size
    size_t buff_len(0);
    size_t actual_read(0);

    if(!conn->read((void*)&buff_len, sizeof(buff_len), &actual_read, 10000)) {
        std::stringstream ss;
        ss << "ERROR: ReadReply: Failed to read from the pipe, reason: " << conn->getLastError();
        errmsg = ss.str();
        return false;
    }

    if(actual_read != sizeof(buff_len)) {
        std::stringstream ss;
        ss << "ERROR: ReadReply: Protocol error: expected" << (sizeof(buff_len)) << "bytes, got: " << (int)actual_read
           << " reason: " << conn->getLastError();
        errmsg = ss.str();
        return false;
    }

    if((buff_len / (1024 * 1024)) > 15) {
        // Dont read buffers larger than 15MB...
        errmsg = "Buffer size is larger than 15MB";
        return false;
    }

    char* data = new char[buff_len];
    CharDeleter deleter(data);

    int bytes_left(buff_len);
    size_t bytes_read(0);
    while(bytes_left > 0) {
        if(!conn->read(data + bytes_read, bytes_left, &actual_read, 10000)) {
            std::stringstream ss;
            ss << "ERROR: Protocol error: expected " << buff_len << " bytes, got " << actual_read;
            return false;
        }
        bytes_left -= actual_read;
        bytes_read += actual_read;
    }

    reply.fromBinary(data);
    return true;
}

bool clIndexerProtocol::ReadRequest(clNamedPipe* conn, clIndexerRequest& req)
{
    // first we read sizeof(size_t) to get the actual data size
    size_t buff_len(0);
    size_t actual_read(0);

    if(!conn->read((void*)&buff_len, sizeof(buff_len), &actual_read, -1)) {
        fprintf(stderr, "ERROR: Failed to read from the pipe, reason: %d\n", conn->getLastError());
        return false;
    }

    if(actual_read != sizeof(buff_len)) {
        fprintf(stderr, "ERROR: Protocol error: expected %u bytes, got %u\n", (unsigned int)sizeof(buff_len),
            (unsigned int)actual_read);
        return false;
    }

    if(buff_len == 0) return false;

    char* data = new char[buff_len];
    CharDeleter deleter(data);

    int bytes_left(buff_len);
    size_t bytes_read(0);
    while(bytes_left > 0) {
        if(!conn->read(data + bytes_read, bytes_left, &actual_read, -1)) {
            fprintf(stderr, "ERROR: [%s] Protocol error: expected %u bytes, got %u\n", __PRETTY_FUNCTION__,
                (unsigned int)buff_len, (unsigned int)actual_read);
            return false;
        }
        bytes_left -= actual_read;
        bytes_read += actual_read;
    }

    req.fromBinary(data);
    return true;
}

bool clIndexerProtocol::SendReply(clNamedPipe* conn, clIndexerReply& reply)
{
    size_t buff_size(0);
    char* data = reply.toBinary(buff_size);
    CharDeleter deleter(data);

    // send the reply size
    size_t written(0);
    conn->write((void*)&buff_size, sizeof(buff_size), &written, -1);

    int bytes_left(buff_size);
    int bytes_to_write(0);
    int bytes_written(0);

    while(bytes_left > 0) {
        // we write in chunks of 3000 bytes
        if(bytes_left < 3000) {
            bytes_to_write = bytes_left;
        } else {
            bytes_to_write = 3000;
        }

        size_t actual_written(0);
        if(!conn->write(data + bytes_written, bytes_to_write, &actual_written, -1)) {
            return false;
        }

        bytes_left -= actual_written;
        bytes_written += actual_written;
    }

    // the above problem does not exist under Windows' NamedPipes
    return true;
}

bool clIndexerProtocol::SendRequest(clNamedPipe* conn, clIndexerRequest& req)
{
    size_t size(0);
    size_t written(0);

    char* data = req.toBinary(size);
    CharDeleter deleter(data);

    // write request
    if(!conn->write((void*)&size, sizeof(size), &written, -1)) {
        printf("ERROR: [%s] protocol error: rc %d\n", __PRETTY_FUNCTION__, conn->getLastError());
        return false;
    }

    int bytes_left(size);
    int bytes_to_write(0);
    int bytes_written(0);

    while(bytes_left > 0) {
        // we write in chunks of 3000 bytes
        if(bytes_left < 3000) {
            bytes_to_write = bytes_left;
        } else {
            bytes_to_write = 3000;
        }

        size_t actual_written(0);
        if(!conn->write(data + bytes_written, bytes_to_write, &actual_written, -1)) {
            return false;
        }

        bytes_left -= actual_written;
        bytes_written += actual_written;
    }
    return true;
}
