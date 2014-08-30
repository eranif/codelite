<?php

// Start of Phar v.2.0.1

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
	 * Construct a Phar archive object
	 * @link http://www.php.net/manual/en/phar.construct.php
	 * @param fname string <p>
	 *       Path to an existing Phar archive or to-be-created archive
	 *      </p>
	 * @param flags int[optional] <p>
	 *       Flags to pass to parent class RecursiveDirectoryIterator.
	 *      </p>
	 * @param alias string[optional] <p>
	 *       Alias with which this Phar archive should be referred to in calls to stream
	 *       functionality.
	 *      </p>
	 * @return void 
	 */
	public function __construct ($fname, $flags = null, $alias = null) {}

	public function __destruct () {}

	/**
	 * Add an empty directory to the phar archive
	 * @link http://www.php.net/manual/en/phar.addemptydir.php
	 * @param dirname string <p>
	 *       The name of the empty directory to create in the phar archive
	 *      </p>
	 * @return void no return value, exception is thrown on failure.
	 */
	public function addEmptyDir ($dirname) {}

	/**
	 * Add a file from the filesystem to the phar archive
	 * @link http://www.php.net/manual/en/phar.addfile.php
	 * @param file string <p>
	 *       Full or relative path to a file on disk to be added
	 *       to the phar archive.
	 *      </p>
	 * @param localname string[optional] <p>
	 *       Path that the file will be stored in the archive.
	 *      </p>
	 * @return void no return value, exception is thrown on failure.
	 */
	public function addFile ($file, $localname = null) {}

	/**
	 * Add a file from the filesystem to the phar archive
	 * @link http://www.php.net/manual/en/phar.addfromstring.php
	 * @param localname string <p>
	 *       Path that the file will be stored in the archive.
	 *      </p>
	 * @param contents string <p>
	 *       The file contents to store
	 *      </p>
	 * @return void no return value, exception is thrown on failure.
	 */
	public function addFromString ($localname, $contents) {}

	/**
	 * Construct a phar archive from the files within a directory.
	 * @link http://www.php.net/manual/en/phar.buildfromdirectory.php
	 * @param base_dir string <p>
	 *       The full or relative path to the directory that contains all files
	 *       to add to the archive.
	 *      </p>
	 * @param regex string[optional] <p>
	 *       An optional pcre regular expression that is used to filter the
	 *       list of files.  Only file paths matching the regular expression
	 *       will be included in the archive.
	 *      </p>
	 * @return array Phar::buildFromDirectory returns an associative array
	 *   mapping internal path of file to the full path of the file on the
	 *   filesystem.
	 */
	public function buildFromDirectory ($base_dir, $regex = null) {}

	/**
	 * Construct a phar archive from an iterator.
	 * @link http://www.php.net/manual/en/phar.buildfromiterator.php
	 * @param iter Iterator <p>
	 *       Any iterator that either associatively maps phar file to location or
	 *       returns SplFileInfo objects
	 *      </p>
	 * @param base_directory string[optional] <p>
	 *       For iterators that return SplFileInfo objects, the portion of each
	 *       file's full path to remove when adding to the phar archive
	 *      </p>
	 * @return array Phar::buildFromIterator returns an associative array
	 *   mapping internal path of file to the full path of the file on the
	 *   filesystem.
	 */
	public function buildFromIterator ($iter, $base_directory = null) {}

	/**
	 * Compresses all files in the current Phar archive
	 * @link http://www.php.net/manual/en/phar.compressfiles.php
	 * @param compression int <p>
	 *       Compression must be one of Phar::GZ,
	 *       Phar::BZ2 to add compression, or Phar::NONE
	 *       to remove compression.
	 *      </p>
	 * @return void 
	 */
	public function compressFiles ($compression) {}

	/**
	 * Decompresses all files in the current Phar archive
	 * @link http://www.php.net/manual/en/phar.decompressfiles.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function decompressFiles () {}

	/**
	 * Compresses the entire Phar archive using Gzip or Bzip2 compression
	 * @link http://www.php.net/manual/en/phar.compress.php
	 * @param compression int <p>
	 *       Compression must be one of Phar::GZ,
	 *       Phar::BZ2 to add compression, or Phar::NONE
	 *       to remove compression.
	 *      </p>
	 * @param extension string[optional] <p>
	 *       By default, the extension is .phar.gz
	 *       or .phar.bz2 for compressing phar archives, and
	 *       .phar.tar.gz or .phar.tar.bz2 for
	 *       compressing tar archives.  For decompressing, the default file extensions
	 *       are .phar and .phar.tar.
	 *      </p>
	 * @return object a Phar object.
	 */
	public function compress ($compression, $extension = null) {}

	/**
	 * Decompresses the entire Phar archive
	 * @link http://www.php.net/manual/en/phar.decompress.php
	 * @param extension string[optional] <p>
	 *       For decompressing, the default file extensions
	 *       are .phar and .phar.tar.
	 *       Use this parameter to specify another file extension.  Be aware
	 *       that all executable phar archives must contain .phar
	 *       in their filename.
	 *      </p>
	 * @return object A Phar object is returned.
	 */
	public function decompress ($extension = null) {}

	/**
	 * Convert a phar archive to another executable phar archive file format
	 * @link http://www.php.net/manual/en/phar.converttoexecutable.php
	 * @param format int[optional] <p>
	 *       This should be one of Phar::PHAR, Phar::TAR,
	 *       or Phar::ZIP.  If set to &null;, the existing file format
	 *       will be preserved.
	 *      </p>
	 * @param compression int[optional] <p>
	 *       This should be one of Phar::NONE for no whole-archive
	 *       compression, Phar::GZ for zlib-based compression, and
	 *       Phar::BZ2 for bzip-based compression.
	 *      </p>
	 * @param extension string[optional] <p>
	 *       This parameter is used to override the default file extension for a
	 *       converted archive.  Note that all zip- and tar-based phar archives must contain
	 *       .phar in their file extension in order to be processed as a
	 *       phar archive.
	 *      </p>
	 *      <p>
	 *       If converting to a phar-based archive, the default extensions are
	 *       .phar, .phar.gz, or .phar.bz2
	 *       depending on the specified compression.  For tar-based phar archives, the
	 *       default extensions are .phar.tar, .phar.tar.gz,
	 *       and .phar.tar.bz2.  For zip-based phar archives, the
	 *       default extension is .phar.zip.
	 *      </p>
	 * @return Phar The method returns a Phar object on success and throws an
	 *   exception on failure.
	 */
	public function convertToExecutable ($format = null, $compression = null, $extension = null) {}

	/**
	 * Convert a phar archive to a non-executable tar or zip file
	 * @link http://www.php.net/manual/en/phar.converttodata.php
	 * @param format int[optional] <p>
	 *       This should be one of Phar::TAR
	 *       or Phar::ZIP.  If set to &null;, the existing file format
	 *       will be preserved.
	 *      </p>
	 * @param compression int[optional] <p>
	 *       This should be one of Phar::NONE for no whole-archive
	 *       compression, Phar::GZ for zlib-based compression, and
	 *       Phar::BZ2 for bzip-based compression.
	 *      </p>
	 * @param extension string[optional] <p>
	 *       This parameter is used to override the default file extension for a
	 *       converted archive.  Note that .phar cannot be used
	 *       anywhere in the filename for a non-executable tar or zip archive.
	 *      </p>
	 *      <p>
	 *       If converting to a tar-based phar archive, the
	 *       default extensions are .tar, .tar.gz,
	 *       and .tar.bz2 depending on specified compression.
	 *       For zip-based archives, the
	 *       default extension is .zip.
	 *      </p>
	 * @return PharData The method returns a PharData object on success and throws an
	 *   exception on failure.
	 */
	public function convertToData ($format = null, $compression = null, $extension = null) {}

	/**
	 * Copy a file internal to the phar archive to another new file within the phar
	 * @link http://www.php.net/manual/en/phar.copy.php
	 * @param oldfile string <p>
	 *      </p>
	 * @param newfile string <p>
	 *      </p>
	 * @return bool returns true on success, but it is safer to encase method call in a
	 *   try/catch block and assume success if no exception is thrown.
	 */
	public function copy ($oldfile, $newfile) {}

	/**
	 * Returns the number of entries (files) in the Phar archive
	 * @link http://www.php.net/manual/en/phar.count.php
	 * @return int The number of files contained within this phar, or 0 (the number zero)
	 *   if none.
	 */
	public function count () {}

	/**
	 * Delete a file within a phar archive
	 * @link http://www.php.net/manual/en/phar.delete.php
	 * @param entry string <p>
	 *       Path within an archive to the file to delete.
	 *      </p>
	 * @return bool returns true on success, but it is better to check for thrown exception,
	 *   and assume success if none is thrown.
	 */
	public function delete ($entry) {}

	/**
	 * Deletes the global metadata of the phar
	 * @link http://www.php.net/manual/en/phar.delmetadata.php
	 * @return bool returns true on success, but it is better to check for thrown exception,
	 *   and assume success if none is thrown.
	 */
	public function delMetadata () {}

	/**
	 * Extract the contents of a phar archive to a directory
	 * @link http://www.php.net/manual/en/phar.extractto.php
	 * @param pathto string <p>
	 *       Path within an archive to the file to delete.
	 *      </p>
	 * @param files string|array[optional] <p>
	 *       The name of a file or directory to extract, or an array of files/directories to extract
	 *      </p>
	 * @param overwrite bool[optional] <p>
	 *       Set to true to enable overwriting existing files
	 *      </p>
	 * @return bool returns true on success, but it is better to check for thrown exception,
	 *   and assume success if none is thrown.
	 */
	public function extractTo ($pathto, $files = null, $overwrite = null) {}

	public function getAlias () {}

	public function getPath () {}

	/**
	 * Returns phar archive meta-data
	 * @link http://www.php.net/manual/en/phar.getmetadata.php
	 * @return mixed any PHP variable that can be serialized and is stored as meta-data for the Phar archive,
	 *   or &null; if no meta-data is stored.
	 */
	public function getMetadata () {}

	/**
	 * Return whether phar was modified
	 * @link http://www.php.net/manual/en/phar.getmodified.php
	 * @return bool true if the phar has been modified since opened, false if not.
	 */
	public function getModified () {}

	/**
	 * Return MD5/SHA1/SHA256/SHA512/OpenSSL signature of a Phar archive
	 * @link http://www.php.net/manual/en/phar.getsignature.php
	 * @return array Array with the opened archive's signature in hash key and MD5,
	 *   SHA-1,
	 *   SHA-256, SHA-512, or OpenSSL
	 *   in hash_type.  This signature is a hash calculated on the
	 *   entire phar's contents, and may be used to verify the integrity of the archive.
	 *   A valid signature is absolutely required of all executable phar archives if the
	 *   phar.require_hash INI variable
	 *   is set to true.
	 */
	public function getSignature () {}

	/**
	 * Return the PHP loader or bootstrap stub of a Phar archive
	 * @link http://www.php.net/manual/en/phar.getstub.php
	 * @return string a string containing the contents of the bootstrap loader (stub) of
	 *   the current Phar archive.
	 */
	public function getStub () {}

	/**
	 * Return version info of Phar archive
	 * @link http://www.php.net/manual/en/phar.getversion.php
	 * @return string The opened archive's API version.  This is not to be confused with
	 *   the API version that the loaded phar extension will use to create
	 *   new phars.  Each Phar archive has the API version hard-coded into
	 *   its manifest.  See Phar file format
	 *   documentation for more information.
	 */
	public function getVersion () {}

	/**
	 * Returns whether phar has global meta-data
	 * @link http://www.php.net/manual/en/phar.hasmetadata.php
	 * @return bool true if meta-data has been set, and false if not.
	 */
	public function hasMetadata () {}

	/**
	 * Used to determine whether Phar write operations are being buffered, or are flushing directly to disk
	 * @link http://www.php.net/manual/en/phar.isbuffering.php
	 * @return bool true if the write operations are being buffer, false otherwise.
	 */
	public function isBuffering () {}

	/**
	 * Returns Phar::GZ or PHAR::BZ2 if the entire phar archive is compressed (.tar.gz/tar.bz and so on)
	 * @link http://www.php.net/manual/en/phar.iscompressed.php
	 * @return mixed Phar::GZ, Phar::BZ2 or false
	 */
	public function isCompressed () {}

	/**
	 * Returns true if the phar archive is based on the tar/phar/zip file format depending on the parameter
	 * @link http://www.php.net/manual/en/phar.isfileformat.php
	 * @param format int <p>
	 *       Either Phar::PHAR, Phar::TAR, or
	 *       Phar::ZIP to test for the format of the archive.
	 *      </p>
	 * @return bool true if the phar archive matches the file format requested by the parameter
	 */
	public function isFileFormat ($format) {}

	/**
	 * Returns true if the phar archive can be modified
	 * @link http://www.php.net/manual/en/phar.iswritable.php
	 * @return bool true if the phar archive can be modified
	 */
	public function isWritable () {}

	/**
	 * determines whether a file exists in the phar
	 * @link http://www.php.net/manual/en/phar.offsetexists.php
	 * @param offset string <p>
	 *       The filename (relative path) to look for in a Phar.
	 *      </p>
	 * @return bool true if the file exists within the phar, or false if not.
	 */
	public function offsetExists ($offset) {}

	/**
	 * Gets a <classname>PharFileInfo</classname> object for a specific file
	 * @link http://www.php.net/manual/en/phar.offsetget.php
	 * @param offset string <p>
	 *       The filename (relative path) to look for in a Phar.
	 *      </p>
	 * @return int A PharFileInfo object is returned that can be used to
	 *   iterate over a file's contents or to retrieve information about the current file.
	 */
	public function offsetGet ($offset) {}

	/**
	 * set the contents of an internal file to those of an external file
	 * @link http://www.php.net/manual/en/phar.offsetset.php
	 * @param offset string <p>
	 *       The filename (relative path) to modify in a Phar.
	 *      </p>
	 * @param value string <p>
	 *       Content of the file.
	 *      </p>
	 * @return void No return values.
	 */
	public function offsetSet ($offset, $value) {}

	/**
	 * remove a file from a phar
	 * @link http://www.php.net/manual/en/phar.offsetunset.php
	 * @param offset string <p>
	 *       The filename (relative path) to modify in a Phar.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function offsetUnset ($offset) {}

	/**
	 * Set the alias for the Phar archive
	 * @link http://www.php.net/manual/en/phar.setalias.php
	 * @param alias string <p>
	 *       A shorthand string that this archive can be referred to in phar
	 *       stream wrapper access.
	 *      </p>
	 * @return bool 
	 */
	public function setAlias ($alias) {}

	/**
	 * Used to set the PHP loader or bootstrap stub of a Phar archive to the default loader
	 * @link http://www.php.net/manual/en/phar.setdefaultstub.php
	 * @param index string[optional] <p>
	 *       Relative path within the phar archive to run if accessed on the command-line
	 *      </p>
	 * @param webindex string[optional] <p>
	 *       Relative path within the phar archive to run if accessed through a web browser
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function setDefaultStub ($index = null, $webindex = null) {}

	/**
	 * Sets phar archive meta-data
	 * @link http://www.php.net/manual/en/phardata.setmetadata.php
	 * @param metadata mixed <p>
	 *       Any PHP variable containing information to store that describes the phar archive
	 *      </p>
	 * @param metadata mixed 
	 * @return void 
	 */
	public function setMetadata ($metadata, $metadata) {}

	/**
	 * set the signature algorithm for a phar and apply it.  The
	 * @link http://www.php.net/manual/en/phardata.setsignaturealgorithm.php
	 * @param sigtype int <p>
	 *       One of Phar::MD5,
	 *   Phar::SHA1, Phar::SHA256,
	 *   Phar::SHA512, or Phar::PGP
	 *      </p>
	 * @param privatekey string[optional] <p>
	 *       The contents of an OpenSSL private key, as extracted from a certificate or
	 *       OpenSSL key file:
	 *       
	 *        setSignatureAlgorithm(Phar::OPENSSL, $pkey);
	 * ?>
	 *        ]]>
	 *       
	 *       See phar introduction for instructions on
	 *       naming and placement of the public key file.
	 *      </p>
	 * @param sigtype int 
	 * @return void 
	 */
	public function setSignatureAlgorithm ($sigtype, $privatekey = null, $sigtype) {}

	/**
	 * Used to set the PHP loader or bootstrap stub of a Phar archive
	 * @link http://www.php.net/manual/en/phar.setstub.php
	 * @param stub string <p>
	 *       A string or an open stream handle to use as the executable stub for this
	 *       phar archive.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function setStub ($stub) {}

	/**
	 * Start buffering Phar write operations, do not modify the Phar object on disk
	 * @link http://www.php.net/manual/en/phar.startbuffering.php
	 * @return void 
	 */
	public function startBuffering () {}

	/**
	 * Stop buffering write requests to the Phar archive, and save changes to disk
	 * @link http://www.php.net/manual/en/phar.stopbuffering.php
	 * @return void 
	 */
	public function stopBuffering () {}

	/**
	 * Returns the api version
	 * @link http://www.php.net/manual/en/phar.apiversion.php
	 * @return string The API version string as in &quot;1.0.0&quot;.
	 */
	final public static function apiVersion () {}

	/**
	 * Returns whether phar extension supports compression using either zlib or bzip2
	 * @link http://www.php.net/manual/en/phar.cancompress.php
	 * @param type int[optional] <p>
	 *       Either Phar::GZ or Phar::BZ2 can be
	 *       used to test whether compression is possible with a specific compression
	 *       algorithm (zlib or bzip2).
	 *      </p>
	 * @return bool true if compression/decompression is available, false if not.
	 */
	final public static function canCompress ($type = null) {}

	/**
	 * Returns whether phar extension supports writing and creating phars
	 * @link http://www.php.net/manual/en/phar.canwrite.php
	 * @return bool true if write access is enabled, false if it is disabled.
	 */
	final public static function canWrite () {}

	/**
	 * Create a phar-file format specific stub
	 * @link http://www.php.net/manual/en/phar.createdefaultstub.php
	 * @param indexfile string[optional] 
	 * @param webindexfile string[optional] 
	 * @return string a string containing the contents of a customized bootstrap loader (stub)
	 *   that allows the created Phar archive to work with or without the Phar extension
	 *   enabled.
	 */
	final public static function createDefaultStub ($indexfile = null, $webindexfile = null) {}

	/**
	 * Return array of supported compression algorithms
	 * @link http://www.php.net/manual/en/phar.getsupportedcompression.php
	 * @return array an array containing any of Phar::GZ or
	 *   Phar::BZ2, depending on the availability of
	 *   the zlib extension or the
	 *   bz2 extension.
	 */
	final public static function getSupportedCompression () {}

	/**
	 * Return array of supported signature types
	 * @link http://www.php.net/manual/en/phar.getsupportedsignatures.php
	 * @return array an array containing any of MD5, SHA-1,
	 *   SHA-256, SHA-512, or OpenSSL.
	 */
	final public static function getSupportedSignatures () {}

	/**
	 * instructs phar to intercept fopen, file_get_contents, opendir, and all of the stat-related functions
	 * @link http://www.php.net/manual/en/phar.interceptfilefuncs.php
	 * @return void 
	 */
	final public static function interceptFileFuncs () {}

	/**
	 * Returns whether the given filename is a valid phar filename
	 * @link http://www.php.net/manual/en/phar.isvalidpharfilename.php
	 * @param filename string <p>
	 *       The name or full path to a phar archive not yet created
	 *      </p>
	 * @param executable bool[optional] <p>
	 *       This parameter determines whether the filename should be treated as
	 *       a phar executable archive, or a data non-executable archive
	 *      </p>
	 * @return bool true if the filename is valid, false if not.
	 */
	final public static function isValidPharFilename ($filename, $executable = null) {}

	/**
	 * Loads any phar archive with an alias
	 * @link http://www.php.net/manual/en/phar.loadphar.php
	 * @param filename string <p>
	 *       the full or relative path to the phar archive to open
	 *      </p>
	 * @param alias string[optional] <p>
	 *       The alias that may be used to refer to the phar archive.  Note
	 *       that many phar archives specify an explicit alias inside the
	 *       phar archive, and a PharException will be thrown if
	 *       a new alias is specified in this case.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	final public static function loadPhar ($filename, $alias = null) {}

	/**
	 * Reads the currently executed file (a phar) and registers its manifest
	 * @link http://www.php.net/manual/en/phar.mapphar.php
	 * @param alias string[optional] <p>
	 *       The alias that can be used in phar:// URLs to
	 *       refer to this archive, rather than its full path.
	 *      </p>
	 * @param dataoffset int[optional] <p>
	 *       Unused variable, here for compatibility with PEAR's PHP_Archive.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	final public static function mapPhar ($alias = null, $dataoffset = null) {}

	/**
	 * Returns the full path on disk or full phar URL to the currently executing Phar archive
	 * @link http://www.php.net/manual/en/phar.running.php
	 * @param retphar bool[optional] <p>
	 *       If false, the full path on disk to the phar
	 *       archive is returned.  If true, a full phar URL is returned.
	 *      </p>
	 * @return string the filename if valid, empty string otherwise.
	 */
	final public static function running ($retphar = null) {}

	/**
	 * Mount an external path or file to a virtual location within the phar archive
	 * @link http://www.php.net/manual/en/phar.mount.php
	 * @param pharpath string <p>
	 *       The internal path within the phar archive to use as the mounted path location.
	 *       If executed within a phar archive, this may be a relative path, otherwise this must
	 *       be a full phar URL.
	 *      </p>
	 * @param externalpath string <p>
	 *       A path or URL to an external file or directory to mount within the phar archive
	 *      </p>
	 * @return void No return.  PharException is thrown on failure.
	 */
	final public static function mount ($pharpath, $externalpath) {}

	/**
	 * Defines a list of up to 4 $_SERVER variables that should be modified for execution
	 * @link http://www.php.net/manual/en/phar.mungserver.php
	 * @param munglist array <p>
	 *       an array containing as string indices any of
	 *       REQUEST_URI, PHP_SELF,
	 *       SCRIPT_NAME and SCRIPT_FILENAME.
	 *       Other values trigger an exception, and Phar::mungServer
	 *       is case-sensitive.
	 *      </p>
	 * @return void No return.
	 */
	final public static function mungServer (array $munglist) {}

	/**
	 * Completely remove a phar archive from disk and from memory
	 * @link http://www.php.net/manual/en/phar.unlinkarchive.php
	 * @param archive string <p>
	 *       The path on disk to the phar archive.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	final public static function unlinkArchive ($archive) {}

	/**
	 * mapPhar for web-based phars. front controller for web applications
	 * @link http://www.php.net/manual/en/phar.webphar.php
	 * @param alias string[optional] <p>
	 *       The alias that can be used in phar:// URLs to
	 *       refer to this archive, rather than its full path.
	 *      </p>
	 * @param index string[optional] <p>
	 *       The location within the phar of the directory index.
	 *      </p>
	 * @param f404 string[optional] <p>
	 *       The location of the script to run when a file is not found.  This
	 *       script should output the proper HTTP 404 headers.
	 *      </p>
	 * @param mimetypes array[optional] <p>
	 *       An array mapping additional file extensions to MIME type.
	 *       If the default mapping is sufficient, pass an empty array.
	 *       By default, these extensions are mapped to these MIME types:
	 *       
	 *         Phar::PHPS, // pass to highlight_file()
	 *    'c' => 'text/plain',
	 *    'cc' => 'text/plain',
	 *    'cpp' => 'text/plain',
	 *    'c++' => 'text/plain',
	 *    'dtd' => 'text/plain',
	 *    'h' => 'text/plain',
	 *    'log' => 'text/plain',
	 *    'rng' => 'text/plain',
	 *    'txt' => 'text/plain',
	 *    'xsd' => 'text/plain',
	 *    'php' => Phar::PHP, // parse as PHP
	 *    'inc' => Phar::PHP, // parse as PHP
	 *    'avi' => 'video/avi',
	 *    'bmp' => 'image/bmp',
	 *    'css' => 'text/css',
	 *    'gif' => 'image/gif',
	 *    'htm' => 'text/html',
	 *    'html' => 'text/html',
	 *    'htmls' => 'text/html',
	 *    'ico' => 'image/x-ico',
	 *    'jpe' => 'image/jpeg',
	 *    'jpg' => 'image/jpeg',
	 *    'jpeg' => 'image/jpeg',
	 *    'js' => 'application/x-javascript',
	 *    'midi' => 'audio/midi',
	 *    'mid' => 'audio/midi',
	 *    'mod' => 'audio/mod',
	 *    'mov' => 'movie/quicktime',
	 *    'mp3' => 'audio/mp3',
	 *    'mpg' => 'video/mpeg',
	 *    'mpeg' => 'video/mpeg',
	 *    'pdf' => 'application/pdf',
	 *    'png' => 'image/png',
	 *    'swf' => 'application/shockwave-flash',
	 *    'tif' => 'image/tiff',
	 *    'tiff' => 'image/tiff',
	 *    'wav' => 'audio/wav',
	 *    'xbm' => 'image/xbm',
	 *    'xml' => 'text/xml',
	 * );
	 * ?>
	 *        ]]>
	 *       
	 *      </p>
	 * @param rewrites array[optional] <p>
	 *       An array mapping URI to internal file, simulating mod_rewrite of apache.
	 *       For example:
	 *       
	 *         'myinfo.php'
	 * );
	 * ?>
	 *        ]]>
	 *       
	 *       would route calls to http:///myphar.phar/myinfo
	 *       to the file phar:///path/to/myphar.phar/myinfo.php, preserving
	 *       GET/POST.  This does not quite work like mod_rewrite in that it would not
	 *       match http:///myphar.phar/myinfo/another.
	 *      </p>
	 * @return void 
	 */
	final public static function webPhar ($alias = null, $index = null, $f404 = null, array $mimetypes = null, array $rewrites = null) {}

	/**
	 * Returns whether current entry is a directory and not '.' or '..'
	 * @link http://www.php.net/manual/en/recursivedirectoryiterator.haschildren.php
	 * @param allow_links bool[optional] <p>
	 *       
	 *      </p>
	 * @return bool whether the current entry is a directory, but not '.' or '..'
	 */
	public function hasChildren ($allow_links = null) {}

	/**
	 * Returns an iterator for the current entry if it is a directory
	 * @link http://www.php.net/manual/en/recursivedirectoryiterator.getchildren.php
	 * @return object An iterator for the current entry, if it is a directory.
	 */
	public function getChildren () {}

	/**
	 * Get sub path
	 * @link http://www.php.net/manual/en/recursivedirectoryiterator.getsubpath.php
	 * @return string The sub path (sub directory).
	 */
	public function getSubPath () {}

	/**
	 * Get sub path and name
	 * @link http://www.php.net/manual/en/recursivedirectoryiterator.getsubpathname.php
	 * @return string The sub path (sub directory) and filename.
	 */
	public function getSubPathname () {}

	/**
	 * Rewinds back to the beginning
	 * @link http://www.php.net/manual/en/filesystemiterator.rewind.php
	 * @return void 
	 */
	public function rewind () {}

	/**
	 * Move to the next file
	 * @link http://www.php.net/manual/en/filesystemiterator.next.php
	 * @return void 
	 */
	public function next () {}

	/**
	 * Retrieve the key for the current file
	 * @link http://www.php.net/manual/en/filesystemiterator.key.php
	 * @return string the pathname or filename depending on the set flags.
	 *   See the FilesystemIterator constants.
	 */
	public function key () {}

	/**
	 * The current file
	 * @link http://www.php.net/manual/en/filesystemiterator.current.php
	 * @return mixed The filename, file information, or $this depending on the set flags.
	 *   See the FilesystemIterator constants.
	 */
	public function current () {}

	/**
	 * Get the handling flags
	 * @link http://www.php.net/manual/en/filesystemiterator.getflags.php
	 * @return int The integer value of the set flags.
	 */
	public function getFlags () {}

	/**
	 * Sets handling flags
	 * @link http://www.php.net/manual/en/filesystemiterator.setflags.php
	 * @param flags int[optional] <p>
	 *       The handling flags to set.
	 *       See the FilesystemIterator constants.
	 *      </p>
	 * @return void 
	 */
	public function setFlags ($flags = null) {}

	/**
	 * Return file name of current DirectoryIterator item.
	 * @link http://www.php.net/manual/en/directoryiterator.getfilename.php
	 * @return string the file name of the current DirectoryIterator item.
	 */
	public function getFilename () {}

	/**
	 * Returns the file extension component of path
	 * @link http://www.php.net/manual/en/directoryiterator.getextension.php
	 * @return string 
	 */
	public function getExtension () {}

	/**
	 * Get base name of current DirectoryIterator item.
	 * @link http://www.php.net/manual/en/directoryiterator.getbasename.php
	 * @param suffix string[optional] <p>
	 *       If the base name ends in suffix, 
	 *       this will be cut.
	 *      </p>
	 * @return string The base name of the current DirectoryIterator item.
	 */
	public function getBasename ($suffix = null) {}

	/**
	 * Determine if current DirectoryIterator item is '.' or '..'
	 * @link http://www.php.net/manual/en/directoryiterator.isdot.php
	 * @return bool true if the entry is . or ..,
	 *   otherwise false
	 */
	public function isDot () {}

	/**
	 * Check whether current DirectoryIterator position is a valid file
	 * @link http://www.php.net/manual/en/directoryiterator.valid.php
	 * @return bool true if the position is valid, otherwise false
	 */
	public function valid () {}

	/**
	 * Seek to a DirectoryIterator item
	 * @link http://www.php.net/manual/en/directoryiterator.seek.php
	 * @param position int <p>
	 *       The zero-based numeric position to seek to.
	 *      </p>
	 * @return void 
	 */
	public function seek ($position) {}

	/**
	 * Get file name as a string
	 * @link http://www.php.net/manual/en/directoryiterator.tostring.php
	 * @return string the file name of the current DirectoryIterator item.
	 */
	public function __toString () {}

	/**
	 * Gets the path to the file
	 * @link http://www.php.net/manual/en/splfileinfo.getpathname.php
	 * @return string The path to the file.
	 */
	public function getPathname () {}

	/**
	 * Gets file permissions
	 * @link http://www.php.net/manual/en/splfileinfo.getperms.php
	 * @return int the file permissions.
	 */
	public function getPerms () {}

	/**
	 * Gets the inode for the file
	 * @link http://www.php.net/manual/en/splfileinfo.getinode.php
	 * @return int the inode number for the filesystem object.
	 */
	public function getInode () {}

	/**
	 * Gets file size
	 * @link http://www.php.net/manual/en/splfileinfo.getsize.php
	 * @return int The filesize in bytes.
	 */
	public function getSize () {}

	/**
	 * Gets the owner of the file
	 * @link http://www.php.net/manual/en/splfileinfo.getowner.php
	 * @return int The owner id in numerical format.
	 */
	public function getOwner () {}

	/**
	 * Gets the file group
	 * @link http://www.php.net/manual/en/splfileinfo.getgroup.php
	 * @return int The group id in numerical format.
	 */
	public function getGroup () {}

	/**
	 * Gets last access time of the file
	 * @link http://www.php.net/manual/en/splfileinfo.getatime.php
	 * @return int the time the file was last accessed.
	 */
	public function getATime () {}

	/**
	 * Gets the last modified time
	 * @link http://www.php.net/manual/en/splfileinfo.getmtime.php
	 * @return int the last modified time for the file, in a Unix timestamp.
	 */
	public function getMTime () {}

	/**
	 * Gets the inode change time
	 * @link http://www.php.net/manual/en/splfileinfo.getctime.php
	 * @return int The last change time, in a Unix timestamp.
	 */
	public function getCTime () {}

	/**
	 * Gets file type
	 * @link http://www.php.net/manual/en/splfileinfo.gettype.php
	 * @return string A string representing the type of the entry.
	 *   May be one of file, link,
	 *   or dir
	 */
	public function getType () {}

	/**
	 * Tells if file is readable
	 * @link http://www.php.net/manual/en/splfileinfo.isreadable.php
	 * @return bool true if readable, false otherwise.
	 */
	public function isReadable () {}

	/**
	 * Tells if the file is executable
	 * @link http://www.php.net/manual/en/splfileinfo.isexecutable.php
	 * @return bool true if executable, false otherwise.
	 */
	public function isExecutable () {}

	/**
	 * Tells if the object references a regular file
	 * @link http://www.php.net/manual/en/splfileinfo.isfile.php
	 * @return bool true if the file exists and is a regular file (not a link), false otherwise.
	 */
	public function isFile () {}

	/**
	 * Tells if the file is a directory
	 * @link http://www.php.net/manual/en/splfileinfo.isdir.php
	 * @return bool true if a directory, false otherwise.
	 */
	public function isDir () {}

	/**
	 * Tells if the file is a link
	 * @link http://www.php.net/manual/en/splfileinfo.islink.php
	 * @return bool true if the file is a link, false otherwise.
	 */
	public function isLink () {}

	/**
	 * Gets the target of a link
	 * @link http://www.php.net/manual/en/splfileinfo.getlinktarget.php
	 * @return string the target of the filesystem link.
	 */
	public function getLinkTarget () {}

	/**
	 * Gets absolute path to file
	 * @link http://www.php.net/manual/en/splfileinfo.getrealpath.php
	 * @return string the path to the file.
	 */
	public function getRealPath () {}

	/**
	 * Gets an SplFileInfo object for the file
	 * @link http://www.php.net/manual/en/splfileinfo.getfileinfo.php
	 * @param class_name string[optional] <p>
	 *       Name of an SplFileInfo derived class to use. 
	 *      </p>
	 * @return SplFileInfo An SplFileInfo object created for the file.
	 */
	public function getFileInfo ($class_name = null) {}

	/**
	 * Gets an SplFileInfo object for the path
	 * @link http://www.php.net/manual/en/splfileinfo.getpathinfo.php
	 * @param class_name string[optional] <p>
	 *       Name of an SplFileInfo derived class to use.
	 *      </p>
	 * @return SplFileInfo an SplFileInfo object for the parent path of the file.
	 */
	public function getPathInfo ($class_name = null) {}

	/**
	 * Gets an SplFileObject object for the file
	 * @link http://www.php.net/manual/en/splfileinfo.openfile.php
	 * @param open_mode string[optional] <p>
	 *       The mode for opening the file. See the fopen
	 *       documentation for descriptions of possible modes. The default 
	 *       is read only.
	 *      </p>
	 * @param use_include_path bool[optional] <p>
	 *       &parameter.use_include_path;
	 *      </p>
	 * @param context resource[optional] <p>
	 *       &parameter.context;
	 *      </p>
	 * @return SplFileObject The opened file as an SplFileObject object.
	 */
	public function openFile ($open_mode = null, $use_include_path = null, $context = null) {}

	/**
	 * Sets the class name used with <methodname>SplFileInfo::openFile</methodname>
	 * @link http://www.php.net/manual/en/splfileinfo.setfileclass.php
	 * @param class_name string[optional] <p>
	 *       The class name to use when openFile() is called. 
	 *      </p>
	 * @return void 
	 */
	public function setFileClass ($class_name = null) {}

	/**
	 * Sets the class used with getFileInfo and getPathInfo
	 * @link http://www.php.net/manual/en/splfileinfo.setinfoclass.php
	 * @param class_name string[optional] <p>
	 *       The class name to use.
	 *      </p>
	 * @return void 
	 */
	public function setInfoClass ($class_name = null) {}

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
	const SKIP_DOTS = 4096;
	const UNIX_PATHS = 8192;


	/**
	 * Construct a non-executable tar or zip archive object
	 * @link http://www.php.net/manual/en/phardata.construct.php
	 * @param fname string <p>
	 *       Path to an existing tar/zip archive or to-be-created archive
	 *      </p>
	 * @param flags int[optional] <p>
	 *       Flags to pass to Phar parent class
	 *       RecursiveDirectoryIterator.
	 *      </p>
	 * @param alias string[optional] <p>
	 *       Alias with which this Phar archive should be referred to in calls to stream
	 *       functionality.
	 *      </p>
	 * @param format int[optional] <p>
	 *       One of the
	 *       file format constants
	 *       available within the Phar class.
	 *      </p>
	 * @return void 
	 */
	public function __construct ($fname, $flags = null, $alias = null, $format = null) {}

	public function __destruct () {}

	/**
	 * Add an empty directory to the tar/zip archive
	 * @link http://www.php.net/manual/en/phardata.addemptydir.php
	 * @param dirname string <p>
	 *       The name of the empty directory to create in the phar archive
	 *      </p>
	 * @return bool no return value, exception is thrown on failure.
	 */
	public function addEmptyDir ($dirname) {}

	/**
	 * Add a file from the filesystem to the tar/zip archive
	 * @link http://www.php.net/manual/en/phardata.addfile.php
	 * @param file string <p>
	 *       Full or relative path to a file on disk to be added
	 *       to the phar archive.
	 *      </p>
	 * @param localname string[optional] <p>
	 *       Path that the file will be stored in the archive.
	 *      </p>
	 * @return void no return value, exception is thrown on failure.
	 */
	public function addFile ($file, $localname = null) {}

	/**
	 * Add a file from the filesystem to the tar/zip archive
	 * @link http://www.php.net/manual/en/phardata.addfromstring.php
	 * @param localname string <p>
	 *       Path that the file will be stored in the archive.
	 *      </p>
	 * @param contents string <p>
	 *       The file contents to store
	 *      </p>
	 * @return bool no return value, exception is thrown on failure.
	 */
	public function addFromString ($localname, $contents) {}

	/**
	 * Construct a tar/zip archive from the files within a directory.
	 * @link http://www.php.net/manual/en/phardata.buildfromdirectory.php
	 * @param base_dir string <p>
	 *       The full or relative path to the directory that contains all files
	 *       to add to the archive.
	 *      </p>
	 * @param regex string[optional] <p>
	 *       An optional pcre regular expression that is used to filter the
	 *       list of files.  Only file paths matching the regular expression
	 *       will be included in the archive.
	 *      </p>
	 * @return array Phar::buildFromDirectory returns an associative array
	 *   mapping internal path of file to the full path of the file on the
	 *   filesystem.
	 */
	public function buildFromDirectory ($base_dir, $regex = null) {}

	/**
	 * Construct a tar or zip archive from an iterator.
	 * @link http://www.php.net/manual/en/phardata.buildfromiterator.php
	 * @param iter Iterator <p>
	 *       Any iterator that either associatively maps tar/zip file to location or
	 *       returns SplFileInfo objects
	 *      </p>
	 * @param base_directory string[optional] <p>
	 *       For iterators that return SplFileInfo objects, the portion of each
	 *       file's full path to remove when adding to the tar/zip archive
	 *      </p>
	 * @return array PharData::buildFromIterator returns an associative array
	 *   mapping internal path of file to the full path of the file on the
	 *   filesystem.
	 */
	public function buildFromIterator ($iter, $base_directory = null) {}

	/**
	 * Compresses all files in the current tar/zip archive
	 * @link http://www.php.net/manual/en/phardata.compressfiles.php
	 * @param compression int <p>
	 *       Compression must be one of Phar::GZ,
	 *       Phar::BZ2 to add compression, or Phar::NONE
	 *       to remove compression.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function compressFiles ($compression) {}

	/**
	 * Decompresses all files in the current zip archive
	 * @link http://www.php.net/manual/en/phardata.decompressfiles.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function decompressFiles () {}

	/**
	 * Compresses the entire tar/zip archive using Gzip or Bzip2 compression
	 * @link http://www.php.net/manual/en/phardata.compress.php
	 * @param compression int <p>
	 *       Compression must be one of Phar::GZ,
	 *       Phar::BZ2 to add compression, or Phar::NONE
	 *       to remove compression.
	 *      </p>
	 * @param extension string[optional] <p>
	 *       By default, the extension is .tar.gz or .tar.bz2
	 *       for compressing a tar, and .tar for decompressing.
	 *      </p>
	 * @return object A PharData object is returned.
	 */
	public function compress ($compression, $extension = null) {}

	/**
	 * Decompresses the entire Phar archive
	 * @link http://www.php.net/manual/en/phardata.decompress.php
	 * @param extension string[optional] <p>
	 *       For decompressing, the default file extension
	 *       is .phar.tar.
	 *       Use this parameter to specify another file extension.  Be aware
	 *       that no non-executable archives cannot contain .phar
	 *       in their filename.
	 *      </p>
	 * @return object A PharData object is returned.
	 */
	public function decompress ($extension = null) {}

	/**
	 * Convert a non-executable tar/zip archive to an executable phar archive
	 * @link http://www.php.net/manual/en/phardata.converttoexecutable.php
	 * @param format int[optional] <p>
	 *       This should be one of Phar::PHAR, Phar::TAR,
	 *       or Phar::ZIP.  If set to &null;, the existing file format
	 *       will be preserved.
	 *      </p>
	 * @param compression int[optional] <p>
	 *       This should be one of Phar::NONE for no whole-archive
	 *       compression, Phar::GZ for zlib-based compression, and
	 *       Phar::BZ2 for bzip-based compression.
	 *      </p>
	 * @param extension string[optional] <p>
	 *       This parameter is used to override the default file extension for a
	 *       converted archive.  Note that all zip- and tar-based phar archives must contain
	 *       .phar in their file extension in order to be processed as a
	 *       phar archive.
	 *      </p>
	 *      <p>
	 *       If converting to a phar-based archive, the default extensions are
	 *       .phar, .phar.gz, or .phar.bz2
	 *       depending on the specified compression.  For tar-based phar archives, the
	 *       default extensions are .phar.tar, .phar.tar.gz,
	 *       and .phar.tar.bz2.  For zip-based phar archives, the
	 *       default extension is .phar.zip.
	 *      </p>
	 * @return Phar The method returns a Phar object on success and throws an
	 *   exception on failure.
	 */
	public function convertToExecutable ($format = null, $compression = null, $extension = null) {}

	/**
	 * Convert a phar archive to a non-executable tar or zip file
	 * @link http://www.php.net/manual/en/phardata.converttodata.php
	 * @param format int[optional] <p>
	 *       This should be one of Phar::TAR
	 *       or Phar::ZIP.  If set to &null;, the existing file format
	 *       will be preserved.
	 *      </p>
	 * @param compression int[optional] <p>
	 *       This should be one of Phar::NONE for no whole-archive
	 *       compression, Phar::GZ for zlib-based compression, and
	 *       Phar::BZ2 for bzip-based compression.
	 *      </p>
	 * @param extension string[optional] <p>
	 *       This parameter is used to override the default file extension for a
	 *       converted archive.  Note that .phar cannot be used
	 *       anywhere in the filename for a non-executable tar or zip archive.
	 *      </p>
	 *      <p>
	 *       If converting to a tar-based phar archive, the
	 *       default extensions are .tar, .tar.gz,
	 *       and .tar.bz2 depending on specified compression.
	 *       For zip-based archives, the
	 *       default extension is .zip.
	 *      </p>
	 * @return PharData The method returns a PharData object on success and throws an
	 *   exception on failure.
	 */
	public function convertToData ($format = null, $compression = null, $extension = null) {}

	/**
	 * Copy a file internal to the phar archive to another new file within the phar
	 * @link http://www.php.net/manual/en/phardata.copy.php
	 * @param oldfile string <p>
	 *      </p>
	 * @param newfile string <p>
	 *      </p>
	 * @return bool returns true on success, but it is safer to encase method call in a
	 *   try/catch block and assume success if no exception is thrown.
	 */
	public function copy ($oldfile, $newfile) {}

	public function count () {}

	/**
	 * Delete a file within a tar/zip archive
	 * @link http://www.php.net/manual/en/phardata.delete.php
	 * @param entry string <p>
	 *       Path within an archive to the file to delete.
	 *      </p>
	 * @return bool returns true on success, but it is better to check for thrown exception,
	 *   and assume success if none is thrown.
	 */
	public function delete ($entry) {}

	/**
	 * Deletes the global metadata of a zip archive
	 * @link http://www.php.net/manual/en/phardata.delmetadata.php
	 * @return bool returns true on success, but it is better to check for thrown exception,
	 *   and assume success if none is thrown.
	 */
	public function delMetadata () {}

	/**
	 * Extract the contents of a tar/zip archive to a directory
	 * @link http://www.php.net/manual/en/phardata.extractto.php
	 * @param pathto string <p>
	 *       Path within an archive to the file to delete.
	 *      </p>
	 * @param files string|array[optional] <p>
	 *       The name of a file or directory to extract, or an array of files/directories to extract
	 *      </p>
	 * @param overwrite bool[optional] <p>
	 *       Set to true to enable overwriting existing files
	 *      </p>
	 * @return bool returns true on success, but it is better to check for thrown exception,
	 *   and assume success if none is thrown.
	 */
	public function extractTo ($pathto, $files = null, $overwrite = null) {}

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

	/**
	 * Returns true if the tar/zip archive can be modified
	 * @link http://www.php.net/manual/en/phardata.iswritable.php
	 * @return bool true if the tar/zip archive can be modified
	 */
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
	 * set the contents of a file within the tar/zip to those of an external file or string
	 * @link http://www.php.net/manual/en/phardata.offsetset.php
	 * @param offset string <p>
	 *       The filename (relative path) to modify in a tar or zip archive.
	 *      </p>
	 * @param value string <p>
	 *       Content of the file.
	 *      </p>
	 * @return void No return values.
	 */
	public function offsetSet ($offset, $value) {}

	/**
	 * remove a file from a tar/zip archive
	 * @link http://www.php.net/manual/en/phardata.offsetunset.php
	 * @param offset string <p>
	 *       The filename (relative path) to modify in the tar/zip archive.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function offsetUnset ($offset) {}

	/**
	 * dummy function (Phar::setAlias is not valid for PharData)
	 * @link http://www.php.net/manual/en/phardata.setalias.php
	 * @param alias string <p>
	 *       A shorthand string that this archive can be referred to in phar
	 *       stream wrapper access.  This parameter is ignored.
	 *      </p>
	 * @return bool 
	 */
	public function setAlias ($alias) {}

	/**
	 * dummy function (Phar::setDefaultStub is not valid for PharData)
	 * @link http://www.php.net/manual/en/phardata.setdefaultstub.php
	 * @param index string[optional] <p>
	 *       Relative path within the phar archive to run if accessed on the command-line
	 *      </p>
	 * @param webindex string[optional] <p>
	 *       Relative path within the phar archive to run if accessed through a web browser
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function setDefaultStub ($index = null, $webindex = null) {}

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
	 * dummy function (Phar::setStub is not valid for PharData)
	 * @link http://www.php.net/manual/en/phardata.setstub.php
	 * @param stub string <p>
	 *       A string or an open stream handle to use as the executable stub for this
	 *       phar archive.  This parameter is ignored.
	 *      </p>
	 * @return bool Returns true on success or false on failure.
	 */
	public function setStub ($stub) {}

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
	 * Returns whether current entry is a directory and not '.' or '..'
	 * @link http://www.php.net/manual/en/recursivedirectoryiterator.haschildren.php
	 * @param allow_links bool[optional] <p>
	 *       
	 *      </p>
	 * @return bool whether the current entry is a directory, but not '.' or '..'
	 */
	public function hasChildren ($allow_links = null) {}

	/**
	 * Returns an iterator for the current entry if it is a directory
	 * @link http://www.php.net/manual/en/recursivedirectoryiterator.getchildren.php
	 * @return object An iterator for the current entry, if it is a directory.
	 */
	public function getChildren () {}

	/**
	 * Get sub path
	 * @link http://www.php.net/manual/en/recursivedirectoryiterator.getsubpath.php
	 * @return string The sub path (sub directory).
	 */
	public function getSubPath () {}

	/**
	 * Get sub path and name
	 * @link http://www.php.net/manual/en/recursivedirectoryiterator.getsubpathname.php
	 * @return string The sub path (sub directory) and filename.
	 */
	public function getSubPathname () {}

	/**
	 * Rewinds back to the beginning
	 * @link http://www.php.net/manual/en/filesystemiterator.rewind.php
	 * @return void 
	 */
	public function rewind () {}

	/**
	 * Move to the next file
	 * @link http://www.php.net/manual/en/filesystemiterator.next.php
	 * @return void 
	 */
	public function next () {}

	/**
	 * Retrieve the key for the current file
	 * @link http://www.php.net/manual/en/filesystemiterator.key.php
	 * @return string the pathname or filename depending on the set flags.
	 *   See the FilesystemIterator constants.
	 */
	public function key () {}

	/**
	 * The current file
	 * @link http://www.php.net/manual/en/filesystemiterator.current.php
	 * @return mixed The filename, file information, or $this depending on the set flags.
	 *   See the FilesystemIterator constants.
	 */
	public function current () {}

	/**
	 * Get the handling flags
	 * @link http://www.php.net/manual/en/filesystemiterator.getflags.php
	 * @return int The integer value of the set flags.
	 */
	public function getFlags () {}

	/**
	 * Sets handling flags
	 * @link http://www.php.net/manual/en/filesystemiterator.setflags.php
	 * @param flags int[optional] <p>
	 *       The handling flags to set.
	 *       See the FilesystemIterator constants.
	 *      </p>
	 * @return void 
	 */
	public function setFlags ($flags = null) {}

	/**
	 * Return file name of current DirectoryIterator item.
	 * @link http://www.php.net/manual/en/directoryiterator.getfilename.php
	 * @return string the file name of the current DirectoryIterator item.
	 */
	public function getFilename () {}

	/**
	 * Returns the file extension component of path
	 * @link http://www.php.net/manual/en/directoryiterator.getextension.php
	 * @return string 
	 */
	public function getExtension () {}

	/**
	 * Get base name of current DirectoryIterator item.
	 * @link http://www.php.net/manual/en/directoryiterator.getbasename.php
	 * @param suffix string[optional] <p>
	 *       If the base name ends in suffix, 
	 *       this will be cut.
	 *      </p>
	 * @return string The base name of the current DirectoryIterator item.
	 */
	public function getBasename ($suffix = null) {}

	/**
	 * Determine if current DirectoryIterator item is '.' or '..'
	 * @link http://www.php.net/manual/en/directoryiterator.isdot.php
	 * @return bool true if the entry is . or ..,
	 *   otherwise false
	 */
	public function isDot () {}

	/**
	 * Check whether current DirectoryIterator position is a valid file
	 * @link http://www.php.net/manual/en/directoryiterator.valid.php
	 * @return bool true if the position is valid, otherwise false
	 */
	public function valid () {}

	/**
	 * Seek to a DirectoryIterator item
	 * @link http://www.php.net/manual/en/directoryiterator.seek.php
	 * @param position int <p>
	 *       The zero-based numeric position to seek to.
	 *      </p>
	 * @return void 
	 */
	public function seek ($position) {}

	/**
	 * Get file name as a string
	 * @link http://www.php.net/manual/en/directoryiterator.tostring.php
	 * @return string the file name of the current DirectoryIterator item.
	 */
	public function __toString () {}

	/**
	 * Gets the path to the file
	 * @link http://www.php.net/manual/en/splfileinfo.getpathname.php
	 * @return string The path to the file.
	 */
	public function getPathname () {}

	/**
	 * Gets file permissions
	 * @link http://www.php.net/manual/en/splfileinfo.getperms.php
	 * @return int the file permissions.
	 */
	public function getPerms () {}

	/**
	 * Gets the inode for the file
	 * @link http://www.php.net/manual/en/splfileinfo.getinode.php
	 * @return int the inode number for the filesystem object.
	 */
	public function getInode () {}

	/**
	 * Gets file size
	 * @link http://www.php.net/manual/en/splfileinfo.getsize.php
	 * @return int The filesize in bytes.
	 */
	public function getSize () {}

	/**
	 * Gets the owner of the file
	 * @link http://www.php.net/manual/en/splfileinfo.getowner.php
	 * @return int The owner id in numerical format.
	 */
	public function getOwner () {}

	/**
	 * Gets the file group
	 * @link http://www.php.net/manual/en/splfileinfo.getgroup.php
	 * @return int The group id in numerical format.
	 */
	public function getGroup () {}

	/**
	 * Gets last access time of the file
	 * @link http://www.php.net/manual/en/splfileinfo.getatime.php
	 * @return int the time the file was last accessed.
	 */
	public function getATime () {}

	/**
	 * Gets the last modified time
	 * @link http://www.php.net/manual/en/splfileinfo.getmtime.php
	 * @return int the last modified time for the file, in a Unix timestamp.
	 */
	public function getMTime () {}

	/**
	 * Gets the inode change time
	 * @link http://www.php.net/manual/en/splfileinfo.getctime.php
	 * @return int The last change time, in a Unix timestamp.
	 */
	public function getCTime () {}

	/**
	 * Gets file type
	 * @link http://www.php.net/manual/en/splfileinfo.gettype.php
	 * @return string A string representing the type of the entry.
	 *   May be one of file, link,
	 *   or dir
	 */
	public function getType () {}

	/**
	 * Tells if file is readable
	 * @link http://www.php.net/manual/en/splfileinfo.isreadable.php
	 * @return bool true if readable, false otherwise.
	 */
	public function isReadable () {}

	/**
	 * Tells if the file is executable
	 * @link http://www.php.net/manual/en/splfileinfo.isexecutable.php
	 * @return bool true if executable, false otherwise.
	 */
	public function isExecutable () {}

	/**
	 * Tells if the object references a regular file
	 * @link http://www.php.net/manual/en/splfileinfo.isfile.php
	 * @return bool true if the file exists and is a regular file (not a link), false otherwise.
	 */
	public function isFile () {}

	/**
	 * Tells if the file is a directory
	 * @link http://www.php.net/manual/en/splfileinfo.isdir.php
	 * @return bool true if a directory, false otherwise.
	 */
	public function isDir () {}

	/**
	 * Tells if the file is a link
	 * @link http://www.php.net/manual/en/splfileinfo.islink.php
	 * @return bool true if the file is a link, false otherwise.
	 */
	public function isLink () {}

	/**
	 * Gets the target of a link
	 * @link http://www.php.net/manual/en/splfileinfo.getlinktarget.php
	 * @return string the target of the filesystem link.
	 */
	public function getLinkTarget () {}

	/**
	 * Gets absolute path to file
	 * @link http://www.php.net/manual/en/splfileinfo.getrealpath.php
	 * @return string the path to the file.
	 */
	public function getRealPath () {}

	/**
	 * Gets an SplFileInfo object for the file
	 * @link http://www.php.net/manual/en/splfileinfo.getfileinfo.php
	 * @param class_name string[optional] <p>
	 *       Name of an SplFileInfo derived class to use. 
	 *      </p>
	 * @return SplFileInfo An SplFileInfo object created for the file.
	 */
	public function getFileInfo ($class_name = null) {}

	/**
	 * Gets an SplFileInfo object for the path
	 * @link http://www.php.net/manual/en/splfileinfo.getpathinfo.php
	 * @param class_name string[optional] <p>
	 *       Name of an SplFileInfo derived class to use.
	 *      </p>
	 * @return SplFileInfo an SplFileInfo object for the parent path of the file.
	 */
	public function getPathInfo ($class_name = null) {}

	/**
	 * Gets an SplFileObject object for the file
	 * @link http://www.php.net/manual/en/splfileinfo.openfile.php
	 * @param open_mode string[optional] <p>
	 *       The mode for opening the file. See the fopen
	 *       documentation for descriptions of possible modes. The default 
	 *       is read only.
	 *      </p>
	 * @param use_include_path bool[optional] <p>
	 *       &parameter.use_include_path;
	 *      </p>
	 * @param context resource[optional] <p>
	 *       &parameter.context;
	 *      </p>
	 * @return SplFileObject The opened file as an SplFileObject object.
	 */
	public function openFile ($open_mode = null, $use_include_path = null, $context = null) {}

	/**
	 * Sets the class name used with <methodname>SplFileInfo::openFile</methodname>
	 * @link http://www.php.net/manual/en/splfileinfo.setfileclass.php
	 * @param class_name string[optional] <p>
	 *       The class name to use when openFile() is called. 
	 *      </p>
	 * @return void 
	 */
	public function setFileClass ($class_name = null) {}

	/**
	 * Sets the class used with getFileInfo and getPathInfo
	 * @link http://www.php.net/manual/en/splfileinfo.setinfoclass.php
	 * @param class_name string[optional] <p>
	 *       The class name to use.
	 *      </p>
	 * @return void 
	 */
	public function setInfoClass ($class_name = null) {}

	final public function _bad_state_ex () {}

}

