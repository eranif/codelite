<?php

// Start of zip v.1.12.4

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
	const OPSYS_DOS = 0;
	const OPSYS_AMIGA = 1;
	const OPSYS_OPENVMS = 2;
	const OPSYS_UNIX = 3;
	const OPSYS_VM_CMS = 4;
	const OPSYS_ATARI_ST = 5;
	const OPSYS_OS_2 = 6;
	const OPSYS_MACINTOSH = 7;
	const OPSYS_Z_SYSTEM = 8;
	const OPSYS_Z_CPM = 9;
	const OPSYS_WINDOWS_NTFS = 10;
	const OPSYS_MVS = 11;
	const OPSYS_VSE = 12;
	const OPSYS_ACORN_RISC = 13;
	const OPSYS_VFAT = 14;
	const OPSYS_ALTERNATE_MVS = 15;
	const OPSYS_BEOS = 16;
	const OPSYS_TANDEM = 17;
	const OPSYS_OS_400 = 18;
	const OPSYS_OS_X = 19;
	const OPSYS_DEFAULT = 3;


	/**
	 * @param filename
	 * @param flags[optional]
	 */
	public function open ($filename, $flags) {}

	/**
	 * @param password
	 */
	public function setPassword ($password) {}

	public function close () {}

	public function getStatusString () {}

	/**
	 * @param dirname
	 */
	public function addEmptyDir ($dirname) {}

	/**
	 * @param name
	 * @param content
	 */
	public function addFromString ($name, $content) {}

	/**
	 * @param filepath
	 * @param entryname[optional]
	 * @param start[optional]
	 * @param length[optional]
	 */
	public function addFile ($filepath, $entryname, $start, $length) {}

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
	 * @param index
	 * @param new_name
	 */
	public function renameIndex ($index, $new_name) {}

	/**
	 * @param name
	 * @param new_name
	 */
	public function renameName ($name, $new_name) {}

	/**
	 * @param comment
	 */
	public function setArchiveComment ($comment) {}

	/**
	 * @param flags[optional]
	 */
	public function getArchiveComment ($flags) {}

	/**
	 * @param index
	 * @param comment
	 */
	public function setCommentIndex ($index, $comment) {}

	/**
	 * @param name
	 * @param comment
	 */
	public function setCommentName ($name, $comment) {}

	/**
	 * @param index
	 * @param flags[optional]
	 */
	public function getCommentIndex ($index, $flags) {}

	/**
	 * @param name
	 * @param flags[optional]
	 */
	public function getCommentName ($name, $flags) {}

	/**
	 * @param index
	 */
	public function deleteIndex ($index) {}

	/**
	 * @param name
	 */
	public function deleteName ($name) {}

	/**
	 * @param filename
	 * @param flags[optional]
	 */
	public function statName ($filename, $flags) {}

	/**
	 * @param index
	 * @param flags[optional]
	 */
	public function statIndex ($index, $flags) {}

	/**
	 * @param filename
	 * @param flags[optional]
	 */
	public function locateName ($filename, $flags) {}

	/**
	 * @param index
	 * @param flags[optional]
	 */
	public function getNameIndex ($index, $flags) {}

	public function unchangeArchive () {}

	public function unchangeAll () {}

	/**
	 * @param index
	 */
	public function unchangeIndex ($index) {}

	/**
	 * @param name
	 */
	public function unchangeName ($name) {}

	/**
	 * @param pathto
	 * @param files[optional]
	 */
	public function extractTo ($pathto, $files) {}

	/**
	 * @param entryname
	 * @param len[optional]
	 * @param flags[optional]
	 */
	public function getFromName ($entryname, $len, $flags) {}

	/**
	 * @param index
	 * @param len[optional]
	 * @param flags[optional]
	 */
	public function getFromIndex ($index, $len, $flags) {}

	/**
	 * @param entryname
	 */
	public function getStream ($entryname) {}

	/**
	 * @param name
	 * @param opsys
	 * @param attr
	 * @param flags[optional]
	 */
	public function setExternalAttributesName ($name, $opsys, $attr, $flags) {}

	/**
	 * @param index
	 * @param opsys
	 * @param attr
	 * @param flags[optional]
	 */
	public function setExternalAttributesIndex ($index, $opsys, $attr, $flags) {}

	/**
	 * @param name
	 * @param opsys
	 * @param attr
	 * @param flags[optional]
	 */
	public function getExternalAttributesName ($name, &$opsys, &$attr, $flags) {}

	/**
	 * @param index
	 * @param opsys
	 * @param attr
	 * @param flags[optional]
	 */
	public function getExternalAttributesIndex ($index, &$opsys, &$attr, $flags) {}

}

/**
 * @param filename
 */
function zip_open ($filename) {}

/**
 * @param zip
 */
function zip_close ($zip) {}

/**
 * @param zip
 */
function zip_read ($zip) {}

/**
 * @param zip_dp
 * @param zip_entry
 * @param mode[optional]
 */
function zip_entry_open ($zip_dp, $zip_entry, $mode) {}

/**
 * @param zip_ent
 */
function zip_entry_close ($zip_ent) {}

/**
 * @param zip_entry
 * @param len[optional]
 */
function zip_entry_read ($zip_entry, $len) {}

/**
 * @param zip_entry
 */
function zip_entry_filesize ($zip_entry) {}

/**
 * @param zip_entry
 */
function zip_entry_name ($zip_entry) {}

/**
 * @param zip_entry
 */
function zip_entry_compressedsize ($zip_entry) {}

/**
 * @param zip_entry
 */
function zip_entry_compressionmethod ($zip_entry) {}

// End of zip v.1.12.4
?>
