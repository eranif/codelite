<?php

// Start of sockets v.

/**
 * Runs the select() system call on the given arrays of sockets with a specified timeout
 * @link http://www.php.net/manual/en/function.socket-select.php
 * @param read array <p>
 *       The sockets listed in the read array will be
 *       watched to see if characters become available for reading (more
 *       precisely, to see if a read will not block - in particular, a socket
 *       resource is also ready on end-of-file, in which case a
 *       socket_read will return a zero length string).
 *      </p>
 * @param write array <p>
 *       The sockets listed in the write array will be
 *       watched to see if a write will not block.
 *      </p>
 * @param except array <p>
 *       The sockets listed in the except array will be
 *       watched for exceptions.
 *      </p>
 * @param tv_sec int <p>
 *       The tv_sec and tv_usec
 *       together form the timeout parameter. The
 *       timeout is an upper bound on the amount of time
 *       elapsed before socket_select return.
 *       tv_sec may be zero , causing
 *       socket_select to return immediately. This is useful
 *       for polling. If tv_sec is &null; (no timeout),
 *       socket_select can block indefinitely.
 *      </p>
 * @param tv_usec int[optional] <p>
 *      </p>
 * @return int On success socket_select returns the number of
 *   socket resources contained in the modified arrays, which may be zero if
 *   the timeout expires before anything interesting happens. On error false
 *   is returned. The error code can be retrieved with
 *   socket_last_error.
 *  </p>
 *  
 *   <p>
 *    Be sure to use the === operator when checking for an
 *    error. Since the socket_select may return 0 the
 *    comparison with == would evaluate to true:
 *    
 *     Understanding socket_select's result
 *     
 * ]]>
 */
function socket_select (array &$read, array &$write, array &$except, $tv_sec, $tv_usec = null) {}

/**
 * Create a socket (endpoint for communication)
 * @link http://www.php.net/manual/en/function.socket-create.php
 * @param domain int <p>
 *       The domain parameter specifies the protocol
 *       family to be used by the socket.
 *      </p>
 *      <table>
 *       Available address/protocol families
 *       
 *        
 *         <tr valign="top">
 *          <td>Domain</td>
 *          <td>Description</td>
 *         </tr>
 *        
 *        
 *         <tr valign="top">
 *          <td>AF_INET</td>
 *          <td>
 *           IPv4 Internet based protocols. TCP and UDP are common protocols of
 *           this protocol family.
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>AF_INET6</td>
 *          <td>
 *           IPv6 Internet based protocols. TCP and UDP are common protocols of
 *           this protocol family.
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>AF_UNIX</td>
 *          <td>
 *           Local communication protocol family. High efficiency and low
 *           overhead make it a great form of IPC (Interprocess Communication).
 *          </td>
 *         </tr>
 *        
 *       
 *      </table>
 * @param type int <p>
 *       The type parameter selects the type of communication
 *       to be used by the socket.
 *      </p>
 *      <table>
 *       Available socket types
 *       
 *        
 *         <tr valign="top">
 *          <td>Type</td>
 *          <td>Description</td>
 *         </tr>
 *        
 *        
 *         <tr valign="top">
 *          <td>SOCK_STREAM</td>
 *          <td>
 *           Provides sequenced, reliable, full-duplex, connection-based byte streams.
 *           An out-of-band data transmission mechanism may be supported.
 *           The TCP protocol is based on this socket type.
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>SOCK_DGRAM</td>
 *          <td>
 *           Supports datagrams (connectionless, unreliable messages of a fixed maximum length).
 *           The UDP protocol is based on this socket type.
 *          </td>
 *         </tr>
 *           <tr valign="top">
 *          <td>SOCK_SEQPACKET</td>
 *          <td>
 *           Provides a sequenced, reliable, two-way connection-based data transmission path for
 *           datagrams of fixed maximum length;  a consumer is required to read an
 *           entire packet with each read call.
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>SOCK_RAW</td>
 *          <td>
 *           Provides raw network protocol access. This special type of socket
 *           can be used to manually construct any type of protocol. A common use
 *           for this socket type is to perform ICMP requests (like ping).
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>SOCK_RDM</td>
 *          <td>
 *           Provides a reliable datagram layer that does not guarantee ordering.
 *           This is most likely not implemented on your operating system.
 *          </td>
 *         </tr>
 *        
 *       
 *      </table>
 * @param protocol int <p>
 *       The protocol parameter sets the specific
 *       protocol within the specified domain to be used
 *       when communicating on the returned socket. The proper value can be
 *       retrieved by name by using getprotobyname. If
 *       the desired protocol is TCP, or UDP the corresponding constants
 *       SOL_TCP, and SOL_UDP
 *       can also be used.
 *      </p>
 *      <table>
 *       Common protocols
 *       
 *        
 *         <tr valign="top">
 *          <td>Name</td>
 *          <td>Description</td>
 *         </tr>
 *        
 *        
 *         <tr valign="top">
 *          <td>icmp</td>
 *          <td>
 *           The Internet Control Message Protocol is used primarily by gateways
 *           and hosts to report errors in datagram communication. The "ping"
 *           command (present in most modern operating systems) is an example
 *           application of ICMP.
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>udp</td>
 *          <td>
 *           The User Datagram Protocol is a connectionless, unreliable,
 *           protocol with fixed record lengths. Due to these aspects, UDP
 *           requires a minimum amount of protocol overhead.
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>tcp</td>
 *          <td>
 *           The Transmission Control Protocol is a reliable, connection based,
 *           stream oriented, full duplex protocol. TCP guarantees that all data packets
 *           will be received in the order in which they were sent. If any packet is somehow
 *           lost during communication, TCP will automatically retransmit the packet until
 *           the destination host acknowledges that packet. For reliability and performance
 *           reasons, the TCP implementation itself decides the appropriate octet boundaries
 *           of the underlying datagram communication layer. Therefore, TCP applications must
 *           allow for the possibility of partial record transmission.
 *          </td>
 *         </tr>
 *        
 *       
 *      </table>
 * @return resource socket_create returns a socket resource on success,
 *   or false on error. The actual error code can be retrieved by calling
 *   socket_last_error. This error code may be passed to
 *   socket_strerror to get a textual explanation of the
 *   error.
 */