class PharFileInfo extends SplFileInfo  {

	/**
	 * Construct a Phar entry object
	 * @link http://www.php.net/manual/en/pharfileinfo.construct.php
	 * @param entry string <p>
	 *       The full url to retrieve a file.  If you wish to retrieve the information
	 *       for the file my/file.php from the phar boo.phar,
	 *       the entry should be phar://boo.phar/my/file.php.
	 *      </p>
	 * @return void 
	 */
	public function __construct ($entry) {}

	public function __destruct () {}

	/**
	 * Sets file-specific permission bits
	 * @link http://www.php.net/manual/en/pharfileinfo.chmod.php
	 * @param permissions int <p>
	 *       permissions (see chmod)
	 *      </p>
	 * @return void 
	 */
	public function chmod ($permissions) {}

	/**
	 * Compresses the current Phar entry with either zlib or bzip2 compression
	 * @link http://www.php.net/manual/en/pharfileinfo.compress.php
	 * @param compression int 
	 * @return bool Returns true on success or false on failure.
	 */
	public function compress ($compression) {}

	/**
	 * Decompresses the current Phar entry within the phar
	 * @link http://www.php.net/manual/en/pharfileinfo.decompress.php
	 * @return bool Returns true on success or false on failure.
	 */
	public function decompress () {}

