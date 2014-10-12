<?php

// Start of Phar v.2.0.2

class PharException extends Exception  {
	protected $message;
	protected $code;
	protected $file;
	protected $line;


	final private function __clone () {}

	/**
	 * @param message[optional]
	 * @param code[optional]
	 * @param previous[optional]
	 */
	public function __construct ($message, $code, $previous) {}

	final public function getMessage () {}

	final public function getCode () {}

	final public function getFile () {}

	final public function getLine () {}

	final public function getTrace () {}

	final public function getPrevious () {}

	final public function getTraceAsString () {}

	public function __toString () {}

}

class Phar extends RecursiveDirectoryIterator implements RecursiveIterator, SeekableIterator, Traversable, Iterator, Countable, ArrayAccess {
	const CURRENT_MODE_MASK = 240;
	const CURRENT_AS_PATHNAME = 32;
	const CURRENT_AS_FILEINFO = 0;
	const CURRENT_AS_SELF = 16;
	const KEY_MODE_MASK = 3840;
	const KEY_AS_PATHNAME = 0;
	const FOLLOW_SYMLINKS = 512;
	const KEY_AS_FILENAME = 256;
	const NEW_CURRENT_AND_KEY = 256;
	const OTHER_MODE_MASK = 12288;
	const SKIP_DOTS = 4096;
	const UNIX_PATHS = 8192;
	const BZ2 = 8192;
	const GZ = 4096;
	const NONE = 0;
	const PHAR = 1;
	const TAR = 2;
	const ZIP = 3;
	const COMPRESSED = 61440;
	const PHP = 0;
	const PHPS = 1;
	const MD5 = 1;
	const OPENSSL = 16;
	const SHA1 = 2;
	const SHA256 = 3;
	const SHA512 = 4;


	/**
	 * @param filename
	 * @param flags[optional]
	 * @param alias[optional]
	 * @param fileformat[optional]
	 */
	public function __construct ($filename, $flags, $alias, $fileformat) {}

	public function __destruct () {}

	/**
	 * @param dirname[optional]
	 */
	public function addEmptyDir ($dirname) {}

	/**
	 * @param filename
	 * @param localname[optional]
	 */
	public function addFile ($filename, $localname) {}

	/**
	 * @param localname
	 * @param contents[optional]
	 */
	public function addFromString ($localname, $contents) {}

	/**
	 * @param base_dir
	 * @param regex[optional]
	 */
	public function buildFromDirectory ($base_dir, $regex) {}

	/**
	 * @param iterator
	 * @param base_directory[optional]
	 */
	public function buildFromIterator ($iterator, $base_directory) {}

	/**
	 * @param compression_type
	 */
	public function compressFiles ($compression_type) {}

	public function decompressFiles () {}

	/**
	 * @param compression_type
	 * @param file_ext[optional]
	 */
	public function compress ($compression_type, $file_ext) {}

	/**
	 * @param file_ext[optional]
	 */
	public function decompress ($file_ext) {}

	/**
	 * @param format[optional]
	 * @param compression_type[optional]
	 * @param file_ext[optional]
	 */
	public function convertToExecutable ($format, $compression_type, $file_ext) {}

	/**
	 * @param format[optional]
	 * @param compression_type[optional]
	 * @param file_ext[optional]
	 */
	public function convertToData ($format, $compression_type, $file_ext) {}

	/**
	 * @param newfile
	 * @param oldfile
	 */
	public function copy ($newfile, $oldfile) {}

	public function count () {}

	/**
	 * @param entry
	 */
	public function delete ($entry) {}

	public function delMetadata () {}

	/**
	 * @param pathto
	 * @param files[optional]
	 * @param overwrite[optional]
	 */
	public function extractTo ($pathto, $files, $overwrite) {}

	public function getAlias () {}

	public function getPath () {}

	public function getMetadata () {}

	public function getModified () {}

	public function getSignature () {}

	public function getStub () {}

	public function getVersion () {}

	public function hasMetadata () {}

	public function isBuffering () {}

	public function isCompressed () {}

	/**
	 * @param fileformat
	 */
	public function isFileFormat ($fileformat) {}

	public function isWritable () {}

	/**
	 * @param entry
	 */
	public function offsetExists ($entry) {}

	/**
	 * @param entry
	 */
	public function offsetGet ($entry) {}

	/**
	 * @param entry
	 * @param value
	 */
	public function offsetSet ($entry, $value) {}

	/**
	 * @param entry
	 */
	public function offsetUnset ($entry) {}

