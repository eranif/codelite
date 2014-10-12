<?php

// Start of ftp v.

/**
 * @param host
 * @param port[optional]
 * @param timeout[optional]
 */
function ftp_connect ($host, $port, $timeout) {}

/**
 * @param host
 * @param port[optional]
 * @param timeout[optional]
 */
function ftp_ssl_connect ($host, $port, $timeout) {}

/**
 * @param ftp
 * @param username
 * @param password
 */
function ftp_login ($ftp, $username, $password) {}

/**
 * @param ftp
 */
function ftp_pwd ($ftp) {}

/**
 * @param ftp
 */
function ftp_cdup ($ftp) {}

/**
 * @param ftp
 * @param directory
 */
function ftp_chdir ($ftp, $directory) {}

/**
 * @param ftp
 * @param command
 */
function ftp_exec ($ftp, $command) {}

/**
 * @param ftp
 * @param command
 */
function ftp_raw ($ftp, $command) {}

/**
 * @param ftp
 * @param directory
 */
function ftp_mkdir ($ftp, $directory) {}

/**
 * @param ftp
 * @param directory
 */
function ftp_rmdir ($ftp, $directory) {}

/**
 * @param ftp
 * @param mode
 * @param filename
 */
function ftp_chmod ($ftp, $mode, $filename) {}

/**
 * @param ftp
 * @param size
 * @param response[optional]
 */
function ftp_alloc ($ftp, $size, &$response) {}

/**
 * @param ftp
 * @param directory
 */
function ftp_nlist ($ftp, $directory) {}

/**
 * @param ftp
 * @param directory
 * @param recursive[optional]
 */
function ftp_rawlist ($ftp, $directory, $recursive) {}

/**
 * @param ftp
 */
function ftp_systype ($ftp) {}

/**
 * @param ftp
 * @param pasv
 */
function ftp_pasv ($ftp, $pasv) {}

/**
 * @param ftp
 * @param local_file
 * @param remote_file
 * @param mode
 * @param resume_pos[optional]
 */
function ftp_get ($ftp, $local_file, $remote_file, $mode, $resume_pos) {}

/**
 * @param ftp
 * @param fp
 * @param remote_file
 * @param mode
 * @param resumepos[optional]
 */
function ftp_fget ($ftp, $fp, $remote_file, $mode, $resumepos) {}

/**
 * @param ftp
 * @param remote_file
 * @param local_file
 * @param mode
 * @param startpos[optional]
 */
function ftp_put ($ftp, $remote_file, $local_file, $mode, $startpos) {}

/**
 * @param ftp
 * @param remote_file
 * @param fp
 * @param mode
 * @param startpos[optional]
 */
function ftp_fput ($ftp, $remote_file, $fp, $mode, $startpos) {}

/**
 * @param ftp
 * @param filename
 */
function ftp_size ($ftp, $filename) {}

/**
 * @param ftp
 * @param filename
 */
function ftp_mdtm ($ftp, $filename) {}

/**
 * @param ftp
 * @param src
 * @param dest
 */
function ftp_rename ($ftp, $src, $dest) {}

/**
 * @param ftp
 * @param file
 */
function ftp_delete ($ftp, $file) {}

/**
 * @param ftp
 * @param cmd
 */
function ftp_site ($ftp, $cmd) {}

/**
 * @param ftp
 */
function ftp_close ($ftp) {}

/**
 * @param ftp
 * @param option
 * @param value
 */
function ftp_set_option ($ftp, $option, $value) {}

/**
 * @param ftp
 * @param option
 */
function ftp_get_option ($ftp, $option) {}

/**
 * @param ftp
 * @param fp
 * @param remote_file
 * @param mode
 * @param resumepos[optional]
 */
function ftp_nb_fget ($ftp, $fp, $remote_file, $mode, $resumepos) {}

/**
 * @param ftp
 * @param local_file
 * @param remote_file
 * @param mode
 * @param resume_pos[optional]
 */
function ftp_nb_get ($ftp, $local_file, $remote_file, $mode, $resume_pos) {}

/**
 * @param ftp
 */
function ftp_nb_continue ($ftp) {}

/**
 * @param ftp
 * @param remote_file
 * @param local_file
 * @param mode
 * @param startpos[optional]
 */
function ftp_nb_put ($ftp, $remote_file, $local_file, $mode, $startpos) {}

/**
 * @param ftp
 * @param remote_file
 * @param fp
 * @param mode
 * @param startpos[optional]
 */
function ftp_nb_fput ($ftp, $remote_file, $fp, $mode, $startpos) {}

/**
 * @param ftp
 */
function ftp_quit ($ftp) {}


/**
 * <p></p>
 * @link http://www.php.net/manual/en/ftp.constants.php
 */
define ('FTP_ASCII', 1);

/**
 * <p></p>
 * @link http://www.php.net/manual/en/ftp.constants.php
 */
define ('FTP_TEXT', 1);

/**
 * <p></p>
 * @link http://www.php.net/manual/en/ftp.constants.php
 */
define ('FTP_BINARY', 2);

/**
 * <p></p>
 * @link http://www.php.net/manual/en/ftp.constants.php
 */
define ('FTP_IMAGE', 2);

/**
 * <p>
 *     Automatically determine resume position and start position for GET and PUT requests
 *     (only works if FTP_AUTOSEEK is enabled)
 *    </p>
 * @link http://www.php.net/manual/en/ftp.constants.php
 */
define ('FTP_AUTORESUME', -1);

/**
 * <p>
 *     See ftp_set_option for information.
 *    </p>
 * @link http://www.php.net/manual/en/ftp.constants.php
 */
define ('FTP_TIMEOUT_SEC', 0);

/**
 * <p>
 *     See ftp_set_option for information.
 *    </p>
 * @link http://www.php.net/manual/en/ftp.constants.php
 */
define ('FTP_AUTOSEEK', 1);

/**
 * <p>
 *     Asynchronous transfer has failed
 *    </p>
 * @link http://www.php.net/manual/en/ftp.constants.php
 */
define ('FTP_FAILED', 0);

/**
 * <p>
 *     Asynchronous transfer has finished
 *    </p>
 * @link http://www.php.net/manual/en/ftp.constants.php
 */
define ('FTP_FINISHED', 1);

/**
 * <p>
 *     Asynchronous transfer is still active
 *    </p>
 * @link http://www.php.net/manual/en/ftp.constants.php
 */
define ('FTP_MOREDATA', 2);

// End of ftp v.
?>
