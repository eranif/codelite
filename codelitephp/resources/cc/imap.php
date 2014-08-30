<?php

// Start of imap v.

/**
 * Open an <acronym>IMAP</acronym> stream to a mailbox
 * @link http://www.php.net/manual/en/function.imap-open.php
 * @param mailbox string <p>
 *       A mailbox name consists of a server and a mailbox path on this server.
 *       The special name INBOX stands for the current users
 *       personal mailbox. Mailbox names that contain international characters
 *       besides those in the printable ASCII space have to be encoded width
 *       imap_utf7_encode.
 *      </p>
 *      <p>
 *       The server part, which is enclosed in '{' and '}', consists of the servers
 *       name or ip address, an optional port (prefixed by ':'), and an optional
 *       protocol specification (prefixed by '/'). 
 *      </p>
 *      <p>
 *       The server part is mandatory in all mailbox
 *       parameters. 
 *      </p>
 *      <p>
 *       All names which start with { are remote names, and are
 *       in the form "{" remote_system_name [":" port] [flags] "}"
 *       [mailbox_name] where:
 *       
 *        
 *         
 *          remote_system_name - Internet domain name or
 *          bracketed IP address of server.
 * @param username string <p>
 *       The user name
 *      </p>
 * @param password string <p>
 *       The password associated with the username
 *      </p>
 * @param options int[optional] <p>
 *       The options are a bit mask with one or more of
 *       the following:
 *       
 *        
 *         
 *          OP_READONLY - Open mailbox read-only
 * @param n_retries int[optional] <p>
 *       Number of maximum connect attempts
 *      </p>
 * @param params array[optional] <p>
 *       Connection parameters, the following (string) keys maybe used 
 *       to set one or more connection parameters:
 *       
 *        
 *         
 *          DISABLE_AUTHENTICATOR - Disable authentication properties
 * @return resource an IMAP stream on success or false on error.
 */
function imap_open ($mailbox, $username, $password, $options = null, $n_retries = null, array $params = null) {}

/**
 * Reopen <acronym>IMAP</acronym> stream to new mailbox
 * @link http://www.php.net/manual/en/function.imap-reopen.php
 * @param imap_stream resource 
 * @param mailbox string <p>
 *       The mailbox name, see imap_open for more
 *       information
 *      </p>
 * @param options int[optional] <p>
 *       The options are a bit mask with one or more of
 *       the following:
 *       
 *        
 *         
 *          OP_READONLY - Open mailbox read-only
 * @param n_retries int[optional] <p>
 *       Number of maximum connect attempts
 *      </p>
 * @return bool true if the stream is reopened, false otherwise.
 */
function imap_reopen ($imap_stream, $mailbox, $options = null, $n_retries = null) {}

/**
 * Close an IMAP stream
 * @link http://www.php.net/manual/en/function.imap-close.php
 * @param imap_stream resource 
 * @param flag int[optional] <p>
 *       If set to CL_EXPUNGE, the function will silently
 *       expunge the mailbox before closing, removing all messages marked for
 *       deletion. You can achieve the same thing by using
 *       imap_expunge
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imap_close ($imap_stream, $flag = null) {}

/**
 * Gets the number of messages in the current mailbox
 * @link http://www.php.net/manual/en/function.imap-num-msg.php
 * @param imap_stream resource 
 * @return int Return the number of messages in the current mailbox, as an integer.
 */
function imap_num_msg ($imap_stream) {}

/**
 * Gets the number of recent messages in current mailbox
 * @link http://www.php.net/manual/en/function.imap-num-recent.php
 * @param imap_stream resource 
 * @return int the number of recent messages in the current mailbox, as an
 *   integer.
 */
function imap_num_recent ($imap_stream) {}

/**
 * Returns headers for all messages in a mailbox
 * @link http://www.php.net/manual/en/function.imap-headers.php
 * @param imap_stream resource 
 * @return array an array of string formatted with header info. One
 *   element per mail message.
 */
function imap_headers ($imap_stream) {}

/**
 * Read the header of the message
 * @link http://www.php.net/manual/en/function.imap-headerinfo.php
 * @param imap_stream resource 
 * @param msg_number int <p>
 *       The message number
 *      </p>
 * @param fromlength int[optional] <p>
 *       Number of characters for the fetchfrom property.
 *       Must be greater than or equal to zero.
 *      </p>
 * @param subjectlength int[optional] <p>
 *       Number of characters for the fetchsubject property
 *       Must be greater than or equal to zero.
 *      </p>
 * @param defaulthost string[optional] <p>
 *      </p>
 * @return object the information in an object with following properties:
 *   
 *    
 *     
 *      toaddress - full to: line, up to 1024 characters
 *     
 *    
 *    
 *     
 *      to - an array of objects from the To: line, with the following 
 *      properties: personal, adl,
 *      mailbox, and host
 *     
 *    
 *    
 *     
 *      fromaddress - full from: line, up to 1024 characters
 *     
 *    
 *    
 *     
 *      from - an array of objects from the From: line, with the following 
 *      properties: personal, adl,
 *      mailbox, and host
 *     
 *    
 *    
 *     
 *      ccaddress - full cc: line, up to 1024 characters
 *     
 *    
 *    
 *     
 *      cc - an array of objects from the Cc: line, with the following 
 *      properties: personal, adl,
 *      mailbox, and host
 *     
 *    
 *    
 *     
 *      bccaddress - full bcc: line, up to 1024 characters
 *     
 *    
 *    
 *     
 *      bcc - an array of objects from the Bcc: line, with the following 
 *      properties: personal, adl,
 *      mailbox, and host
 *     
 *    
 *    
 *     
 *      reply_toaddress - full Reply-To: line, up to 1024 characters
 *     
 *    
 *    
 *     
 *      reply_to - an array of objects from the Reply-To: line, with the following
 *      properties: personal, adl,
 *      mailbox, and host
 *     
 *    
 *    
 *     
 *      senderaddress - full sender: line, up to 1024 characters
 *     
 *    
 *    
 *     
 *      sender - an array of objects from the Sender: line, with the following 
 *      properties: personal, adl,
 *      mailbox, and host
 *     
 *    
 *    
 *     
 *      return_pathaddress - full Return-Path: line, up to 1024 characters
 *     
 *    
 *    
 *     
 *      return_path - an array of objects from the Return-Path: line, with the
 *      following properties: personal, 
 *      adl, mailbox, and 
 *      host
 *     
 *    
 *    
 *     
 *      remail - 
 *     
 *    
 *    
 *     
 *      date - The message date as found in its headers
 *     
 *    
 *    
 *     
 *      Date - Same as date
 *     
 *    
 *    
 *     
 *      subject - The message subject
 *     
 *    
 *    
 *     
 *      Subject - Same a subject 
 *     
 *    
 *    
 *     
 *      in_reply_to - 
 *     
 *    
 *    
 *     
 *      message_id - 
 *     
 *    
 *    
 *     
 *      newsgroups - 
 *     
 *    
 *    
 *     
 *      followup_to - 
 *     
 *    
 *    
 *     
 *      references - 
 *     
 *    
 *    
 *     
 *      Recent - R if recent and seen, N
 *      if recent and not seen, ' ' if not recent.
 *     
 *    
 *    
 *     
 *      Unseen - U if not seen AND not recent, ' ' if seen
 *      OR not seen and recent
 *     
 *    
 *    
 *     
 *      Flagged - F if flagged, ' ' if not flagged
 *     
 *    
 *    
 *     
 *      Answered - A if answered, ' ' if unanswered
 *     
 *    
 *    
 *     
 *      Deleted - D if deleted, ' ' if not deleted
 *     
 *    
 *    
 *     
 *      Draft - X if draft, ' ' if not draft
 *     
 *    
 *    
 *     
 *      Msgno - The message number
 *     
 *    
 *    
 *     
 *      MailDate - 
 *     
 *    
 *    
 *     
 *      Size - The message size
 *     
 *    
 *    
 *     
 *      udate - mail message date in Unix time
 *     
 *    
 *    
 *     
 *      fetchfrom - from line formatted to fit fromlength
 *      characters
 *     
 *    
 *    
 *     
 *      fetchsubject - subject line formatted to fit 
 *      subjectlength characters
 */