	/**
	 * Deletes the metadata of the entry
	 * @link http://www.php.net/manual/en/pharfileinfo.delmetadata.php
	 * @return bool true if successful, false if the entry had no metadata.
	 *   As with all functionality that modifies the contents of
	 *   a phar, the phar.readonly INI variable
	 *   must be off in order to succeed if the file is within a Phar
	 *   archive.  Files within PharData archives do not have
	 *   this restriction.
	 */
	public function delMetadata () {}

	/**
	 * Returns the actual size of the file (with compression) inside the Phar archive
	 * @link http://www.php.net/manual/en/pharfileinfo.getcompressedsize.php
	 * @return int The size in bytes of the file within the Phar archive on disk.
	 */
	public function getCompressedSize () {}

	/**
	 * Returns CRC32 code or throws an exception if CRC has not been verified
	 * @link http://www.php.net/manual/en/pharfileinfo.getcrc32.php
	 * @return int The crc32 checksum of the file within the Phar archive.
	 */
	public function getCRC32 () {}

	public function getContent () {}

	/**
	 * Returns file-specific meta-data saved with a file
	 * @link http://www.php.net/manual/en/pharfileinfo.getmetadata.php
	 * @return mixed any PHP variable that can be serialized and is stored as meta-data for the file,
	 *   or &null; if no meta-data is stored.
	 */
	public function getMetadata () {}

