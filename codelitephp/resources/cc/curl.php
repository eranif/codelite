<?php

// Start of curl v.

class CURLFile  {
	public $name;
	public $mime;
	public $postname;


	/**
	 * @param filename
	 * @param mimetype[optional]
	 * @param postname[optional]
	 */
	public function __construct ($filename, $mimetype, $postname) {}

	public function getFilename () {}

	public function getMimeType () {}

	/**
	 * @param name
	 */
	public function setMimeType ($name) {}

	public function getPostFilename () {}

	/**
	 * @param name
	 */
	public function setPostFilename ($name) {}

	public function __wakeup () {}

}

/**
 * @param url[optional]
 */
function curl_init ($url) {}

/**
 * @param ch
 */
function curl_copy_handle ($ch) {}

/**
 * @param version[optional]
 */
function curl_version ($version) {}

/**
 * @param ch
 * @param option
 * @param value
 */
function curl_setopt ($ch, $option, $value) {}

/**
 * @param ch
 * @param options
 */
function curl_setopt_array ($charray , $options) {}

/**
 * @param ch
 */
function curl_exec ($ch) {}

/**
 * @param ch
 * @param option[optional]
 */
function curl_getinfo ($ch, $option) {}

/**
 * @param ch
 */
function curl_error ($ch) {}

/**
 * @param ch
 */
function curl_errno ($ch) {}

/**
 * @param ch
 */
function curl_close ($ch) {}

/**
 * @param errornum
 */
function curl_strerror ($errornum) {}

/**
 * @param errornum
 */
function curl_multi_strerror ($errornum) {}

/**
 * @param ch
 */
function curl_reset ($ch) {}

/**
 * @param ch
 * @param str
 */
function curl_escape ($ch, $str) {}

/**
 * @param ch
 * @param str
 */
function curl_unescape ($ch, $str) {}

/**
 * @param ch
 * @param bitmask
 */
function curl_pause ($ch, $bitmask) {}

function curl_multi_init () {}

/**
 * @param mh
 * @param ch
 */
function curl_multi_add_handle ($mh, $ch) {}

/**
 * @param mh
 * @param ch
 */
function curl_multi_remove_handle ($mh, $ch) {}

/**
 * @param mh
 * @param timeout[optional]
 */
function curl_multi_select ($mh, $timeout) {}

/**
 * @param mh
 * @param still_running[optional]
 */
function curl_multi_exec ($mh, &$still_running) {}

/**
 * @param ch
 */
function curl_multi_getcontent ($ch) {}

/**
 * @param mh
 * @param msgs_in_queue[optional]
 */
function curl_multi_info_read ($mh, &$msgs_in_queue) {}

/**
 * @param mh
 */
function curl_multi_close ($mh) {}

/**
 * @param sh
 * @param option
 * @param value
 */
function curl_multi_setopt ($sh, $option, $value) {}

function curl_share_init () {}

/**
 * @param sh
 */
function curl_share_close ($sh) {}

/**
 * @param sh
 * @param option
 * @param value
 */
function curl_share_setopt ($sh, $option, $value) {}

/**
 * @param filename
 * @param mimetype[optional]
 * @param postname[optional]
 */
function curl_file_create ($filename, $mimetype, $postname) {}


