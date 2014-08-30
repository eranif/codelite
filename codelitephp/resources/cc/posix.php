<?php

// Start of posix v.

/**
 * Send a signal to a process
 * @link http://www.php.net/manual/en/function.posix-kill.php
 * @param pid int <p>
 *       The process identifier.
 *      </p>
 * @param sig int <p>
 *       One of the PCNTL signals constants.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function posix_kill ($pid, $sig) {}

/**
 * Return the current process identifier
 * @link http://www.php.net/manual/en/function.posix-getpid.php
 * @return int the identifier, as an integer.
 */
function posix_getpid () {}

/**
 * Return the parent process identifier
 * @link http://www.php.net/manual/en/function.posix-getppid.php
 * @return int the identifier, as an integer.
 */
function posix_getppid () {}

/**
 * Return the real user ID of the current process
 * @link http://www.php.net/manual/en/function.posix-getuid.php
 * @return int the user id, as an integer
 */
function posix_getuid () {}

/**
 * Set the UID of the current process
 * @link http://www.php.net/manual/en/function.posix-setuid.php
 * @param uid int <p>
 *       The user id.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function posix_setuid ($uid) {}

/**
 * Return the effective user ID of the current process
 * @link http://www.php.net/manual/en/function.posix-geteuid.php
 * @return int the user id, as an integer
 */
function posix_geteuid () {}

/**
 * Set the effective UID of the current process
 * @link http://www.php.net/manual/en/function.posix-seteuid.php
 * @param uid int <p>
 *       The user id.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function posix_seteuid ($uid) {}

/**
 * Return the real group ID of the current process
 * @link http://www.php.net/manual/en/function.posix-getgid.php
 * @return int the real group id, as an integer.
 */
function posix_getgid () {}

/**
 * Set the GID of the current process
 * @link http://www.php.net/manual/en/function.posix-setgid.php
 * @param gid int <p>
 *       The group id.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function posix_setgid ($gid) {}

/**
 * Return the effective group ID of the current process
 * @link http://www.php.net/manual/en/function.posix-getegid.php
 * @return int an integer of the effective group ID.
 */
function posix_getegid () {}

/**
 * Set the effective GID of the current process
 * @link http://www.php.net/manual/en/function.posix-setegid.php
 * @param gid int <p>
 *       The group id.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function posix_setegid ($gid) {}

/**
 * Return the group set of the current process
 * @link http://www.php.net/manual/en/function.posix-getgroups.php
 * @return array an array of integers containing the numeric group ids of the group
 *   set of the current process.
 */
function posix_getgroups () {}

/**
 * Return login name
 * @link http://www.php.net/manual/en/function.posix-getlogin.php
 * @return string the login name of the user, as a string.
 */
function posix_getlogin () {}

/**
 * Return the current process group identifier
 * @link http://www.php.net/manual/en/function.posix-getpgrp.php
 * @return int the identifier, as an integer.
 */
function posix_getpgrp () {}

/**
 * Make the current process a session leader
 * @link http://www.php.net/manual/en/function.posix-setsid.php
 * @return int the session id, or -1 on errors.
 */
function posix_setsid () {}

/**
 * Set process group id for job control
 * @link http://www.php.net/manual/en/function.posix-setpgid.php
 * @param pid int <p>
 *       The process id.
 *      </p>
 * @param pgid int <p>
 *       The process group id.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function posix_setpgid ($pid, $pgid) {}

/**
 * Get process group id for job control
 * @link http://www.php.net/manual/en/function.posix-getpgid.php
 * @param pid int <p>
 *       The process id.
 *      </p>
 * @return int the identifier, as an integer.
 */
function posix_getpgid ($pid) {}

/**
 * Get the current sid of the process
 * @link http://www.php.net/manual/en/function.posix-getsid.php
 * @param pid int <p>
 *       The process identifier. If set to 0, the current process is
 *       assumed.  If an invalid pid is
 *       specified, then false is returned and an error is set which
 *       can be checked with posix_get_last_error.
 *      </p>
 * @return int the identifier, as an integer.
 */
function posix_getsid ($pid) {}

