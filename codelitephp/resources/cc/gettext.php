<?php

// Start of gettext v.

/**
 * Sets the default domain
 * @link http://www.php.net/manual/en/function.textdomain.php
 * @param text_domain string <p>
 *       The new message domain, or &null; to get the current setting without
 *       changing it
 *      </p>
 * @return string If successful, this function returns the current message
 *   domain, after possibly changing it.
 */
function textdomain ($text_domain) {}

/**
 * Lookup a message in the current domain
 * @link http://www.php.net/manual/en/function.gettext.php
 * @param message string <p>
 *       The message being translated.
 *      </p>
 * @return string a translated string if one is found in the 
 *   translation table, or the submitted message if not found.
 */
function gettext ($message) {}

/**
 * @param msgid
 */
function _ ($msgid) {}

/**
 * Override the current domain
 * @link http://www.php.net/manual/en/function.dgettext.php
 * @param domain string <p>
 *       The domain
 *      </p>
 * @param message string <p>
 *       The message
 *      </p>
 * @return string A string on success.
 */
function dgettext ($domain, $message) {}

/**
 * Overrides the domain for a single lookup
 * @link http://www.php.net/manual/en/function.dcgettext.php
 * @param domain string <p>
 *       The domain
 *      </p>
 * @param message string <p>
 *       The message
 *      </p>
 * @param category int <p>
 *       The category
 *      </p>
 * @return string A string on success.
 */
function dcgettext ($domain, $message, $category) {}

/**
 * Sets the path for a domain
 * @link http://www.php.net/manual/en/function.bindtextdomain.php
 * @param domain string <p>
 *       The domain
 *      </p>
 * @param directory string <p>
 *       The directory path
 *      </p>
 * @return string The full pathname for the domain currently being set.
 */
function bindtextdomain ($domain, $directory) {}

/**
 * Plural version of gettext
 * @link http://www.php.net/manual/en/function.ngettext.php
 * @param msgid1 string <p>
 *      </p>
 * @param msgid2 string <p>
 *      </p>
 * @param n int <p>
 *      </p>
 * @return string correct plural form of message identified by 
 *   msgid1 and msgid2
 *   for count n.
 */
function ngettext ($msgid1, $msgid2, $n) {}

/**
 * Plural version of dgettext
 * @link http://www.php.net/manual/en/function.dngettext.php
 * @param domain string <p>
 *       The domain
 *      </p>
 * @param msgid1 string <p>
 *      </p>
 * @param msgid2 string <p>
 *      </p>
 * @param n int <p>
 *      </p>
 * @return string A string on success.
 */
function dngettext ($domain, $msgid1, $msgid2, $n) {}

/**
 * Plural version of dcgettext
 * @link http://www.php.net/manual/en/function.dcngettext.php
 * @param domain string <p>
 *       The domain
 *      </p>
 * @param msgid1 string <p>
 *      </p>
 * @param msgid2 string <p>
 *      </p>
 * @param n int <p>
 *      </p>
 * @param category int <p>
 *      </p>
 * @return string A string on success.
 */
function dcngettext ($domain, $msgid1, $msgid2, $n, $category) {}

/**
 * Specify the character encoding in which the messages from the DOMAIN message catalog will be returned
 * @link http://www.php.net/manual/en/function.bind-textdomain-codeset.php
 * @param domain string <p>
 *       The domain
 *      </p>
 * @param codeset string <p>
 *       The code set
 *      </p>
 * @return string A string on success.
 */
function bind_textdomain_codeset ($domain, $codeset) {}

// End of gettext v.
?>
