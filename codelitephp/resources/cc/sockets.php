<?php

// Start of sockets v.

/**
 * @param read_fds
 * @param write_fds
 * @param except_fds
 * @param tv_sec
 * @param tv_usec[optional]
 */
function socket_select (&$read_fds, &$write_fds, &$except_fds, $tv_sec, $tv_usec) {}

/**
 * @param domain
 * @param type
 * @param protocol
 */
function socket_create ($domain, $type, $protocol) {}

/**
 * @param port
 * @param backlog[optional]
 */
function socket_create_listen ($port, $backlog) {}

/**
 * @param domain
 * @param type
 * @param protocol
 * @param fd
 */
function socket_create_pair ($domain, $type, $protocol, &$fd) {}

/**
 * @param socket
 */
function socket_accept ($socket) {}

/**
 * @param socket
 */
function socket_set_nonblock ($socket) {}

/**
 * @param socket
 */
function socket_set_block ($socket) {}

/**
 * @param socket
 * @param backlog[optional]
 */
function socket_listen ($socket, $backlog) {}

/**
 * @param socket
 */
function socket_close ($socket) {}

/**
 * @param socket
 * @param buf
 * @param length[optional]
 */
function socket_write ($socket, $buf, $length) {}

/**
 * @param socket
 * @param length
 * @param type[optional]
 */
function socket_read ($socket, $length, $type) {}

/**
 * @param socket
 * @param addr
 * @param port[optional]
 */
function socket_getsockname ($socket, &$addr, &$port) {}

/**
 * @param socket
 * @param addr
 * @param port[optional]
 */
function socket_getpeername ($socket, &$addr, &$port) {}

/**
 * @param socket
 * @param addr
 * @param port[optional]
 */
function socket_connect ($socket, $addr, $port) {}

/**
 * @param errno
 */
function socket_strerror ($errno) {}

/**
 * @param socket
 * @param addr
 * @param port[optional]
 */
function socket_bind ($socket, $addr, $port) {}

/**
 * @param socket
 * @param buf
 * @param len
 * @param flags
 */
function socket_recv ($socket, &$buf, $len, $flags) {}

/**
 * @param socket
 * @param buf
 * @param len
 * @param flags
 */
function socket_send ($socket, $buf, $len, $flags) {}

/**
 * @param socket
 * @param buf
 * @param len
 * @param flags
 * @param name
 * @param port[optional]
 */
function socket_recvfrom ($socket, &$buf, $len, $flags, &$name, &$port) {}

/**
 * @param socket
 * @param buf
 * @param len
 * @param flags
 * @param addr
 * @param port[optional]
 */
function socket_sendto ($socket, $buf, $len, $flags, $addr, $port) {}

/**
 * @param socket
 * @param level
 * @param optname
 */
function socket_get_option ($socket, $level, $optname) {}

/**
 * @param socket
 * @param level
 * @param optname
 * @param optval
 */
function socket_set_option ($socket, $level, $optname, $optval) {}

/**
 * @param socket
 * @param how[optional]
 */
function socket_shutdown ($socket, $how) {}

/**
 * @param socket[optional]
 */
function socket_last_error ($socket) {}

/**
 * @param socket[optional]
 */
function socket_clear_error ($socket) {}

/**
 * @param stream
 */
function socket_import_stream ($stream) {}

/**
 * @param socket
 * @param msghdr
 * @param flags
 */
function socket_sendmsg ($socket, $msghdr, $flags) {}

/**
 * @param socket
 * @param msghdr
 * @param flags
 */
function socket_recvmsg ($socket, &$msghdr, $flags) {}

/**
 * @param level
 * @param type
 */
function socket_cmsg_space ($level, $type) {}

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
define ('MSG_CTRUNC', 8);
define ('MSG_TRUNC', 32);
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
define ('MSG_CONFIRM', 2048);
define ('MSG_ERRQUEUE', 8192);
define ('MSG_NOSIGNAL', 16384);
define ('MSG_DONTWAIT', 64);
define ('MSG_MORE', 32768);
define ('MSG_CMSG_CLOEXEC', 1073741824);
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
define ('SO_BINDTODEVICE', 25);
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
define ('MCAST_JOIN_GROUP', 42);
define ('MCAST_LEAVE_GROUP', 45);
define ('MCAST_BLOCK_SOURCE', 43);
define ('MCAST_UNBLOCK_SOURCE', 44);
define ('MCAST_JOIN_SOURCE_GROUP', 46);
define ('MCAST_LEAVE_SOURCE_GROUP', 47);
define ('IP_MULTICAST_IF', 32);
define ('IP_MULTICAST_TTL', 33);
define ('IP_MULTICAST_LOOP', 34);
define ('IPV6_MULTICAST_IF', 17);
define ('IPV6_MULTICAST_HOPS', 18);
define ('IPV6_MULTICAST_LOOP', 19);

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
define ('IPPROTO_IP', 0);
define ('IPPROTO_IPV6', 41);
define ('SOL_TCP', 6);
define ('SOL_UDP', 17);
define ('IPV6_UNICAST_HOPS', 16);
define ('IPV6_RECVPKTINFO', 49);
define ('IPV6_PKTINFO', 50);
define ('IPV6_RECVHOPLIMIT', 51);
define ('IPV6_HOPLIMIT', 52);
define ('IPV6_RECVTCLASS', 66);
define ('IPV6_TCLASS', 67);
define ('SCM_RIGHTS', 1);
define ('SCM_CREDENTIALS', 2);
define ('SO_PASSCRED', 16);

// End of sockets v.
?>