/**
 * Get system name
 * @link http://www.php.net/manual/en/function.posix-uname.php
 * @return array a hash of strings with information about the
 *   system. The indices of the hash are
 *   
 *    
 *      sysname - operating system name (e.g. Linux)
 *     
 *    
 *      nodename - system name (e.g. valiant)
 *     
 *    
 *      release - operating system release (e.g. 2.2.10)
 *     
 *    
 *      version - operating system version (e.g. #4 Tue Jul 20
 *      17:01:36 MEST 1999)
 *     
 *    
 *      machine - system architecture (e.g. i586)
 *     
 *    
 *      domainname - DNS domainname (e.g. example.com)
 *     
 *   
 *  </p>
 *  <p>
 *   domainname is a GNU extension and not part of POSIX.1, so this
 *   field is only available on GNU systems or when using the GNU
 *   libc.
 */
function posix_uname () {}

/**
 * Get process times
 * @link http://www.php.net/manual/en/function.posix-times.php
 * @return array a hash of strings with information about the current
 *   process CPU usage. The indices of the hash are:
 *   
 *    
 *     
 *      ticks - the number of clock ticks that have elapsed since
 *      reboot.
 *     
 *    
 *    
 *     
 *      utime - user time used by the current process.
 *     
 *    
 *    
 *     
 *      stime - system time used by the current process.
 *     
 *    
 *    
 *     
 *      cutime - user time used by current process and children.
 *     
 *    
 *    
 *     
 *      cstime - system time used by current process and children.
 */
function posix_times () {}

/**
 * Get path name of controlling terminal
 * @link http://www.php.net/manual/en/function.posix-ctermid.php
 * @return string Upon successful completion, returns string of the pathname to 
 *   the current controlling terminal.  Otherwise false is returned and errno
 *   is set, which can be checked with posix_get_last_error.
 */
function posix_ctermid () {}

/**
 * Determine terminal device name
 * @link http://www.php.net/manual/en/function.posix-ttyname.php
 * @param fd int <p>
 *       The file descriptor.
 *      </p>
 * @return string On success, returns a string of the absolute path of the
 *   fd. On failure, returns false
 */
function posix_ttyname ($fd) {}

/**
 * Determine if a file descriptor is an interactive terminal
 * @link http://www.php.net/manual/en/function.posix-isatty.php
 * @param fd int <p>
 *       The file descriptor.
 *      </p>
 * @return bool true if fd is an open descriptor connected
 *   to a terminal and false otherwise.
 */
function posix_isatty ($fd) {}

/**
 * Pathname of current directory
 * @link http://www.php.net/manual/en/function.posix-getcwd.php
 * @return string a string of the absolute pathname on success.  
 *   On error, returns false and sets errno which can be checked with
 *   posix_get_last_error.
 */
function posix_getcwd () {}

/**
 * Create a fifo special file (a named pipe)
 * @link http://www.php.net/manual/en/function.posix-mkfifo.php
 * @param pathname string <p>
 *       Path to the FIFO file.
 *      </p>
 * @param mode int <p>
 *       The second parameter mode has to be given in
 *       octal notation (e.g. 0644). The permission of the newly created
 *       FIFO also depends on the setting of the current
 *       umask. The permissions of the created file are
 *       (mode & ~umask).
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function posix_mkfifo ($pathname, $mode) {}

/**
 * Create a special or ordinary file (POSIX.1)
 * @link http://www.php.net/manual/en/function.posix-mknod.php
 * @param pathname string <p>
 *       The file to create
 *      </p>
 * @param mode int <p>
 *       This parameter is constructed by a bitwise OR between file type (one of
 *       the following constants: POSIX_S_IFREG,
 *       POSIX_S_IFCHR, POSIX_S_IFBLK,
 *       POSIX_S_IFIFO or
 *       POSIX_S_IFSOCK) and permissions.
 *      </p>
 * @param major int[optional] <p>
 *       The major device kernel identifier (required to pass when using
 *       S_IFCHR or S_IFBLK).
 *      </p>
 * @param minor int[optional] <p>
 *       The minor device kernel identifier.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function posix_mknod ($pathname, $mode, $major = null, $minor = null) {}

/**
 * Determine accessibility of a file
 * @link http://www.php.net/manual/en/function.posix-access.php
 * @param file string <p>
 *        The name of the file to be tested.
 *       </p>
 * @param mode int[optional] <p>
 *        A mask consisting of one or more of POSIX_F_OK,
 *        POSIX_R_OK, POSIX_W_OK and
 *        POSIX_X_OK.
 *       </p>
 *       <p>
 *        POSIX_R_OK, POSIX_W_OK and
 *        POSIX_X_OK request checking whether the file
 *        exists and has read, write and execute permissions, respectively.
 *        POSIX_F_OK just requests checking for the
 *        existence of the file.
 *       </p>
 * @return bool Returns true on success or false on failure.
 */
function posix_access ($file, $mode = null) {}

