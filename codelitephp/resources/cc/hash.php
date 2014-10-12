<?php

// Start of hash v.1.0

/**
 * @param algo
 * @param data
 * @param raw_output[optional]
 */
function hash ($algo, $data, $raw_output) {}

/**
 * @param algo
 * @param filename
 * @param raw_output[optional]
 */
function hash_file ($algo, $filename, $raw_output) {}

/**
 * @param algo
 * @param data
 * @param key
 * @param raw_output[optional]
 */
function hash_hmac ($algo, $data, $key, $raw_output) {}

/**
 * @param algo
 * @param filename
 * @param key
 * @param raw_output[optional]
 */
function hash_hmac_file ($algo, $filename, $key, $raw_output) {}

/**
 * @param algo
 * @param options[optional]
 * @param key[optional]
 */
function hash_init ($algo, $options, $key) {}

/**
 * @param context
 * @param data
 */
function hash_update ($context, $data) {}

/**
 * @param context
 * @param handle
 * @param length[optional]
 */
function hash_update_stream ($context, $handle, $length) {}

/**
 * @param context
 * @param filename
 * @param context[optional]
 */
function hash_update_file ($context, $filename, $context) {}

/**
 * @param context
 * @param raw_output[optional]
 */
function hash_final ($context, $raw_output) {}

/**
 * @param context
 */
function hash_copy ($context) {}

function hash_algos () {}

/**
 * @param algo
 * @param password
 * @param salt
 * @param iterations
 * @param length[optional]
 * @param raw_output[optional]
 */
function hash_pbkdf2 ($algo, $password, $salt, $iterations, $length, $raw_output) {}

/**
 * @param known_string
 * @param user_string
 */
function hash_equals ($known_string, $user_string) {}


/**
 * Optional flag for hash_init.
 *     Indicates that the HMAC digest-keying algorithm should be
 *     applied to the current hashing context.
 * @link http://www.php.net/manual/en/hash.constants.php
 */
define ('HASH_HMAC', 1);

// End of hash v.1.0
?>