function imap_headerinfo ($imap_stream, $msg_number, $fromlength = null, $subjectlength = null, $defaulthost = null) {}

/**
 * Parse mail headers from a string
 * @link http://www.php.net/manual/en/function.imap-rfc822-parse-headers.php
 * @param headers string <p>
 *       The parsed headers data
 *      </p>
 * @param defaulthost string[optional] <p>
 *       The default host name
 *      </p>
 * @return object an object similar to the one returned by
 *   imap_header, except for the flags and other 
 *   properties that come from the IMAP server.
 */
function imap_rfc822_parse_headers ($headers, $defaulthost = null) {}

/**
 * Returns a properly formatted email address given the mailbox, host, and personal info
 * @link http://www.php.net/manual/en/function.imap-rfc822-write-address.php
 * @param mailbox string <p>
 *       The mailbox name, see imap_open for more
 *       information
 *      </p>
 * @param host string <p>
 *       The email host part
 *      </p>
 * @param personal string <p>
 *       The name of the account owner
 *      </p>
 * @return string a string properly formatted email address as defined in RFC2822.
 */
function imap_rfc822_write_address ($mailbox, $host, $personal) {}

/**
 * Parses an address string
 * @link http://www.php.net/manual/en/function.imap-rfc822-parse-adrlist.php
 * @param address string <p>
 *       A string containing addresses
 *      </p>
 * @param default_host string <p>
 *       The default host name
 *      </p>
 * @return array an array of objects. The objects properties are:
 *  </p>
 *  <p>
 *   
 *    
 *     
 *      mailbox  - the mailbox name (username)
 *     
 *    
 *    
 *     
 *      host - the host name
 *     
 *    
 *    
 *     
 *      personal - the personal name
 *     
 *    
 *    
 *     
 *      adl - at domain source route
 */
function imap_rfc822_parse_adrlist ($address, $default_host) {}

/**
 * Read the message body
 * @link http://www.php.net/manual/en/function.imap-body.php
 * @param imap_stream resource 
 * @param msg_number int <p>
 *       The message number
 *      </p>
 * @param options int[optional] <p>
 *       The optional options are a bit mask
 *       with one or more of the following:
 *       
 *        
 *         
 *          FT_UID - The msg_number is a UID
 * @return string the body of the specified message, as a string.
 */
function imap_body ($imap_stream, $msg_number, $options = null) {}

/**
 * Read the structure of a specified body section of a specific message
 * @link http://www.php.net/manual/en/function.imap-bodystruct.php
 * @param imap_stream resource 
 * @param msg_number int <p>
 *       The message number
 *      </p>
 * @param section string <p>
 *       The body section to read
 *      </p>
 * @return object the information in an object, for a detailed description
 *   of the object structure and properties see 
 *   imap_fetchstructure.
 */
function imap_bodystruct ($imap_stream, $msg_number, $section) {}

/**
 * Fetch a particular section of the body of the message
 * @link http://www.php.net/manual/en/function.imap-fetchbody.php
 * @param imap_stream resource 
 * @param msg_number int <p>
 *       The message number
 *      </p>
 * @param section string <p>
 *       The part number. It is a string of integers delimited by period which
 *       index into a body part list as per the IMAP4 specification
 *      </p>
 * @param options int[optional] <p>
 *       A bitmask with one or more of the following:
 *       
 *        
 *         
 *          FT_UID - The msg_number is a UID
 * @return string a particular section of the body of the specified messages as a
 *   text string.
 */
function imap_fetchbody ($imap_stream, $msg_number, $section, $options = null) {}

/**
 * Fetch MIME headers for a particular section of the message
 * @link http://www.php.net/manual/en/function.imap-fetchmime.php
 * @param imap_stream resource 
 * @param msg_number int <p>
 *       The message number
 *      </p>
 * @param section string <p>
 *       The part number. It is a string of integers delimited by period which
 *       index into a body part list as per the IMAP4 specification
 *      </p>
 * @param options int[optional] <p>
 *       A bitmask with one or more of the following:
 *       
 *        
 *         
 *          FT_UID - The msg_number is a UID
 * @return string the MIME headers of a particular section of the body of the specified messages as a
 *   text string.
 */
function imap_fetchmime ($imap_stream, $msg_number, $section, $options = null) {}

/**
 * Save a specific body section to a file
 * @link http://www.php.net/manual/en/function.imap-savebody.php
 * @param imap_stream resource 
 * @param file mixed <p>
 *       The path to the saved file as a string, or a valid file descriptor
 *       returned by fopen.
 *      </p>
 * @param msg_number int <p>
 *       The message number
 *      </p>
 * @param part_number string[optional] <p>
 *       The part number. It is a string of integers delimited by period which
 *       index into a body part list as per the IMAP4 specification
 *      </p>
 * @param options int[optional] <p>
 *       A bitmask with one or more of the following:
 *       
 *        
 *         
 *          FT_UID - The msg_number is a UID
 * @return bool Returns true on success or false on failure.
 */
function imap_savebody ($imap_stream, $file, $msg_number, $part_number = null, $options = null) {}