/**
 * Return info about a group by name
 * @link http://www.php.net/manual/en/function.posix-getgrnam.php
 * @param name string <p>The name of the group</p>
 * @return array The array elements returned are:
 *   <table>
 *    The group information array
 *    
 *     
 *      <tr valign="top">
 *       <td>Element</td>
 *       <td>Description</td>
 *      </tr>
 *     
 *     
 *      <tr valign="top">
 *       <td>name</td>
 *       <td>
 *        The name element contains the name of the group. This is
 *        a short, usually less than 16 character "handle" of the
 *        group, not the real, full name.  This should be the same as
 *        the name parameter used when
 *        calling the function, and hence redundant.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>passwd</td>
 *       <td>
 *        The passwd element contains the group's password in an
 *        encrypted format. Often, for example on a system employing
 *        "shadow" passwords, an asterisk is returned instead.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>gid</td>
 *       <td>
 *        Group ID of the group in numeric form.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>members</td>
 *       <td>
 *        This consists of an array of
 *        string's for all the members in the group.
 *       </td>
 *      </tr>
 *     
 *    
 *   </table>
 */
function posix_getgrnam ($name) {}

/**
 * Return info about a group by group id
 * @link http://www.php.net/manual/en/function.posix-getgrgid.php
 * @param gid int <p>
 *       The group id.
 *      </p>
 * @return array The array elements returned are:
 *   <table>
 *    The group information array
 *    
 *     
 *      <tr valign="top">
 *       <td>Element</td>
 *       <td>Description</td>
 *      </tr>
 *     
 *     
 *      <tr valign="top">
 *       <td>name</td>
 *       <td>
 *        The name element contains the name of the group. This is
 *        a short, usually less than 16 character "handle" of the
 *        group, not the real, full name.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>passwd</td>
 *       <td>
 *        The passwd element contains the group's password in an
 *        encrypted format. Often, for example on a system employing
 *        "shadow" passwords, an asterisk is returned instead.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>gid</td>
 *       <td>
 *        Group ID, should be the same as the
 *        gid parameter used when calling the
 *        function, and hence redundant.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>members</td>
 *       <td>
 *        This consists of an array of
 *        string's for all the members in the group.
 *       </td>
 *      </tr>
 *     
 *    
 *   </table>
 */
function posix_getgrgid ($gid) {}

/**
 * Return info about a user by username
 * @link http://www.php.net/manual/en/function.posix-getpwnam.php
 * @param username string <p>
 *       An alphanumeric username.
 *      </p>
 * @return array On success an array with the following elements is returned, else 
 *   false is returned:
 *   <table>
 *    The user information array
 *    
 *     
 *      <tr valign="top">
 *       <td>Element</td>
 *       <td>Description</td>
 *      </tr>
 *     
 *     
 *      <tr valign="top">
 *       <td>name</td>
 *       <td>
 *        The name element contains the username of the user. This is
 *        a short, usually less than 16 character "handle" of the
 *        user, not the real, full name. This should be the same as
 *        the username parameter used when
 *        calling the function, and hence redundant.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>passwd</td>
 *       <td>
 *        The passwd element contains the user's password in an
 *        encrypted format. Often, for example on a system employing
 *        "shadow" passwords, an asterisk is returned instead.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>uid</td>
 *       <td>
 *        User ID of the user in numeric form.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>gid</td>
 *       <td>
 *        The group ID of the user. Use the function
 *        posix_getgrgid to resolve the group
 *        name and a list of its members.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>gecos</td>
 *       <td>
 *        GECOS is an obsolete term that refers to the finger
 *        information field on a Honeywell batch processing system.
 *        The field, however, lives on, and its contents have been
 *        formalized by POSIX. The field contains a comma separated
 *        list containing the user's full name, office phone, office
 *        number, and home phone number. On most systems, only the
 *        user's full name is available.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>dir</td>
 *       <td>
 *        This element contains the absolute path to the home
 *        directory of the user.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>shell</td>
 *       <td>
 *        The shell element contains the absolute path to the
 *        executable of the user's default shell.
 *       </td>
 *      </tr>
 *     
 *    
 *   </table>
 */
function posix_getpwnam ($username) {}

