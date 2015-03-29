
/**
 * @brief The TCPSocket interface provides access to a raw TCP socket.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/TCPSocket
 */
function TCPSocket() {

	/**
	 * @brief The close method cleanly closes the connection.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TCPSocket/close
	 */
	this.close = function() {};

	/**
	 * @brief The open method is used to open a TCP connection to a given server on a given port.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TCPSocket/open
	 * @param host - A string representing the host name of the server to connect to.
	 * @param port - A number representing the port to connect to.
	 * @param options - An object specifying the details of the socket. This object expects one or more of the following properties:

 
  useSecureTransport: true to create an SSL socket. Defaults to false.
  binaryType: a string with the value string (default) or arraybuffer. If the latter is used the argument for the send() method must be an ArrayBuffer and the data received from the server will be available as an ArrayBuffer. Otherwise, a string is expected.
	 */
	this.CPSocket.open(host, port [', options'])

	/**
	 * @brief The listen method is used to listen on a given port on the device.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TCPSocket/listen
	 * @param port - A number representing the port the server socket is listening on. Pass -1 to indicate no preference, and a port will be selected automatically.
	 * @param options Optional - An object specifying the details of the socket. This object expects the following property:

 
  binaryType: a string with the value string (default) or arraybuffer. If the latter is used, the argument for the send() method must be an ArrayBuffer and the data received from the server will be available as an ArrayBuffer. Otherwise, a string is expected.
	 * @param backlog Optional - A number representing the maximum length the queue of pending connections may grow to. This parameter may be silently limited by the operating system. Pass -1 to use the default value.
	 */
	this.CPSocket.listen(port [', options [', backlog']'])

	/**
	 * @brief The resume method resumes reading incoming data and allows the data event to be triggered again.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TCPSocket/resume
	 */
	this.resume = function() {};

	/**
	 * @brief The send method is used to buffer data to be sent to the server.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TCPSocket/send
	 * @param data - The data to write to the socket. Depending on the binaryType option passed to the open() method, it will be a string or an ArrayBuffer.
	 * @param byteOffset - The offset within the data from which to begin writing. Only relevant for ArrayBuffer data.
	 * @param byteLength - The number of bytes to write. Only relevant for ArrayBuffer data.
	 */
	this.CPSocket.send(data)

	/**
	 * @brief The suspend method pauses reading incoming data and prevents the data event from being triggered until the resume() method is called.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TCPSocket/suspend
	 */
	this.suspend = function() {};

	/**
	 * @brief The upgradeToSecure method allows turning a TCP non secured connection into a secured one.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/TCPSocket/upgradeToSecure
	 */
	this.upgradeToSecure = function() {};

}

