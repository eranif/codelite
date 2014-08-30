<?php

// Start of hash v.1.0

/**
 * Generate a hash value (message digest)
 * @link http://www.php.net/manual/en/function.hash.php
 * @param algo string <p>
 *       Name of selected hashing algorithm (i.e. "md5", "sha256", "haval160,4", etc..)
 *      </p>
 * @param data string <p>
 *       Message to be hashed.
 *      </p>
 * @param raw_output bool[optional] <p>
 *       When set to true, outputs raw binary data.
 *       false outputs lowercase hexits.
 *      </p>
 * @return string a string containing the calculated message digest as lowercase hexits
 *   unless raw_output is set to true in which case the raw
 *   binary representation of the message digest is returned.
 */
function hash ($algo, $data, $raw_output = null) {}

/**
 * Generate a hash value using the contents of a given file
 * @link http://www.php.net/manual/en/function.hash-file.php
 * @param algo string <p>
 *       Name of selected hashing algorithm (i.e. "md5", "sha256", "haval160,4", etc..)
 *      </p>
 * @param filename string <p>
 *       URL describing location of file to be hashed; Supports fopen wrappers.
 *      </p>
 * @param raw_output bool[optional] <p>
 *       When set to true, outputs raw binary data.
 *       false outputs lowercase hexits.
 *      </p>
 * @return string a string containing the calculated message digest as lowercase hexits
 *   unless raw_output is set to true in which case the raw
 *   binary representation of the message digest is returned.
 */
function hash_file ($algo, $filename, $raw_output = null) {}

/**
 * Generate a keyed hash value using the HMAC method
 * @link http://www.php.net/manual/en/function.hash-hmac.php
 * @param algo string <p>
 *       Name of selected hashing algorithm (i.e. "md5", "sha256", "haval160,4", etc..) See hash_algos for a list of supported algorithms.
 *      </p>
 * @param data string <p>
 *       Message to be hashed.
 *      </p>
 * @param key string <p>
 *       Shared secret key used for generating the HMAC variant of the message digest.
 *      </p>
 * @param raw_output bool[optional] <p>
 *       When set to true, outputs raw binary data.
 *       false outputs lowercase hexits.
 *      </p>
 * @return string a string containing the calculated message digest as lowercase hexits
 *   unless raw_output is set to true in which case the raw
 *   binary representation of the message digest is returned.
 */
function hash_hmac ($algo, $data, $key, $raw_output = null) {}

/**
 * Generate a keyed hash value using the HMAC method and the contents of a given file
 * @link http://www.php.net/manual/en/function.hash-hmac-file.php
 * @param algo string <p>
 *       Name of selected hashing algorithm (i.e. "md5", "sha256", "haval160,4", etc..) See hash_algos for a list of supported algorithms.
 *      </p>
 * @param filename string <p>
 *       URL describing location of file to be hashed; Supports fopen wrappers.
 *      </p>
 * @param key string <p>
 *       Shared secret key used for generating the HMAC variant of the message digest.
 *      </p>
 * @param raw_output bool[optional] <p>
 *       When set to true, outputs raw binary data.
 *       false outputs lowercase hexits.
 *      </p>
 * @return string a string containing the calculated message digest as lowercase hexits
 *   unless raw_output is set to true in which case the raw
 *   binary representation of the message digest is returned.
 */
function hash_hmac_file ($algo, $filename, $key, $raw_output = null) {}

/**
 * Initialize an incremental hashing context
 * @link http://www.php.net/manual/en/function.hash-init.php
 * @param algo string <p>
 *       Name of selected hashing algorithm (i.e. "md5", "sha256", "haval160,4", etc..)
 *      </p>
 * @param options int[optional] <p>
 *       Optional settings for hash generation, currently supports only one option:
 *       HASH_HMAC.  When specified, the key
 *       must be specified.
 *      </p>
 * @param key string[optional] <p>
 *       When HASH_HMAC is specified for options,
 *       a shared secret key to be used with the HMAC hashing method must be supplied in this
 *       parameter.
 *      </p>
 * @return resource a Hashing Context resource for use with hash_update,
 *   hash_update_stream, hash_update_file,
 *   and hash_final.
 */
function hash_init ($algo, $options = null, $key = null) {}

/**
 * Pump data into an active hashing context
 * @link http://www.php.net/manual/en/function.hash-update.php
 * @param context resource <p>
 *       Hashing context returned by hash_init.
 *      </p>
 * @param data string <p>
 *       Message to be included in the hash digest.
 *      </p>
 * @return bool true.
 */
function hash_update ($context, $data) {}

/**
 * Pump data into an active hashing context from an open stream
 * @link http://www.php.net/manual/en/function.hash-update-stream.php
 * @param context resource <p>
 *       Hashing context returned by hash_init.
 *      </p>
 * @param handle resource <p>
 *       Open file handle as returned by any stream creation function.
 *      </p>
 * @param length int[optional] <p>
 *       Maximum number of characters to copy from handle
 *       into the hashing context.
 *      </p>
 * @return int Actual number of bytes added to the hashing context from handle.
 */
function hash_update_stream ($context, $handle, $length = null) {}

/**
 * Pump data into an active hashing context from a file
 * @link http://www.php.net/manual/en/function.hash-update-file.php
 * @param context resource <p>
 *       Stream context as returned by stream_context_create.
 *      </p>
 * @param filename string <p>
 *       URL describing location of file to be hashed; Supports fopen wrappers.
 *      </p>
 * @param context resource[optional] 
 * @return bool Returns true on success or false on failure.
 */
function hash_update_file ($context, $filename, $context = null) {}

/**
 * Finalize an incremental hash and return resulting digest
 * @link http://www.php.net/manual/en/function.hash-final.php
 * @param context resource <p>
 *       Hashing context returned by hash_init.
 *      </p>
 * @param raw_output bool[optional] <p>
 *       When set to true, outputs raw binary data.
 *       false outputs lowercase hexits.
 *      </p>
 * @return string a string containing the calculated message digest as lowercase hexits
 *   unless raw_output is set to true in which case the raw
 *   binary representation of the message digest is returned.
 */
function hash_final ($context, $raw_output = null) {}

/**
 * Copy hashing context
 * @link http://www.php.net/manual/en/function.hash-copy.php
 * @param context resource <p>
 *       Hashing context returned by hash_init.
 *      </p>
 * @return resource a copy of Hashing Context resource.
 */
function hash_copy ($context) {}

/**
 * Return a list of registered hashing algorithms
 * @link http://www.php.net/manual/en/function.hash-algos.php
 * @return array a numerically indexed array containing the list of supported
 *   hashing algorithms.
 */
function hash_algos () {}


/**
 * Optional flag for hash_init.
 *     Indicates that the HMAC digest-keying algorithm should be
 *     applied to the current hashing context.
 * @link http://www.php.net/manual/en/hash.constants.php
 */
define ('HASH_HMAC', 1);

// End of hash v.1.0
?>
