<?php

// Start of curl v.

class CURLFile  {
	public $name;
	public $mime;
	public $postname;


	/**
	 * Create a CURLFile object
	 * @link http://www.php.net/manual/en/curlfile.construct.php
	 * @param filename string <p>
	 *      Path to the file which will be uploaded.
	 *     </p>
	 * @param mimetype string[optional] <p>
	 *      Mimetype of the file.
	 *     </p>
	 * @param postname string[optional] <p>
	 *      Name of the file.
	 *     </p>
	 * @return string a CURLFile object.
	 */
	public function __construct ($filename, $mimetype = null, $postname = null) {}

	/**
	 * Get file name
	 * @link http://www.php.net/manual/en/curlfile.getfilename.php
	 * @return string file name.
	 */
	public function getFilename () {}

	/**
	 * Get MIME type
	 * @link http://www.php.net/manual/en/curlfile.getmimetype.php
	 * @return string MIME type.
	 */
	public function getMimeType () {}

	/**
	 * Set MIME type
	 * @link http://www.php.net/manual/en/curlfile.setmimetype.php
	 * @param mime string <p>
	 *      
	 *     </p>
	 * @return void 
	 */
	public function setMimeType ($mime) {}

	/**
	 * Get file name for POST
	 * @link http://www.php.net/manual/en/curlfile.getpostfilename.php
	 * @return string file name for POST.
	 */
	public function getPostFilename () {}

	/**
	 * Set file name for POST
	 * @link http://www.php.net/manual/en/curlfile.setpostfilename.php
	 * @param postname string <p>
	 *      
	 *     </p>
	 * @return void 
	 */
	public function setPostFilename ($postname) {}

	/**
	 * Unserialization handler
	 * @link http://www.php.net/manual/en/curlfile.wakeup.php
	 * @return void 
	 */
	public function __wakeup () {}

}

/**
 * Initialize a cURL session
 * @link http://www.php.net/manual/en/function.curl-init.php
 * @param url string[optional] <p>
 *       If provided, the CURLOPT_URL option will be set
 *       to its value. You can manually set this using the 
 *       curl_setopt function.
 *      </p>
 *      
 *       <p>
 *        The file protocol is disabled by cURL if
 *        open_basedir is set.
 *       </p>
 * @return resource a cURL handle on success, false on errors.
 */
function curl_init ($url = null) {}

/**
 * Copy a cURL handle along with all of its preferences
 * @link http://www.php.net/manual/en/function.curl-copy-handle.php
 * @param ch resource 
 * @return resource a new cURL handle.
 */
function curl_copy_handle ($ch) {}

/**
 * Gets cURL version information
 * @link http://www.php.net/manual/en/function.curl-version.php
 * @param age int[optional] <p>
 *      </p>
 * @return array an associative array with the following elements: 
 *   
 *    
 *     
 *      <tr valign="top">
 *       <td>Indice</td>
 *       <td>Value description</td>
 *      </tr>
 *     
 *     
 *      <tr valign="top">
 *       <td>version_number</td>
 *       <td>cURL 24 bit version number</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>version</td>
 *       <td>cURL version number, as a string</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>ssl_version_number</td>
 *       <td>OpenSSL 24 bit version number</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>ssl_version</td>
 *       <td>OpenSSL version number, as a string</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>libz_version</td>
 *       <td>zlib version number, as a string</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>host</td>
 *       <td>Information about the host where cURL was built</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>age</td>
 *       <td></td>
 *      </tr>
 *      <tr valign="top">
 *       <td>features</td>
 *       <td>A bitmask of the CURL_VERSION_XXX constants</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>protocols</td>
 *       <td>An array of protocols names supported by cURL</td>
 *      </tr>
 */
function curl_version ($age = null) {}

