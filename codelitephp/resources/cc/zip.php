<?php

// Start of zip v.1.9.1

class ZipArchive  {
	const CREATE = 1;
	const EXCL = 2;
	const CHECKCONS = 4;
	const OVERWRITE = 8;
	const FL_NOCASE = 1;
	const FL_NODIR = 2;
	const FL_COMPRESSED = 4;
	const FL_UNCHANGED = 8;
	const CM_DEFAULT = -1;
	const CM_STORE = 0;
	const CM_SHRINK = 1;
	const CM_REDUCE_1 = 2;
	const CM_REDUCE_2 = 3;
	const CM_REDUCE_3 = 4;
	const CM_REDUCE_4 = 5;
	const CM_IMPLODE = 6;
	const CM_DEFLATE = 8;
	const CM_DEFLATE64 = 9;
	const CM_PKWARE_IMPLODE = 10;
	const CM_BZIP2 = 12;
	const CM_LZMA = 14;
	const CM_TERSE = 18;
	const CM_LZ77 = 19;
	const CM_WAVPACK = 97;
	const CM_PPMD = 98;
	const ER_OK = 0;
	const ER_MULTIDISK = 1;
	const ER_RENAME = 2;
	const ER_CLOSE = 3;
	const ER_SEEK = 4;
	const ER_READ = 5;
	const ER_WRITE = 6;
	const ER_CRC = 7;
	const ER_ZIPCLOSED = 8;
	const ER_NOENT = 9;
	const ER_EXISTS = 10;
	const ER_OPEN = 11;
	const ER_TMPOPEN = 12;
	const ER_ZLIB = 13;
	const ER_MEMORY = 14;
	const ER_CHANGED = 15;
	const ER_COMPNOTSUPP = 16;
	const ER_EOF = 17;
	const ER_INVAL = 18;
	const ER_NOZIP = 19;
	const ER_INTERNAL = 20;
	const ER_INCONS = 21;
	const ER_REMOVE = 22;
	const ER_DELETED = 23;


	/**
	 * Open a ZIP file archive
	 * @link http://www.php.net/manual/en/function.ziparchive-open.php
	 * @param filename string <p>
	 *       The file name of the ZIP archive to open.
	 *      </p>
	 * @param flags int[optional] <p>
	 *       The mode to use to open the archive.
	 *       
	 *        
	 *         <p>
	 *          ZIPARCHIVE::OVERWRITE
	 *         </p>
	 * @return mixed Error codes
	 *      
	 *      <p>
	 *       Returns true on success or the error code.
	 *       
	 *        
	 *         <p>
	 *          ZIPARCHIVE::ER_EXISTS
	 *         </p>
	 *        
	 *        
	 *         <p>
	 *          ZIPARCHIVE::ER_INCONS
	 *         </p>
	 *        
	 *        
	 *         <p>
	 *          ZIPARCHIVE::ER_INVAL
	 *         </p>
	 *        
	 *        
	 *         <p>
	 *          ZIPARCHIVE::ER_MEMORY
	 *         </p>
	 *        
	 *        
	 *         <p>
	 *          ZIPARCHIVE::ER_NOENT
	 *         </p>
	 *        
	 *        
	 *         <p>
	 *          ZIPARCHIVE::ER_NOZIP
	 *         </p>
	 *        
	 *        
	 *         <p>
	 *          ZIPARCHIVE::ER_OPEN
	 *         </p>
	 *        
	 *        
	 *         <p>
	 *          ZIPARCHIVE::ER_READ
	 *         </p>
	 *        
	 *        
	 *         <p>
	 *          ZIPARCHIVE::ER_SEEK
	 *         </p>
	 *        
	 *       
	 *      </p>
	 */
	public function open ($filename, $flags = null) {}

