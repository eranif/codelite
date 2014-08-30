<?php

// Start of apc v.

/**
 * Fetch a stored variable from the cache
 * @link http://www.php.net/manual/en/function.apc-fetch.php
 * @param key mixed <p>
 *       The key used to store the value (with
 *       apc_store). If an array is passed then each
 *       element is fetched and returned.
 *      </p>
 * @param success bool[optional] <p>
 *       Set to true in success and false in failure.
 *      </p>
 * @return mixed The stored variable or array of variables on success; false on failure
 */
function apc_fetch ($key, &$success = null) {}

/**
 * Cache a variable in the data store
 * @link http://www.php.net/manual/en/function.apc-store.php
 * @param key string <p>
 *       Store the variable using this name. keys are
 *       cache-unique, so storing a second value with the same
 *       key will overwrite the original value.
 *      </p>
 * @param var mixed <p>
 *       The variable to store
 *      </p>
 * @param ttl int[optional] <p>
 *       Time To Live; store var in the cache for
 *       ttl seconds. After the
 *       ttl has passed, the stored variable will be
 *       expunged from the cache (on the next request). If no ttl
 *       is supplied (or if the ttl is
 *       0), the value will persist until it is removed from
 *       the cache manually, or otherwise fails to exist in the cache (clear,
 *       restart, etc.).
 *      </p>
 * @return bool Returns true on success or false on failure.
 *   Second syntax returns array with error keys.
 */
function apc_store ($key, $var, $ttl = null) {}

/**
 * Cache a variable in the data store
 * @link http://www.php.net/manual/en/function.apc-add.php
 * @param key string <p>
 *       Store the variable using this name. keys are
 *       cache-unique, so attempting to use apc_add to
 *       store data with a key that already exists will not overwrite the
 *       existing data, and will instead return false. (This is the only
 *       difference between apc_add and
 *       apc_store.)
 *      </p>
 * @param var mixed[optional] <p>
 *       The variable to store
 *      </p>
 * @param ttl int[optional] <p>
 *       Time To Live; store var in the cache for
 *       ttl seconds. After the
 *       ttl has passed, the stored variable will be
 *       expunged from the cache (on the next request). If no ttl
 *       is supplied (or if the ttl is
 *       0), the value will persist until it is removed from
 *       the cache manually, or otherwise fails to exist in the cache (clear,
 *       restart, etc.).
 *      </p>
 * @return bool Returns true on success or false on failure.
 *   Second syntax returns array with error keys.
 */
function apc_add ($key, $var = null, $ttl = null) {}

/**
 * Removes a stored variable from the cache
 * @link http://www.php.net/manual/en/function.apc-delete.php
 * @param key string <p>
 *       The key used to store the value (with
 *       apc_store).
 *      </p>
 * @return mixed Returns true on success or false on failure.
 */
function apc_delete ($key) {}

/**
 * Retrieves cached information from APC's data store
 * @link http://www.php.net/manual/en/function.apc-cache-info.php
 * @param cache_type string[optional] <p>
 *       If cache_type is "user",
 *       information about the user cache will be returned.
 *      </p>
 *      <p> 
 *       If cache_type is "filehits",
 *       information about which files have been served from the bytecode cache 
 *       for the current request will be returned. This feature must be enabled at
 *       compile time using --enable-filehits.
 *      </p>
 *      <p>
 *       If an invalid or no cache_type is specified, information about 
 *       the system cache (cached files) will be returned.
 *      </p>
 * @param limited bool[optional] <p>
 *       If limited is true, the
 *       return value will exclude the individual list of cache entries.  This
 *       is useful when trying to optimize calls for statistics gathering.
 *      </p>
 * @return array Array of cached data (and meta-data)&return.falseforfailure;
 */
function apc_cache_info ($cache_type = null, $limited = null) {}

/**
 * Clears the APC cache
 * @link http://www.php.net/manual/en/function.apc-clear-cache.php
 * @param cache_type string[optional] <p>
 *       If cache_type is "user", the
 *       user cache will be cleared; otherwise, the system cache (cached files)
 *       will be cleared.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function apc_clear_cache ($cache_type = null) {}

/**
 * Defines a set of constants for retrieval and mass-definition
 * @link http://www.php.net/manual/en/function.apc-define-constants.php
 * @param key string <p>
 *       The key serves as the name of the constant set
 *       being stored. This key is used to retrieve the
 *       stored constants in apc_load_constants.
 *      </p>
 * @param constants array <p>
 *       An associative array of constant_name => value
 *       pairs. The constant_name must follow the normal
 *       constant naming rules.
 *       value must evaluate to a scalar value.
 *      </p>
 * @param case_sensitive bool[optional] <p>
 *       The default behaviour for constants is to be declared case-sensitive;
 *       i.e. CONSTANT and Constant
 *       represent different values. If this parameter evaluates to false the
 *       constants will be declared as case-insensitive symbols.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function apc_define_constants ($key, array $constants, $case_sensitive = null) {}

/**
 * Loads a set of constants from the cache
 * @link http://www.php.net/manual/en/function.apc-load-constants.php
 * @param key string <p>
 *       The name of the constant set (that was stored with
 *       apc_define_constants) to be retrieved.
 *      </p>
 * @param case_sensitive bool[optional] <p>
 *       The default behaviour for constants is to be declared case-sensitive;
 *       i.e. CONSTANT and Constant
 *       represent different values. If this parameter evaluates to false the
 *       constants will be declared as case-insensitive symbols.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function apc_load_constants ($key, $case_sensitive = null) {}

/**
 * Retrieves APC's Shared Memory Allocation information
 * @link http://www.php.net/manual/en/function.apc-sma-info.php
 * @param limited bool[optional] <p>
 *       When set to false (default) apc_sma_info will
 *       return a detailed information about each segment.
 *      </p>
 * @return array Array of Shared Memory Allocation data; false on failure.
 */
function apc_sma_info ($limited = null) {}

/**
 * Stores a file in the bytecode cache, bypassing all filters.
 * @link http://www.php.net/manual/en/function.apc-compile-file.php
 * @param filename string <p>
 *       Full or relative path to a PHP file that will be compiled and stored in
 *       the bytecode cache.
 *      </p>
 * @param atomic bool[optional] 
 * @return mixed Returns true on success or false on failure.
 */
function apc_compile_file ($filename, $atomic = null) {}

// End of apc v.
?>