/**
 * Set an option for a cURL transfer
 * @link http://www.php.net/manual/en/function.curl-setopt.php
 * @param ch resource 
 * @param option int <p>
 *       The CURLOPT_XXX option to set.
 *      </p>
 * @param value mixed <p>
 *       The value to be set on option.
 *      </p>
 *      <p>
 *       value should be a bool for the
 *       following values of the option parameter:
 *       
 *        
 *         
 *          <tr valign="top">
 *           Option</td>
 *           Set value to</td>
 *           Notes</td>
 *          </tr>
 *         
 *         
 *          <tr valign="top">
 *           CURLOPT_AUTOREFERER</td>
 *           
 *            true to automatically set the Referer: field in
 *            requests where it follows a Location: redirect.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_BINARYTRANSFER</td>
 *           
 *            true to return the raw output when
 *            CURLOPT_RETURNTRANSFER is used.
 *           </td>
 *           
 *            From PHP 5.1.3, this option has no effect: the raw output will
 *            always be returned when
 *            CURLOPT_RETURNTRANSFER is used.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_COOKIESESSION</td>
 *           
 *            true to mark this as a new cookie "session". It will force libcurl
 *            to ignore all cookies it is about to load that are "session cookies"
 *            from the previous session. By default, libcurl always stores and
 *            loads all cookies, independent if they are session cookies or not.
 *            Session cookies are cookies without expiry date and they are meant
 *            to be alive and existing for this "session" only.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_CERTINFO</td>
 *           
 *            true to output SSL certification information to STDERR
 *            on secure transfers.
 *           </td>
 *           
 *            Added in cURL 7.19.1. 
 *            Available since PHP 5.3.2. 
 *            Requires CURLOPT_VERBOSE to be on to have an effect.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_CONNECT_ONLY</td>
 *           
 *            true tells the library to perform all the required proxy authentication 
 *            and connection setup, but no data transfer. This option is implemented for 
 *            HTTP, SMTP and POP3.
 *           </td>
 *           
 *            Added in 7.15.2.
 *            Available since PHP 5.5.0.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_CRLF</td>
 *           
 *            true to convert Unix newlines to CRLF newlines
 *            on transfers.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_DNS_USE_GLOBAL_CACHE</td>
 *           
 *            true to use a global DNS cache. This option is
 *            not thread-safe and is enabled by default.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_FAILONERROR</td>
 *           
 *            true to fail verbosely if the HTTP code returned
 *            is greater than or equal to 400. The default behavior is to return
 *            the page normally, ignoring the code.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_FILETIME</td>
 *           
 *            true to attempt to retrieve the modification
 *            date of the remote document. This value can be retrieved using
 *            the CURLINFO_FILETIME option with
 *            curl_getinfo.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_FOLLOWLOCATION</td>
 *           
 *            true to follow any
 *            "Location: " header that the server sends as
 *            part of the HTTP header (note this is recursive, PHP will follow as
 *            many "Location: " headers that it is sent,
 *            unless CURLOPT_MAXREDIRS is set).
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_FORBID_REUSE</td>
 *           
 *            true to force the connection to explicitly
 *            close when it has finished processing, and not be pooled for reuse.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_FRESH_CONNECT</td>
 *           
 *            true to force the use of a new connection
 *            instead of a cached one.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_FTP_USE_EPRT</td>
 *           
 *            true to use EPRT (and LPRT) when doing active
 *            FTP downloads. Use false to disable EPRT and LPRT and use PORT
 *            only.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_FTP_USE_EPSV</td>
 *           
 *            true to first try an EPSV command for FTP
 *            transfers before reverting back to PASV. Set to false
 *            to disable EPSV.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_FTP_CREATE_MISSING_DIRS</td>
 *           
 *            true to create missing directories when an FTP operation
 *            encounters a path that currently doesn't exist.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_FTPAPPEND</td>
 *           
 *            true to append to the remote file instead of
 *            overwriting it.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_TCP_NODELAY</td>
 *           
 *            Pass a long specifying whether the TCP_NODELAY option is to be set or
 *            cleared (1 = set, 0 = clear). The option is cleared by default.
 *           </td>
 *           
 *            Available since PHP 5.2.1 for versions compiled with libcurl 7.11.2 or
 *            greater.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_FTPASCII</td>
 *           
 *            An alias of
 *            CURLOPT_TRANSFERTEXT. Use that instead.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_FTPLISTONLY</td>
 *           
 *            true to only list the names of an FTP
 *            directory.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_HEADER</td>
 *           
 *            true to include the header in the output.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>CURLINFO_HEADER_OUT</td>
 *           
 *            true to track the handle's request string.
 *           </td>
 *           
 *            Available since PHP 5.1.3. The CURLINFO_
 *            prefix is intentional.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_HTTPGET</td>
 *           
 *            true to reset the HTTP request method to GET.
 *            Since GET is the default, this is only necessary if the request
 *            method has been changed.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_HTTPPROXYTUNNEL</td>
 *           
 *            true to tunnel through a given HTTP proxy.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_MUTE</td>
 *           
 *            true to be completely silent with regards to
 *            the cURL functions.
 *           </td>
 *           
 *            Removed in cURL 7.15.5 (You can use CURLOPT_RETURNTRANSFER instead)
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_NETRC</td>
 *           
 *            true to scan the ~/.netrc
 *            file to find a username and password for the remote site that
 *            a connection is being established with.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_NOBODY</td>
 *           
 *            true to exclude the body from the output.
 *            Request method is then set to HEAD. Changing this to false does
 *            not change it to GET.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_NOPROGRESS</td>
 *           <p>
 *            true to disable the progress meter for cURL transfers.
 *            
 *             <p>
 *              PHP automatically sets this option to true, this should only be
 *              changed for debugging purposes.
 *             </p>
 *            
 *            </p></td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_NOSIGNAL</td>
 *           
 *            true to ignore any cURL function that causes a
 *            signal to be sent to the PHP process. This is turned on by default
 *            in multi-threaded SAPIs so timeout options can still be used.
 *           </td>
 *           
 *            Added in cURL 7.10.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_POST</td>
 *           
 *            true to do a regular HTTP POST. This POST is the
 *            normal application/x-www-form-urlencoded kind,
 *            most commonly used by HTML forms.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_PUT</td>
 *           
 *            true to HTTP PUT a file. The file to PUT must
 *            be set with CURLOPT_INFILE and
 *            CURLOPT_INFILESIZE.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_RETURNTRANSFER</td>
 *           
 *            true to return the transfer as a string of the
 *            return value of curl_exec instead of outputting
 *            it out directly.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_SAFE_UPLOAD</td>
 *           
 *            true to disable support for the @ prefix for
 *            uploading files in CURLOPT_POSTFIELDS, which
 *            means that values starting with @ can be safely
 *            passed as fields. CURLFile may be used for
 *            uploads instead.
 *           </td>
 *           
 *            Added in PHP 5.5.0 with false as the default value. PHP 5.6.0
 *            changes the default value to true.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_SSL_VERIFYPEER</td>
 *           
 *            false to stop cURL from verifying the peer's
 *            certificate. Alternate certificates to verify against can be
 *            specified with the CURLOPT_CAINFO option
 *            or a certificate directory can be specified with the
 *            CURLOPT_CAPATH option.
 *           </td>
 *           
 *            true by default as of cURL 7.10. Default bundle installed as of
 *            cURL 7.10.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_TRANSFERTEXT</td>
 *           
 *            true to use ASCII mode for FTP transfers.
 *            For LDAP, it retrieves data in plain text instead of HTML. On
 *            Windows systems, it will not set STDOUT to binary
 *            mode.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_UNRESTRICTED_AUTH</td>
 *           
 *            true to keep sending the username and password
 *            when following locations (using
 *            CURLOPT_FOLLOWLOCATION), even when the
 *            hostname has changed.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_UPLOAD</td>
 *           
 *            true to prepare for an upload.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_VERBOSE</td>
 *           
 *            true to output verbose information. Writes
 *            output to STDERR, or the file specified using
 *            CURLOPT_STDERR.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *         
 *        
 *       
 *      </p>
 *      <p>
 *       value should be an integer for the
 *       following values of the option parameter:
 *       
 *        
 *         
 *          <tr valign="top">
 *           <td>Option</td>
 *           <td>Set value to</td>
 *           <td>Notes</td>
 *          </tr>
 *         
 *         
 *          <tr valign="top">
 *           CURLOPT_BUFFERSIZE</td>
 *           
 *            The size of the buffer to use for each read. There is no guarantee
 *            this request will be fulfilled, however.
 *           </td>
 *           
 *            Added in cURL 7.10.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_CLOSEPOLICY</td>
 *           
 *            One of the CURLCLOSEPOLICY_* values.
 *            
 *             <p>
 *              This option is deprecated, as it was never implemented in cURL and
 *              never had any effect.
 *             </p>
 *            
 *           </td>
 *           
 *            Removed in PHP 5.6.0.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>CURLOPT_CONNECTTIMEOUT</td>
 *           
 *            The number of seconds to wait while trying to connect. Use 0 to
 *            wait indefinitely.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_CONNECTTIMEOUT_MS</td>
 *           
 *            The number of milliseconds to wait while trying to connect. Use 0 to
 *            wait indefinitely.
 *            
 *            If libcurl is built to use the standard system name resolver, that
 *            portion of the connect will still use full-second resolution for
 *            timeouts with a minimum timeout allowed of one second.
 *           </td>
 *           
 *            Added in cURL 7.16.2. Available since PHP 5.2.3.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_DNS_CACHE_TIMEOUT</td>
 *           
 *            The number of seconds to keep DNS entries in memory. This
 *            option is set to 120 (2 minutes) by default.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_FTPSSLAUTH</td>
 *           
 *            The FTP authentication method (when is activated):
 *            CURLFTPAUTH_SSL (try SSL first),
 *            CURLFTPAUTH_TLS (try TLS first), or
 *            CURLFTPAUTH_DEFAULT (let cURL decide).
 *           </td>
 *           
 *            Added in cURL 7.12.2.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_HTTP_VERSION</td>
 *           
 *            CURL_HTTP_VERSION_NONE (default, lets CURL
 *            decide which version to use),
 *            CURL_HTTP_VERSION_1_0 (forces HTTP/1.0),
 *            or CURL_HTTP_VERSION_1_1 (forces HTTP/1.1).
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_HTTPAUTH</td>
 *           
 *            <p>
 *             The HTTP authentication method(s) to use. The options are:
 *             CURLAUTH_BASIC,
 *             CURLAUTH_DIGEST,
 *             CURLAUTH_GSSNEGOTIATE,
 *             CURLAUTH_NTLM,
 *             CURLAUTH_ANY, and
 *             CURLAUTH_ANYSAFE.
 *            </p>
 *            <p>
 *             The bitwise | (or) operator can be used to combine
 *             more than one method. If this is done, cURL will poll the server to see
 *             what methods it supports and pick the best one.
 *            </p>
 *            <p>
 *             CURLAUTH_ANY is an alias for
 *             CURLAUTH_BASIC | CURLAUTH_DIGEST | CURLAUTH_GSSNEGOTIATE | CURLAUTH_NTLM.
 *            </p>
 *            <p>
 *             CURLAUTH_ANYSAFE is an alias for
 *             CURLAUTH_DIGEST | CURLAUTH_GSSNEGOTIATE | CURLAUTH_NTLM.
 *            </p>
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_INFILESIZE</td>
 *           
 *            The expected size, in bytes, of the file when uploading a file to
 *            a remote site. Note that using this option will not stop libcurl
 *            from sending more data, as exactly what is sent depends on
 *            CURLOPT_READFUNCTION.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_LOW_SPEED_LIMIT</td>
 *           
 *            The transfer speed, in bytes per second, that the transfer should be
 *            below during the count of CURLOPT_LOW_SPEED_TIME
 *            seconds before PHP considers the transfer too slow and aborts.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_LOW_SPEED_TIME</td>
 *           
 *            The number of seconds the transfer speed should be below
 *            CURLOPT_LOW_SPEED_LIMIT before PHP considers
 *            the transfer too slow and aborts.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_MAXCONNECTS</td>
 *           
 *            The maximum amount of persistent connections that are allowed.
 *            When the limit is reached,
 *            CURLOPT_CLOSEPOLICY is used to determine
 *            which connection to close.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_MAXREDIRS</td>
 *           
 *            The maximum amount of HTTP redirections to follow. Use this option
 *            alongside CURLOPT_FOLLOWLOCATION.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_PORT</td>
 *           
 *            An alternative port number to connect to.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_PROTOCOLS</td>
 *           
 *            <p>
 *             Bitmask of CURLPROTO_* values. If used, this bitmask 
 *             limits what protocols libcurl may use in the transfer. This allows you to have
 *             a libcurl built to support a wide range of protocols but still limit specific
 *             transfers to only be allowed to use a subset of them. By default libcurl will
 *             accept all protocols it supports. 
 *             See also CURLOPT_REDIR_PROTOCOLS.
 *            </p>
 *            <p>
 *             Valid protocol options are: 
 *             CURLPROTO_HTTP,
 *             CURLPROTO_HTTPS,
 *             CURLPROTO_FTP,
 *             CURLPROTO_FTPS,
 *             CURLPROTO_SCP,
 *             CURLPROTO_SFTP,
 *             CURLPROTO_TELNET,
 *             CURLPROTO_LDAP,
 *             CURLPROTO_LDAPS,
 *             CURLPROTO_DICT,
 *             CURLPROTO_FILE,
 *             CURLPROTO_TFTP,
 *             CURLPROTO_ALL
 *            </p>
 *           </td>
 *           
 *            Added in cURL 7.19.4.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_PROXYAUTH</td>
 *           
 *            The HTTP authentication method(s) to use for the proxy connection.
 *            Use the same bitmasks as described in
 *            CURLOPT_HTTPAUTH. For proxy authentication,
 *            only CURLAUTH_BASIC and
 *            CURLAUTH_NTLM are currently supported.
 *           </td>
 *           
 *            Added in cURL 7.10.7.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_PROXYPORT</td>
 *           
 *            The port number of the proxy to connect to. This port number can
 *            also be set in CURLOPT_PROXY.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_PROXYTYPE</td>
 *           
 *            Either CURLPROXY_HTTP (default) or
 *            CURLPROXY_SOCKS5.
 *           </td>
 *           
 *            Added in cURL 7.10.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_REDIR_PROTOCOLS</td>
 *           
 *            Bitmask of CURLPROTO_* values. If used, this bitmask
 *            limits what protocols libcurl may use in a transfer that it follows to in
 *            a redirect when CURLOPT_FOLLOWLOCATION is enabled.
 *            This allows you to limit specific transfers to only be allowed to use a subset
 *            of protocols in redirections. By default libcurl will allow all protocols
 *            except for FILE and SCP. This is a difference compared to pre-7.19.4 versions
 *            which unconditionally would follow to all protocols supported. 
 *            See also CURLOPT_PROTOCOLS for protocol constant values.
 *           </td>
 *           
 *            Added in cURL 7.19.4.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_RESUME_FROM</td>
 *           
 *            The offset, in bytes, to resume a transfer from.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_SSL_VERIFYHOST</td>
 *           
 *            1 to check the existence of a common name in the
 *            SSL peer certificate. 2 to check the existence of
 *            a common name and also verify that it matches the hostname
 *            provided. In production environments the value of this option
 *            should be kept at 2 (default value).
 *           </td>
 *           
 *            Support for value 1 removed in cURL 7.28.1
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_SSLVERSION</td>
 *           
 *            The SSL version (2 or 3) to use. By default PHP will try to determine
 *            this itself, although in some cases this must be set manually.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_TIMECONDITION</td>
 *           
 *            How CURLOPT_TIMEVALUE is treated.
 *            Use CURL_TIMECOND_IFMODSINCE to return the
 *            page only if it has been modified since the time specified in
 *            CURLOPT_TIMEVALUE. If it hasn't been modified,
 *            a "304 Not Modified" header will be returned
 *            assuming CURLOPT_HEADER is true.
 *            Use CURL_TIMECOND_IFUNMODSINCE for the reverse
 *            effect. CURL_TIMECOND_IFMODSINCE is the
 *            default.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_TIMEOUT</td>
 *           
 *            The maximum number of seconds to allow cURL functions to execute.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_TIMEOUT_MS</td>
 *           
 *            The maximum number of milliseconds to allow cURL functions to
 *            execute.
 *            
 *            If libcurl is built to use the standard system name resolver, that
 *            portion of the connect will still use full-second resolution for
 *            timeouts with a minimum timeout allowed of one second.
 *           </td>
 *           
 *            Added in cURL 7.16.2. Available since PHP 5.2.3.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_TIMEVALUE</td>
 *           
 *            The time in seconds since January 1st, 1970. The time will be used
 *            by CURLOPT_TIMECONDITION. By default,
 *            CURL_TIMECOND_IFMODSINCE is used.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_MAX_RECV_SPEED_LARGE</td>
 *           
 *            If a download exceeds this speed (counted in bytes per second) on
 *            cumulative average during the transfer, the transfer will pause to
 *            keep the average rate less than or equal to the parameter value.
 *            Defaults to unlimited speed.
 *           </td>
 *           
 *            Added in cURL 7.15.5. Available since PHP 5.4.0.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_MAX_SEND_SPEED_LARGE</td>
 *           
 *            If an upload exceeds this speed (counted in bytes per second) on
 *            cumulative average during the transfer, the transfer will pause to
 *            keep the average rate less than or equal to the parameter value.
 *            Defaults to unlimited speed.
 *           </td>
 *           
 *            Added in cURL 7.15.5. Available since PHP 5.4.0.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_SSH_AUTH_TYPES</td>
 *           
 *            A bitmask consisting of one or more of 
 *            CURLSSH_AUTH_PUBLICKEY, 
 *            CURLSSH_AUTH_PASSWORD, 
 *            CURLSSH_AUTH_HOST, 
 *            CURLSSH_AUTH_KEYBOARD. Set to 
 *            CURLSSH_AUTH_ANY to let libcurl pick one.
 *           </td>
 *           
 *            Added in cURL 7.16.1. 
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_IPRESOLVE</td>
 *           
 *            Allows an application to select what kind of IP addresses to use when
 *            resolving host names. This is only interesting when using host names that
 *            resolve addresses using more than one version of IP, possible values are
 *            CURL_IPRESOLVE_WHATEVER, 
 *            CURL_IPRESOLVE_V4, 
 *            CURL_IPRESOLVE_V6, by default
 *            CURL_IPRESOLVE_WHATEVER.
 *           </td>
 *           
 *            Added in cURL 7.10.8.
 *           </td>
 *          </tr>
 *         
 *        
 *       
 *      </p>
 *      <p>
 *       value should be a string for the
 *       following values of the option parameter:
 *       
 *        
 *         
 *          <tr valign="top">
 *           <td>Option</td>
 *           <td>Set value to</td>
 *           <td>Notes</td>
 *          </tr>
 *         
 *         
 *          <tr valign="top">
 *           CURLOPT_CAINFO</td>
 *           
 *            The name of a file holding one or more certificates to verify the
 *            peer with. This only makes sense when used in combination with
 *            CURLOPT_SSL_VERIFYPEER.
 *           </td>
 *           
 *            Requires absolute path.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_CAPATH</td>
 *           
 *            A directory that holds multiple CA certificates. Use this option
 *            alongside CURLOPT_SSL_VERIFYPEER.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_COOKIE</td>
 *           
 *            The contents of the "Cookie: " header to be
 *            used in the HTTP request.
 *            Note that multiple cookies are separated with a semicolon followed
 *            by a space (e.g., "fruit=apple; colour=red")
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_COOKIEFILE</td>
 *           
 *            The name of the file containing the cookie data. The cookie file can
 *            be in Netscape format, or just plain HTTP-style headers dumped into
 *            a file.
 *            If the name is an empty string, no cookies are loaded, but cookie
 *            handling is still enabled.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_COOKIEJAR</td>
 *           
 *            The name of a file to save all internal cookies to when the handle is closed, 
 *            e.g. after a call to curl_close.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_CUSTOMREQUEST</td>
 *           <p>
 *            A custom request method to use instead of
 *            "GET" or "HEAD" when doing
 *            a HTTP request. This is useful for doing
 *            "DELETE" or other, more obscure HTTP requests.
 *            Valid values are things like "GET",
 *            "POST", "CONNECT" and so on;
 *            i.e. Do not enter a whole HTTP request line here. For instance,
 *            entering "GET /index.html HTTP/1.0\r\n\r\n"
 *            would be incorrect.
 *            
 *             <p>
 *              Don't do this without making sure the server supports the custom
 *              request method first.
 *             </p>
 *            
 *            </p></td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_EGDSOCKET</td>
 *           
 *            Like CURLOPT_RANDOM_FILE, except a filename
 *            to an Entropy Gathering Daemon socket.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_ENCODING</td>
 *           
 *            The contents of the "Accept-Encoding: " header.
 *            This enables decoding of the response. Supported encodings are
 *            "identity", "deflate", and
 *            "gzip". If an empty string, "",
 *            is set, a header containing all supported encoding types is sent.
 *           </td>
 *           
 *            Added in cURL 7.10.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_FTPPORT</td>
 *           
 *            The value which will be used to get the IP address to use
 *            for the FTP "PORT" instruction. The "PORT" instruction tells
 *            the remote server to connect to our specified IP address.  The
 *            string may be a plain IP address, a hostname, a network
 *            interface name (under Unix), or just a plain '-' to use the
 *            systems default IP address.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_INTERFACE</td>
 *           
 *            The name of the outgoing network interface to use. This can be an
 *            interface name, an IP address or a host name.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_KEYPASSWD</td>
 *           
 *            The password required to use the CURLOPT_SSLKEY 
 *            or CURLOPT_SSH_PRIVATE_KEYFILE private key. 
 *           </td>
 *           
 *            Added in cURL 7.16.1. 
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_KRB4LEVEL</td>
 *           
 *            The KRB4 (Kerberos 4) security level. Any of the following values
 *            (in order from least to most powerful) are valid:
 *            "clear",
 *            "safe",
 *            "confidential",
 *            "private"..
 *            If the string does not match one of these,
 *            "private" is used. Setting this option to &null;
 *            will disable KRB4 security. Currently KRB4 security only works
 *            with FTP transactions.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_POSTFIELDS</td>
 *           
 *            
 *             The full data to post in a HTTP "POST" operation.
 *             To post a file, prepend a filename with @ and
 *             use the full path. The filetype can be explicitly specified by
 *             following the filename with the type in the format
 *             ';type=mimetype'. This parameter can either be
 *             passed as a urlencoded string like 'para1=val1&para2=val2&...' 
 *             or as an array with the field name as key and field data as value.
 *             If value is an array, the
 *             Content-Type header will be set to
 *             multipart/form-data.
 *            
 *            
 *             As of PHP 5.2.0, value must be an array if
 *             files are passed to this option with the @ prefix.
 *            
 *            
 *             As of PHP 5.5.0, the @ prefix is deprecated and
 *             files can be sent using CURLFile. The 
 *             @ prefix can be disabled for safe passing of
 *             values beginning with @ by setting the 
 *             CURLOPT_SAFE_UPLOAD option to true.
 *            
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_PROXY</td>
 *           
 *            The HTTP proxy to tunnel requests through.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_PROXYUSERPWD</td>
 *           
 *            A username and password formatted as
 *            "[username]:[password]" to use for the
 *            connection to the proxy.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_RANDOM_FILE</td>
 *           
 *            A filename to be used to seed the random number generator for SSL.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_RANGE</td>
 *           
 *            Range(s) of data to retrieve in the format
 *            "X-Y" where X or Y are optional. HTTP transfers
 *            also support several intervals, separated with commas in the format
 *            "X-Y,N-M".
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_REFERER</td>
 *           
 *            The contents of the "Referer: " header to be used
 *            in a HTTP request.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_SSH_HOST_PUBLIC_KEY_MD5</td>
 *           
 *            A string containing 32 hexadecimal digits. The string should be the 
 *            MD5 checksum of the remote host's public key, and libcurl will reject 
 *            the connection to the host unless the md5sums match. 
 *            This option is only for SCP and SFTP transfers.
 *           </td>
 *           
 *            Added in cURL 7.17.1. 
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_SSH_PUBLIC_KEYFILE</td>
 *           
 *            The file name for your public key. If not used, libcurl defaults to 
 *            $HOME/.ssh/id_dsa.pub if the HOME environment variable is set, 
 *            and just "id_dsa.pub" in the current directory if HOME is not set.
 *           </td>
 *           
 *            Added in cURL 7.16.1. 
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_SSH_PRIVATE_KEYFILE</td>
 *           
 *            The file name for your private key. If not used, libcurl defaults to 
 *            $HOME/.ssh/id_dsa if the HOME environment variable is set, 
 *            and just "id_dsa" in the current directory if HOME is not set. 
 *            If the file is password-protected, set the password with 
 *            CURLOPT_KEYPASSWD.
 *           </td>
 *           
 *            Added in cURL 7.16.1. 
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_SSL_CIPHER_LIST</td>
 *           
 *            A list of ciphers to use for SSL. For example,
 *            RC4-SHA and TLSv1 are valid
 *            cipher lists.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_SSLCERT</td>
 *           
 *            The name of a file containing a PEM formatted certificate.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_SSLCERTPASSWD</td>
 *           
 *            The password required to use the
 *            CURLOPT_SSLCERT certificate.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_SSLCERTTYPE</td>
 *           
 *            The format of the certificate. Supported formats are
 *            "PEM" (default), "DER",
 *            and "ENG".
 *           </td>
 *           
 *            Added in cURL 7.9.3.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_SSLENGINE</td>
 *           
 *            The identifier for the crypto engine of the private SSL key
 *            specified in CURLOPT_SSLKEY.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_SSLENGINE_DEFAULT</td>
 *           
 *            The identifier for the crypto engine used for asymmetric crypto
 *            operations.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_SSLKEY</td>
 *           
 *            The name of a file containing a private SSL key.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_SSLKEYPASSWD</td>
 *           <p>
 *            The secret password needed to use the private SSL key specified in
 *            CURLOPT_SSLKEY.
 *            
 *             <p>
 *              Since this option contains a sensitive password, remember to keep
 *              the PHP script it is contained within safe.
 *             </p>
 *            
 *            </p></td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_SSLKEYTYPE</td>
 *           
 *            The key type of the private SSL key specified in
 *            CURLOPT_SSLKEY. Supported key types are
 *            "PEM" (default), "DER",
 *            and "ENG".
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_URL</td>
 *           
 *            The URL to fetch. This can also be set when initializing a
 *            session with curl_init.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_USERAGENT</td>
 *           
 *            The contents of the "User-Agent: " header to be
 *            used in a HTTP request.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_USERPWD</td>
 *           
 *            A username and password formatted as
 *            "[username]:[password]" to use for the
 *            connection.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *         
 *        
 *       
 *      </p>
 *      <p>
 *       value should be an array for the
 *       following values of the option parameter:
 *       
 *        
 *         
 *          <tr valign="top">
 *           <td>Option</td>
 *           <td>Set value to</td>
 *           <td>Notes</td>
 *          </tr>
 *         
 *         
 *          <tr valign="top">
 *           CURLOPT_HTTP200ALIASES</td>
 *           
 *            An array of HTTP 200 responses that will be treated as valid
 *            responses and not as errors.
 *           </td>
 *           
 *            Added in cURL 7.10.3.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_HTTPHEADER</td>
 *           
 *            An array of HTTP header fields to set, in the format
 *            
 *             array('Content-type: text/plain', 'Content-length: 100')
 *            
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_POSTQUOTE</td>
 *           
 *            An array of FTP commands to execute on the server after the FTP
 *            request has been performed.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_QUOTE</td>
 *           
 *            An array of FTP commands to execute on the server prior to the FTP
 *            request.
 *           </td>
 *           
 *           </td>
 *          </tr>
 *         
 *        
 *       
 *      </p>
 *      <p>
 *       value should be a stream resource (using
 *       fopen, for example) for the following values of the
 *       option parameter:
 *       
 *        
 *         
 *          <tr valign="top">
 *           <td>Option</td>
 *           <td>Set value to</td>
 *          </tr>
 *         
 *         
 *          <tr valign="top">
 *           CURLOPT_FILE</td>
 *           
 *            The file that the transfer should be written to. The default
 *            is STDOUT (the browser window).
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_INFILE</td>
 *           
 *            The file that the transfer should be read from when uploading.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_STDERR</td>
 *           
 *            An alternative location to output errors to instead of
 *            STDERR.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_WRITEHEADER</td>
 *           
 *            The file that the header part of the transfer is written to.
 *           </td>
 *          </tr>
 *         
 *        
 *       
 *      </p>
 *      <p>
 *       value should be the name of a valid function or a Closure 
 *       for the following values of the option parameter:
 *       
 *        
 *         
 *          <tr valign="top">
 *           <td>Option</td>
 *           <td>Set value to</td>
 *          </tr>
 *         
 *         
 *          <tr valign="top">
 *           CURLOPT_HEADERFUNCTION</td>
 *           
 *            A callback accepting two parameters.
 *            The first is the cURL resource, the second is a
 *            string with the header data to be written. The header data must
 *            be written when by this callback. Return the number of 
 *            bytes written.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_PASSWDFUNCTION</td>
 *           
 *            A callback accepting three parameters. 
 *            The first is the cURL resource, the second is a
 *            string containing a password prompt, and the third is the maximum
 *            password length. Return the string containing the password.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_PROGRESSFUNCTION</td>
 *           
 *            <p>
 *             A callback accepting five parameters.
 *             The first is the cURL resource, the second is the total number of
 *             bytes expected to be downloaded in this transfer, the third is
 *             the number of bytes downloaded so far, the fourth is the total
 *             number of bytes expected to be uploaded in this transfer, and the
 *             fifth is the number of bytes uploaded so far.
 *            </p>
 *            
 *             <p>
 *              The callback is only called when the CURLOPT_NOPROGRESS
 *              option is set to false.
 *             </p>
 *            
 *            <p>
 *             Return a non-zero value to abort the transfer. In which case, the
 *             transfer will set a CURLE_ABORTED_BY_CALLBACK
 *             error.
 *            </p>
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_READFUNCTION</td>
 *           
 *            A callback accepting three parameters. 
 *            The first is the cURL resource, the second is a
 *            stream resource provided to cURL through the option
 *            CURLOPT_INFILE, and the third is the maximum
 *            amount of data to be read. The callback must return a string
 *            with a length equal or smaller than the amount of data requested,
 *            typically by reading it from the passed stream resource. It should
 *            return an empty string to signal EOF.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLOPT_WRITEFUNCTION</td>
 *           
 *            A callback accepting two parameters. 
 *            The first is the cURL resource, and the second is a
 *            string with the data to be written. The data must be saved by
 *            this callback. It must return the exact number of bytes written 
 *            or the transfer will be aborted with an error.
 *           </td>
 *          </tr>
 *         
 *        
 *       
 *      </p>
 *      <p>
 *       Other values:
 *       
 *        
 *         
 *          <tr valign="top">
 *           <td>Option</td>
 *           <td>Set value to</td>
 *          </tr>
 *         
 *         
 *          <tr valign="top">
 *           CURLOPT_SHARE</td>
 *           
 *            A result of curl_share_init. Makes the cURL
 *            handle to use the data from the shared handle.
 *           </td>
 *          </tr>
 *         
 *        
 *       
 *      </p>
 * @return bool Returns true on success, false on failure.
 */
