<?php

// Start of Zend Session Clustering v.

function zend_sc_get_daemon_status () {}

function zend_sc_get_statistics () {}

function zend_sc_graceful_shutdown () {}

function zend_sc_get_last_error () {}

function zend_sc_get_last_error_code () {}

function zend_sc_reset_last_error () {}

function zend_sc_reload_configuration () {}

function zend_sc_get_replacements_list () {}

function zend_sc_get_known_hosts () {}

define ('ZEND_SC_STATUS_OFF', 0);
define ('ZEND_SC_STATUS_OK', 2);
define ('ZEND_SC_STATUS_INTERNAL_ERROR', 3);
define ('ZEND_SC_STATUS_STANDBY', 1);
define ('ZEND_SC_STATUS_SHUTDOWN_IN_PROCESS', 4);
define ('ZEND_SC_STATUS_SHUTDOWN_FAILED', 5);
define ('ZEND_SC_STATUS_STARTING_UP_ERROR', 7);
define ('ZEND_SC_STATUS_STARTING_UP', 6);
define ('ZEND_SC_NO_ERROR', 0);
define ('ZEND_SC_NETWORK_ERROR', 1);
define ('ZEND_SC_TIMEOUT_ERROR', 2);
define ('ZEND_SC_UNKNOWN_ERROR', 3);

// End of Zend Session Clustering v.
?>
