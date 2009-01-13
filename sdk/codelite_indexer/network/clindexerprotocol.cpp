#include "clindexerprotocol.h"
#include <memory>

clIndexerProtocol::clIndexerProtocol()
{
}

clIndexerProtocol::~clIndexerProtocol()
{
}

bool clIndexerProtocol::ReadReply(clNamedPipe* conn, clIndexerReply& reply)
{
	std::auto_ptr<char> sp;

	// first we read sizeof(size_t) to get the actual data size
	size_t buff_len(0);
	size_t actual_read(0);

	if ( !conn->read((void*)&buff_len, sizeof(buff_len), &actual_read, -1) ) {
		fprintf(stderr, "ERROR: Failed to read from the pipe, reason: %d\n", conn->getLastError());
		return false;
	}

	if (actual_read != sizeof(buff_len)) {
		fprintf(stderr, "ERROR: Protocol error: expected %d bytes, got %d\n", sizeof(buff_len), actual_read);
		return false;
	}

	char *data = new char[buff_len];
	sp.reset(data);

	int bytes_left(buff_len);
	size_t bytes_read(0);
	while(bytes_left > 0){
		if ( !conn->read(data+bytes_read, bytes_left, &actual_read, -1) ) {
			fprintf(stderr, "ERROR: Protocol error: expected %d bytes, got %d\n", buff_len, actual_read);
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
	std::auto_ptr<char> sp;

	// first we read sizeof(size_t) to get the actual data size
	size_t buff_len(0);
	size_t actual_read(0);

	if ( !conn->read((void*)&buff_len, sizeof(buff_len), &actual_read, -1) ) {
		fprintf(stderr, "ERROR: Failed to read from the pipe, reason: %d\n", conn->getLastError());
		return false;
	}

	if (actual_read != sizeof(buff_len)) {
		fprintf(stderr, "ERROR: Protocol error: expected %d bytes, got %d\n", sizeof(buff_len), actual_read);
		return false;
	}

	char *data = new char[buff_len];
	sp.reset(data);

	int bytes_left(buff_len);
	size_t bytes_read(0);
	while(bytes_left > 0){
		if ( !conn->read(data+bytes_read, bytes_left, &actual_read, -1) ) {
			fprintf(stderr, "ERROR: Protocol error: expected %d bytes, got %d\n", buff_len, actual_read);
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
	char *data = reply.toBinary(buff_size);
	std::auto_ptr<char> sp(data);

	// send the reply size
	size_t written(0);
	conn->write((void*)&buff_size, sizeof(buff_size), &written, -1);


	int bytes_left(buff_size);
	int bytes_to_write(0);
	int bytes_written(0);

	while (bytes_left > 0) {
		// we write in chunks of 3000 bytes
		if (bytes_left < 3000) {
			bytes_to_write = bytes_left;
		} else {
			bytes_to_write = 3000;
		}

		size_t actual_written(0);
		if ( !conn->write(data+bytes_written, bytes_to_write, &actual_written, -1) ) {
			return false;
		}

		bytes_left -= actual_written;
		bytes_written += actual_written;
	}
	return true;
}

bool clIndexerProtocol::SendRequest(clNamedPipe* conn, clIndexerRequest& req)
{
	size_t size(0);
	size_t written(0);

	char *data = req.toBinary(size);
	std::auto_ptr<char> sp(data);

	// write request
	if(!conn->write((void*)&size, sizeof(size), &written, -1)) {
		printf("ERROR: protocol error: rc %d\n", conn->getLastError());
		return false;
	}

	int bytes_left(size);
	int bytes_to_write(0);
	int bytes_written(0);

	while (bytes_left > 0) {
		// we write in chunks of 3000 bytes
		if (bytes_left < 3000) {
			bytes_to_write = bytes_left;
		} else {
			bytes_to_write = 3000;
		}

		size_t actual_written(0);
		if ( !conn->write(data+bytes_written, bytes_to_write, &actual_written, -1) ) {
			return false;
		}

		bytes_left -= actual_written;
		bytes_written += actual_written;
	}
	return true;
}