function curl_setopt ($ch, $option, $value) {}

/**
 * Set multiple options for a cURL transfer
 * @link http://www.php.net/manual/en/function.curl-setopt-array.php
 * @param ch resource 
 * @param options array <p>
 *       An array specifying which options to set and their values.
 *       The keys should be valid curl_setopt constants or
 *       their integer equivalents.
 *      </p>
 * @return bool true if all options were successfully set. If an option could
 *   not be successfully set, false is immediately returned, ignoring any
 *   future options in the options array.
 */
function curl_setopt_array ($ch, array $options) {}

/**
 * Perform a cURL session
 * @link http://www.php.net/manual/en/function.curl-exec.php
 * @param ch resource 
 * @return mixed Returns true on success, false on failure. However, if the CURLOPT_RETURNTRANSFER
 *   option is set, it will return
 *   the result on success, false on failure.
 */
function curl_exec ($ch) {}

/**
 * Get information regarding a specific transfer
 * @link http://www.php.net/manual/en/function.curl-getinfo.php
 * @param ch resource 
 * @param opt int[optional] <p>
 *       This may be one of the following constants:
 *       
 *        
 *         
 *          CURLINFO_EFFECTIVE_URL - Last effective URL
 * @return mixed If opt is given, returns its value.
 *   Otherwise, returns an associative array with the following elements 
 *   (which correspond to opt), or false on failure:
 *   
 *    
 *     
 *      "url"
 *     
 *    
 *    
 *     
 *      "content_type"
 *     
 *    
 *    
 *     
 *      "http_code"
 *     
 *    
 *    
 *     
 *      "header_size"
 *     
 *    
 *    
 *     
 *      "request_size"
 *     
 *    
 *    
 *     
 *      "filetime"
 *     
 *    
 *    
 *     
 *      "ssl_verify_result"
 *     
 *    
 *    
 *     
 *      "redirect_count"
 *     
 *    
 *    
 *     
 *      "total_time"
 *     
 *    
 *    
 *     
 *      "namelookup_time"
 *     
 *    
 *    
 *     
 *      "connect_time"
 *     
 *    
 *    
 *     
 *      "pretransfer_time"
 *     
 *    
 *    
 *     
 *      "size_upload"
 *     
 *    
 *    
 *     
 *      "size_download"
 *     
 *    
 *    
 *     
 *      "speed_download"
 *     
 *    
 *    
 *     
 *      "speed_upload"
 *     
 *    
 *    
 *     
 *      "download_content_length"
 *     
 *    
 *    
 *     
 *      "upload_content_length"
 *     
 *    
 *    
 *     
 *      "starttransfer_time"
 *     
 *    
 *    
 *     
 *      "redirect_time"
 *     
 *    
 *    
 *     
 *      "certinfo"
 *     
 *    
 *    
 *     
 *      "request_header" (This is only set if the CURLINFO_HEADER_OUT 
 *      is set by a previous call to curl_setopt)
 */