/**
 * Available since PHP 5.1.0
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLOPT_AUTOREFERER', 58);
define ('CURLOPT_BINARYTRANSFER', 19914);
define ('CURLOPT_BUFFERSIZE', 98);
define ('CURLOPT_CAINFO', 10065);
define ('CURLOPT_CAPATH', 10097);
define ('CURLOPT_CONNECTTIMEOUT', 78);
define ('CURLOPT_COOKIE', 10022);
define ('CURLOPT_COOKIEFILE', 10031);
define ('CURLOPT_COOKIEJAR', 10082);

/**
 * Available since PHP 5.1.0
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLOPT_COOKIESESSION', 96);
define ('CURLOPT_CRLF', 27);
define ('CURLOPT_CUSTOMREQUEST', 10036);
define ('CURLOPT_DNS_CACHE_TIMEOUT', 92);
define ('CURLOPT_DNS_USE_GLOBAL_CACHE', 91);
define ('CURLOPT_EGDSOCKET', 10077);
define ('CURLOPT_ENCODING', 10102);
define ('CURLOPT_FAILONERROR', 45);
define ('CURLOPT_FILE', 10001);
define ('CURLOPT_FILETIME', 69);

/**
 * This constant is not available when open_basedir 
 *     or safe_mode are enabled.
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLOPT_FOLLOWLOCATION', 52);
define ('CURLOPT_FORBID_REUSE', 75);
define ('CURLOPT_FRESH_CONNECT', 74);
define ('CURLOPT_FTPAPPEND', 50);
define ('CURLOPT_FTPLISTONLY', 48);
define ('CURLOPT_FTPPORT', 10017);
define ('CURLOPT_FTP_USE_EPRT', 106);
define ('CURLOPT_FTP_USE_EPSV', 85);
define ('CURLOPT_HEADER', 42);
define ('CURLOPT_HEADERFUNCTION', 20079);
define ('CURLOPT_HTTP200ALIASES', 10104);
define ('CURLOPT_HTTPGET', 80);
define ('CURLOPT_HTTPHEADER', 10023);
define ('CURLOPT_HTTPPROXYTUNNEL', 61);
define ('CURLOPT_HTTP_VERSION', 84);
define ('CURLOPT_INFILE', 10009);
define ('CURLOPT_INFILESIZE', 14);
define ('CURLOPT_INTERFACE', 10062);
define ('CURLOPT_KRB4LEVEL', 10063);
define ('CURLOPT_LOW_SPEED_LIMIT', 19);
define ('CURLOPT_LOW_SPEED_TIME', 20);
define ('CURLOPT_MAXCONNECTS', 71);
define ('CURLOPT_MAXREDIRS', 68);
define ('CURLOPT_NETRC', 51);
define ('CURLOPT_NOBODY', 44);
define ('CURLOPT_NOPROGRESS', 43);
define ('CURLOPT_NOSIGNAL', 99);
define ('CURLOPT_PORT', 3);
define ('CURLOPT_POST', 47);
define ('CURLOPT_POSTFIELDS', 10015);
define ('CURLOPT_POSTQUOTE', 10039);
define ('CURLOPT_PREQUOTE', 10093);

/**
 * Available since PHP 5.2.4
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLOPT_PRIVATE', 10103);

/**
 * Available since PHP 5.3.0
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLOPT_PROGRESSFUNCTION', 20056);
define ('CURLOPT_PROXY', 10004);
define ('CURLOPT_PROXYPORT', 59);
define ('CURLOPT_PROXYTYPE', 101);
define ('CURLOPT_PROXYUSERPWD', 10006);
define ('CURLOPT_PUT', 54);
define ('CURLOPT_QUOTE', 10028);
define ('CURLOPT_RANDOM_FILE', 10076);
define ('CURLOPT_RANGE', 10007);
define ('CURLOPT_READDATA', 10009);
define ('CURLOPT_READFUNCTION', 20012);
define ('CURLOPT_REFERER', 10016);
define ('CURLOPT_RESUME_FROM', 21);
define ('CURLOPT_RETURNTRANSFER', 19913);
define ('CURLOPT_SHARE', 10100);
define ('CURLOPT_SSLCERT', 10025);
define ('CURLOPT_SSLCERTPASSWD', 10026);
define ('CURLOPT_SSLCERTTYPE', 10086);
define ('CURLOPT_SSLENGINE', 10089);
define ('CURLOPT_SSLENGINE_DEFAULT', 90);
define ('CURLOPT_SSLKEY', 10087);
define ('CURLOPT_SSLKEYPASSWD', 10026);
define ('CURLOPT_SSLKEYTYPE', 10088);
define ('CURLOPT_SSLVERSION', 32);
define ('CURLOPT_SSL_CIPHER_LIST', 10083);
define ('CURLOPT_SSL_VERIFYHOST', 81);
define ('CURLOPT_SSL_VERIFYPEER', 64);
define ('CURLOPT_STDERR', 10037);
define ('CURLOPT_TELNETOPTIONS', 10070);
define ('CURLOPT_TIMECONDITION', 33);
define ('CURLOPT_TIMEOUT', 13);
define ('CURLOPT_TIMEVALUE', 34);
define ('CURLOPT_TRANSFERTEXT', 53);
define ('CURLOPT_UNRESTRICTED_AUTH', 105);
define ('CURLOPT_UPLOAD', 46);
define ('CURLOPT_URL', 10002);
define ('CURLOPT_USERAGENT', 10018);
define ('CURLOPT_USERPWD', 10005);
define ('CURLOPT_VERBOSE', 41);
define ('CURLOPT_WRITEFUNCTION', 20011);
define ('CURLOPT_WRITEHEADER', 10029);
define ('CURLE_ABORTED_BY_CALLBACK', 42);
define ('CURLE_BAD_CALLING_ORDER', 44);
define ('CURLE_BAD_CONTENT_ENCODING', 61);
define ('CURLE_BAD_DOWNLOAD_RESUME', 36);
define ('CURLE_BAD_FUNCTION_ARGUMENT', 43);
define ('CURLE_BAD_PASSWORD_ENTERED', 46);
define ('CURLE_COULDNT_CONNECT', 7);
define ('CURLE_COULDNT_RESOLVE_HOST', 6);
define ('CURLE_COULDNT_RESOLVE_PROXY', 5);
define ('CURLE_FAILED_INIT', 2);
define ('CURLE_FILE_COULDNT_READ_FILE', 37);
define ('CURLE_FTP_ACCESS_DENIED', 9);
define ('CURLE_FTP_BAD_DOWNLOAD_RESUME', 36);
define ('CURLE_FTP_CANT_GET_HOST', 15);
define ('CURLE_FTP_CANT_RECONNECT', 16);
define ('CURLE_FTP_COULDNT_GET_SIZE', 32);
define ('CURLE_FTP_COULDNT_RETR_FILE', 19);
define ('CURLE_FTP_COULDNT_SET_ASCII', 29);
define ('CURLE_FTP_COULDNT_SET_BINARY', 17);
define ('CURLE_FTP_COULDNT_STOR_FILE', 25);
define ('CURLE_FTP_COULDNT_USE_REST', 31);
define ('CURLE_FTP_PARTIAL_FILE', 18);
define ('CURLE_FTP_PORT_FAILED', 30);
define ('CURLE_FTP_QUOTE_ERROR', 21);
define ('CURLE_FTP_USER_PASSWORD_INCORRECT', 10);
define ('CURLE_FTP_WEIRD_227_FORMAT', 14);
define ('CURLE_FTP_WEIRD_PASS_REPLY', 11);
define ('CURLE_FTP_WEIRD_PASV_REPLY', 13);
define ('CURLE_FTP_WEIRD_SERVER_REPLY', 8);
define ('CURLE_FTP_WEIRD_USER_REPLY', 12);
define ('CURLE_FTP_WRITE_ERROR', 20);
define ('CURLE_FUNCTION_NOT_FOUND', 41);
define ('CURLE_GOT_NOTHING', 52);
define ('CURLE_HTTP_NOT_FOUND', 22);
define ('CURLE_HTTP_PORT_FAILED', 45);
define ('CURLE_HTTP_POST_ERROR', 34);
define ('CURLE_HTTP_RANGE_ERROR', 33);
define ('CURLE_HTTP_RETURNED_ERROR', 22);
define ('CURLE_LDAP_CANNOT_BIND', 38);
define ('CURLE_LDAP_SEARCH_FAILED', 39);
define ('CURLE_LIBRARY_NOT_FOUND', 40);
define ('CURLE_MALFORMAT_USER', 24);
define ('CURLE_OBSOLETE', 50);
define ('CURLE_OK', 0);
define ('CURLE_OPERATION_TIMEDOUT', 28);
define ('CURLE_OPERATION_TIMEOUTED', 28);
define ('CURLE_OUT_OF_MEMORY', 27);
define ('CURLE_PARTIAL_FILE', 18);
define ('CURLE_READ_ERROR', 26);
define ('CURLE_RECV_ERROR', 56);
define ('CURLE_SEND_ERROR', 55);
define ('CURLE_SHARE_IN_USE', 57);
define ('CURLE_SSL_CACERT', 60);
define ('CURLE_SSL_CERTPROBLEM', 58);
define ('CURLE_SSL_CIPHER', 59);
define ('CURLE_SSL_CONNECT_ERROR', 35);
define ('CURLE_SSL_ENGINE_NOTFOUND', 53);
define ('CURLE_SSL_ENGINE_SETFAILED', 54);
define ('CURLE_SSL_PEER_CERTIFICATE', 51);
define ('CURLE_TELNET_OPTION_SYNTAX', 49);
define ('CURLE_TOO_MANY_REDIRECTS', 47);
define ('CURLE_UNKNOWN_TELNET_OPTION', 48);
define ('CURLE_UNSUPPORTED_PROTOCOL', 1);
define ('CURLE_URL_MALFORMAT', 3);
define ('CURLE_URL_MALFORMAT_USER', 4);
define ('CURLE_WRITE_ERROR', 23);
define ('CURLINFO_CONNECT_TIME', 3145733);
define ('CURLINFO_CONTENT_LENGTH_DOWNLOAD', 3145743);
define ('CURLINFO_CONTENT_LENGTH_UPLOAD', 3145744);
define ('CURLINFO_CONTENT_TYPE', 1048594);
define ('CURLINFO_EFFECTIVE_URL', 1048577);
define ('CURLINFO_FILETIME', 2097166);

/**
 * Available since PHP 5.1.3
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLINFO_HEADER_OUT', 2);
define ('CURLINFO_HEADER_SIZE', 2097163);
define ('CURLINFO_HTTP_CODE', 2097154);
define ('CURLINFO_LASTONE', 42);
define ('CURLINFO_NAMELOOKUP_TIME', 3145732);
define ('CURLINFO_PRETRANSFER_TIME', 3145734);

/**
 * Available since PHP 5.2.4
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLINFO_PRIVATE', 1048597);
define ('CURLINFO_REDIRECT_COUNT', 2097172);
define ('CURLINFO_REDIRECT_TIME', 3145747);
define ('CURLINFO_REQUEST_SIZE', 2097164);
define ('CURLINFO_SIZE_DOWNLOAD', 3145736);
define ('CURLINFO_SIZE_UPLOAD', 3145735);
define ('CURLINFO_SPEED_DOWNLOAD', 3145737);
define ('CURLINFO_SPEED_UPLOAD', 3145738);
define ('CURLINFO_SSL_VERIFYRESULT', 2097165);
define ('CURLINFO_STARTTRANSFER_TIME', 3145745);
define ('CURLINFO_TOTAL_TIME', 3145731);
define ('CURLMSG_DONE', 1);
define ('CURLVERSION_NOW', 3);
define ('CURLM_BAD_EASY_HANDLE', 2);
define ('CURLM_BAD_HANDLE', 1);
define ('CURLM_CALL_MULTI_PERFORM', -1);
define ('CURLM_INTERNAL_ERROR', 4);
define ('CURLM_OK', 0);
define ('CURLM_OUT_OF_MEMORY', 3);
define ('CURLPROXY_HTTP', 0);
define ('CURLPROXY_SOCKS4', 4);
define ('CURLPROXY_SOCKS5', 5);
define ('CURLSHOPT_NONE', 0);
define ('CURLSHOPT_SHARE', 1);
define ('CURLSHOPT_UNSHARE', 2);
define ('CURL_HTTP_VERSION_1_0', 1);
define ('CURL_HTTP_VERSION_1_1', 2);
define ('CURL_HTTP_VERSION_NONE', 0);
define ('CURL_LOCK_DATA_COOKIE', 2);
define ('CURL_LOCK_DATA_DNS', 3);
define ('CURL_LOCK_DATA_SSL_SESSION', 4);
define ('CURL_NETRC_IGNORED', 0);
define ('CURL_NETRC_OPTIONAL', 1);
define ('CURL_NETRC_REQUIRED', 2);
define ('CURL_SSLVERSION_DEFAULT', 0);
define ('CURL_SSLVERSION_SSLv2', 2);
define ('CURL_SSLVERSION_SSLv3', 3);
define ('CURL_SSLVERSION_TLSv1', 1);
define ('CURL_TIMECOND_IFMODSINCE', 1);
define ('CURL_TIMECOND_IFUNMODSINCE', 2);
define ('CURL_TIMECOND_LASTMOD', 3);
define ('CURL_TIMECOND_NONE', 0);
define ('CURL_VERSION_IPV6', 1);
define ('CURL_VERSION_KERBEROS4', 2);
define ('CURL_VERSION_LIBZ', 8);
define ('CURL_VERSION_SSL', 4);
define ('CURLOPT_HTTPAUTH', 107);
define ('CURLAUTH_ANY', -17);
define ('CURLAUTH_ANYSAFE', -18);
define ('CURLAUTH_BASIC', 1);
define ('CURLAUTH_DIGEST', 2);
define ('CURLAUTH_GSSNEGOTIATE', 4);
define ('CURLAUTH_NONE', 0);
define ('CURLAUTH_NTLM', 8);
define ('CURLINFO_HTTP_CONNECTCODE', 2097174);
define ('CURLOPT_FTP_CREATE_MISSING_DIRS', 110);
define ('CURLOPT_PROXYAUTH', 111);
define ('CURLE_FILESIZE_EXCEEDED', 63);
define ('CURLE_LDAP_INVALID_URL', 62);
define ('CURLINFO_HTTPAUTH_AVAIL', 2097175);
define ('CURLINFO_RESPONSE_CODE', 2097154);
define ('CURLINFO_PROXYAUTH_AVAIL', 2097176);
define ('CURLOPT_FTP_RESPONSE_TIMEOUT', 112);
define ('CURLOPT_IPRESOLVE', 113);
define ('CURLOPT_MAXFILESIZE', 114);
define ('CURL_IPRESOLVE_V4', 1);
define ('CURL_IPRESOLVE_V6', 2);
define ('CURL_IPRESOLVE_WHATEVER', 0);
define ('CURLE_FTP_SSL_FAILED', 64);

/**
 * Available since PHP 5.2.0
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLFTPSSL_ALL', 3);

/**
 * Available since PHP 5.2.0
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLFTPSSL_CONTROL', 2);

/**
 * Available since PHP 5.2.0
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLFTPSSL_NONE', 0);

/**
 * Available since PHP 5.2.0
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLFTPSSL_TRY', 1);

/**
 * Available since PHP 5.2.0
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLOPT_FTP_SSL', 119);
define ('CURLOPT_NETRC_FILE', 10118);

/**
 * Available since PHP 5.1.0
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLFTPAUTH_DEFAULT', 0);

/**
 * Available since PHP 5.1.0
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLFTPAUTH_SSL', 1);

/**
 * Available since PHP 5.1.0
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLFTPAUTH_TLS', 2);

/**
 * Available since PHP 5.1.0
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLOPT_FTPSSLAUTH', 129);
define ('CURLOPT_FTP_ACCOUNT', 10134);

/**
 * Available since PHP 5.2.1
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLOPT_TCP_NODELAY', 121);
define ('CURLINFO_OS_ERRNO', 2097177);
define ('CURLINFO_NUM_CONNECTS', 2097178);
define ('CURLINFO_SSL_ENGINES', 4194331);
define ('CURLINFO_COOKIELIST', 4194332);
define ('CURLOPT_COOKIELIST', 10135);
define ('CURLOPT_IGNORE_CONTENT_LENGTH', 136);
define ('CURLOPT_FTP_SKIP_PASV_IP', 137);
define ('CURLOPT_FTP_FILEMETHOD', 138);
define ('CURLOPT_CONNECT_ONLY', 141);
define ('CURLOPT_LOCALPORT', 139);
define ('CURLOPT_LOCALPORTRANGE', 140);
define ('CURLFTPMETHOD_MULTICWD', 1);
define ('CURLFTPMETHOD_NOCWD', 2);
define ('CURLFTPMETHOD_SINGLECWD', 3);
define ('CURLINFO_FTP_ENTRY_PATH', 1048606);
define ('CURLOPT_FTP_ALTERNATIVE_TO_USER', 10147);

/**
 * Available since PHP 5.4.0 and cURL 7.15.5
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLOPT_MAX_RECV_SPEED_LARGE', 30146);

/**
 * Available since PHP 5.4.0 and cURL 7.15.5
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLOPT_MAX_SEND_SPEED_LARGE', 30145);
define ('CURLOPT_SSL_SESSIONID_CACHE', 150);

/**
 * Available since PHP 5.5.0 and cURL 7.16.0.
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLMOPT_PIPELINING', 3);
define ('CURLE_SSH', 79);
define ('CURLOPT_FTP_SSL_CCC', 154);
define ('CURLOPT_SSH_AUTH_TYPES', 151);
define ('CURLOPT_SSH_PRIVATE_KEYFILE', 10153);
define ('CURLOPT_SSH_PUBLIC_KEYFILE', 10152);
define ('CURLFTPSSL_CCC_ACTIVE', 2);
define ('CURLFTPSSL_CCC_NONE', 0);
define ('CURLFTPSSL_CCC_PASSIVE', 1);
define ('CURLOPT_CONNECTTIMEOUT_MS', 156);
define ('CURLOPT_HTTP_CONTENT_DECODING', 158);
define ('CURLOPT_HTTP_TRANSFER_DECODING', 157);
define ('CURLOPT_TIMEOUT_MS', 155);

/**
 * Available since PHP 5.5.0 and cURL 7.16.3.
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLMOPT_MAXCONNECTS', 6);
define ('CURLOPT_KRBLEVEL', 10063);
define ('CURLOPT_NEW_DIRECTORY_PERMS', 160);
define ('CURLOPT_NEW_FILE_PERMS', 159);
define ('CURLOPT_APPEND', 50);
define ('CURLOPT_DIRLISTONLY', 48);
define ('CURLOPT_USE_SSL', 119);
define ('CURLUSESSL_ALL', 3);
define ('CURLUSESSL_CONTROL', 2);
define ('CURLUSESSL_NONE', 0);
define ('CURLUSESSL_TRY', 1);
define ('CURLOPT_SSH_HOST_PUBLIC_KEY_MD5', 10162);
define ('CURLOPT_PROXY_TRANSFER_MODE', 166);

/**
 * Available since PHP 5.5.0 and cURL 7.18.0.
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLPAUSE_ALL', 5);

/**
 * Available since PHP 5.5.0 and cURL 7.18.0.
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLPAUSE_CONT', 0);

/**
 * Available since PHP 5.5.0 and cURL 7.18.0.
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLPAUSE_RECV', 1);

/**
 * Available since PHP 5.5.0 and cURL 7.18.0.
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLPAUSE_RECV_CONT', 0);

/**
 * Available since PHP 5.5.0 and cURL 7.18.0.
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLPAUSE_SEND', 4);

/**
 * Available since PHP 5.5.0 and cURL 7.18.0.
 * @link http://www.php.net/manual/en/curl.constants.php
 */