function socket_create ($domain, $type, $protocol) {}

/**
 * Opens a socket on port to accept connections
 * @link http://www.php.net/manual/en/function.socket-create-listen.php
 * @param port int <p>
 *       The port on which to listen on all interfaces.
 *      </p>
 * @param backlog int[optional] <p>
 *       The backlog parameter defines the maximum length
 *       the queue of pending connections may grow to.
 *       SOMAXCONN may be passed as
 *       backlog parameter, see
 *       socket_listen for more information.
 *      </p>
 * @return resource socket_create_listen returns a new socket resource
 *   on success or false on error. The error code can be retrieved with
 *   socket_last_error. This code may be passed to
 *   socket_strerror to get a textual explanation of the
 *   error.
 */
function socket_create_listen ($port, $backlog = null) {}

/**
 * Creates a pair of indistinguishable sockets and stores them in an array
 * @link http://www.php.net/manual/en/function.socket-create-pair.php
 * @param domain int <p>
 *       The domain parameter specifies the protocol
 *       family to be used by the socket. See socket_create
 *       for the full list.
 *      </p>
 * @param type int <p>
 *       The type parameter selects the type of communication
 *       to be used by the socket. See socket_create for the 
 *       full list.
 *      </p>
 * @param protocol int <p>
 *       The protocol parameter sets the specific
 *       protocol within the specified domain to be used
 *       when communicating on the returned socket. The proper value can be retrieved by
 *       name by using getprotobyname. If
 *       the desired protocol is TCP, or UDP the corresponding constants
 *       SOL_TCP, and SOL_UDP
 *       can also be used.
 *      </p>
 *      <p>
 *       See socket_create for the full list of supported 
 *       protocols.
 *      </p>
 * @param fd array <p>
 *       Reference to an array in which the two socket resources will be inserted.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function socket_create_pair ($domain, $type, $protocol, array &$fd) {}

/**
 * Accepts a connection on a socket
 * @link http://www.php.net/manual/en/function.socket-accept.php
 * @param socket resource <p>
 *       A valid socket resource created with socket_create.
 *      </p>
 * @return resource a new socket resource on success, or false on error. The actual
 *   error code can be retrieved by calling
 *   socket_last_error. This error code may be passed to
 *   socket_strerror to get a textual explanation of the
 *   error.
 */
function socket_accept ($socket) {}

/**
 * Sets nonblocking mode for file descriptor fd
 * @link http://www.php.net/manual/en/function.socket-set-nonblock.php
 * @param socket resource <p>
 *       A valid socket resource created with socket_create
 *       or socket_accept.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function socket_set_nonblock ($socket) {}

/**
 * Sets blocking mode on a socket resource
 * @link http://www.php.net/manual/en/function.socket-set-block.php
 * @param socket resource <p>
 *       A valid socket resource created with socket_create
 *       or socket_accept.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function socket_set_block ($socket) {}

/**
 * Listens for a connection on a socket
 * @link http://www.php.net/manual/en/function.socket-listen.php
 * @param socket resource <p>
 *       A valid socket resource created with socket_create.
 *      </p>
 * @param backlog int[optional] <p>
 *       A maximum of backlog incoming connections will be
 *       queued for processing. If a connection request arrives with the queue
 *       full the client may receive an error with an indication of
 *       ECONNREFUSED, or, if the underlying protocol supports
 *       retransmission, the request may be ignored so that retries may succeed.
 *      </p>
 *      
 *       <p>
 *        The maximum number passed to the backlog
 *        parameter highly depends on the underlying platform. On Linux, it is
 *        silently truncated to SOMAXCONN. On win32, if
 *        passed SOMAXCONN, the underlying service provider
 *        responsible for the socket will set the backlog to a maximum
 *        reasonable value. There is no standard provision to
 *        find out the actual backlog value on this platform.
 *       </p>
 * @return bool Returns true on success or false on failure. The error code can be retrieved with
 *   socket_last_error. This code may be passed to
 *   socket_strerror to get a textual explanation of the
 *   error.
 */
