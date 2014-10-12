<?php

// Start of imap v.

/**
 * @param mailbox
 * @param user
 * @param password
 * @param options[optional]
 * @param n_retries[optional]
 * @param params[optional]
 */
function imap_open ($mailbox, $user, $password, $options, $n_retries, $params) {}

/**
 * @param stream_id
 * @param mailbox
 * @param options[optional]
 * @param n_retries[optional]
 */
function imap_reopen ($stream_id, $mailbox, $options, $n_retries) {}

/**
 * @param stream_id
 * @param options[optional]
 */
function imap_close ($stream_id, $options) {}

/**
 * @param stream_id
 */
function imap_num_msg ($stream_id) {}

/**
 * @param stream_id
 */
function imap_num_recent ($stream_id) {}

/**
 * @param stream_id
 */
function imap_headers ($stream_id) {}

/**
 * @param stream_id
 * @param msg_no
 * @param from_length[optional]
 * @param subject_length[optional]
 * @param default_host[optional]
 */
function imap_headerinfo ($stream_id, $msg_no, $from_length, $subject_length, $default_host) {}

/**
 * @param headers
 * @param default_host[optional]
 */
function imap_rfc822_parse_headers ($headers, $default_host) {}

/**
 * @param mailbox
 * @param host
 * @param personal
 */
function imap_rfc822_write_address ($mailbox, $host, $personal) {}

/**
 * @param address_string
 * @param default_host
 */
function imap_rfc822_parse_adrlist ($address_string, $default_host) {}

/**
 * @param stream_id
 * @param msg_no
 * @param options[optional]
 */
function imap_body ($stream_id, $msg_no, $options) {}

/**
 * @param stream_id
 * @param msg_no
 * @param section
 */
function imap_bodystruct ($stream_id, $msg_no, $section) {}

/**
 * @param stream_id
 * @param msg_no
 * @param section
 * @param options[optional]
 */
function imap_fetchbody ($stream_id, $msg_no, $section, $options) {}

/**
 * @param stream_id
 * @param msg_no
 * @param section
 * @param options[optional]
 */
function imap_fetchmime ($stream_id, $msg_no, $section, $options) {}

/**
 * @param stream_id
 * @param file
 * @param msg_no
 * @param section[optional]
 * @param options[optional]
 */
function imap_savebody ($stream_id, $file, $msg_no, $section, $options) {}

/**
 * @param stream_id
 * @param msg_no
 * @param options[optional]
 */
function imap_fetchheader ($stream_id, $msg_no, $options) {}

/**
 * @param stream_id
 * @param msg_no
 * @param options[optional]
 */
function imap_fetchstructure ($stream_id, $msg_no, $options) {}

/**
 * @param stream_id
 * @param flags[optional]
 */
function imap_gc ($stream_id, $flags) {}

/**
 * @param stream_id
 */
function imap_expunge ($stream_id) {}

/**
 * @param stream_id
 * @param msg_no
 * @param options[optional]
 */
function imap_delete ($stream_id, $msg_no, $options) {}

/**
 * @param stream_id
 * @param msg_no
 * @param flags[optional]
 */
function imap_undelete ($stream_id, $msg_no, $flags) {}

/**
 * @param stream_id
 */
function imap_check ($stream_id) {}

/**
 * @param stream_id
 * @param ref
 * @param pattern
 * @param content
 */
function imap_listscan ($stream_id, $ref, $pattern, $content) {}

/**
 * @param stream_id
 * @param msglist
 * @param mailbox
 * @param options[optional]
 */
function imap_mail_copy ($stream_id, $msglist, $mailbox, $options) {}

/**
 * @param stream_id
 * @param sequence
 * @param mailbox
 * @param options[optional]
 */
function imap_mail_move ($stream_id, $sequence, $mailbox, $options) {}

/**
 * @param envelope
 * @param body
 */
function imap_mail_compose ($envelope, $body) {}

/**
 * @param stream_id
 * @param mailbox
 */
function imap_createmailbox ($stream_id, $mailbox) {}

/**
 * @param stream_id
 * @param old_name
 * @param new_name
 */
function imap_renamemailbox ($stream_id, $old_name, $new_name) {}

/**
 * @param stream_id
 * @param mailbox
 */
function imap_deletemailbox ($stream_id, $mailbox) {}

/**
 * @param stream_id
 * @param mailbox
 */
function imap_subscribe ($stream_id, $mailbox) {}

/**
 * @param stream_id
 * @param mailbox
 */
function imap_unsubscribe ($stream_id, $mailbox) {}

/**
 * @param stream_id
 * @param folder
 * @param message
 * @param options[optional]
 * @param date[optional]
 */
function imap_append ($stream_id, $folder, $message, $options, $date) {}

/**
 * @param stream_id
 */
function imap_ping ($stream_id) {}

/**
 * @param text
 */
function imap_base64 ($text) {}

/**
 * @param text
 */
function imap_qprint ($text) {}