/**
 * Returns header for a message
 * @link http://www.php.net/manual/en/function.imap-fetchheader.php
 * @param imap_stream resource 
 * @param msg_number int <p>
 *       The message number
 *      </p>
 * @param options int[optional] <p>
 *       The possible options are:
 *       
 *        
 *         
 *          FT_UID - The msgno
 *          argument is a UID
 * @return string the header of the specified message as a text string.
 */
function imap_fetchheader ($imap_stream, $msg_number, $options = null) {}

/**
 * Read the structure of a particular message
 * @link http://www.php.net/manual/en/function.imap-fetchstructure.php
 * @param imap_stream resource 
 * @param msg_number int <p>
 *       The message number
 *      </p>
 * @param options int[optional] <p>
 *       This optional parameter only has a single option, 
 *       FT_UID, which tells the function to treat the
 *       msg_number argument as a 
 *       UID.
 *      </p>
 * @return object an object includes the envelope, internal date, size, flags and
 *   body structure along with a similar object for each mime attachment. The
 *   structure of the returned objects is as follows:
 *  </p>
 *  <p>
 *   <table>
 *    
 *     Returned Objects for imap_fetchstructure
 *    
 *    
 *     
 *      <tr valign="top">
 *       <td>type</td>
 *       <td>Primary body type</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>encoding</td>
 *       <td>Body transfer encoding</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>ifsubtype</td>
 *       <td>true if there is a subtype string</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>subtype</td>
 *       <td>MIME subtype</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>ifdescription</td>
 *       <td>true if there is a description string</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>description</td>
 *       <td>Content description string</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>ifid</td>
 *       <td>true if there is an identification string</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>id</td>
 *       <td>Identification string</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>lines</td>
 *       <td>Number of lines</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>bytes</td>
 *       <td>Number of bytes</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>ifdisposition</td>
 *       <td>true if there is a disposition string</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>disposition</td>
 *       <td>Disposition string</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>ifdparameters</td>
 *       <td>true if the dparameters array exists</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>dparameters</td>
 *       <td>An array of objects where each object has an
 *        "attribute" and a "value"
 *        property corresponding to the parameters on the
 *        Content-disposition MIME
 *        header.</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>ifparameters</td>
 *       <td>true if the parameters array exists</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>parameters</td>
 *       <td>An array of objects where each object has an
 *        "attribute" and a "value"
 *        property.</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>parts</td>
 *       <td>An array of objects identical in structure to the top-level
 *       object, each of which corresponds to a MIME body
 *       part.</td>
 *      </tr>
 *     
 *    
 *   </table>
 *  </p>
 *  <p>
 *   <table>
 *     Primary body type (may vary with used library)
 *     
 *      
 *      <tr valign="top"><td>0</td><td>text</td></tr>
 *      <tr valign="top"><td>1</td><td>multipart</td></tr>
 *      <tr valign="top"><td>2</td><td>message</td></tr>
 *      <tr valign="top"><td>3</td><td>application</td></tr>
 *      <tr valign="top"><td>4</td><td>audio</td></tr>
 *      <tr valign="top"><td>5</td><td>image</td></tr>
 *      <tr valign="top"><td>6</td><td>video</td></tr>
 *      <tr valign="top"><td>7</td><td>other</td></tr>
 *      
 *     
 *   </table>
 *  </p>
 *  <p>
 *   <table>
 *     Transfer encodings (may vary with used library)
 *     
 *      
 *      <tr valign="top"><td>0</td><td>7BIT</td></tr>
 *      <tr valign="top"><td>1</td><td>8BIT</td></tr>
 *      <tr valign="top"><td>2</td><td>BINARY</td></tr>
 *      <tr valign="top"><td>3</td><td>BASE64</td></tr>
 *      <tr valign="top"><td>4</td><td>QUOTED-PRINTABLE</td></tr>
 *      <tr valign="top"><td>5</td><td>OTHER</td></tr>
 *     
 *    
 *   </table>
 */
function imap_fetchstructure ($imap_stream, $msg_number, $options = null) {}

/**
 * Clears IMAP cache
 * @link http://www.php.net/manual/en/function.imap-gc.php
 * @param imap_stream resource 
 * @param caches int <p>
 *       Specifies the cache to purge. It may one or a combination
 *       of the following constants: 
 *       IMAP_GC_ELT (message cache elements), 
 *       IMAP_GC_ENV (enveloppe and bodies),
 *       IMAP_GC_TEXTS (texts).
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imap_gc ($imap_stream, $caches) {}

/**
 * Delete all messages marked for deletion
 * @link http://www.php.net/manual/en/function.imap-expunge.php
 * @param imap_stream resource 
 * @return bool true.
 */
function imap_expunge ($imap_stream) {}

/**
 * Mark a message for deletion from current mailbox
 * @link http://www.php.net/manual/en/function.imap-delete.php
 * @param imap_stream resource 
 * @param msg_number int <p>
 *       The message number
 *      </p>
 * @param options int[optional] <p>
 *       You can set the FT_UID which tells the function
 *       to treat the msg_number argument as an
 *       UID.
 *      </p>
 * @return bool true.
 */
function imap_delete ($imap_stream, $msg_number, $options = null) {}

/**
 * Unmark the message which is marked deleted
 * @link http://www.php.net/manual/en/function.imap-undelete.php
 * @param imap_stream resource 
 * @param msg_number int <p>
 *       The message number
 *      </p>
 * @param flags int[optional] <p>
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imap_undelete ($imap_stream, $msg_number, $flags = null) {}

/**
 * Check current mailbox
 * @link http://www.php.net/manual/en/function.imap-check.php
 * @param imap_stream resource 
 * @return object the information in an object with following properties:
 *   
 *    
 *     
 *      Date - current system time formatted according to RFC2822
 *     
 *    
 *    
 *     
 *      Driver - protocol used to access this mailbox:
 *      POP3, IMAP, NNTP
 *     
 *    
 *    
 *     
 *      Mailbox - the mailbox name
 *     
 *    
 *    
 *     
 *      Nmsgs - number of messages in the mailbox
 *     
 *    
 *    
 *     
 *      Recent - number of recent messages in the mailbox
 *     
 *    
 *   
 *  </p>
 *  <p>
 *   Returns false on failure.
 */
function imap_check ($imap_stream) {}

/**
 * Returns the list of mailboxes that matches the given text
 * @link http://www.php.net/manual/en/function.imap-listscan.php
 * @param imap_stream resource 
 * @param ref string <p>
 *       ref should normally be just the server 
 *       specification as described in imap_open
 *      </p>
 * @param pattern string &imap.pattern;
 * @param content string <p>
 *       The searched string
 *      </p>
 * @return array an array containing the names of the mailboxes that have
 *   content in the text of the mailbox.
 */