function socket_listen ($socket, $backlog = null) {}

/**
 * Closes a socket resource
 * @link http://www.php.net/manual/en/function.socket-close.php
 * @param socket resource <p>
 *        A valid socket resource created with socket_create
 *        or socket_accept.
 *       </p>
 * @return void 
 */
function socket_close ($socket) {}

/**
 * Write to a socket
 * @link http://www.php.net/manual/en/function.socket-write.php
 * @param socket resource <p>
 *      </p>
 * @param buffer string <p>
 *       The buffer to be written.
 *      </p>
 * @param length int[optional] <p>
 *       The optional parameter length can specify an
 *       alternate length of bytes written to the socket. If this length is
 *       greater then the buffer length, it is silently truncated to the length
 *       of the buffer.
 *      </p>
 * @return int the number of bytes successfully written to the socket&return.falseforfailure;.
 *   The error code can be retrieved with
 *   socket_last_error. This code may be passed to
 *   socket_strerror to get a textual explanation of the
 *   error.
 *  </p>
 *  
 *   <p>
 *    It is perfectly valid for socket_write to
 *    return zero which means no bytes have been written. Be sure to use the
 *    === operator to check for false in case of an
 *    error.
 */
function socket_write ($socket, $buffer, $length = null) {}

/**
 * Reads a maximum of length bytes from a socket
 * @link http://www.php.net/manual/en/function.socket-read.php
 * @param socket resource <p>
 *       A valid socket resource created with socket_create
 *       or socket_accept.
 *      </p>
 * @param length int <p>
 *       The maximum number of bytes read is specified by the
 *       length parameter. Otherwise you can use
 *       &#92;r, &#92;n,
 *       or &#92;0 to end reading (depending on the type
 *       parameter, see below).
 *      </p>
 * @param type int[optional] <p>
 *       Optional type parameter is a named constant:
 *       
 *        
 *         
 *          PHP_BINARY_READ (Default) - use the system
 *          recv() function. Safe for reading binary data.
 * @return string socket_read returns the data as a string on success,
 *   or false on error (including if the remote host has closed the
 *   connection). The error code can be retrieved with
 *   socket_last_error. This code may be passed to
 *   socket_strerror to get a textual representation of
 *   the error.
 *  </p>
 *  
 *   <p>
 *    socket_read returns a zero length string ("")
 *    when there is no more data to read.
 */
function socket_read ($socket, $length, $type = null) {}

/**
 * Queries the local side of the given socket which may either result in host/port or in a Unix filesystem path, dependent on its type
 * @link http://www.php.net/manual/en/function.socket-getsockname.php
 * @param socket resource <p>
 *       A valid socket resource created with socket_create 
 *       or socket_accept.
 *      </p>
 * @param addr string <p>
 *       If the given socket is of type AF_INET
 *       or AF_INET6, socket_getsockname
 *       will return the local IP address in appropriate notation (e.g.
 *       127.0.0.1 or fe80::1) in the
 *       address parameter and, if the optional
 *       port parameter is present, also the associated port.
 *      </p>
 *      <p>
 *       If the given socket is of type AF_UNIX,
 *       socket_getsockname will return the Unix filesystem
 *       path (e.g. /var/run/daemon.sock) in the
 *       address parameter.
 *      </p>
 * @param port int[optional] <p>
 *       If provided, this will hold the associated port.
 *      </p>
 * @return bool Returns true on success or false on failure. socket_getsockname may also return
 *   false if the socket type is not any of AF_INET,
 *   AF_INET6, or AF_UNIX, in which
 *   case the last socket error code is not updated.
 */
function socket_getsockname ($socket, &$addr, &$port = null) {}

/**
 * Queries the remote side of the given socket which may either result in host/port or in a Unix filesystem path, dependent on its type
 * @link http://www.php.net/manual/en/function.socket-getpeername.php
 * @param socket resource <p>
 *       A valid socket resource created with socket_create
 *       or socket_accept.
 *      </p>
 * @param address string <p>
 *       If the given socket is of type AF_INET or
 *       AF_INET6, socket_getpeername
 *       will return the peers (remote) IP address in
 *       appropriate notation (e.g. 127.0.0.1 or
 *       fe80::1) in the address
 *       parameter and, if the optional port parameter is
 *       present, also the associated port.
 *      </p>
 *      <p>
 *       If the given socket is of type AF_UNIX,
 *       socket_getpeername will return the Unix filesystem
 *       path (e.g. /var/run/daemon.sock) in the
 *       address parameter.
 *      </p>
 * @param port int[optional] <p>
 *       If given, this will hold the port associated to
 *       address.
 *      </p>
 * @return bool Returns true on success or false on failure. socket_getpeername may also return
 *   false if the socket type is not any of AF_INET,
 *   AF_INET6, or AF_UNIX, in which
 *   case the last socket error code is not updated.
 */
function socket_getpeername ($socket, &$address, &$port = null) {}