	/**
	 * @param alias
	 */
	public function setAlias ($alias) {}

	/**
	 * @param index[optional]
	 * @param webindex[optional]
	 */
	public function setDefaultStub ($index, $webindex) {}

	/**
	 * @param metadata
	 */
	public function setMetadata ($metadata) {}

	/**
	 * @param algorithm
	 * @param privatekey[optional]
	 */
	public function setSignatureAlgorithm ($algorithm, $privatekey) {}

	/**
	 * @param newstub
	 * @param maxlen[optional]
	 */
	public function setStub ($newstub, $maxlen) {}

	public function startBuffering () {}

	public function stopBuffering () {}

	final public static function apiVersion () {}

	/**
	 * @param method[optional]
	 */
	final public static function canCompress ($method) {}

	final public static function canWrite () {}

	/**
	 * @param index[optional]
	 * @param webindex[optional]
	 */
	final public static function createDefaultStub ($index, $webindex) {}

	final public static function getSupportedCompression () {}

	final public static function getSupportedSignatures () {}

	final public static function interceptFileFuncs () {}

	/**
	 * @param filename
	 * @param executable[optional]
	 */
	final public static function isValidPharFilename ($filename, $executable) {}

	/**
	 * @param filename
	 * @param alias[optional]
	 */
	final public static function loadPhar ($filename, $alias) {}

	/**
	 * @param alias[optional]
	 * @param offset[optional]
	 */
	final public static function mapPhar ($alias, $offset) {}

	/**
	 * @param retphar
	 */
	final public static function running ($retphar) {}

	/**
	 * @param inphar
	 * @param externalfile
	 */
	final public static function mount ($inphar, $externalfile) {}

	/**
	 * @param munglist
	 */
	final public static function mungServer ($munglist) {}

	/**
	 * @param archive
	 */
	final public static function unlinkArchive ($archive) {}

	/**
	 * @param alias[optional]
	 * @param index[optional]
	 * @param f404[optional]
	 * @param mimetypes[optional]
	 * @param rewrites[optional]
	 */
	final public static function webPhar ($alias, $index, $f404, $mimetypes, $rewrites) {}

	/**
	 * @param allow_links[optional]
	 */
	public function hasChildren ($allow_links) {}

	public function getChildren () {}

	public function getSubPath () {}

	public function getSubPathname () {}

	public function rewind () {}

	public function next () {}

	public function key () {}

	public function current () {}

	public function getFlags () {}

	/**
	 * @param flags[optional]
	 */
	public function setFlags ($flags) {}

	public function getFilename () {}

	public function getExtension () {}

	/**
	 * @param suffix[optional]
	 */
	public function getBasename ($suffix) {}

	public function isDot () {}

	public function valid () {}

	/**
	 * @param position
	 */
	public function seek ($position) {}

	public function __toString () {}

	public function getPathname () {}

	public function getPerms () {}

	public function getInode () {}

	public function getSize () {}

	public function getOwner () {}

	public function getGroup () {}

	public function getATime () {}

	public function getMTime () {}

	public function getCTime () {}

	public function getType () {}

	public function isReadable () {}

	public function isExecutable () {}

	public function isFile () {}

	public function isDir () {}

	public function isLink () {}

	public function getLinkTarget () {}

	public function getRealPath () {}

	/**
	 * @param class_name[optional]
	 */
	public function getFileInfo ($class_name) {}

	/**
	 * @param class_name[optional]
	 */
	public function getPathInfo ($class_name) {}

	/**
	 * @param open_mode[optional]
	 * @param use_include_path[optional]
	 * @param context[optional]
	 */
	public function openFile ($open_mode, $use_include_path, $context) {}

	/**
	 * @param class_name[optional]
	 */
	public function setFileClass ($class_name) {}

	/**
	 * @param class_name[optional]
	 */
	public function setInfoClass ($class_name) {}

	final public function _bad_state_ex () {}

}

class PharData extends RecursiveDirectoryIterator implements RecursiveIterator, SeekableIterator, Traversable, Iterator, Countable, ArrayAccess {
	const CURRENT_MODE_MASK = 240;
	const CURRENT_AS_PATHNAME = 32;
	const CURRENT_AS_FILEINFO = 0;
	const CURRENT_AS_SELF = 16;
	const KEY_MODE_MASK = 3840;
	const KEY_AS_PATHNAME = 0;
	const FOLLOW_SYMLINKS = 512;
	const KEY_AS_FILENAME = 256;
	const NEW_CURRENT_AND_KEY = 256;
	const OTHER_MODE_MASK = 12288;
	const SKIP_DOTS = 4096;
	const UNIX_PATHS = 8192;