function imap_listscan ($imap_stream, $ref, $pattern, $content) {}

/**
 * Copy specified messages to a mailbox
 * @link http://www.php.net/manual/en/function.imap-mail-copy.php
 * @param imap_stream resource 
 * @param msglist string <p>
 *       msglist is a range not just message
 *       numbers (as described in RFC2060).
 *      </p>
 * @param mailbox string <p>
 *       The mailbox name, see imap_open for more
 *       information
 *      </p>
 * @param options int[optional] <p>
 *       options is a bitmask of one or more of
 *       
 *        
 *         
 *          CP_UID - the sequence numbers contain UIDS
 * @return bool Returns true on success or false on failure.
 */
function imap_mail_copy ($imap_stream, $msglist, $mailbox, $options = null) {}

/**
 * Move specified messages to a mailbox
 * @link http://www.php.net/manual/en/function.imap-mail-move.php
 * @param imap_stream resource 
 * @param msglist string <p>
 *       msglist is a range not just message numbers
 *       (as described in RFC2060).
 *      </p>
 * @param mailbox string <p>
 *       The mailbox name, see imap_open for more
 *       information
 *      </p>
 * @param options int[optional] <p>
 *       options is a bitmask and may contain the single option:
 *       
 *        
 *         
 *          CP_UID - the sequence numbers contain UIDS
 * @return bool Returns true on success or false on failure.
 */
function imap_mail_move ($imap_stream, $msglist, $mailbox, $options = null) {}

/**
 * Create a MIME message based on given envelope and body sections
 * @link http://www.php.net/manual/en/function.imap-mail-compose.php
 * @param envelope array <p>
 *       An associative array of headers fields. Valid keys are: "remail",
 *       "return_path", "date", "from", "reply_to", "in_reply_to", "subject",
 *       "to", "cc", "bcc", "message_id" and "custom_headers" (which contains
 *       associative array of other headers).
 *      </p>
 * @param body array <p>
 *       An indexed array of bodies
 *      </p>
 *      <p>
 *       A body is an associative array which can consist of the following keys:
 *       "type", "encoding", "charset", "type.parameters", "subtype", "id",
 *       "description", "disposition.type", "disposition", "contents.data",
 *       "lines", "bytes" and "md5".
 *      </p>
 * @return string the MIME message.
 */
function imap_mail_compose (array $envelope, array $body) {}

/**
 * Create a new mailbox
 * @link http://www.php.net/manual/en/function.imap-createmailbox.php
 * @param imap_stream resource 
 * @param mailbox string <p>
 *       The mailbox name, see imap_open for more
 *       information. Names containing international characters should be
 *       encoded by imap_utf7_encode
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imap_createmailbox ($imap_stream, $mailbox) {}

/**
 * Rename an old mailbox to new mailbox
 * @link http://www.php.net/manual/en/function.imap-renamemailbox.php
 * @param imap_stream resource 
 * @param old_mbox string <p>
 *       The old mailbox name, see imap_open for more
 *       information
 *      </p>
 * @param new_mbox string <p>
 *       The new mailbox name, see imap_open for more
 *       information
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imap_renamemailbox ($imap_stream, $old_mbox, $new_mbox) {}

/**
 * Delete a mailbox
 * @link http://www.php.net/manual/en/function.imap-deletemailbox.php
 * @param imap_stream resource 
 * @param mailbox string <p>
 *       The mailbox name, see imap_open for more
 *       information
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imap_deletemailbox ($imap_stream, $mailbox) {}

/**
 * Subscribe to a mailbox
 * @link http://www.php.net/manual/en/function.imap-subscribe.php
 * @param imap_stream resource 
 * @param mailbox string <p>
 *       The mailbox name, see imap_open for more
 *       information
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imap_subscribe ($imap_stream, $mailbox) {}

/**
 * Unsubscribe from a mailbox
 * @link http://www.php.net/manual/en/function.imap-unsubscribe.php
 * @param imap_stream resource 
 * @param mailbox string <p>
 *       The mailbox name, see imap_open for more
 *       information
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imap_unsubscribe ($imap_stream, $mailbox) {}

/**
 * Append a string message to a specified mailbox
 * @link http://www.php.net/manual/en/function.imap-append.php
 * @param imap_stream resource 
 * @param mailbox string <p>
 *       The mailbox name, see imap_open for more
 *       information
 *      </p>
 * @param message string <p>
 *       The message to be append, as a string
 *      </p>
 *      <p>
 *       When talking to the Cyrus IMAP server, you must use "\r\n" as
 *       your end-of-line terminator instead of "\n" or the operation will
 *       fail
 *      </p>
 * @param options string[optional] <p>
 *       If provided, the options will also be written
 *       to the mailbox
 *      </p>
 * @param internal_date string[optional] <p>
 *       If this parameter is set, it will set the INTERNALDATE on the appended message.  The parameter should be a date string that conforms to the rfc2060 specifications for a date_time value.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imap_append ($imap_stream, $mailbox, $message, $options = null, $internal_date = null) {}

/**
 * Check if the IMAP stream is still active
 * @link http://www.php.net/manual/en/function.imap-ping.php
 * @param imap_stream resource 
 * @return bool true if the stream is still alive, false otherwise.
 */
function imap_ping ($imap_stream) {}

/**
 * Decode BASE64 encoded text
 * @link http://www.php.net/manual/en/function.imap-base64.php
 * @param text string <p>
 *       The encoded text
 *      </p>
 * @return string the decoded message as a string.
 */
function imap_base64 ($text) {}

/**
 * Convert a quoted-printable string to an 8 bit string
 * @link http://www.php.net/manual/en/function.imap-qprint.php
 * @param string string <p>
 *       A quoted-printable string
 *      </p>
 * @return string an 8 bits string.
 */
function imap_qprint ($string) {}

/**
 * Convert an 8bit string to a quoted-printable string
 * @link http://www.php.net/manual/en/function.imap-8bit.php
 * @param string string <p>
 *       The 8bit string to convert
 *      </p>
 * @return string a quoted-printable string.
 */
function imap_8bit ($string) {}

/**
 * Convert an 8bit string to a base64 string
 * @link http://www.php.net/manual/en/function.imap-binary.php
 * @param string string <p>
 *       The 8bit string
 *      </p>
 * @return string a base64 encoded string.
 */
function imap_binary ($string) {}

/**
 * Converts MIME-encoded text to UTF-8
 * @link http://www.php.net/manual/en/function.imap-utf8.php
 * @param mime_encoded_text string <p>
 *       A MIME encoded string. MIME encoding method and the UTF-8 
 *       specification are described in RFC2047 and RFC2044 respectively.
 *      </p>
 * @return string an UTF-8 encoded string.
 */