/**
 * Initiates a connection on a socket
 * @link http://www.php.net/manual/en/function.socket-connect.php
 * @param socket resource <p>
 *      </p>
 * @param address string <p>
 *       The address parameter is either an IPv4 address
 *       in dotted-quad notation (e.g. 127.0.0.1) if 
 *       socket is AF_INET, a valid 
 *       IPv6 address (e.g. ::1) if IPv6 support is enabled and 
 *       socket is AF_INET6
 *       or the pathname of a Unix domain socket, if the socket family is
 *       AF_UNIX.
 *      </p>
 * @param port int[optional] <p>
 *       The port parameter is only used and is mandatory
 *       when connecting to an AF_INET or an 
 *       AF_INET6 socket, and designates
 *       the port on the remote host to which a connection should be made.
 *      </p>
 * @return bool Returns true on success or false on failure. The error code can be retrieved with
 *   socket_last_error. This code may be passed to
 *   socket_strerror to get a textual explanation of the
 *   error.
 *  </p>
 *  
 *   <p>
 *    If the socket is non-blocking then this function returns false with an
 *    error Operation now in progress.
 */
function socket_connect ($socket, $address, $port = null) {}

/**
 * Return a string describing a socket error
 * @link http://www.php.net/manual/en/function.socket-strerror.php
 * @param errno int <p>
 *       A valid socket error number, likely produced by 
 *       socket_last_error.
 *      </p>
 * @return string the error message associated with the errno
 *   parameter.
 */
function socket_strerror ($errno) {}

/**
 * Binds a name to a socket
 * @link http://www.php.net/manual/en/function.socket-bind.php
 * @param socket resource <p>
 *       A valid socket resource created with socket_create.
 *      </p>
 * @param address string <p>
 *       If the socket is of the AF_INET family, the
 *       address is an IP in dotted-quad notation
 *       (e.g. 127.0.0.1).
 *      </p>
 *      <p>
 *       If the socket is of the AF_UNIX family, the
 *       address is the path of a
 *       Unix-domain socket (e.g. /tmp/my.sock).
 *      </p>
 * @param port int[optional] <p>
 *       The port parameter is only used when
 *       binding an AF_INET socket, and designates
 *       the port on which to listen for connections.
 *      </p>
 * @return bool Returns true on success or false on failure.
 *  </p>
 *  <p>
 *   The error code can be retrieved with socket_last_error.
 *   This code may be passed to socket_strerror to get a
 *   textual explanation of the error.
 */
function socket_bind ($socket, $address, $port = null) {}

/**
 * Receives data from a connected socket
 * @link http://www.php.net/manual/en/function.socket-recv.php
 * @param socket resource <p>
 *       The socket must be a socket resource previously
 *       created by socket_create().
 *      </p>
 * @param buf string <p>
 *       The data received will be fetched to the variable specified with
 *       buf. If an error occurs, if the
 *       connection is reset, or if no data is
 *       available, buf will be set to &null;.
 *      </p>
 * @param len int <p>
 *       Up to len bytes will be fetched from remote host.
 *      </p>
 * @param flags int <p>
 *       The value of flags can be any combination of 
 *       the following flags, joined with the binary OR (|)
 *       operator.
 *      </p>
 *      
 *      <table>
 *       Possible values for flags
 *       
 *        
 *         <tr valign="top">
 *          <td>Flag</td>
 *          <td>Description</td>
 *         </tr>
 *        
 *        
 *         <tr valign="top">
 *          <td>MSG_OOB</td>
 *          <td>
 *           Process out-of-band data.
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>MSG_PEEK</td>
 *          <td>
 *           Receive data from the beginning of the receive queue without
 *           removing it from the queue.
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>MSG_WAITALL</td>
 *          <td>
 *           Block until at least len are received.
 *           However, if a signal is caught or the remote host disconnects, the
 *           function may return less data.
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>MSG_DONTWAIT</td>
 *          <td>
 *           With this flag set, the function returns even if it would normally
 *           have blocked.
 *          </td>
 *         </tr>
 *        
 *       
 *      </table>
 * @return int socket_recv returns the number of bytes received,
 *   or false if there was an error. The actual error code can be retrieved by 
 *   calling socket_last_error. This error code may be
 *   passed to socket_strerror to get a textual explanation
 *   of the error.
 */
function socket_recv ($socket, &$buf, $len, $flags) {}

/**
 * Sends data to a connected socket
 * @link http://www.php.net/manual/en/function.socket-send.php
 * @param socket resource <p>
 *       A valid socket resource created with socket_create
 *       or socket_accept.
 *      </p>
 * @param buf string <p>
 *       A buffer containing the data that will be sent to the remote host.
 *      </p>
 * @param len int <p>
 *       The number of bytes that will be sent to the remote host from 
 *       buf.
 *      </p>
 * @param flags int <p>
 *       The value of flags can be any combination of 
 *       the following flags, joined with the binary OR (|)
 *       operator.
 *       <table>
 *        Possible values for flags
 *        
 *         
 *          <tr valign="top">
 *           <td>MSG_OOB</td>
 *           <td>
 *            Send OOB (out-of-band) data.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>MSG_EOR</td>
 *           <td>
 *            Indicate a record mark. The sent data completes the record.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>MSG_EOF</td>
 *           <td>
 *            Close the sender side of the socket and include an appropriate
 *            notification of this at the end of the sent data. The sent data
 *            completes the transaction.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>MSG_DONTROUTE</td>
 *           <td>
 *            Bypass routing, use direct interface.
 *           </td>
 *          </tr>
 *         
 *        
 *       </table>
 *      </p>
 * @return int socket_send returns the number of bytes sent, or false on error.
 */