	/**
	 * Close the active archive (opened or newly created)
	 * @link http://www.php.net/manual/en/function.ziparchive-close.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function close () {}

	/**
	 * Returns the status error message, system and/or zip messages
	 * @link http://www.php.net/manual/en/function.ziparchive-getstatusstring.php
	 * @return string a string with the status message on success&return.falseforfailure;.
	 */
	public function getStatusString () {}

	/**
	 * Add a new directory
	 * @link http://www.php.net/manual/en/function.ziparchive-addemptydir.php
	 * @param dirname string <p>
	 *       The directory to add.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function addEmptyDir ($dirname) {}

	/**
	 * Add a file to a ZIP archive using its contents
	 * @link http://www.php.net/manual/en/function.ziparchive-addfromstring.php
	 * @param localname string <p>
	 *       The name of the entry to create.
	 *      </p>
	 * @param contents string <p>
	 *       The contents to use to create the entry. It is used in a binary
	 *       safe mode.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function addFromString ($localname, $contents) {}

	/**
	 * Adds a file to a ZIP archive from the given path
	 * @link http://www.php.net/manual/en/function.ziparchive-addfile.php
	 * @param filename string <p>
	 *       The path to the file to add.
	 *      </p>
	 * @param localname string[optional] <p>
	 *       If supplied, this is the local name inside the ZIP archive that will override the filename.
	 *      </p>
	 * @param start int[optional] <p>
	 *       This parameter is not used but is required to extend ZipArchive.
	 *      </p>
	 * @param length int[optional] <p>
	 *       This parameter is not used but is required to extend ZipArchive.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function addFile ($filename, $localname = null, $start = null, $length = null) {}

	/**
	 * @param pattern
	 * @param flags[optional]
	 * @param options[optional]
	 */
	public function addGlob ($pattern, $flags, $options) {}

	/**
	 * @param pattern
	 * @param path[optional]
	 * @param options[optional]
	 */
	public function addPattern ($pattern, $path, $options) {}

	/**
	 * Renames an entry defined by its index
	 * @link http://www.php.net/manual/en/function.ziparchive-renameindex.php
	 * @param index int <p>
	 *       Index of the entry to rename.
	 *      </p>
	 * @param newname string <p>
	 *       New name.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function renameIndex ($index, $newname) {}

	/**
	 * Renames an entry defined by its name
	 * @link http://www.php.net/manual/en/function.ziparchive-renamename.php
	 * @param name string <p>
	 *       Name of the entry to rename.
	 *      </p>
	 * @param newname string <p>
	 *       New name.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function renameName ($name, $newname) {}

	/**
	 * Set the comment of a ZIP archive
	 * @link http://www.php.net/manual/en/function.ziparchive-setarchivecomment.php
	 * @param comment string <p>
	 *       The contents of the comment.
	 *      </p>
	 * @return mixed Returns true on success or false on failure.
	 */
	public function setArchiveComment ($comment) {}

	/**
	 * Returns the Zip archive comment
	 * @link http://www.php.net/manual/en/function.ziparchive-getarchivecomment.php
	 * @param flags int[optional] <p>
	 *       If flags is set to ZIPARCHIVE::FL_UNCHANGED, the original unchanged
	 *       comment is returned.
	 *      </p>
	 * @return string the Zip archive comment&return.falseforfailure;.
	 */
	public function getArchiveComment ($flags = null) {}

	/**
	 * Set the comment of an entry defined by its index
	 * @link http://www.php.net/manual/en/function.ziparchive-setcommentindex.php
	 * @param index int <p>
	 *       Index of the entry.
	 *      </p>
	 * @param comment string <p>
	 *       The contents of the comment.
	 *      </p>
	 * @return mixed Returns true on success or false on failure.
	 */
	public function setCommentIndex ($index, $comment) {}

	/**
	 * Set the comment of an entry defined by its name
	 * @link http://www.php.net/manual/en/function.ziparchive-setCommentName.php
	 * @param name string <p>
	 *       Name of the entry.
	 *      </p>
	 * @param comment string <p>
	 *       The contents of the comment.
	 *      </p>
	 * @return mixed Returns true on success or false on failure.
	 */
	public function setCommentName ($name, $comment) {}