function imap_utf8 ($mime_encoded_text) {}

/**
 * Returns status information on a mailbox
 * @link http://www.php.net/manual/en/function.imap-status.php
 * @param imap_stream resource 
 * @param mailbox string <p>
 *       The mailbox name, see imap_open for more
 *       information
 *      </p>
 * @param options int <p>
 *       Valid flags are:
 *       
 *        
 *         
 *          SA_MESSAGES - set $status->messages to the
 *          number of messages in the mailbox
 * @return object This function returns an object containing status information.
 *   The object has the following properties: messages,
 *   recent, unseen, 
 *   uidnext, and uidvalidity.  
 *  </p>
 *  <p>
 *   flags is also set, which contains a bitmask which can
 *   be checked against any of the above constants.
 */
function imap_status ($imap_stream, $mailbox, $options) {}

/**
 * Get information about the current mailbox
 * @link http://www.php.net/manual/en/function.imap-mailboxmsginfo.php
 * @param imap_stream resource 
 * @return object the information in an object with following properties:
 *   <table>
 *    Mailbox properties
 *    
 *     
 *      <tr valign="top">
 *       <td>Date</td>
 *       <td>date of last change (current datetime)</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>Driver</td>
 *       <td>driver</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>Mailbox</td>
 *       <td>name of the mailbox</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>Nmsgs</td>
 *       <td>number of messages</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>Recent</td>
 *       <td>number of recent messages</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>Unread</td>
 *       <td>number of unread messages</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>Deleted</td>
 *       <td>number of deleted messages</td>
 *      </tr>
 *      <tr valign="top">
 *       <td>Size</td>
 *       <td>mailbox size</td>
 *      </tr>
 *     
 *    
 *   </table>
 *  </p>
 *  <p>
 *   Returns false on failure.
 */
function imap_mailboxmsginfo ($imap_stream) {}

/**
 * Sets flags on messages
 * @link http://www.php.net/manual/en/function.imap-setflag-full.php
 * @param imap_stream resource 
 * @param sequence string <p>
 *       A sequence of message numbers. You can enumerate desired messages
 *       with the X,Y syntax, or retrieve all messages 
 *       within an interval with the X:Y syntax
 *      </p>
 * @param flag string <p>
 *       The flags which you can set are \Seen, 
 *       \Answered, \Flagged,
 *       \Deleted, and \Draft as
 *       defined by RFC2060.
 *      </p>
 * @param options int[optional] <p>
 *       A bit mask that may contain the single option:
 *       
 *        
 *         
 *          ST_UID - The sequence argument contains UIDs
 *          instead of sequence numbers
 * @return bool Returns true on success or false on failure.
 */
function imap_setflag_full ($imap_stream, $sequence, $flag, $options = null) {}

/**
 * Clears flags on messages
 * @link http://www.php.net/manual/en/function.imap-clearflag-full.php
 * @param imap_stream resource 
 * @param sequence string <p>
 *       A sequence of message numbers. You can enumerate desired messages
 *       with the X,Y syntax, or retrieve all messages 
 *       within an interval with the X:Y syntax
 *      </p>
 * @param flag string <p>
 *       The flags which you can unset are "\\Seen", "\\Answered", "\\Flagged",
 *       "\\Deleted", and "\\Draft" (as defined by RFC2060)
 *      </p>
 * @param options int[optional] <p>
 *       options are a bit mask and may contain
 *       the single option:
 *       
 *        
 *         
 *          ST_UID - The sequence argument contains UIDs
 *          instead of sequence numbers
 * @return bool Returns true on success or false on failure.
 */
function imap_clearflag_full ($imap_stream, $sequence, $flag, $options = null) {}

/**
 * Gets and sort messages
 * @link http://www.php.net/manual/en/function.imap-sort.php
 * @param imap_stream resource 
 * @param criteria int <p>
 *       Criteria can be one (and only one) of the following:
 *       
 *        
 *         
 *          SORTDATE - message Date
 * @param reverse int <p>
 *       Set this to 1 for reverse sorting
 *      </p>
 * @param options int[optional] <p>
 *       The options are a bitmask of one or more of the
 *       following:
 *       
 *        
 *         
 *          SE_UID - Return UIDs instead of sequence numbers
 * @param search_criteria string[optional] <p>
 *      </p>
 * @param charset string[optional] <p>
 *      </p>
 * @return array an array of message numbers sorted by the given
 *   parameters.
 */
function imap_sort ($imap_stream, $criteria, $reverse, $options = null, $search_criteria = null, $charset = null) {}

/**
 * This function returns the UID for the given message sequence number
 * @link http://www.php.net/manual/en/function.imap-uid.php
 * @param imap_stream resource 
 * @param msg_number int <p>
 *       The message number.
 *      </p>
 * @return int The UID of the given message.
 */
function imap_uid ($imap_stream, $msg_number) {}

/**
 * Gets the message sequence number for the given UID
 * @link http://www.php.net/manual/en/function.imap-msgno.php
 * @param imap_stream resource 
 * @param uid int <p>
 *       The message UID
 *      </p>
 * @return int the message sequence number for the given 
 *   uid.
 */
function imap_msgno ($imap_stream, $uid) {}

/**
 * Read the list of mailboxes
 * @link http://www.php.net/manual/en/function.imap-list.php
 * @param imap_stream resource 
 * @param ref string <p>
 *       ref should normally be just the server
 *       specification as described in imap_open.
 *      </p>
 * @param pattern string &imap.pattern;
 * @return array an array containing the names of the mailboxes.
 */
function imap_list ($imap_stream, $ref, $pattern) {}

/**
 * List all the subscribed mailboxes
 * @link http://www.php.net/manual/en/function.imap-lsub.php
 * @param imap_stream resource 
 * @param ref string <p>
 *       ref should normally be just the server 
 *       specification as described in imap_open
 *      </p>
 * @param pattern string &imap.pattern;
 * @return array an array of all the subscribed mailboxes.
 */
function imap_lsub ($imap_stream, $ref, $pattern) {}