	/**
	 * Returns the Phar file entry flags
	 * @link http://www.php.net/manual/en/pharfileinfo.getpharflags.php
	 * @return int The Phar flags (always 0 in the current implementation)
	 */
	public function getPharFlags () {}

	/**
	 * Returns the metadata of the entry
	 * @link http://www.php.net/manual/en/pharfileinfo.hasmetadata.php
	 * @return bool false if no metadata is set or is &null;, true if metadata is not &null;
	 */
	public function hasMetadata () {}

	/**
	 * Returns whether the entry is compressed
	 * @link http://www.php.net/manual/en/pharfileinfo.iscompressed.php
	 * @param compression_type int[optional] <p>
	 *       One of Phar::GZ or Phar::BZ2,
	 *       defaults to any compression.
	 *      </p>
	 * @return bool true if the file is compressed within the Phar archive, false if not.
	 */
	public function isCompressed ($compression_type = null) {}

	/**
	 * Returns whether file entry has had its CRC verified
	 * @link http://www.php.net/manual/en/pharfileinfo.iscrcchecked.php
	 * @return bool true if the file has had its CRC verified, false if not.
	 */
	public function isCRCChecked () {}

	/**
	 * Sets file-specific meta-data saved with a file
	 * @link http://www.php.net/manual/en/pharfileinfo.setmetadata.php
	 * @param metadata mixed <p>
	 *       Any PHP variable containing information to store alongside a file
	 *      </p>
	 * @return void 
	 */
	public function setMetadata ($metadata) {}