	/**
	 * @param filename
	 * @param flags[optional]
	 * @param alias[optional]
	 * @param fileformat[optional]
	 */
	public function __construct ($filename, $flags, $alias, $fileformat) {}

	public function __destruct () {}

	/**
	 * @param dirname[optional]
	 */
	public function addEmptyDir ($dirname) {}

	/**
	 * @param filename
	 * @param localname[optional]
	 */
	public function addFile ($filename, $localname) {}

	/**
	 * @param localname
	 * @param contents[optional]
	 */
	public function addFromString ($localname, $contents) {}

	/**
	 * @param base_dir
	 * @param regex[optional]
	 */
	public function buildFromDirectory ($base_dir, $regex) {}

	/**
	 * @param iterator
	 * @param base_directory[optional]
	 */
	public function buildFromIterator ($iterator, $base_directory) {}

	/**
	 * @param compression_type
	 */
	public function compressFiles ($compression_type) {}

	public function decompressFiles () {}

	/**
	 * @param compression_type
	 * @param file_ext[optional]
	 */
	public function compress ($compression_type, $file_ext) {}

	/**
	 * @param file_ext[optional]
	 */
	public function decompress ($file_ext) {}

	/**
	 * @param format[optional]
	 * @param compression_type[optional]
	 * @param file_ext[optional]
	 */
	public function convertToExecutable ($format, $compression_type, $file_ext) {}

	/**
	 * @param format[optional]
	 * @param compression_type[optional]
	 * @param file_ext[optional]
	 */
	public function convertToData ($format, $compression_type, $file_ext) {}

	/**
	 * @param newfile
	 * @param oldfile
	 */
	public function copy ($newfile, $oldfile) {}

	public function count () {}

	/**
	 * @param entry
	 */
	public function delete ($entry) {}

	public function delMetadata () {}

	/**
	 * @param pathto
	 * @param files[optional]
	 * @param overwrite[optional]
	 */
	public function extractTo ($pathto, $files, $overwrite) {}

	public function getAlias () {}

	public function getPath () {}

	public function getMetadata () {}

	public function getModified () {}

	public function getSignature () {}

	public function getStub () {}

	public function getVersion () {}

	public function hasMetadata () {}

	public function isBuffering () {}

	public function isCompressed () {}

	/**
	 * @param fileformat
	 */
	public function isFileFormat ($fileformat) {}

	public function isWritable () {}

	/**
	 * @param entry
	 */
	public function offsetExists ($entry) {}

	/**
	 * @param entry
	 */
	public function offsetGet ($entry) {}

	/**
	 * @param entry
	 * @param value
	 */
	public function offsetSet ($entry, $value) {}

	/**
	 * @param entry
	 */
	public function offsetUnset ($entry) {}

	/**
	 * @param alias
	 */
	public function setAlias ($alias) {}

	/**
	 * @param index[optional]
	 * @param webindex[optional]
	 */
	public function setDefaultStub ($index, $webindex) {}

	/**
	 * @param metadata
	 */
	public function setMetadata ($metadata) {}

	/**
	 * @param algorithm
	 * @param privatekey[optional]
	 */
	public function setSignatureAlgorithm ($algorithm, $privatekey) {}

	/**
	 * @param newstub
	 * @param maxlen[optional]
	 */
	public function setStub ($newstub, $maxlen) {}

	public function startBuffering () {}

	public function stopBuffering () {}

	final public static function apiVersion () {}

	/**
	 * @param method[optional]
	 */
	final public static function canCompress ($method) {}

	final public static function canWrite () {}

	/**
	 * @param index[optional]
	 * @param webindex[optional]
	 */
	final public static function createDefaultStub ($index, $webindex) {}

	final public static function getSupportedCompression () {}

	final public static function getSupportedSignatures () {}

	final public static function interceptFileFuncs () {}

	/**
	 * @param filename
	 * @param executable[optional]
	 */
	final public static function isValidPharFilename ($filename, $executable) {}

	/**
	 * @param filename
	 * @param alias[optional]
	 */
	final public static function loadPhar ($filename, $alias) {}

	/**
	 * @param alias[optional]
	 * @param offset[optional]
	 */
	final public static function mapPhar ($alias, $offset) {}

	/**
	 * @param retphar
	 */
	final public static function running ($retphar) {}