function socket_send ($socket, $buf, $len, $flags) {}

/**
 * Receives data from a socket whether or not it is connection-oriented
 * @link http://www.php.net/manual/en/function.socket-recvfrom.php
 * @param socket resource <p>
 *       The socket must be a socket resource previously
 *       created by socket_create().
 *      </p>
 * @param buf string <p>
 *       The data received will be fetched to the variable specified with
 *       buf.
 *      </p>
 * @param len int <p>
 *       Up to len bytes will be fetched from remote host.
 *      </p>
 * @param flags int <p>
 *       The value of flags can be any combination of 
 *       the following flags, joined with the binary OR (|)
 *       operator.
 *      </p>
 *      
 *      <table>
 *       Possible values for flags
 *       
 *        
 *         <tr valign="top">
 *          <td>Flag</td>
 *          <td>Description</td>
 *         </tr>
 *        
 *        
 *         <tr valign="top">
 *          <td>MSG_OOB</td>
 *          <td>
 *           Process out-of-band data.
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>MSG_PEEK</td>
 *          <td>
 *           Receive data from the beginning of the receive queue without
 *           removing it from the queue.
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>MSG_WAITALL</td>
 *          <td>
 *           Block until at least len are received.
 *           However, if a signal is caught or the remote host disconnects, the
 *           function may return less data.
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>MSG_DONTWAIT</td>
 *          <td>
 *           With this flag set, the function returns even if it would normally
 *           have blocked.
 *          </td>
 *         </tr>
 *        
 *       
 *      </table>
 * @param name string <p>
 *       If the socket is of the type AF_UNIX type,
 *       name is the path to the file. Else, for
 *       unconnected sockets, name is the IP address of,
 *       the remote host, or &null; if the socket is connection-oriented.
 *      </p>
 * @param port int[optional] <p>
 *       This argument only applies to AF_INET and
 *       AF_INET6 sockets, and specifies the remote port
 *       from which the data is received. If the socket is connection-oriented,
 *       port will be &null;.
 *      </p>
 * @return int socket_recvfrom returns the number of bytes received,
 *   or false if there was an error. The actual error code can be retrieved by 
 *   calling socket_last_error. This error code may be
 *   passed to socket_strerror to get a textual explanation
 *   of the error.
 */
function socket_recvfrom ($socket, &$buf, $len, $flags, &$name, &$port = null) {}

/**
 * Sends a message to a socket, whether it is connected or not
 * @link http://www.php.net/manual/en/function.socket-sendto.php
 * @param socket resource <p>
 *       A valid socket resource created using socket_create.
 *      </p>
 * @param buf string <p>
 *       The sent data will be taken from buffer buf.
 *      </p>
 * @param len int <p>
 *       len bytes from buf will be
 *       sent.
 *      </p>
 * @param flags int <p>
 *       The value of flags can be any combination of 
 *       the following flags, joined with the binary OR (|)
 *       operator.
 *       <table>
 *        Possible values for flags
 *        
 *         
 *          <tr valign="top">
 *           <td>MSG_OOB</td>
 *           <td>
 *            Send OOB (out-of-band) data.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>MSG_EOR</td>
 *           <td>
 *            Indicate a record mark. The sent data completes the record.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>MSG_EOF</td>
 *           <td>
 *            Close the sender side of the socket and include an appropriate
 *            notification of this at the end of the sent data. The sent data
 *            completes the transaction.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>MSG_DONTROUTE</td>
 *           <td>
 *            Bypass routing, use direct interface.
 *           </td>
 *          </tr>
 *         
 *        
 *       </table>
 *      </p>
 * @param addr string <p>
 *       IP address of the remote host.
 *      </p>
 * @param port int[optional] <p>
 *       port is the remote port number at which the data
 *       will be sent.
 *      </p>
 * @return int socket_sendto returns the number of bytes sent to the
 *   remote host, or false if an error occurred.
 */
function socket_sendto ($socket, $buf, $len, $flags, $addr, $port = null) {}