	/**
	 * Gets the path without filename
	 * @link http://www.php.net/manual/en/splfileinfo.getpath.php
	 * @return string the path to the file.
	 */
	public function getPath () {}

	/**
	 * Gets the filename
	 * @link http://www.php.net/manual/en/splfileinfo.getfilename.php
	 * @return string The filename.
	 */
	public function getFilename () {}

	/**
	 * Gets the file extension
	 * @link http://www.php.net/manual/en/splfileinfo.getextension.php
	 * @return string a string containing the file extension, or an
	 *    empty string if the file has no extension.
	 */
	public function getExtension () {}

	/**
	 * Gets the base name of the file
	 * @link http://www.php.net/manual/en/splfileinfo.getbasename.php
	 * @param suffix string[optional] <p>
	 *       Optional suffix to omit from the base name returned.
	 *      </p>
	 * @return string the base name without path information.
	 */
	public function getBasename ($suffix = null) {}

	/**
	 * Gets the path to the file
	 * @link http://www.php.net/manual/en/splfileinfo.getpathname.php
	 * @return string The path to the file.
	 */
	public function getPathname () {}

	/**
	 * Gets file permissions
	 * @link http://www.php.net/manual/en/splfileinfo.getperms.php
	 * @return int the file permissions.
	 */
	public function getPerms () {}

