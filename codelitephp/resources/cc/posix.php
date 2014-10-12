<?php

// Start of posix v.

/**
 * @param pid
 * @param sig
 */
function posix_kill ($pid, $sig) {}

function posix_getpid () {}

function posix_getppid () {}

function posix_getuid () {}

/**
 * @param uid
 */
function posix_setuid ($uid) {}

function posix_geteuid () {}

/**
 * @param uid
 */
function posix_seteuid ($uid) {}

function posix_getgid () {}

/**
 * @param gid
 */
function posix_setgid ($gid) {}

function posix_getegid () {}

/**
 * @param gid
 */
function posix_setegid ($gid) {}

function posix_getgroups () {}

function posix_getlogin () {}

function posix_getpgrp () {}

function posix_setsid () {}

/**
 * @param pid
 * @param pgid
 */
function posix_setpgid ($pid, $pgid) {}

/**
 * @param pid
 */
function posix_getpgid ($pid) {}

/**
 * @param pid
 */
function posix_getsid ($pid) {}

function posix_uname () {}

function posix_times () {}

function posix_ctermid () {}

/**
 * @param fd
 */
function posix_ttyname ($fd) {}

/**
 * @param fd
 */
function posix_isatty ($fd) {}

function posix_getcwd () {}

/**
 * @param pathname
 * @param mode
 */
function posix_mkfifo ($pathname, $mode) {}

/**
 * @param pathname
 * @param mode
 * @param major[optional]
 * @param minor[optional]
 */
function posix_mknod ($pathname, $mode, $major, $minor) {}

/**
 * @param file
 * @param mode[optional]
 */
function posix_access ($file, $mode) {}

/**
 * @param name
 */
function posix_getgrnam ($name) {}

/**
 * @param gid
 */
function posix_getgrgid ($gid) {}

/**
 * @param username
 */
function posix_getpwnam ($username) {}

/**
 * @param uid
 */
function posix_getpwuid ($uid) {}

function posix_getrlimit () {}

function posix_get_last_error () {}

function posix_errno () {}

/**
 * @param errno
 */
function posix_strerror ($errno) {}

/**
 * @param name
 * @param base_group_id
 */
function posix_initgroups ($name, $base_group_id) {}


/**
 * Check whether the file exists.
 * @link http://www.php.net/manual/en/posix.constants.php
 */
define ('POSIX_F_OK', 0);

/**
 * Check whether the file exists and has execute permissions.
 * @link http://www.php.net/manual/en/posix.constants.php
 */
define ('POSIX_X_OK', 1);

/**
 * Check whether the file exists and has write permissions.
 * @link http://www.php.net/manual/en/posix.constants.php
 */
define ('POSIX_W_OK', 2);

/**
 * Check whether the file exists and has read permissions.
 * @link http://www.php.net/manual/en/posix.constants.php
 */
define ('POSIX_R_OK', 4);

/**
 * Normal file
 * @link http://www.php.net/manual/en/posix.constants.php
 */
define ('POSIX_S_IFREG', 32768);

/**
 * Character special file
 * @link http://www.php.net/manual/en/posix.constants.php
 */
define ('POSIX_S_IFCHR', 8192);

/**
 * Block special file
 * @link http://www.php.net/manual/en/posix.constants.php
 */
define ('POSIX_S_IFBLK', 24576);

/**
 * FIFO (named pipe) special file
 * @link http://www.php.net/manual/en/posix.constants.php
 */
define ('POSIX_S_IFIFO', 4096);

/**
 * Socket
 * @link http://www.php.net/manual/en/posix.constants.php
 */
define ('POSIX_S_IFSOCK', 49152);

// End of posix v.
?>
