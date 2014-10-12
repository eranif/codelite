<?php

// Start of ldap v.

/**
 * @param hostname[optional]
 * @param port[optional]
 */
function ldap_connect ($hostname, $port) {}

/**
 * @param link_identifier
 */
function ldap_close ($link_identifier) {}

/**
 * @param link_identifier
 * @param bind_rdn[optional]
 * @param bind_password[optional]
 */
function ldap_bind ($link_identifier, $bind_rdn, $bind_password) {}

/**
 * @param link
 * @param binddn[optional]
 * @param password[optional]
 * @param sasl_mech[optional]
 * @param sasl_realm[optional]
 * @param sasl_authz_id[optional]
 * @param props[optional]
 */
function ldap_sasl_bind ($link, $binddn, $password, $sasl_mech, $sasl_realm, $sasl_authz_id, $props) {}

/**
 * @param link_identifier
 */
function ldap_unbind ($link_identifier) {}

/**
 * @param link_identifier
 * @param base_dn
 * @param filter
 * @param attributes[optional]
 * @param attrsonly[optional]
 * @param sizelimit[optional]
 * @param timelimit[optional]
 * @param deref[optional]
 */
function ldap_read ($link_identifier, $base_dn, $filter, $attributes, $attrsonly, $sizelimit, $timelimit, $deref) {}

/**
 * @param link_identifier
 * @param base_dn
 * @param filter
 * @param attributes[optional]
 * @param attrsonly[optional]
 * @param sizelimit[optional]
 * @param timelimit[optional]
 * @param deref[optional]
 */
function ldap_list ($link_identifier, $base_dn, $filter, $attributes, $attrsonly, $sizelimit, $timelimit, $deref) {}

/**
 * @param link_identifier
 * @param base_dn
 * @param filter
 * @param attributes[optional]
 * @param attrsonly[optional]
 * @param sizelimit[optional]
 * @param timelimit[optional]
 * @param deref[optional]
 */
function ldap_search ($link_identifier, $base_dn, $filter, $attributes, $attrsonly, $sizelimit, $timelimit, $deref) {}

/**
 * @param link_identifier
 */
function ldap_free_result ($link_identifier) {}

/**
 * @param link_identifier
 * @param result_identifier
 */
function ldap_count_entries ($link_identifier, $result_identifier) {}

/**
 * @param link_identifier
 * @param result_identifier
 */
function ldap_first_entry ($link_identifier, $result_identifier) {}

/**
 * @param link_identifier
 * @param result_identifier
 */
function ldap_next_entry ($link_identifier, $result_identifier) {}

/**
 * @param link_identifier
 * @param result_identifier
 */
function ldap_get_entries ($link_identifier, $result_identifier) {}

/**
 * @param link_identifier
 * @param result_entry_identifier
 */
function ldap_first_attribute ($link_identifier, $result_entry_identifier) {}

/**
 * @param link_identifier
 * @param result_entry_identifier
 */
function ldap_next_attribute ($link_identifier, $result_entry_identifier) {}

/**
 * @param link_identifier
 * @param result_entry_identifier
 */
function ldap_get_attributes ($link_identifier, $result_entry_identifier) {}

/**
 * @param link_identifier
 * @param result_entry_identifier
 * @param attribute
 */
function ldap_get_values ($link_identifier, $result_entry_identifier, $attribute) {}

/**
 * @param link_identifier
 * @param result_entry_identifier
 * @param attribute
 */
function ldap_get_values_len ($link_identifier, $result_entry_identifier, $attribute) {}

/**
 * @param link_identifier
 * @param result_entry_identifier
 */
function ldap_get_dn ($link_identifier, $result_entry_identifier) {}

/**
 * @param dn
 * @param with_attrib
 */
function ldap_explode_dn ($dn, $with_attrib) {}

/**
 * @param dn
 */
function ldap_dn2ufn ($dn) {}

/**
 * @param link_identifier
 * @param dn
 * @param entry
 */
function ldap_add ($link_identifier, $dn, $entry) {}

/**
 * @param link_identifier
 * @param dn
 */
function ldap_delete ($link_identifier, $dn) {}

/**
 * @param link_identifier
 * @param dn
 * @param modifications_info
 */
function ldap_modify_batch ($link_identifier, $dnarray , $modifications_info) {}

/**
 * @param link_identifier
 * @param dn
 * @param entry
 */
function ldap_modify ($link_identifier, $dn, $entry) {}

/**
 * @param link_identifier
 * @param dn
 * @param entry
 */
function ldap_mod_add ($link_identifier, $dn, $entry) {}

/**
 * @param link_identifier
 * @param dn
 * @param entry
 */
function ldap_mod_replace ($link_identifier, $dn, $entry) {}

/**
 * @param link_identifier
 * @param dn
 * @param entry
 */
function ldap_mod_del ($link_identifier, $dn, $entry) {}

/**
 * @param link_identifier
 */
function ldap_errno ($link_identifier) {}

/**
 * @param errno
 */
function ldap_err2str ($errno) {}

/**
 * @param link_identifier
 */
function ldap_error ($link_identifier) {}

/**
 * @param link_identifier
 * @param dn
 * @param attribute
 * @param value
 */
function ldap_compare ($link_identifier, $dn, $attribute, $value) {}

/**
 * @param link
 * @param result
 * @param sortfilter
 */
function ldap_sort ($link, $result, $sortfilter) {}

/**
 * @param link_identifier
 * @param dn
 * @param newrdn
 * @param newparent
 * @param deleteoldrdn
 */