	/**
	 * Gets the inode for the file
	 * @link http://www.php.net/manual/en/splfileinfo.getinode.php
	 * @return int the inode number for the filesystem object.
	 */
	public function getInode () {}

	/**
	 * Gets file size
	 * @link http://www.php.net/manual/en/splfileinfo.getsize.php
	 * @return int The filesize in bytes.
	 */
	public function getSize () {}

	/**
	 * Gets the owner of the file
	 * @link http://www.php.net/manual/en/splfileinfo.getowner.php
	 * @return int The owner id in numerical format.
	 */
	public function getOwner () {}

	/**
	 * Gets the file group
	 * @link http://www.php.net/manual/en/splfileinfo.getgroup.php
	 * @return int The group id in numerical format.
	 */
	public function getGroup () {}

	/**
	 * Gets last access time of the file
	 * @link http://www.php.net/manual/en/splfileinfo.getatime.php
	 * @return int the time the file was last accessed.
	 */
	public function getATime () {}

	/**
	 * Gets the last modified time
	 * @link http://www.php.net/manual/en/splfileinfo.getmtime.php
	 * @return int the last modified time for the file, in a Unix timestamp.
	 */
	public function getMTime () {}

	/**
	 * Gets the inode change time
	 * @link http://www.php.net/manual/en/splfileinfo.getctime.php
	 * @return int The last change time, in a Unix timestamp.
	 */
	public function getCTime () {}