/**
 * Read an overview of the information in the headers of the given message
 * @link http://www.php.net/manual/en/function.imap-fetch-overview.php
 * @param imap_stream resource 
 * @param sequence string <p>
 *       A message sequence description. You can enumerate desired messages
 *       with the X,Y syntax, or retrieve all messages 
 *       within an interval with the X:Y syntax
 *      </p>
 * @param options int[optional] <p>
 *       sequence will contain a sequence of message
 *       indices or UIDs, if this parameter is set to 
 *       FT_UID. 
 *      </p>
 * @return array an array of objects describing one message header each.
 *   The object will only define a property if it exists. The possible
 *   properties are:
 *   
 *    
 *     
 *      subject - the messages subject
 *     
 *    
 *    
 *     
 *      from - who sent it
 *     
 *    
 *    
 *     
 *      to - recipient
 *     
 *    
 *    
 *     
 *      date - when was it sent
 *     
 *    
 *    
 *     
 *      message_id - Message-ID
 *     
 *    
 *    
 *     
 *      references - is a reference to this message id
 *     
 *    
 *    
 *     
 *      in_reply_to - is a reply to this message id
 *     
 *    
 *    
 *     
 *      size - size in bytes
 *     
 *    
 *    
 *     
 *      uid - UID the message has in the mailbox
 *     
 *    
 *    
 *     
 *      msgno - message sequence number in the mailbox
 *     
 *    
 *    
 *     
 *      recent - this message is flagged as recent
 *     
 *    
 *    
 *     
 *      flagged -  this message is flagged
 *     
 *    
 *    
 *     
 *      answered -  this message is flagged as answered
 *     
 *    
 *    
 *     
 *      deleted -  this message is flagged for deletion
 *     
 *    
 *    
 *     
 *      seen -  this message is flagged as already read
 *     
 *    
 *    
 *     
 *      draft -  this message is flagged as being a draft
 */
function imap_fetch_overview ($imap_stream, $sequence, $options = null) {}

/**
 * Returns all IMAP alert messages that have occurred
 * @link http://www.php.net/manual/en/function.imap-alerts.php
 * @return array an array of all of the IMAP alert messages generated or false if
 *   no alert messages are available.
 */
function imap_alerts () {}

/**
 * Returns all of the IMAP errors that have occured
 * @link http://www.php.net/manual/en/function.imap-errors.php
 * @return array This function returns an array of all of the IMAP error messages
 *   generated since the last imap_errors call,
 *   or the beginning of the page. Returns false if no error messages are
 *   available.
 */
function imap_errors () {}

/**
 * Gets the last IMAP error that occurred during this page request
 * @link http://www.php.net/manual/en/function.imap-last-error.php
 * @return string the full text of the last IMAP error message that occurred on the
 *   current page. Returns false if no error messages are available.
 */
function imap_last_error () {}

/**
 * This function returns an array of messages matching the given search criteria
 * @link http://www.php.net/manual/en/function.imap-search.php
 * @param imap_stream resource 
 * @param criteria string <p>
 *       A string, delimited by spaces, in which the following keywords are
 *       allowed. Any multi-word arguments (e.g.
 *       FROM "joey smith") must be quoted. Results will match
 *       all criteria entries.
 *       
 *        
 *         
 *          ALL - return all messages matching the rest of the criteria
 * @param options int[optional] <p>
 *       Valid values for options are 
 *       SE_UID, which causes the returned array to
 *       contain UIDs instead of messages sequence numbers.
 *      </p>
 * @param charset string[optional] <p>
 *      </p>
 * @return array an array of message numbers or UIDs.
 *  </p>
 *  <p>
 *   Return false if it does not understand the search
 *   criteria or no messages have been found.
 */
function imap_search ($imap_stream, $criteria, $options = null, $charset = null) {}

/**
 * Decodes a modified UTF-7 encoded string
 * @link http://www.php.net/manual/en/function.imap-utf7-decode.php
 * @param text string <p>
 *       A modified UTF-7 encoding string, as defined in RFC 2060, section 5.1.3 (original UTF-7
 *       was defined in RFC1642).
 *      </p>
 * @return string a string that is encoded in ISO-8859-1 and consists of the same
 *   sequence of characters in text, or false
 *   if text contains invalid modified UTF-7 sequence
 *   or text contains a character that is not part of
 *   ISO-8859-1 character set.
 */
function imap_utf7_decode ($text) {}

/**
 * Converts ISO-8859-1 string to modified UTF-7 text
 * @link http://www.php.net/manual/en/function.imap-utf7-encode.php
 * @param data string <p>
 *       An ISO-8859-1 string.
 *      </p>
 * @return string data encoded with the modified UTF-7
 *   encoding as defined in RFC 2060, 
 *   section 5.1.3 (original UTF-7 was defined in RFC1642).
 */
function imap_utf7_encode ($data) {}

/**
 * @param in
 */
function imap_utf8_to_mutf7 ($in) {}

/**
 * @param in
 */
function imap_mutf7_to_utf8 ($in) {}

/**
 * Decode MIME header elements
 * @link http://www.php.net/manual/en/function.imap-mime-header-decode.php
 * @param text string <p>
 *       The MIME text
 *      </p>
 * @return array The decoded elements are returned in an array of objects, where each
 *   object has two properties, charset and 
 *   text.
 *  </p>
 *  <p>
 *   If the element hasn't been encoded, and in other words is in
 *   plain US-ASCII, the charset property of that element is
 *   set to default.
 */
function imap_mime_header_decode ($text) {}

/**
 * Returns a tree of threaded message
 * @link http://www.php.net/manual/en/function.imap-thread.php
 * @param imap_stream resource 
 * @param options int[optional] <p>
 *      </p>
 * @return array imap_thread returns an associative array containing
 *   a tree of messages threaded by REFERENCES, or false
 *   on error.
 *  </p>
 *  <p>
 *   Every message in the current mailbox will be represented by three entries
 *   in the resulting array:
 *   
 *    <p>
 *     $thread["XX.num"] - current message number
 *    </p>
 *    <p>
 *     $thread["XX.next"]
 *    </p>
 *    <p>
 *     $thread["XX.branch"]
 *    </p>
 */
function imap_thread ($imap_stream, $options = null) {}

/**
 * Set or fetch imap timeout
 * @link http://www.php.net/manual/en/function.imap-timeout.php
 * @param timeout_type int <p>
 *       One of the following:
 *       IMAP_OPENTIMEOUT,
 *       IMAP_READTIMEOUT,
 *       IMAP_WRITETIMEOUT, or
 *       IMAP_CLOSETIMEOUT.
 *      </p>
 * @param timeout int[optional] <p>
 *       The timeout, in seconds.
 *      </p>
 * @return mixed If the timeout parameter is set, this function
 *   returns true on success and false on failure.
 *  </p>
 *  <p>
 *   If timeout  is not provided or evaluates to -1,
 *   the current timeout value of timeout_type is
 *   returned as an integer.
 */
function imap_timeout ($timeout_type, $timeout = null) {}