	/**
	 * @param inphar
	 * @param externalfile
	 */
	final public static function mount ($inphar, $externalfile) {}

	/**
	 * @param munglist
	 */
	final public static function mungServer ($munglist) {}

	/**
	 * @param archive
	 */
	final public static function unlinkArchive ($archive) {}

	/**
	 * @param alias[optional]
	 * @param index[optional]
	 * @param f404[optional]
	 * @param mimetypes[optional]
	 * @param rewrites[optional]
	 */
	final public static function webPhar ($alias, $index, $f404, $mimetypes, $rewrites) {}

	/**
	 * @param allow_links[optional]
	 */
	public function hasChildren ($allow_links) {}

	public function getChildren () {}

	public function getSubPath () {}

	public function getSubPathname () {}

	public function rewind () {}

	public function next () {}

	public function key () {}

	public function current () {}

	public function getFlags () {}

	/**
	 * @param flags[optional]
	 */
	public function setFlags ($flags) {}

	public function getFilename () {}

	public function getExtension () {}

	/**
	 * @param suffix[optional]
	 */
	public function getBasename ($suffix) {}

	public function isDot () {}

	public function valid () {}

	/**
	 * @param position
	 */
	public function seek ($position) {}

	public function __toString () {}

	public function getPathname () {}

	public function getPerms () {}

	public function getInode () {}

	public function getSize () {}

	public function getOwner () {}

	public function getGroup () {}

	public function getATime () {}

	public function getMTime () {}

	public function getCTime () {}

	public function getType () {}

	public function isReadable () {}

	public function isExecutable () {}

	public function isFile () {}

	public function isDir () {}

	public function isLink () {}

	public function getLinkTarget () {}

	public function getRealPath () {}

	/**
	 * @param class_name[optional]
	 */
	public function getFileInfo ($class_name) {}

	/**
	 * @param class_name[optional]
	 */
	public function getPathInfo ($class_name) {}

	/**
	 * @param open_mode[optional]
	 * @param use_include_path[optional]
	 * @param context[optional]
	 */
	public function openFile ($open_mode, $use_include_path, $context) {}

	/**
	 * @param class_name[optional]
	 */
	public function setFileClass ($class_name) {}

	/**
	 * @param class_name[optional]
	 */
	public function setInfoClass ($class_name) {}

	final public function _bad_state_ex () {}

}

class PharFileInfo extends SplFileInfo  {

	/**
	 * @param filename
	 */
	public function __construct ($filename) {}

	public function __destruct () {}

	/**
	 * @param perms
	 */
	public function chmod ($perms) {}

	/**
	 * @param compression_type
	 */
	public function compress ($compression_type) {}

	public function decompress () {}

	public function delMetadata () {}

	public function getCompressedSize () {}

	public function getCRC32 () {}

	public function getContent () {}

	public function getMetadata () {}

	public function getPharFlags () {}

	public function hasMetadata () {}

	/**
	 * @param compression_type[optional]
	 */
	public function isCompressed ($compression_type) {}

	public function isCRCChecked () {}

	/**
	 * @param metadata
	 */
	public function setMetadata ($metadata) {}

	public function getPath () {}

	public function getFilename () {}

	public function getExtension () {}

	/**
	 * @param suffix[optional]
	 */
	public function getBasename ($suffix) {}

	public function getPathname () {}

	public function getPerms () {}

	public function getInode () {}

	public function getSize () {}

	public function getOwner () {}

	public function getGroup () {}

	public function getATime () {}

	public function getMTime () {}

	public function getCTime () {}

	public function getType () {}

	public function isWritable () {}

	public function isReadable () {}

	public function isExecutable () {}

	public function isFile () {}

	public function isDir () {}

	public function isLink () {}

	public function getLinkTarget () {}

	public function getRealPath () {}

	/**
	 * @param class_name[optional]
	 */
	public function getFileInfo ($class_name) {}

	/**
	 * @param class_name[optional]
	 */
	public function getPathInfo ($class_name) {}

	/**
	 * @param open_mode[optional]
	 * @param use_include_path[optional]
	 * @param context[optional]
	 */
	public function openFile ($open_mode, $use_include_path, $context) {}

	/**
	 * @param class_name[optional]
	 */
	public function setFileClass ($class_name) {}

	/**
	 * @param class_name[optional]
	 */
	public function setInfoClass ($class_name) {}

	final public function _bad_state_ex () {}

	public function __toString () {}

}
// End of Phar v.2.0.2
?>