function curl_getinfo ($ch, $opt = null) {}

/**
 * Return a string containing the last error for the current session
 * @link http://www.php.net/manual/en/function.curl-error.php
 * @param ch resource 
 * @return string the error message or '' (the empty string) if no
 *   error occurred.
 */
function curl_error ($ch) {}

/**
 * Return the last error number
 * @link http://www.php.net/manual/en/function.curl-errno.php
 * @param ch resource 
 * @return int the error number or 0 (zero) if no error
 *   occurred.
 */
function curl_errno ($ch) {}

/**
 * Close a cURL session
 * @link http://www.php.net/manual/en/function.curl-close.php
 * @param ch resource 
 * @return void 
 */
function curl_close ($ch) {}

/**
 * Return string describing the given error code
 * @link http://www.php.net/manual/en/function.curl-strerror.php
 * @param errornum int <p>
 *      One of the cURL error codes constants.
 *     </p>
 * @return string error description or &null; for invalid error code.
 */
function curl_strerror ($errornum) {}

/**
 * Return string describing error code
 * @link http://www.php.net/manual/en/function.curl-multi-strerror.php
 * @param errornum int <p>
 *      One of the CURLM error codes constants.
 *     </p>
 * @return string error string for valid error code, &null; otherwise.
 */
function curl_multi_strerror ($errornum) {}