	/**
	 * Gets file type
	 * @link http://www.php.net/manual/en/splfileinfo.gettype.php
	 * @return string A string representing the type of the entry.
	 *   May be one of file, link,
	 *   or dir
	 */
	public function getType () {}

	/**
	 * Tells if the entry is writable
	 * @link http://www.php.net/manual/en/splfileinfo.iswritable.php
	 * @return bool true if writable, false otherwise;
	 */
	public function isWritable () {}

	/**
	 * Tells if file is readable
	 * @link http://www.php.net/manual/en/splfileinfo.isreadable.php
	 * @return bool true if readable, false otherwise.
	 */
	public function isReadable () {}

	/**
	 * Tells if the file is executable
	 * @link http://www.php.net/manual/en/splfileinfo.isexecutable.php
	 * @return bool true if executable, false otherwise.
	 */
	public function isExecutable () {}

	/**
	 * Tells if the object references a regular file
	 * @link http://www.php.net/manual/en/splfileinfo.isfile.php
	 * @return bool true if the file exists and is a regular file (not a link), false otherwise.
	 */
	public function isFile () {}

	/**
	 * Tells if the file is a directory
	 * @link http://www.php.net/manual/en/splfileinfo.isdir.php
	 * @return bool true if a directory, false otherwise.
	 */
	public function isDir () {}