	/**
	 * Returns the comment of an entry using the entry index
	 * @link http://www.php.net/manual/en/function.ziparchive-getcommentindex.php
	 * @param index int <p>
	 *       Index of the entry
	 *      </p>
	 * @param flags int[optional] <p>
	 *       If flags is set to ZIPARCHIVE::FL_UNCHANGED, the original unchanged
	 *       comment is returned.
	 *      </p>
	 * @return string the comment on success&return.falseforfailure;.
	 */
	public function getCommentIndex ($index, $flags = null) {}

	/**
	 * Returns the comment of an entry using the entry name
	 * @link http://www.php.net/manual/en/function.ziparchive-getcommentname.php
	 * @param name string <p>
	 *       Name of the entry
	 *      </p>
	 * @param flags int[optional] <p>
	 *       If flags is set to ZIPARCHIVE::FL_UNCHANGED, the original unchanged
	 *       comment is returned.
	 *      </p>
	 * @return string the comment on success&return.falseforfailure;.
	 */
	public function getCommentName ($name, $flags = null) {}

	/**
	 * delete an entry in the archive using its index
	 * @link http://www.php.net/manual/en/function.ziparchive-deleteindex.php
	 * @param index int <p>
	 *       Index of the entry to delete.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function deleteIndex ($index) {}

	/**
	 * delete an entry in the archive using its name
	 * @link http://www.php.net/manual/en/function.ziparchive-deletename.php
	 * @param name string <p>
	 *       Name of the entry to delete.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function deleteName ($name) {}

	/**
	 * Get the details of an entry defined by its name.
	 * @link http://www.php.net/manual/en/function.ziparchive-statname.php
	 * @param name name <p>
	 *       Name of the entry
	 *      </p>
	 * @param flags int[optional] <p>
	 *       The flags argument specifies how the name lookup should be done.
	 *       Also, ZIPARCHIVE::FL_UNCHANGED may be ORed to it to request
	 *       information  about  the  original  file  in  the archive,
	 *       ignoring any changes made.
	 *       
	 *        
	 *         <p>
	 *          ZIPARCHIVE::FL_NOCASE
	 *         </p>
	 * @return mixed an array containing the entry details &return.falseforfailure;.
	 */
	public function statName ($name, $flags = null) {}

	/**
	 * Get the details of an entry defined by its index.
	 * @link http://www.php.net/manual/en/function.ziparchive-statindex.php
	 * @param index int <p>
	 *       Index of the entry
	 *      </p>
	 * @param flags int[optional] <p>
	 *       ZIPARCHIVE::FL_UNCHANGED may be ORed to it to request
	 *       information  about  the  original  file  in  the archive,
	 *       ignoring any changes made.
	 *      </p>
	 * @return mixed an array containing the entry details&return.falseforfailure;.
	 */
	public function statIndex ($index, $flags = null) {}

	/**
	 * Returns the index of the entry in the archive
	 * @link http://www.php.net/manual/en/function.ziparchive-locatename.php
	 * @param name string <p>
	 *       The name of the entry to look up
	 *      </p>
	 * @param flags int[optional] <p>
	 *       The flags are specified by ORing the following values,
	 *       or 0 for none of them.
	 *       
	 *        
	 *         <p>
	 *          ZIPARCHIVE::FL_NOCASE
	 *         </p>
	 * @return mixed the index of the entry on success&return.falseforfailure;.
	 */
	public function locateName ($name, $flags = null) {}

	/**
	 * Returns the name of an entry using its index
	 * @link http://www.php.net/manual/en/function.ziparchive-getnameindex.php
	 * @param index int <p>
	 *       Index of the entry.
	 *      </p>
	 * @param flags int[optional] <p>
	 *       If flags is set to ZIPARCHIVE::FL_UNCHANGED, the original unchanged
	 *       name is returned.
	 *      </p>
	 * @return string the name on success&return.falseforfailure;.
	 */
	public function getNameIndex ($index, $flags = null) {}