/**
 * Reset all options of a libcurl session handle
 * @link http://www.php.net/manual/en/function.curl-reset.php
 * @param ch resource 
 * @return void 
 */
function curl_reset ($ch) {}

/**
 * URL encodes the given string
 * @link http://www.php.net/manual/en/function.curl-escape.php
 * @param ch resource 
 * @param str string <p>
 *      The string to be encoded.
 *     </p>
 * @return string escaped string&return.falseforfailure;.
 */
function curl_escape ($ch, $str) {}

/**
 * Decodes the given URL encoded string
 * @link http://www.php.net/manual/en/function.curl-unescape.php
 * @param ch resource 
 * @param str string <p>
 *      The URL encoded string to be decoded.
 *     </p>
 * @return string decoded string &return.falseforfailure;.
 */
function curl_unescape ($ch, $str) {}

/**
 * Pause and unpause a connection
 * @link http://www.php.net/manual/en/function.curl-pause.php
 * @param ch resource 
 * @param bitmask int <p>
 *      One of CURLPAUSE_* constants.
 *     </p>
 * @return int an error code (CURLE_OK for no error).
 */
function curl_pause ($ch, $bitmask) {}

/**
 * Returns a new cURL multi handle
 * @link http://www.php.net/manual/en/function.curl-multi-init.php
 * @return resource a cURL multi handle resource on success, false on failure.
 */