define ('CURLPAUSE_SEND_CONT', 0);
define ('CURL_READFUNC_PAUSE', 268435457);
define ('CURL_WRITEFUNC_PAUSE', 268435457);
define ('CURLINFO_REDIRECT_URL', 1048607);
define ('CURLINFO_APPCONNECT_TIME', 3145761);
define ('CURLINFO_PRIMARY_IP', 1048608);
define ('CURLOPT_ADDRESS_SCOPE', 171);
define ('CURLOPT_CRLFILE', 10169);
define ('CURLOPT_ISSUERCERT', 10170);
define ('CURLOPT_KEYPASSWD', 10026);
define ('CURLSSH_AUTH_ANY', -1);
define ('CURLSSH_AUTH_DEFAULT', -1);
define ('CURLSSH_AUTH_HOST', 4);
define ('CURLSSH_AUTH_KEYBOARD', 8);
define ('CURLSSH_AUTH_NONE', 0);
define ('CURLSSH_AUTH_PASSWORD', 2);
define ('CURLSSH_AUTH_PUBLICKEY', 1);
define ('CURLINFO_CERTINFO', 4194338);
define ('CURLOPT_CERTINFO', 172);
define ('CURLOPT_PASSWORD', 10174);
define ('CURLOPT_POSTREDIR', 161);
define ('CURLOPT_PROXYPASSWORD', 10176);
define ('CURLOPT_PROXYUSERNAME', 10175);
define ('CURLOPT_USERNAME', 10173);
define ('CURLAUTH_DIGEST_IE', 16);
define ('CURLINFO_CONDITION_UNMET', 2097187);
define ('CURLOPT_NOPROXY', 10177);
define ('CURLOPT_PROTOCOLS', 181);
define ('CURLOPT_REDIR_PROTOCOLS', 182);
define ('CURLOPT_SOCKS5_GSSAPI_NEC', 180);
define ('CURLOPT_SOCKS5_GSSAPI_SERVICE', 10179);
define ('CURLOPT_TFTP_BLKSIZE', 178);
define ('CURLPROTO_ALL', -1);
define ('CURLPROTO_DICT', 512);
define ('CURLPROTO_FILE', 1024);
define ('CURLPROTO_FTP', 4);
define ('CURLPROTO_FTPS', 8);
define ('CURLPROTO_HTTP', 1);
define ('CURLPROTO_HTTPS', 2);
define ('CURLPROTO_LDAP', 128);
define ('CURLPROTO_LDAPS', 256);
define ('CURLPROTO_SCP', 16);
define ('CURLPROTO_SFTP', 32);
define ('CURLPROTO_TELNET', 64);
define ('CURLPROTO_TFTP', 2048);
define ('CURLOPT_SSH_KNOWNHOSTS', 10183);
define ('CURLINFO_RTSP_CLIENT_CSEQ', 2097189);
define ('CURLINFO_RTSP_CSEQ_RECV', 2097191);
define ('CURLINFO_RTSP_SERVER_CSEQ', 2097190);
define ('CURLINFO_RTSP_SESSION_ID', 1048612);
define ('CURLOPT_FTP_USE_PRET', 188);
define ('CURLOPT_MAIL_FROM', 10186);
define ('CURLOPT_MAIL_RCPT', 10187);
define ('CURLOPT_RTSP_CLIENT_CSEQ', 193);
define ('CURLOPT_RTSP_REQUEST', 189);
define ('CURLOPT_RTSP_SERVER_CSEQ', 194);
define ('CURLOPT_RTSP_SESSION_ID', 10190);
define ('CURLOPT_RTSP_STREAM_URI', 10191);
define ('CURLOPT_RTSP_TRANSPORT', 10192);
define ('CURLPROTO_IMAP', 4096);
define ('CURLPROTO_IMAPS', 8192);
define ('CURLPROTO_POP3', 16384);
define ('CURLPROTO_POP3S', 32768);
define ('CURLPROTO_RTSP', 262144);
define ('CURLPROTO_SMTP', 65536);
define ('CURLPROTO_SMTPS', 131072);
define ('CURL_RTSPREQ_ANNOUNCE', 3);
define ('CURL_RTSPREQ_DESCRIBE', 2);
define ('CURL_RTSPREQ_GET_PARAMETER', 8);
define ('CURL_RTSPREQ_OPTIONS', 1);
define ('CURL_RTSPREQ_PAUSE', 6);
define ('CURL_RTSPREQ_PLAY', 5);
define ('CURL_RTSPREQ_RECEIVE', 11);
define ('CURL_RTSPREQ_RECORD', 10);
define ('CURL_RTSPREQ_SETUP', 4);
define ('CURL_RTSPREQ_SET_PARAMETER', 9);
define ('CURL_RTSPREQ_TEARDOWN', 7);
define ('CURLINFO_LOCAL_IP', 1048617);
define ('CURLINFO_LOCAL_PORT', 2097194);
define ('CURLINFO_PRIMARY_PORT', 2097192);
define ('CURLOPT_FNMATCH_FUNCTION', 20200);
define ('CURLOPT_WILDCARDMATCH', 197);
define ('CURLPROTO_RTMP', 524288);
define ('CURLPROTO_RTMPE', 2097152);
define ('CURLPROTO_RTMPS', 8388608);
define ('CURLPROTO_RTMPT', 1048576);
define ('CURLPROTO_RTMPTE', 4194304);
define ('CURLPROTO_RTMPTS', 16777216);
define ('CURL_FNMATCHFUNC_FAIL', 2);
define ('CURL_FNMATCHFUNC_MATCH', 0);
define ('CURL_FNMATCHFUNC_NOMATCH', 1);
define ('CURLOPT_SAFE_UPLOAD', -1);

// End of curl v.
?>