/**
 * Retrieve the quota level settings, and usage statics per mailbox
 * @link http://www.php.net/manual/en/function.imap-get-quota.php
 * @param imap_stream resource 
 * @param quota_root string <p>
 *       quota_root should normally be in the form of
 *       user.name where name is the mailbox you wish to
 *       retrieve information about.
 *      </p>
 * @return array an array with integer values limit and usage for the given
 *   mailbox.  The value of limit represents the total amount of space
 *   allowed for this mailbox.  The usage value represents the mailboxes
 *   current level of capacity.  Will return false in the case of failure.
 *  </p>
 *  <p>
 *   As of PHP 4.3, the function more properly reflects the
 *   functionality as dictated by the RFC2087.
 *   The array return value has changed to support an unlimited number of returned 
 *   resources (i.e.  messages, or sub-folders) with each named resource receiving
 *   an individual array key.  Each key value then contains an another array with
 *   the usage and limit values within it.
 *  </p>
 *  <p>
 *   For backwards compatibility reasons, the original access methods are
 *   still available for use, although it is suggested to update.
 */
function imap_get_quota ($imap_stream, $quota_root) {}

/**
 * Retrieve the quota settings per user
 * @link http://www.php.net/manual/en/function.imap-get-quotaroot.php
 * @param imap_stream resource 
 * @param quota_root string <p>
 *       quota_root should normally be in the form of
 *       which mailbox (i.e. INBOX).
 *      </p>
 * @return array an array of integer values pertaining to the specified user
 *   mailbox.  All values contain a key based upon the resource name, and a
 *   corresponding array with the usage and limit values within.
 *  </p>
 *  <p>
 *   This function will return false in the case of call failure, and an
 *   array of information about the connection upon an un-parsable response
 *   from the server.
 */
function imap_get_quotaroot ($imap_stream, $quota_root) {}

/**
 * Sets a quota for a given mailbox
 * @link http://www.php.net/manual/en/function.imap-set-quota.php
 * @param imap_stream resource 
 * @param quota_root string <p>
 *       The mailbox to have a quota set. This should follow the IMAP standard
 *       format for a mailbox: user.name.
 *      </p>
 * @param quota_limit int <p>
 *       The maximum size (in KB) for the quota_root
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imap_set_quota ($imap_stream, $quota_root, $quota_limit) {}

/**
 * Sets the ACL for a giving mailbox
 * @link http://www.php.net/manual/en/function.imap-setacl.php
 * @param imap_stream resource 
 * @param mailbox string <p>
 *       The mailbox name, see imap_open for more
 *       information
 *      </p>
 * @param id string <p>
 *       The user to give the rights to.
 *      </p>
 * @param rights string <p>
 *       The rights to give to the user. Passing an empty string will delete
 *       acl.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imap_setacl ($imap_stream, $mailbox, $id, $rights) {}

/**
 * Gets the ACL for a given mailbox
 * @link http://www.php.net/manual/en/function.imap-getacl.php
 * @param imap_stream resource 
 * @param mailbox string <p>
 *       The mailbox name, see imap_open for more
 *       information
 *      </p>
 * @return array an associative array of "folder" => "acl" pairs.
 */
function imap_getacl ($imap_stream, $mailbox) {}

/**
 * Send an email message
 * @link http://www.php.net/manual/en/function.imap-mail.php
 * @param to string <p>
 *       The receiver
 *      </p>
 * @param subject string <p>
 *       The mail subject
 *      </p>
 * @param message string <p>
 *       The mail body
 *      </p>
 * @param additional_headers string[optional] <p>
 *       As string with additional headers to be set on the mail
 *      </p>
 * @param cc string[optional] <p>
 *      </p>
 * @param bcc string[optional] <p>
 *       The receivers specified in bcc will get the
 *       mail, but are excluded from the headers.
 *      </p>
 * @param rpath string[optional] <p>
 *       Use this parameter to specify return path upon mail delivery failure.
 *       This is useful when using PHP as a mail client for multiple users.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function imap_mail ($to, $subject, $message, $additional_headers = null, $cc = null, $bcc = null, $rpath = null) {}

/**
 * &Alias; <function>imap_headerinfo</function>
 * @link http://www.php.net/manual/en/function.imap-header.php
 * @param stream_id
 * @param msg_no
 * @param from_length[optional]
 * @param subject_length[optional]
 * @param default_host[optional]
 */
function imap_header ($stream_id, $msg_no, $from_length, $subject_length, $default_host) {}

/**
 * &Alias; <function>imap_list</function>
 * @link http://www.php.net/manual/en/function.imap-listmailbox.php
 * @param stream_id
 * @param ref
 * @param pattern
 */
function imap_listmailbox ($stream_id, $ref, $pattern) {}

/**
 * Read the list of mailboxes, returning detailed information on each one
 * @link http://www.php.net/manual/en/function.imap-getmailboxes.php
 * @param imap_stream resource 
 * @param ref string <p>
 *       ref should normally be just the server
 *       specification as described in imap_open
 *      </p>
 * @param pattern string &imap.pattern;
 * @return array an array of objects containing mailbox information. Each
 *   object has the attributes name, specifying
 *   the full name of the mailbox; delimiter,
 *   which is the hierarchy delimiter for the part of the hierarchy
 *   this mailbox is in; and
 *   attributes. Attributes
 *   is a bitmask that can be tested against:
 *   
 *    
 *     <p>
 *      LATT_NOINFERIORS - This mailbox contains, and may not contain any
 *      "children" (there are no mailboxes below this one). Calling 
 *      imap_createmailbox will not work on this mailbox.
 *     </p>
 *    
 *   
 *     <p>
 *      LATT_NOSELECT - This is only a container,
 *      not a mailbox - you cannot open it.
 *     </p>
 *    
 *    
 *     <p>
 *      LATT_MARKED - This mailbox is marked. This means that it may 
 *      contain new messages since the last time it was checked. Not provided by all IMAP
 *      servers.
 *     </p>
 *    
 *    
 *     <p>
 *      LATT_UNMARKED - This mailbox is not marked, does not contain new
 *      messages.  If either MARKED or UNMARKED is
 *      provided, you can assume the IMAP server  supports this feature for this mailbox.
 *     </p>
 */
function imap_getmailboxes ($imap_stream, $ref, $pattern) {}

/**
 * &Alias; <function>imap_listscan</function>
 * @link http://www.php.net/manual/en/function.imap-scanmailbox.php
 * @param stream_id
 * @param ref
 * @param pattern
 * @param content
 */
function imap_scanmailbox ($stream_id, $ref, $pattern, $content) {}

/**
 * &Alias; <function>imap_lsub</function>
 * @link http://www.php.net/manual/en/function.imap-listsubscribed.php
 * @param stream_id
 * @param ref
 * @param pattern
 */
function imap_listsubscribed ($stream_id, $ref, $pattern) {}

