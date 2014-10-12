<?php

// Start of Zend OPcache v.7.0.4-devFE

function opcache_reset () {}

/**
 * @param script
 * @param force[optional]
 */
function opcache_invalidate ($script, $force) {}

/**
 * @param file
 */
function opcache_compile_file ($file) {}

/**
 * @param script
 */
function opcache_is_script_cached ($script) {}

function accelerator_reset () {}

/**
 * @param script
 * @param force[optional]
 */
function accelerator_invalidate ($script, $force) {}

/**
 * @param file
 */
function accelerator_compile_file ($file) {}

/**
 * @param script
 */
function accelerator_is_script_cached ($script) {}

function opcache_get_configuration () {}

/**
 * @param fetch_scripts[optional]
 */
function opcache_get_status ($fetch_scripts) {}

function accelerator_get_configuration () {}

/**
 * @param fetch_scripts[optional]
 */
function accelerator_get_status ($fetch_scripts) {}

// End of Zend OPcache v.7.0.4-devFE
?>