/**
 * Return info about a user by user id
 * @link http://www.php.net/manual/en/function.posix-getpwuid.php
 * @param uid int <p>
 *       The user identifier.
 *      </p>
 * @return array an associative array with the following elements:
 *   <table>
 *    The user information array
 *    
 *     
 *      <tr valign="top">
 *       <td>Element</td>
 *       <td>Description</td>
 *      </tr>
 *     
 *     
 *      <tr valign="top">
 *       <td>name</td>
 *       <td>
 *        The name element contains the username of the user. This is
 *        a short, usually less than 16 character "handle" of the
 *        user, not the real, full name.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>passwd</td>
 *       <td>
 *        The passwd element contains the user's password in an
 *        encrypted format. Often, for example on a system employing
 *        "shadow" passwords, an asterisk is returned instead.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>uid</td>
 *       <td>
 *        User ID, should be the same as the
 *        uid parameter used when calling the
 *        function, and hence redundant.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>gid</td>
 *       <td>
 *        The group ID of the user. Use the function
 *        posix_getgrgid to resolve the group
 *        name and a list of its members.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>gecos</td>
 *       <td>
 *        GECOS is an obsolete term that refers to the finger
 *        information field on a Honeywell batch processing system.
 *        The field, however, lives on, and its contents have been
 *        formalized by POSIX. The field contains a comma separated
 *        list containing the user's full name, office phone, office
 *        number, and home phone number. On most systems, only the
 *        user's full name is available.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>dir</td>
 *       <td>
 *        This element contains the absolute path to the
 *        home directory of the user.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>shell</td>
 *       <td>
 *        The shell element contains the absolute path to the
 *        executable of the user's default shell.
 *       </td>
 *      </tr>
 *     
 *    
 *   </table>
 */
function posix_getpwuid ($uid) {}

/**
 * Return info about system resource limits
 * @link http://www.php.net/manual/en/function.posix-getrlimit.php
 * @return array an associative array of elements for each
 *   limit that is defined. Each limit has a soft and a hard limit.
 *   <table>
 *    List of possible limits returned
 *    
 *     
 *      <tr valign="top">
 *       <td>Limit name</td>
 *       <td>Limit description</td>
 *      </tr>
 *     
 *     
 *      <tr valign="top">
 *       <td>core</td>
 *       <td>
 *        The maximum size of the core file.  When 0, not core files are
 *        created.  When core files are larger than this size, they will
 *        be truncated at this size.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>totalmem</td>
 *       <td>
 *        The maximum size of the memory of the process, in bytes.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>virtualmem</td>
 *       <td>
 *        The maximum size of the virtual memory for the process, in bytes.
 *       </td>
 *      </tr> 
 *      <tr valign="top">
 *       <td>data</td>
 *       <td>
 *        The maximum size of the data segment for the process, in bytes.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>stack</td>
 *       <td>
 *        The maximum size of the process stack, in bytes.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>rss</td>
 *       <td>
 *        The maximum number of virtual pages resident in RAM
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>maxproc</td>
 *       <td>
 *        The maximum number of processes that can be created for the
 *        real user ID of the calling process.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>memlock</td>
 *       <td>
 *        The maximum number of bytes of memory that may be locked into RAM.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>cpu</td>
 *       <td>
 *        The amount of time the process is allowed to use the CPU.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>filesize</td>
 *       <td>
 *        The maximum size of the data segment for the process, in bytes.
 *       </td>
 *      </tr>
 *      <tr valign="top">
 *       <td>openfiles</td>
 *       <td>
 *        One more than the maximum number of open file descriptors.
 *       </td>
 *      </tr>
 *     
 *    
 *   </table>
 */
function posix_getrlimit () {}

/**
 * Retrieve the error number set by the last posix function that failed
 * @link http://www.php.net/manual/en/function.posix-get-last-error.php
 * @return int the errno (error number) set by the last posix function that
 *   failed.  If no errors exist, 0 is returned.
 */
function posix_get_last_error () {}

/**
 * &Alias; <function>posix_get_last_error</function>
 * @link http://www.php.net/manual/en/function.posix-errno.php
 */
function posix_errno () {}

/**
 * Retrieve the system error message associated with the given errno
 * @link http://www.php.net/manual/en/function.posix-strerror.php
 * @param errno int <p>
 *       A POSIX error number, returned by 
 *       posix_get_last_error. If set to 0, then the
 *       string "Success" is returned.
 *      </p>
 * @return string the error message, as a string.
 */
function posix_strerror ($errno) {}

/**
 * Calculate the group access list
 * @link http://www.php.net/manual/en/function.posix-initgroups.php
 * @param name string <p>
 *       The user to calculate the list for.
 *      </p>
 * @param base_group_id int <p>
 *       Typically the group number from the password file.
 *      </p>
 * @return bool Returns true on success or false on failure.
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