/**
 * List all the subscribed mailboxes
 * @link http://www.php.net/manual/en/function.imap-getsubscribed.php
 * @param imap_stream resource 
 * @param ref string <p>
 *       ref should normally be just the server
 *       specification as described in imap_open
 *      </p>
 * @param pattern string &imap.pattern;
 * @return array an array of objects containing mailbox information. Each
 *   object has the attributes name, specifying
 *   the full name of the mailbox; delimiter,
 *   which is the hierarchy delimiter for the part of the hierarchy
 *   this mailbox is in; and
 *   attributes. Attributes
 *   is a bitmask that can be tested against:
 *   
 *    
 *     
 *      LATT_NOINFERIORS - This mailbox has no
 *      "children" (there are no mailboxes below this one).
 *     
 *    
 *   
 *     
 *      LATT_NOSELECT - This is only a container,
 *      not a mailbox - you cannot open it.
 *     
 *    
 *    
 *     
 *      LATT_MARKED - This mailbox is marked.
 *      Only used by UW-IMAPD.
 *     
 *    
 *    
 *     
 *      LATT_UNMARKED - This mailbox is not marked.
 *      Only used by UW-IMAPD.
 */
function imap_getsubscribed ($imap_stream, $ref, $pattern) {}

/**
 * &Alias; <function>imap_body</function>
 * @link http://www.php.net/manual/en/function.imap-fetchtext.php
 * @param stream_id
 * @param msg_no
 * @param options[optional]
 */
function imap_fetchtext ($stream_id, $msg_no, $options) {}

/**
 * &Alias; <function>imap_listscan</function>
 * @link http://www.php.net/manual/en/function.imap-scan.php
 * @param stream_id
 * @param ref
 * @param pattern
 * @param content
 */
function imap_scan ($stream_id, $ref, $pattern, $content) {}

/**
 * &Alias; <function>imap_createmailbox</function>
 * @link http://www.php.net/manual/en/function.imap-create.php
 * @param stream_id
 * @param mailbox
 */
function imap_create ($stream_id, $mailbox) {}

/**
 * &Alias; <function>imap_renamemailbox</function>
 * @link http://www.php.net/manual/en/function.imap-rename.php
 * @param stream_id
 * @param old_name
 * @param new_name
 */
function imap_rename ($stream_id, $old_name, $new_name) {}

define ('NIL', 0);
define ('IMAP_OPENTIMEOUT', 1);
define ('IMAP_READTIMEOUT', 2);
define ('IMAP_WRITETIMEOUT', 3);
define ('IMAP_CLOSETIMEOUT', 4);
define ('OP_DEBUG', 1);

/**
 * Open mailbox read-only
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('OP_READONLY', 2);

/**
 * Don't use or update a .newsrc for news 
 *     (NNTP only)
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('OP_ANONYMOUS', 4);
define ('OP_SHORTCACHE', 8);
define ('OP_SILENT', 16);
define ('OP_PROTOTYPE', 32);

/**
 * For IMAP and NNTP
 *     names, open a connection but don't open a mailbox.
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('OP_HALFOPEN', 64);
define ('OP_EXPUNGE', 128);
define ('OP_SECURE', 256);

/**
 * silently expunge the mailbox before closing when
 *     calling imap_close
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('CL_EXPUNGE', 32768);

/**
 * The parameter is a UID
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('FT_UID', 1);

/**
 * Do not set the \Seen flag if not already set
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('FT_PEEK', 2);
define ('FT_NOT', 4);

/**
 * The return string is in internal format, will not canonicalize to CRLF.
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('FT_INTERNAL', 8);
define ('FT_PREFETCHTEXT', 32);

/**
 * The sequence argument contains UIDs instead of sequence numbers
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('ST_UID', 1);
define ('ST_SILENT', 2);
define ('ST_SET', 4);

/**
 * the sequence numbers contain UIDS
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('CP_UID', 1);

/**
 * Delete the messages from the current mailbox after copying
 *     with imap_mail_copy
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('CP_MOVE', 2);

/**
 * Return UIDs instead of sequence numbers
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('SE_UID', 1);
define ('SE_FREE', 2);

/**
 * Don't prefetch searched messages
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('SE_NOPREFETCH', 4);
define ('SO_FREE', 8);
define ('SO_NOSERVER', 16);
define ('SA_MESSAGES', 1);
define ('SA_RECENT', 2);
define ('SA_UNSEEN', 4);
define ('SA_UIDNEXT', 8);
define ('SA_UIDVALIDITY', 16);
define ('SA_ALL', 31);

/**
 * This mailbox has no "children" (there are no
 *     mailboxes below this one).
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('LATT_NOINFERIORS', 1);

/**
 * This is only a container, not a mailbox - you
 *     cannot open it.
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('LATT_NOSELECT', 2);

/**
 * This mailbox is marked. Only used by UW-IMAPD.
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('LATT_MARKED', 4);

/**
 * This mailbox is not marked. Only used by
 *     UW-IMAPD.
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('LATT_UNMARKED', 8);
define ('LATT_REFERRAL', 16);
define ('LATT_HASCHILDREN', 32);
define ('LATT_HASNOCHILDREN', 64);

/**
 * Sort criteria for imap_sort:
 *     message Date
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('SORTDATE', 0);

/**
 * Sort criteria for imap_sort:
 *     arrival date
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('SORTARRIVAL', 1);

/**
 * Sort criteria for imap_sort:
 *     mailbox in first From address
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('SORTFROM', 2);

/**
 * Sort criteria for imap_sort:
 *     message subject
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('SORTSUBJECT', 3);

/**
 * Sort criteria for imap_sort:
 *     mailbox in first To address
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('SORTTO', 4);

/**
 * Sort criteria for imap_sort:
 *     mailbox in first cc address
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('SORTCC', 5);

/**
 * Sort criteria for imap_sort:
 *     size of message in octets
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('SORTSIZE', 6);
define ('TYPETEXT', 0);
define ('TYPEMULTIPART', 1);
define ('TYPEMESSAGE', 2);
define ('TYPEAPPLICATION', 3);
define ('TYPEAUDIO', 4);
define ('TYPEIMAGE', 5);
define ('TYPEVIDEO', 6);
define ('TYPEMODEL', 7);
define ('TYPEOTHER', 8);
define ('ENC7BIT', 0);
define ('ENC8BIT', 1);
define ('ENCBINARY', 2);
define ('ENCBASE64', 3);
define ('ENCQUOTEDPRINTABLE', 4);
define ('ENCOTHER', 5);

/**
 * Garbage collector, clear message cache elements.
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('IMAP_GC_ELT', 1);

/**
 * Garbage collector, clear envelopes and bodies.
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('IMAP_GC_ENV', 2);

/**
 * Garbage collector, clear texts.
 * @link http://www.php.net/manual/en/imap.constants.php
 */
define ('IMAP_GC_TEXTS', 4);

// End of imap v.
?>