function curl_multi_init () {}

/**
 * Add a normal cURL handle to a cURL multi handle
 * @link http://www.php.net/manual/en/function.curl-multi-add-handle.php
 * @param mh resource 
 * @param ch resource 
 * @return int 0 on success, or one of the CURLM_XXX errors
 *   code.
 */
function curl_multi_add_handle ($mh, $ch) {}

/**
 * Remove a multi handle from a set of cURL handles
 * @link http://www.php.net/manual/en/function.curl-multi-remove-handle.php
 * @param mh resource 
 * @param ch resource 
 * @return int 0 on success, or one of the CURLM_XXX error
 *   codes.
 */
function curl_multi_remove_handle ($mh, $ch) {}

/**
 * Wait for activity on any curl_multi connection
 * @link http://www.php.net/manual/en/function.curl-multi-select.php
 * @param mh resource 
 * @param timeout float[optional] <p>
 *       Time, in seconds, to wait for a response.
 *      </p>
 * @return int On success, returns the number of descriptors contained in 
 *   the descriptor sets.  On failure, this function will return -1 on a select failure or timeout (from the underlying select system call).
 */
function curl_multi_select ($mh, $timeout = null) {}

/**
 * Run the sub-connections of the current cURL handle
 * @link http://www.php.net/manual/en/function.curl-multi-exec.php
 * @param mh resource 
 * @param still_running int <p>
 *       A reference to a flag to tell whether the operations are still running.
 *      </p>
 * @return int A cURL code defined in the cURL Predefined Constants.
 *  </p>
 *  
 *   <p>
 *    This only returns errors regarding the whole multi stack. There might still have 
 *    occurred problems on individual transfers even when this function returns 
 *    CURLM_OK.
 */