function ldap_rename ($link_identifier, $dn, $newrdn, $newparent, $deleteoldrdn) {}

/**
 * @param link_identifier
 * @param option
 * @param retval
 */
function ldap_get_option ($link_identifier, $option, &$retval) {}

/**
 * @param link_identifier
 * @param option
 * @param newval
 */
function ldap_set_option ($link_identifier, $option, $newval) {}

/**
 * @param link
 * @param result
 */
function ldap_first_reference ($link, $result) {}

/**
 * @param link
 * @param entry
 */
function ldap_next_reference ($link, $entry) {}

/**
 * @param link
 * @param entry
 * @param referrals
 */
function ldap_parse_reference ($link, $entry, &$referrals) {}

/**
 * @param link
 * @param result
 * @param errcode
 * @param matcheddn[optional]
 * @param errmsg[optional]
 * @param referrals[optional]
 */
function ldap_parse_result ($link, $result, &$errcode, &$matcheddn, &$errmsg, &$referrals) {}

/**
 * @param link_identifier
 */
function ldap_start_tls ($link_identifier) {}

/**
 * @param link
 * @param callback
 */
function ldap_set_rebind_proc ($link, $callback) {}

/**
 * @param value
 * @param ignore[optional]
 * @param flags[optional]
 */
function ldap_escape ($value, $ignore, $flags) {}

/**
 * @param link
 * @param pagesize
 * @param iscritical[optional]
 * @param cookie[optional]
 */
function ldap_control_paged_result ($link, $pagesize, $iscritical, $cookie) {}

/**
 * @param link
 * @param result
 * @param cookie[optional]
 * @param estimated[optional]
 */
function ldap_control_paged_result_response ($link, $result, &$cookie, &$estimated) {}

define ('LDAP_DEREF_NEVER', 0);
define ('LDAP_DEREF_SEARCHING', 1);
define ('LDAP_DEREF_FINDING', 2);
define ('LDAP_DEREF_ALWAYS', 3);
define ('LDAP_MODIFY_BATCH_ADD', 1);
define ('LDAP_MODIFY_BATCH_REMOVE', 2);
define ('LDAP_MODIFY_BATCH_REMOVE_ALL', 18);
define ('LDAP_MODIFY_BATCH_REPLACE', 3);
define ('LDAP_MODIFY_BATCH_ATTRIB', "attrib");
define ('LDAP_MODIFY_BATCH_MODTYPE', "modtype");
define ('LDAP_MODIFY_BATCH_VALUES', "values");

/**
 * Specifies alternative rules for following aliases at the server.
 * @link http://www.php.net/manual/en/ldap.constants.php
 */
define ('LDAP_OPT_DEREF', 2);

/**
 * <p>
 *     Specifies the maximum number of entries that can be
 *     returned on a search operation.
 *    </p>
 *    
 *     
 *      The actual size limit for operations is also bounded
 *      by the server's configured maximum number of return entries.
 *      The lesser of these two settings is the actual size limit.
 * @link http://www.php.net/manual/en/ldap.constants.php
 */
define ('LDAP_OPT_SIZELIMIT', 3);

/**
 * Specifies the number of seconds to wait for search results.
 *    
 *    
 *     
 *      The actual time limit for operations is also bounded
 *      by the server's configured maximum time.
 *      The lesser of these two settings is the actual time limit.
 * @link http://www.php.net/manual/en/ldap.constants.php
 */
define ('LDAP_OPT_TIMELIMIT', 4);

/**
 * Option for ldap_set_option to allow setting network timeout.
 *     (Available as of PHP 5.3.0)
 * @link http://www.php.net/manual/en/ldap.constants.php
 */
define ('LDAP_OPT_NETWORK_TIMEOUT', 20485);

/**
 * Specifies the LDAP protocol to be used (V2 or V3).
 * @link http://www.php.net/manual/en/ldap.constants.php
 */
define ('LDAP_OPT_PROTOCOL_VERSION', 17);
define ('LDAP_OPT_ERROR_NUMBER', 49);

/**
 * Specifies whether to automatically follow referrals returned
 *     by the LDAP server.
 * @link http://www.php.net/manual/en/ldap.constants.php
 */
define ('LDAP_OPT_REFERRALS', 8);
define ('LDAP_OPT_RESTART', 9);
define ('LDAP_OPT_HOST_NAME', 48);
define ('LDAP_OPT_ERROR_STRING', 50);
define ('LDAP_OPT_MATCHED_DN', 51);

/**
 * Specifies a default list of server controls to be sent with each request.
 * @link http://www.php.net/manual/en/ldap.constants.php
 */
define ('LDAP_OPT_SERVER_CONTROLS', 18);

/**
 * Specifies a default list of client controls to be processed with each request.
 * @link http://www.php.net/manual/en/ldap.constants.php
 */
define ('LDAP_OPT_CLIENT_CONTROLS', 19);

/**
 * Specifies a bitwise level for debug traces.
 * @link http://www.php.net/manual/en/ldap.constants.php
 */
define ('LDAP_OPT_DEBUG_LEVEL', 20481);
define ('LDAP_OPT_X_SASL_MECH', 24832);
define ('LDAP_OPT_X_SASL_REALM', 24833);
define ('LDAP_OPT_X_SASL_AUTHCID', 24834);
define ('LDAP_OPT_X_SASL_AUTHZID', 24835);
define ('LDAP_ESCAPE_FILTER', 1);
define ('LDAP_ESCAPE_DN', 2);

// End of ldap v.
?>