/**
 * @param text
 */
function imap_8bit ($text) {}

/**
 * @param text
 */
function imap_binary ($text) {}

/**
 * @param mime_encoded_text
 */
function imap_utf8 ($mime_encoded_text) {}

/**
 * @param stream_id
 * @param mailbox
 * @param options
 */
function imap_status ($stream_id, $mailbox, $options) {}

/**
 * @param stream_id
 */
function imap_mailboxmsginfo ($stream_id) {}

/**
 * @param stream_id
 * @param sequence
 * @param flag
 * @param options[optional]
 */
function imap_setflag_full ($stream_id, $sequence, $flag, $options) {}

/**
 * @param stream_id
 * @param sequence
 * @param flag
 * @param options[optional]
 */
function imap_clearflag_full ($stream_id, $sequence, $flag, $options) {}

/**
 * @param stream_id
 * @param criteria
 * @param reverse
 * @param options[optional]
 * @param search_criteria[optional]
 * @param charset[optional]
 */
function imap_sort ($stream_id, $criteria, $reverse, $options, $search_criteria, $charset) {}

/**
 * @param stream_id
 * @param msg_no
 */
function imap_uid ($stream_id, $msg_no) {}

/**
 * @param stream_id
 * @param unique_msg_id
 */
function imap_msgno ($stream_id, $unique_msg_id) {}

/**
 * @param stream_id
 * @param ref
 * @param pattern
 */
function imap_list ($stream_id, $ref, $pattern) {}

/**
 * @param stream_id
 * @param ref
 * @param pattern
 */
function imap_lsub ($stream_id, $ref, $pattern) {}

/**
 * @param stream_id
 * @param sequence
 * @param options[optional]
 */
function imap_fetch_overview ($stream_id, $sequence, $options) {}

function imap_alerts () {}

function imap_errors () {}

function imap_last_error () {}

/**
 * @param stream_id
 * @param criteria
 * @param options[optional]
 * @param charset[optional]
 */
function imap_search ($stream_id, $criteria, $options, $charset) {}

/**
 * @param buf
 */
function imap_utf7_decode ($buf) {}

/**
 * @param buf
 */
function imap_utf7_encode ($buf) {}

/**
 * @param str
 */
function imap_mime_header_decode ($str) {}

/**
 * @param stream_id
 * @param options[optional]
 */
function imap_thread ($stream_id, $options) {}

/**
 * @param timeout_type
 * @param timeout[optional]
 */
function imap_timeout ($timeout_type, $timeout) {}

/**
 * @param stream_id
 * @param qroot
 */
function imap_get_quota ($stream_id, $qroot) {}

/**
 * @param stream_id
 * @param mbox
 */
function imap_get_quotaroot ($stream_id, $mbox) {}

/**
 * @param stream_id
 * @param qroot
 * @param mailbox_size
 */
function imap_set_quota ($stream_id, $qroot, $mailbox_size) {}

/**
 * @param stream_id
 * @param mailbox
 * @param id
 * @param rights
 */
function imap_setacl ($stream_id, $mailbox, $id, $rights) {}

/**
 * @param stream_id
 * @param mailbox
 */
function imap_getacl ($stream_id, $mailbox) {}

/**
 * @param to
 * @param subject
 * @param message
 * @param additional_headers[optional]
 * @param cc[optional]
 * @param bcc[optional]
 * @param rpath[optional]
 */
function imap_mail ($to, $subject, $message, $additional_headers, $cc, $bcc, $rpath) {}

/**
 * @param stream_id
 * @param msg_no
 * @param from_length[optional]
 * @param subject_length[optional]
 * @param default_host[optional]
 */
function imap_header ($stream_id, $msg_no, $from_length, $subject_length, $default_host) {}

/**
 * @param stream_id
 * @param ref
 * @param pattern
 */
function imap_listmailbox ($stream_id, $ref, $pattern) {}

/**
 * @param stream_id
 * @param ref
 * @param pattern
 */
function imap_getmailboxes ($stream_id, $ref, $pattern) {}

/**
 * @param stream_id
 * @param ref
 * @param pattern
 * @param content
 */
function imap_scanmailbox ($stream_id, $ref, $pattern, $content) {}

/**
 * @param stream_id
 * @param ref
 * @param pattern
 */
function imap_listsubscribed ($stream_id, $ref, $pattern) {}

/**
 * @param stream_id
 * @param ref
 * @param pattern
 */
function imap_getsubscribed ($stream_id, $ref, $pattern) {}

/**
 * @param stream_id
 * @param msg_no
 * @param options[optional]
 */
function imap_fetchtext ($stream_id, $msg_no, $options) {}

/**
 * @param stream_id
 * @param ref
 * @param pattern
 * @param content
 */
function imap_scan ($stream_id, $ref, $pattern, $content) {}

/**
 * @param stream_id
 * @param mailbox
 */
function imap_create ($stream_id, $mailbox) {}

/**
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