function curl_multi_exec ($mh, &$still_running) {}

/**
 * Return the content of a cURL handle if <constant>CURLOPT_RETURNTRANSFER</constant> is set
 * @link http://www.php.net/manual/en/function.curl-multi-getcontent.php
 * @param ch resource 
 * @return string Return the content of a cURL handle if CURLOPT_RETURNTRANSFER is set.
 */
function curl_multi_getcontent ($ch) {}

/**
 * Get information about the current transfers
 * @link http://www.php.net/manual/en/function.curl-multi-info-read.php
 * @param mh resource 
 * @param msgs_in_queue int[optional] <p>
 *       Number of messages that are still in the queue
 *      </p>
 * @return array On success, returns an associative array for the message, false on failure.
 *  </p>
 *  <p>
 *   <table>
 *    Contents of the returned array
 *    
 *     
 *      <tr valign="top">
 *       <td>Key:</td>
 *       <td>Value:</td>
 *      </tr>
 *     
 *     
 *      <tr valign="top">
 *       <td>msg</td>
 *       <td>The CURLMSG_DONE constant. Other return values
 *       are currently not available.</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>result</td>
 *       <td>One of the CURLE_* constants. If everything is
 *       OK, the CURLE_OK will be the result.</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>handle</td>
 *       <td>Resource of type curl indicates the handle which it concerns.</td>
 *      </tr>
 *     
 *    
 *   </table>
 */