/**
 * Gets socket options for the socket
 * @link http://www.php.net/manual/en/function.socket-get-option.php
 * @param socket resource <p>
 *       A valid socket resource created with socket_create
 *       or socket_accept.
 *      </p>
 * @param level int <p>
 *       The level parameter specifies the protocol
 *       level at which the option resides. For example, to retrieve options at
 *       the socket level, a level parameter of
 *       SOL_SOCKET would be used. Other levels, such as
 *       TCP, can be used by
 *       specifying the protocol number of that level. Protocol numbers can be
 *       found by using the getprotobyname function.
 *      </p>
 * @param optname int <table>
 *       Available Socket Options
 *       
 *        
 *         <tr valign="top">
 *          <td>Option</td>
 *          <td>Description</td>
 *          <td>Type</td>
 *         </tr>
 *        
 *        
 *         <tr valign="top">
 *          <td>SO_DEBUG</td>
 *          <td>
 *           Reports whether debugging information is being recorded.
 *          </td>
 *          <td>
 *           int
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>SO_BROADCAST</td>
 *          <td>
 *           Reports whether transmission of broadcast messages is supported.
 *          </td>
 *          <td>
 *           int
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>SO_REUSEADDR</td>
 *          <td>
 *           Reports whether local addresses can be reused.
 *          </td>
 *          <td>
 *           int
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>SO_KEEPALIVE</td>
 *          <td>
 *           Reports whether connections are kept active with periodic transmission
 *           of messages. If the connected socket fails to respond to these messages,
 *           the connection is broken and processes writing to that socket are notified
 *           with a SIGPIPE signal.
 *          </td>
 *          <td>
 *           int
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>SO_LINGER</td>
 *          <td>
 *           <p>
 *            Reports whether the socket lingers on 
 *            socket_close if data is present. By default, 
 *            when the socket is closed, it attempts to send all unsent data.
 *            In the case of a connection-oriented socket, 
 *            socket_close will wait for its peer to
 *            acknowledge the data. 
 *           </p>
 *           <p>
 *            If l_onoff is non-zero and 
 *            l_linger is zero, all the 
 *            unsent data will be discarded and RST (reset) is sent to the 
 *            peer in the case of a connection-oriented socket. 
 *           </p>
 *           <p>
 *            On the other hand, if l_onoff is 
 *            non-zero and l_linger is non-zero,
 *            socket_close will block until all the data 
 *            is sent or the time specified in l_linger
 *            elapses. If the socket is non-blocking, 
 *            socket_close will fail and return an error.
 *           </p>
 *          </td>
 *          <td>
 *           array. The array will contain two keys:
 *           l_onoff and 
 *           l_linger. 
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>SO_OOBINLINE</td>
 *          <td>
 *           Reports whether the socket leaves out-of-band data inline.
 *          </td>
 *          <td>
 *           int
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>SO_SNDBUF</td>
 *          <td>
 *           Reports the size of the send buffer.
 *          </td>
 *          <td>
 *           int
 *          </td>          
 *         </tr>
 *         <tr valign="top">
 *          <td>SO_RCVBUF</td>
 *          <td>
 *           Reports the size of the receive buffer.
 *          </td>
 *          <td>
 *           int
 *          </td>         
 *         </tr>
 *         <tr valign="top">
 *          <td>SO_ERROR</td>
 *          <td>
 *           Reports information about error status and clears it.
 *          </td>
 *          <td>
 *           int (cannot be set by socket_set_option)
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>SO_TYPE</td>
 *          <td>
 *           Reports the socket type (e.g. 
 *           SOCK_STREAM).
 *          </td>
 *          <td>
 *           int (cannot be set by socket_set_option)
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>SO_DONTROUTE</td>
 *          <td>
 *           Reports whether outgoing messages bypass the standard routing facilities.
 *          </td>
 *          <td>
 *           int
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>SO_RCVLOWAT</td>
 *          <td>
 *           Reports the minimum number of bytes to process for socket 
 *           input operations.
 *          </td>
 *          <td>
 *           int
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>SO_RCVTIMEO</td>
 *          <td>
 *           Reports the timeout value for input operations.
 *          </td>
 *          <td>
 *           array. The array will contain two keys:
 *           sec which is the seconds part on the timeout
 *           value and usec which is the microsecond part 
 *           of the timeout value. 
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>SO_SNDTIMEO</td>
 *          <td>
 *           Reports the timeout value specifying the amount of time that an output
 *           function blocks because flow control prevents data from being sent.
 *          </td>
 *          <td>
 *           array. The array will contain two keys:
 *           sec which is the seconds part on the timeout
 *           value and usec which is the microsecond part 
 *           of the timeout value. 
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>SO_SNDLOWAT</td>
 *          <td>
 *           Reports the minimum number of bytes to process for socket output operations.
 *          </td>
 *          <td>
 *           int
 *          </td>
 *         </tr>
 *         <tr valign="top">
 *          <td>TCP_NODELAY</td>
 *          <td>
 *           Reports whether the Nagle TCP algorithm is disabled.
 *          </td>
 *          <td>
 *           int
 *          </td>
 *         </tr>
 *        
 *       
 *      </table>
 * @return mixed the value of the given option, or false on errors.
 */
function socket_get_option ($socket, $level, $optname) {}