	/**
	 * Revert all global changes done in the archive.
	 * @link http://www.php.net/manual/en/function.ziparchive-unchangearchive.php
	 * @return mixed Returns true on success or false on failure.
	 */
	public function unchangeArchive () {}

	/**
	 * Undo all changes done in the archive
	 * @link http://www.php.net/manual/en/function.ziparchive-unchangeall.php
	 * @return mixed Returns true on success or false on failure.
	 */
	public function unchangeAll () {}

	/**
	 * Revert all changes done to an entry at the given index
	 * @link http://www.php.net/manual/en/function.ziparchive-unchangeindex.php
	 * @param index int <p>
	 *       Index of the entry.
	 *      </p>
	 * @return mixed Returns true on success or false on failure.
	 */
	public function unchangeIndex ($index) {}

	/**
	 * Revert all changes done to an entry with the given name.
	 * @link http://www.php.net/manual/en/function.ziparchive-unchangename.php
	 * @param name string <p>
	 *       Name of the entry.
	 *      </p>
	 * @return mixed Returns true on success or false on failure.
	 */
	public function unchangeName ($name) {}

	/**
	 * Extract the archive contents
	 * @link http://www.php.net/manual/en/function.ziparchive-extractto.php
	 * @param destination string <p>
	 *       Location where to extract the files.
	 *      </p>
	 * @param entries mixed[optional] <p>
	 *       The entries to extract. It accepts either a single entry name or
	 *       an array of names.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function extractTo ($destination, $entries = null) {}

	/**
	 * Returns the entry contents using its name
	 * @link http://www.php.net/manual/en/function.ziparchive-getfromname.php
	 * @param name string <p>
	 *       Name of the entry
	 *      </p>
	 * @param length int[optional] <p>
	 *       The length to be read from the entry. If 0, then the
	 *       entire entry is read.
	 *      </p>
	 * @param flags int[optional] <p>
	 *       The flags to use to open the archive. the following values may
	 *       be ORed to it.
	 *       
	 *        
	 *         <p>
	 *          ZIPARCHIVE::FL_UNCHANGED
	 *         </p>
	 * @return mixed the contents of the entry on success&return.falseforfailure;.
	 */
	public function getFromName ($name, $length = null, $flags = null) {}

	/**
	 * Returns the entry contents using its index
	 * @link http://www.php.net/manual/en/function.ziparchive-getfromindex.php
	 * @param index int <p>
	 *       Index of the entry
	 *      </p>
	 * @param length int[optional] <p>
	 *       The length to be read from the entry. If 0, then the
	 *       entire entry is read.
	 *      </p>
	 * @param flags int[optional] <p>
	 *       The flags to use to open the archive. the following values may
	 *       be ORed to it.
	 *       
	 *        
	 *         <p>
	 *          ZIPARCHIVE::FL_UNCHANGED
	 *         </p>
	 * @return mixed the contents of the entry on success&return.falseforfailure;.
	 */
	public function getFromIndex ($index, $length = null, $flags = null) {}

	/**
	 * Get a file handler to the entry defined by its name (read only).
	 * @link http://www.php.net/manual/en/function.ziparchive-getstream.php
	 * @param name string <p>
	 *       The name of the entry to use.
	 *      </p>
	 * @return resource a file pointer (resource) on success&return.falseforfailure;.
	 */
	public function getStream ($name) {}

}

/**
 * Open a ZIP file archive
 * @link http://www.php.net/manual/en/function.zip-open.php
 * @param filename string <p>
 *       The file name of the ZIP archive to open.
 *      </p>
 * @return mixed a resource handle for later use with
 *   zip_read and zip_close
 *   or returns the number of error if filename does not
 *   exist or in case of other error.
 */