function curl_multi_info_read ($mh, &$msgs_in_queue = null) {}

/**
 * Close a set of cURL handles
 * @link http://www.php.net/manual/en/function.curl-multi-close.php
 * @param mh resource 
 * @return void 
 */
function curl_multi_close ($mh) {}

/**
 * Set an option for the cURL multi handle
 * @link http://www.php.net/manual/en/function.curl-multi-setopt.php
 * @param mh resource <p>
 *      
 *     </p>
 * @param option int <p>
 *      One of the CURLMOPT_* constants.
 *     </p>
 * @param value mixed <p>
 *       The value to be set on option.
 *      </p>
 *      <p>
 *       value should be an int for the
 *       following values of the option parameter:
 *       
 *        
 *         
 *          <tr valign="top">
 *           Option</td>
 *           Set value to</td>
 *          </tr>
 *         
 *         
 *          <tr valign="top">
 *           CURLMOPT_PIPELINING</td>
 *           
 *            Pass 1 to enable or 0 to disable. Enabling pipelining on a multi
 *            handle will make it attempt to perform HTTP Pipelining as far as
 *            possible for transfers using this handle. This means that if you add
 *            a second request that can use an already existing connection, the
 *            second request will be "piped" on the same connection rather than
 *            being executed in parallel.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLMOPT_MAXCONNECTS</td>
 *           
 *            Pass a number that will be used as the maximum amount of
 *            simultaneously open connections that libcurl may cache. Default is
 *            10. When the cache is full, curl closes the oldest one in the cache
 *            to prevent the number of open connections from increasing.
 *           </td>
 *          </tr>
 *         
 *        
 *       
 *      </p>
 * @return bool Returns true on success, false on failure.
 */
function curl_multi_setopt ($mh, $option, $value) {}

/**
 * Initialize a cURL share handle
 * @link http://www.php.net/manual/en/function.curl-share-init.php
 * @return resource resource of type "cURL Share Handle".
 */
function curl_share_init () {}

/**
 * Close a cURL share handle
 * @link http://www.php.net/manual/en/function.curl-share-close.php
 * @param sh resource <p>
 *      A cURL share handle returned by curl_share_init
 *     </p>
 * @return void 
 */
function curl_share_close ($sh) {}

/**
 * Set an option for a cURL share handle.
 * @link http://www.php.net/manual/en/function.curl-share-setopt.php
 * @param sh resource <p>
 *      A cURL share handle returned by curl_share_init.
 *     </p>
 * @param option int <p>
 *      
 *        
 *         
 *          <tr valign="top">
 *           Option</td>
 *           Description</td>
 *          </tr>
 *         
 *         
 *          <tr valign="top">
 *           CURLSHOPT_SHARE</td>
 *           
 *               Specifies a type of data that should be shared.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURLSHOPT_UNSHARE</td>
 *           
 *               Specifies a type of data that will be no longer shared.
 *           </td>
 *          </tr>
 *         
 *        
 *       
 *     </p>
 * @param value string <p>
 *      
 *        
 *         
 *          <tr valign="top">
 *           Value</td>
 *           Description</td>
 *          </tr>
 *         
 *         
 *          <tr valign="top">
 *           CURL_LOCK_DATA_COOKIE</td>
 *           
 *               Shares cookie data.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURL_LOCK_DATA_DNS</td>
 *           
 *               Shares DNS cache. Note that when you use cURL multi handles,
 *               all handles added to the same multi handle will share DNS cache
 *               by default.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           CURL_LOCK_DATA_SSL_SESSION</td>
 *           
 *               Shares SSL session IDs, reducing the time spent on the SSL
 *               handshake when reconnecting to the same server. Note that SSL
 *               session IDs are reused withing the same handle by default.
 *           </td>
 *          </tr>
 *         
 *        
 *       
 *     </p>
 * @return bool Returns true on success, false on failure.
 */
function curl_share_setopt ($sh, $option, $value) {}

/**
 * Create a CURLFile object
 * @link http://www.php.net/manual/en/function.curl-file-create.php
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