/**
 * Sets socket options for the socket
 * @link http://www.php.net/manual/en/function.socket-set-option.php
 * @param socket resource <p>
 *       A valid socket resource created with socket_create
 *       or socket_accept.
 *      </p>
 * @param level int <p>
 *       The level parameter specifies the protocol
 *       level at which the option resides. For example, to retrieve options at
 *       the socket level, a level parameter of
 *       SOL_SOCKET would be used. Other levels, such as
 *       TCP, can be used by specifying the protocol number of that level. 
 *       Protocol numbers can be found by using the 
 *       getprotobyname function.
 *      </p>
 * @param optname int <p>
 *       The available socket options are the same as those for the
 *       socket_get_option function.
 *      </p>
 * @param optval mixed <p>
 *       The option value.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function socket_set_option ($socket, $level, $optname, $optval) {}

/**
 * Shuts down a socket for receiving, sending, or both
 * @link http://www.php.net/manual/en/function.socket-shutdown.php
 * @param socket resource <p>
 *       A valid socket resource created with socket_create.
 *      </p>
 * @param how int[optional] <p>
 *       The value of how can be one of the following:
 *       <table>
 *        possible values for how
 *        
 *         
 *          <tr valign="top">
 *           <td>0</td>
 *           <td>
 *            Shutdown socket reading
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>1</td>
 *           <td>
 *            Shutdown socket writing
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>2</td>
 *           <td>
 *            Shutdown socket reading and writing
 *           </td>
 *          </tr>
 *         
 *        
 *       </table>
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function socket_shutdown ($socket, $how = null) {}

/**
 * Returns the last error on the socket
 * @link http://www.php.net/manual/en/function.socket-last-error.php
 * @param socket resource[optional] <p>
 *       A valid socket resource created with socket_create.
 *      </p>
 * @return int This function returns a socket error code.
 */
function socket_last_error ($socket = null) {}

/**
 * Clears the error on the socket or the last error code
 * @link http://www.php.net/manual/en/function.socket-clear-error.php
 * @param socket resource[optional] <p>
 *       A valid socket resource created with socket_create.
 *      </p>
 * @return void 
 */
function socket_clear_error ($socket = null) {}

/**
 * @param socket
 * @param level
 * @param optname
 */
function socket_getopt ($socket, $level, $optname) {}

/**
 * @param socket
 * @param level
 * @param optname
 * @param optval
 */
function socket_setopt ($socket, $level, $optname, $optval) {}

define ('AF_UNIX', 1);
define ('AF_INET', 2);

