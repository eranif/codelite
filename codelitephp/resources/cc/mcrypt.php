<?php

// Start of mcrypt v.

/**
 * @param cipher
 * @param key
 * @param data
 * @param mode
 * @param iv
 */
function mcrypt_ecb ($cipher, $key, $data, $mode, $iv) {}

/**
 * @param cipher
 * @param key
 * @param data
 * @param mode
 * @param iv
 */
function mcrypt_cbc ($cipher, $key, $data, $mode, $iv) {}

/**
 * @param cipher
 * @param key
 * @param data
 * @param mode
 * @param iv
 */
function mcrypt_cfb ($cipher, $key, $data, $mode, $iv) {}

/**
 * @param cipher
 * @param key
 * @param data
 * @param mode
 * @param iv
 */
function mcrypt_ofb ($cipher, $key, $data, $mode, $iv) {}

/**
 * @param cipher
 * @param module
 */
function mcrypt_get_key_size ($cipher, $module) {}

/**
 * @param cipher
 * @param module
 */
function mcrypt_get_block_size ($cipher, $module) {}

/**
 * @param cipher
 */
function mcrypt_get_cipher_name ($cipher) {}

/**
 * @param size
 * @param source[optional]
 */
function mcrypt_create_iv ($size, $source) {}

/**
 * @param lib_dir[optional]
 */
function mcrypt_list_algorithms ($lib_dir) {}

/**
 * @param lib_dir[optional]
 */
function mcrypt_list_modes ($lib_dir) {}

/**
 * @param cipher
 * @param module
 */
function mcrypt_get_iv_size ($cipher, $module) {}

/**
 * @param cipher
 * @param key
 * @param data
 * @param mode
 * @param iv
 */
function mcrypt_encrypt ($cipher, $key, $data, $mode, $iv) {}

/**
 * @param cipher
 * @param key
 * @param data
 * @param mode
 * @param iv
 */
function mcrypt_decrypt ($cipher, $key, $data, $mode, $iv) {}

/**
 * @param cipher
 * @param cipher_directory
 * @param mode
 * @param mode_directory
 */
function mcrypt_module_open ($cipher, $cipher_directory, $mode, $mode_directory) {}

/**
 * @param td
 * @param key
 * @param iv
 */
function mcrypt_generic_init ($td, $key, $iv) {}

/**
 * @param td
 * @param data
 */
function mcrypt_generic ($td, $data) {}

/**
 * @param td
 * @param data
 */
function mdecrypt_generic ($td, $data) {}

/**
 * @param td
 */
function mcrypt_generic_end ($td) {}

/**
 * @param td
 */
function mcrypt_generic_deinit ($td) {}

/**
 * @param td
 */
function mcrypt_enc_self_test ($td) {}

/**
 * @param td
 */
function mcrypt_enc_is_block_algorithm_mode ($td) {}

/**
 * @param td
 */
function mcrypt_enc_is_block_algorithm ($td) {}

/**
 * @param td
 */
function mcrypt_enc_is_block_mode ($td) {}

/**
 * @param td
 */
function mcrypt_enc_get_block_size ($td) {}

/**
 * @param td
 */
function mcrypt_enc_get_key_size ($td) {}

/**
 * @param td
 */
function mcrypt_enc_get_supported_key_sizes ($td) {}

/**
 * @param td
 */
function mcrypt_enc_get_iv_size ($td) {}

/**
 * @param td
 */
function mcrypt_enc_get_algorithms_name ($td) {}

/**
 * @param td
 */
function mcrypt_enc_get_modes_name ($td) {}

/**
 * @param algorithm
 * @param lib_dir[optional]
 */
function mcrypt_module_self_test ($algorithm, $lib_dir) {}

/**
 * @param mode
 * @param lib_dir[optional]
 */
function mcrypt_module_is_block_algorithm_mode ($mode, $lib_dir) {}

/**
 * @param algorithm
 * @param lib_dir[optional]
 */
function mcrypt_module_is_block_algorithm ($algorithm, $lib_dir) {}

/**
 * @param mode
 * @param lib_dir[optional]
 */
function mcrypt_module_is_block_mode ($mode, $lib_dir) {}

/**
 * @param algorithm
 * @param lib_dir[optional]
 */
function mcrypt_module_get_algo_block_size ($algorithm, $lib_dir) {}

/**
 * @param algorithm
 * @param lib_dir[optional]
 */
function mcrypt_module_get_algo_key_size ($algorithm, $lib_dir) {}

/**
 * @param algorithm
 * @param lib_dir[optional]
 */
function mcrypt_module_get_supported_key_sizes ($algorithm, $lib_dir) {}

/**
 * @param td
 */
function mcrypt_module_close ($td) {}

define ('MCRYPT_ENCRYPT', 0);
define ('MCRYPT_DECRYPT', 1);
define ('MCRYPT_DEV_RANDOM', 0);
define ('MCRYPT_DEV_URANDOM', 1);
define ('MCRYPT_RAND', 2);
define ('MCRYPT_3DES', "tripledes");
define ('MCRYPT_ARCFOUR_IV', "arcfour-iv");
define ('MCRYPT_ARCFOUR', "arcfour");
define ('MCRYPT_BLOWFISH', "blowfish");
define ('MCRYPT_BLOWFISH_COMPAT', "blowfish-compat");
define ('MCRYPT_CAST_128', "cast-128");
define ('MCRYPT_CAST_256', "cast-256");
define ('MCRYPT_CRYPT', "crypt");
define ('MCRYPT_DES', "des");
define ('MCRYPT_ENIGNA', "crypt");
define ('MCRYPT_GOST', "gost");
define ('MCRYPT_LOKI97', "loki97");
define ('MCRYPT_PANAMA', "panama");
define ('MCRYPT_RC2', "rc2");
define ('MCRYPT_RIJNDAEL_128', "rijndael-128");
define ('MCRYPT_RIJNDAEL_192', "rijndael-192");
define ('MCRYPT_RIJNDAEL_256', "rijndael-256");
define ('MCRYPT_SAFER64', "safer-sk64");
define ('MCRYPT_SAFER128', "safer-sk128");
define ('MCRYPT_SAFERPLUS', "saferplus");
define ('MCRYPT_SERPENT', "serpent");
define ('MCRYPT_THREEWAY', "threeway");
define ('MCRYPT_TRIPLEDES', "tripledes");
define ('MCRYPT_TWOFISH', "twofish");
define ('MCRYPT_WAKE', "wake");
define ('MCRYPT_XTEA', "xtea");
define ('MCRYPT_IDEA', "idea");
define ('MCRYPT_MARS', "mars");
define ('MCRYPT_RC6', "rc6");
define ('MCRYPT_SKIPJACK', "skipjack");
define ('MCRYPT_MODE_CBC', "cbc");
define ('MCRYPT_MODE_CFB', "cfb");
define ('MCRYPT_MODE_ECB', "ecb");
define ('MCRYPT_MODE_NOFB', "nofb");
define ('MCRYPT_MODE_OFB', "ofb");
define ('MCRYPT_MODE_STREAM', "stream");

// End of mcrypt v.
?>