	/**
	 * Tells if the file is a link
	 * @link http://www.php.net/manual/en/splfileinfo.islink.php
	 * @return bool true if the file is a link, false otherwise.
	 */
	public function isLink () {}

	/**
	 * Gets the target of a link
	 * @link http://www.php.net/manual/en/splfileinfo.getlinktarget.php
	 * @return string the target of the filesystem link.
	 */
	public function getLinkTarget () {}

	/**
	 * Gets absolute path to file
	 * @link http://www.php.net/manual/en/splfileinfo.getrealpath.php
	 * @return string the path to the file.
	 */
	public function getRealPath () {}

	/**
	 * Gets an SplFileInfo object for the file
	 * @link http://www.php.net/manual/en/splfileinfo.getfileinfo.php
	 * @param class_name string[optional] <p>
	 *       Name of an SplFileInfo derived class to use. 
	 *      </p>
	 * @return SplFileInfo An SplFileInfo object created for the file.
	 */
	public function getFileInfo ($class_name = null) {}

	/**
	 * Gets an SplFileInfo object for the path
	 * @link http://www.php.net/manual/en/splfileinfo.getpathinfo.php
	 * @param class_name string[optional] <p>
	 *       Name of an SplFileInfo derived class to use.
	 *      </p>
	 * @return SplFileInfo an SplFileInfo object for the parent path of the file.
	 */
	public function getPathInfo ($class_name = null) {}

	/**
	 * Gets an SplFileObject object for the file
	 * @link http://www.php.net/manual/en/splfileinfo.openfile.php
	 * @param open_mode string[optional] <p>
	 *       The mode for opening the file. See the fopen
	 *       documentation for descriptions of possible modes. The default 
	 *       is read only.
	 *      </p>
	 * @param use_include_path bool[optional] <p>
	 *       &parameter.use_include_path;
	 *      </p>
	 * @param context resource[optional] <p>
	 *       &parameter.context;
	 *      </p>
	 * @return SplFileObject The opened file as an SplFileObject object.
	 */
	public function openFile ($open_mode = null, $use_include_path = null, $context = null) {}

	/**
	 * Sets the class name used with <methodname>SplFileInfo::openFile</methodname>
	 * @link http://www.php.net/manual/en/splfileinfo.setfileclass.php
	 * @param class_name string[optional] <p>
	 *       The class name to use when openFile() is called. 
	 *      </p>
	 * @return void 
	 */
	public function setFileClass ($class_name = null) {}

	/**
	 * Sets the class used with getFileInfo and getPathInfo
	 * @link http://www.php.net/manual/en/splfileinfo.setinfoclass.php
	 * @param class_name string[optional] <p>
	 *       The class name to use.
	 *      </p>
	 * @return void 
	 */
	public function setInfoClass ($class_name = null) {}

	final public function _bad_state_ex () {}

	/**
	 * Returns the path to the file as a string
	 * @link http://www.php.net/manual/en/splfileinfo.tostring.php
	 * @return void the path to the file.
	 */
	public function __toString () {}

}
// End of Phar v.2.0.1
?>