/**
 * Only available if compiled with IPv6 support.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('AF_INET6', 10);
define ('SOCK_STREAM', 1);
define ('SOCK_DGRAM', 2);
define ('SOCK_RAW', 3);
define ('SOCK_SEQPACKET', 5);
define ('SOCK_RDM', 4);
define ('MSG_OOB', 1);
define ('MSG_WAITALL', 256);
define ('MSG_DONTWAIT', 64);
define ('MSG_PEEK', 2);
define ('MSG_DONTROUTE', 4);

/**
 * Not available on Windows platforms.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('MSG_EOR', 128);

/**
 * Not available on Windows platforms.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('MSG_EOF', 512);
define ('SO_DEBUG', 1);
define ('SO_REUSEADDR', 2);
define ('SO_KEEPALIVE', 9);
define ('SO_DONTROUTE', 5);
define ('SO_LINGER', 13);
define ('SO_BROADCAST', 6);
define ('SO_OOBINLINE', 10);
define ('SO_SNDBUF', 7);
define ('SO_RCVBUF', 8);
define ('SO_SNDLOWAT', 19);
define ('SO_RCVLOWAT', 18);
define ('SO_SNDTIMEO', 21);
define ('SO_RCVTIMEO', 20);
define ('SO_TYPE', 3);
define ('SO_ERROR', 4);
define ('SOL_SOCKET', 1);
define ('SOMAXCONN', 128);

/**
 * Used to disable Nagle TCP algorithm.
 *     Added in PHP 5.2.7.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('TCP_NODELAY', 1);
define ('PHP_NORMAL_READ', 1);
define ('PHP_BINARY_READ', 2);

/**
 * Operation not permitted.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EPERM', 1);

/**
 * No such file or directory.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOENT', 2);

/**
 * Interrupted system call.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EINTR', 4);

/**
 * I/O error.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EIO', 5);

/**
 * No such device or address.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENXIO', 6);

/**
 * Arg list too long.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_E2BIG', 7);

/**
 * Bad file number.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EBADF', 9);

/**
 * Try again.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EAGAIN', 11);

/**
 * Out of memory.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOMEM', 12);

/**
 * Permission denied.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EACCES', 13);

/**
 * Bad address.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EFAULT', 14);

/**
 * Block device required.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOTBLK', 15);

/**
 * Device or resource busy.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EBUSY', 16);

/**
 * File exists.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EEXIST', 17);

/**
 * Cross-device link.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EXDEV', 18);

/**
 * No such device.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENODEV', 19);

/**
 * Not a directory.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOTDIR', 20);

/**
 * Is a directory.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EISDIR', 21);

/**
 * Invalid argument.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EINVAL', 22);

/**
 * File table overflow.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENFILE', 23);

/**
 * Too many open files.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EMFILE', 24);

/**
 * Not a typewriter.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOTTY', 25);

/**
 * No space left on device.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOSPC', 28);

/**
 * Illegal seek.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ESPIPE', 29);

/**
 * Read-only file system.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EROFS', 30);

/**
 * Too many links.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EMLINK', 31);

/**
 * Broken pipe.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EPIPE', 32);

/**
 * File name too long.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENAMETOOLONG', 36);

/**
 * No record locks available.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOLCK', 37);

/**
 * Function not implemented.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOSYS', 38);

/**
 * Directory not empty.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOTEMPTY', 39);

/**
 * Too many symbolic links encountered.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ELOOP', 40);

/**
 * Operation would block.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EWOULDBLOCK', 11);

/**
 * No message of desired type.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOMSG', 42);

/**
 * Identifier removed.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EIDRM', 43);

/**
 * Channel number out of range.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ECHRNG', 44);

/**
 * Level 2 not synchronized.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EL2NSYNC', 45);

/**
 * Level 3 halted.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EL3HLT', 46);

/**
 * Level 3 reset.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EL3RST', 47);

/**
 * Link number out of range.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ELNRNG', 48);

/**
 * Protocol driver not attached.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EUNATCH', 49);

/**
 * No CSI structure available.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOCSI', 50);

/**
 * Level 2 halted.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EL2HLT', 51);

/**
 * Invalid exchange.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EBADE', 52);

/**
 * Invalid request descriptor.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EBADR', 53);

/**
 * Exchange full.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EXFULL', 54);

/**
 * No anode.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOANO', 55);

/**
 * Invalid request code.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EBADRQC', 56);

/**
 * Invalid slot.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EBADSLT', 57);

/**
 * Device not a stream.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOSTR', 60);

/**
 * No data available.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENODATA', 61);

/**
 * Timer expired.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ETIME', 62);

/**
 * Out of streams resources.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOSR', 63);

/**
 * Machine is not on the network.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENONET', 64);

/**
 * Object is remote.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EREMOTE', 66);

/**
 * Link has been severed.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOLINK', 67);

/**
 * Advertise error.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EADV', 68);

/**
 * Srmount error.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ESRMNT', 69);

/**
 * Communication error on send.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ECOMM', 70);

/**
 * Protocol error.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EPROTO', 71);

/**
 * Multihop attempted.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EMULTIHOP', 72);

/**
 * Not a data message.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EBADMSG', 74);

/**
 * Name not unique on network.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOTUNIQ', 76);

/**
 * File descriptor in bad state.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EBADFD', 77);

/**
 * Remote address changed.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EREMCHG', 78);

/**
 * Interrupted system call should be restarted.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ERESTART', 85);

/**
 * Streams pipe error.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ESTRPIPE', 86);

/**
 * Too many users.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EUSERS', 87);

/**
 * Socket operation on non-socket.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOTSOCK', 88);

/**
 * Destination address required.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EDESTADDRREQ', 89);

/**
 * Message too long.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EMSGSIZE', 90);

/**
 * Protocol wrong type for socket.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EPROTOTYPE', 91);
define ('SOCKET_ENOPROTOOPT', 92);

/**
 * Protocol not supported.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EPROTONOSUPPORT', 93);

/**
 * Socket type not supported.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ESOCKTNOSUPPORT', 94);

/**
 * Operation not supported on transport endpoint.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EOPNOTSUPP', 95);

/**
 * Protocol family not supported.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EPFNOSUPPORT', 96);

/**
 * Address family not supported by protocol.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EAFNOSUPPORT', 97);
define ('SOCKET_EADDRINUSE', 98);

/**
 * Cannot assign requested address.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EADDRNOTAVAIL', 99);

/**
 * Network is down.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENETDOWN', 100);

/**
 * Network is unreachable.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENETUNREACH', 101);

/**
 * Network dropped connection because of reset.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENETRESET', 102);

/**
 * Software caused connection abort.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ECONNABORTED', 103);

/**
 * Connection reset by peer.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ECONNRESET', 104);

/**
 * No buffer space available.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOBUFS', 105);

/**
 * Transport endpoint is already connected.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EISCONN', 106);

/**
 * Transport endpoint is not connected.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOTCONN', 107);

/**
 * Cannot send after transport endpoint shutdown.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ESHUTDOWN', 108);

/**
 * Too many references: cannot splice.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ETOOMANYREFS', 109);

/**
 * Connection timed out.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ETIMEDOUT', 110);

/**
 * Connection refused.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ECONNREFUSED', 111);

/**
 * Host is down.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EHOSTDOWN', 112);

/**
 * No route to host.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EHOSTUNREACH', 113);

/**
 * Operation already in progress.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EALREADY', 114);

/**
 * Operation now in progress.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EINPROGRESS', 115);

/**
 * Is a named type file.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EISNAM', 120);

/**
 * Remote I/O error.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EREMOTEIO', 121);

/**
 * Quota exceeded.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EDQUOT', 122);

/**
 * No medium found.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_ENOMEDIUM', 123);

/**
 * Wrong medium type.
 * @link http://www.php.net/manual/en/sockets.constants.php
 */
define ('SOCKET_EMEDIUMTYPE', 124);
define ('SOL_TCP', 6);
define ('SOL_UDP', 17);

// End of sockets v.
?>