function zip_open ($filename) {}

/**
 * Close a ZIP file archive
 * @link http://www.php.net/manual/en/function.zip-close.php
 * @param zip resource <p>
 *       A ZIP file previously opened with zip_open.
 *      </p>
 * @return void 
 */
function zip_close ($zip) {}

/**
 * Read next entry in a ZIP file archive
 * @link http://www.php.net/manual/en/function.zip-read.php
 * @param zip resource <p>
 *       A ZIP file previously opened with zip_open.
 *      </p>
 * @return mixed a directory entry resource for later use with the
 *   zip_entry_... functions, or false if
 *   there are no more entries to read, or an error code if an error
 *   occurred.
 */
function zip_read ($zip) {}

/**
 * Open a directory entry for reading
 * @link http://www.php.net/manual/en/function.zip-entry-open.php
 * @param zip resource <p>
 *       A valid resource handle returned by zip_open.
 *      </p>
 * @param zip_entry resource <p>
 *       A directory entry returned by zip_read.
 *      </p>
 * @param mode string[optional] <p>
 *       Any of the modes specified in the documentation of
 *       fopen.
 *      </p>
 *      
 *       <p>
 *        Currently, mode is ignored and is always
 *        "rb". This is due to the fact that zip support
 *        in PHP is read only access.
 *       </p>
 * @return bool Returns true on success or false on failure.
 *  </p>
 *  
 *   <p>
 *    Unlike fopen and other similar functions,
 *    the return value of zip_entry_open only
 *    indicates the result of the operation and is not needed for
 *    reading or closing the directory entry.
 */
function zip_entry_open ($zip, $zip_entry, $mode = null) {}

/**
 * Close a directory entry
 * @link http://www.php.net/manual/en/function.zip-entry-close.php
 * @param zip_entry resource <p>
 *       A directory entry previously opened zip_entry_open.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function zip_entry_close ($zip_entry) {}

/**
 * Read from an open directory entry
 * @link http://www.php.net/manual/en/function.zip-entry-read.php
 * @param zip_entry resource <p>
 *       A directory entry returned by zip_read.
 *      </p>
 * @param length int[optional] <p>
 *       The number of bytes to return. If not specified, this function will 
 *       attempt to read 1024 bytes.
 *      </p>
 *      
 *       <p>
 *        This should be the uncompressed length you wish to read.
 *       </p>
 * @return string the data read, or false if the end of the file is
 *   reached.
 */
function zip_entry_read ($zip_entry, $length = null) {}

/**
 * Retrieve the actual file size of a directory entry
 * @link http://www.php.net/manual/en/function.zip-entry-filesize.php
 * @param zip_entry resource <p>
 *       A directory entry returned by zip_read.
 *      </p>
 * @return int The size of the directory entry.
 */
function zip_entry_filesize ($zip_entry) {}

/**
 * Retrieve the name of a directory entry
 * @link http://www.php.net/manual/en/function.zip-entry-name.php
 * @param zip_entry resource <p>
 *       A directory entry returned by zip_read.
 *      </p>
 * @return string The name of the directory entry.
 */
function zip_entry_name ($zip_entry) {}

/**
 * Retrieve the compressed size of a directory entry
 * @link http://www.php.net/manual/en/function.zip-entry-compressedsize.php
 * @param zip_entry resource <p>
 *       A directory entry returned by zip_read.
 *      </p>
 * @return int The compressed size.
 */
function zip_entry_compressedsize ($zip_entry) {}

/**
 * Retrieve the compression method of a directory entry
 * @link http://www.php.net/manual/en/function.zip-entry-compressionmethod.php
 * @param zip_entry resource <p>
 *       A directory entry returned by zip_read.
 *      </p>
 * @return string The compression method.
 */
function zip_entry_compressionmethod ($zip_entry) {}

// End of zip v.1.9.1
?>
