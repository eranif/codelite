<?php

// Start of openssl v.

function openssl_get_cert_locations () {}

/**
 * @param privkey
 * @param challenge
 * @param algo[optional]
 */
function openssl_spki_new ($privkey, $challenge, $algo) {}

/**
 * @param spki
 */
function openssl_spki_verify ($spki) {}

/**
 * @param spki
 */
function openssl_spki_export ($spki) {}

/**
 * @param spki
 */
function openssl_spki_export_challenge ($spki) {}

/**
 * @param key
 */
function openssl_pkey_free ($key) {}

/**
 * @param configargs[optional]
 */
function openssl_pkey_new ($configargs) {}

/**
 * @param key
 * @param out
 * @param passphrase[optional]
 * @param config_args[optional]
 */
function openssl_pkey_export ($key, &$out, $passphrase, $config_args) {}

/**
 * @param key
 * @param outfilename
 * @param passphrase[optional]
 * @param config_args[optional]
 */
function openssl_pkey_export_to_file ($key, $outfilename, $passphrase, $config_args) {}

/**
 * @param key
 * @param passphrase[optional]
 */
function openssl_pkey_get_private ($key, $passphrase) {}

/**
 * @param cert
 */
function openssl_pkey_get_public ($cert) {}

/**
 * @param key
 */
function openssl_pkey_get_details ($key) {}

/**
 * @param key
 */
function openssl_free_key ($key) {}

/**
 * @param key
 * @param passphrase[optional]
 */
function openssl_get_privatekey ($key, $passphrase) {}

/**
 * @param cert
 */
function openssl_get_publickey ($cert) {}

/**
 * @param cert
 */
function openssl_x509_read ($cert) {}

/**
 * @param x509
 */
function openssl_x509_free ($x509) {}

/**
 * @param x509
 * @param shortname
 */
function openssl_x509_parse ($x509, $shortname) {}

/**
 * @param x509cert
 * @param purpose
 * @param cainfo
 * @param untrustedfile[optional]
 */
function openssl_x509_checkpurpose ($x509cert, $purpose, $cainfo, $untrustedfile) {}

/**
 * @param cert
 * @param key
 */
function openssl_x509_check_private_key ($cert, $key) {}

/**
 * @param x509
 * @param out
 * @param notext[optional]
 */
function openssl_x509_export ($x509, &$out, $notext) {}

/**
 * @param x509
 * @param method[optional]
 * @param raw_output[optional]
 */
function openssl_x509_fingerprint ($x509, $method, $raw_output) {}

/**
 * @param x509
 * @param outfilename
 * @param notext[optional]
 */
function openssl_x509_export_to_file ($x509, $outfilename, $notext) {}

/**
 * @param x509
 * @param out
 * @param priv_key
 * @param pass
 * @param args
 */
function openssl_pkcs12_export ($x509, &$out, $priv_key, $pass, $args) {}

/**
 * @param x509
 * @param filename
 * @param priv_key
 * @param pass
 * @param args[optional]
 */
function openssl_pkcs12_export_to_file ($x509, $filename, $priv_key, $pass, $args) {}

/**
 * @param PKCS12
 * @param certs
 * @param pass
 */
function openssl_pkcs12_read ($PKCS12, &$certs, $pass) {}

/**
 * @param dn
 * @param privkey
 * @param configargs[optional]
 * @param extraattribs[optional]
 */
function openssl_csr_new ($dn, &$privkey, $configargs, $extraattribs) {}

/**
 * @param csr
 * @param out
 * @param notext[optional]
 */
function openssl_csr_export ($csr, &$out, $notext) {}

/**
 * @param csr
 * @param outfilename
 * @param notext[optional]
 */
function openssl_csr_export_to_file ($csr, $outfilename, $notext) {}

/**
 * @param csr
 * @param x509
 * @param priv_key
 * @param days
 * @param config_args[optional]
 * @param serial[optional]
 */
function openssl_csr_sign ($csr, $x509, $priv_key, $days, $config_args, $serial) {}

/**
 * @param csr
 */
function openssl_csr_get_subject ($csr) {}

/**
 * @param csr
 */
function openssl_csr_get_public_key ($csr) {}

/**
 * @param data
 * @param method
 * @param raw_output[optional]
 */
function openssl_digest ($data, $method, $raw_output) {}

/**
 * @param data
 * @param method
 * @param password
 * @param options[optional]
 * @param iv[optional]
 */
function openssl_encrypt ($data, $method, $password, $options, $iv) {}

/**
 * @param data
 * @param method
 * @param password
 * @param options[optional]
 * @param iv[optional]
 */
function openssl_decrypt ($data, $method, $password, $options, $iv) {}

/**
 * @param method
 */
function openssl_cipher_iv_length ($method) {}

/**
 * @param data
 * @param signature
 * @param key
 * @param method[optional]
 */
function openssl_sign ($data, &$signature, $key, $method) {}

/**
 * @param data
 * @param signature
 * @param key
 * @param method[optional]
 */
function openssl_verify ($data, $signature, $key, $method) {}

/**
 * @param data
 * @param sealdata
 * @param ekeys
 * @param pubkeys
 */
function openssl_seal ($data, &$sealdata, &$ekeys, $pubkeys) {}

/**
 * @param data
 * @param opendata
 * @param ekey
 * @param privkey
 */
function openssl_open ($data, &$opendata, $ekey, $privkey) {}

/**
 * @param password
 * @param salt
 * @param key_length
 * @param iterations
 * @param digest_algorithm[optional]
 */
function openssl_pbkdf2 ($password, $salt, $key_length, $iterations, $digest_algorithm) {}

/**
 * @param filename
 * @param flags
 * @param signerscerts[optional]
 * @param cainfo[optional]
 * @param extracerts[optional]
 * @param content[optional]
 */
function openssl_pkcs7_verify ($filename, $flags, $signerscerts, $cainfo, $extracerts, $content) {}

/**
 * @param infilename
 * @param outfilename
 * @param recipcert
 * @param recipkey[optional]
 */
function openssl_pkcs7_decrypt ($infilename, $outfilename, $recipcert, $recipkey) {}

/**
 * @param infile
 * @param outfile
 * @param signcert
 * @param signkey
 * @param headers
 * @param flags[optional]
 * @param extracertsfilename[optional]
 */
function openssl_pkcs7_sign ($infile, $outfile, $signcert, $signkey, $headers, $flags, $extracertsfilename) {}

/**
 * @param infile
 * @param outfile
 * @param recipcerts
 * @param headers
 * @param flags[optional]
 * @param cipher[optional]
 */
function openssl_pkcs7_encrypt ($infile, $outfile, $recipcerts, $headers, $flags, $cipher) {}

/**
 * @param data
 * @param crypted
 * @param key
 * @param padding[optional]
 */
function openssl_private_encrypt ($data, &$crypted, $key, $padding) {}

/**
 * @param data
 * @param crypted
 * @param key
 * @param padding[optional]
 */
function openssl_private_decrypt ($data, &$crypted, $key, $padding) {}

/**
 * @param data
 * @param crypted
 * @param key
 * @param padding[optional]
 */
function openssl_public_encrypt ($data, &$crypted, $key, $padding) {}

/**
 * @param data
 * @param crypted
 * @param key
 * @param padding[optional]
 */
function openssl_public_decrypt ($data, &$crypted, $key, $padding) {}

/**
 * @param aliases[optional]
 */
function openssl_get_md_methods ($aliases) {}

/**
 * @param aliases[optional]
 */
function openssl_get_cipher_methods ($aliases) {}

/**
 * @param pub_key
 * @param dh_key
 */
function openssl_dh_compute_key ($pub_key, $dh_key) {}

/**
 * @param length
 * @param result_is_strong[optional]
 */
function openssl_random_pseudo_bytes ($length, &$result_is_strong) {}

function openssl_error_string () {}

define ('OPENSSL_VERSION_TEXT', "OpenSSL 1.0.1e 11 Feb 2013");
define ('OPENSSL_VERSION_NUMBER', 268439647);
define ('X509_PURPOSE_SSL_CLIENT', 1);
define ('X509_PURPOSE_SSL_SERVER', 2);
define ('X509_PURPOSE_NS_SSL_SERVER', 3);
define ('X509_PURPOSE_SMIME_SIGN', 4);
define ('X509_PURPOSE_SMIME_ENCRYPT', 5);
define ('X509_PURPOSE_CRL_SIGN', 6);
define ('X509_PURPOSE_ANY', 7);

/**
 * Used as default algorithm by openssl_sign and
 *       openssl_verify.
 * @link http://www.php.net/manual/en/openssl.constants.php
 */
define ('OPENSSL_ALGO_SHA1', 1);
define ('OPENSSL_ALGO_MD5', 2);
define ('OPENSSL_ALGO_MD4', 3);
define ('OPENSSL_ALGO_DSS1', 5);
define ('OPENSSL_ALGO_SHA224', 6);
define ('OPENSSL_ALGO_SHA256', 7);
define ('OPENSSL_ALGO_SHA384', 8);
define ('OPENSSL_ALGO_SHA512', 9);
define ('OPENSSL_ALGO_RMD160', 10);

/**
 * When signing a message, use cleartext signing with the MIME
 *         type "multipart/signed". This is the default
 *         if you do not specify any flags to
 *         openssl_pkcs7_sign.
 *         If you turn this option off, the message will be signed using
 *         opaque signing, which is more resistant to translation by mail relays
 *         but cannot be read by mail agents that do not support S/MIME.
 * @link http://www.php.net/manual/en/openssl.constants.php
 */
define ('PKCS7_DETACHED', 64);

/**
 * Adds text/plain content type headers to encrypted/signed
 *         message. If decrypting or verifying, it strips those headers from
 *         the output - if the decrypted or verified message is not of MIME type
 *         text/plain then an error will occur.
 * @link http://www.php.net/manual/en/openssl.constants.php
 */
define ('PKCS7_TEXT', 1);

/**
 * When verifying a message, certificates (if
 *           any) included in the message are normally searched for the
 *           signing certificate. With this option only the
 *           certificates specified in the extracerts
 *           parameter of openssl_pkcs7_verify are
 *           used.  The supplied certificates can still be used as
 *           untrusted CAs however.
 * @link http://www.php.net/manual/en/openssl.constants.php
 */
define ('PKCS7_NOINTERN', 16);

/**
 * Do not verify the signers certificate of a signed
 *         message.
 * @link http://www.php.net/manual/en/openssl.constants.php
 */
define ('PKCS7_NOVERIFY', 32);

/**
 * Do not chain verification of signers certificates: that is
 *         don't use the certificates in the signed message as untrusted CAs.
 * @link http://www.php.net/manual/en/openssl.constants.php
 */
define ('PKCS7_NOCHAIN', 8);

/**
 * When signing a message the signer's certificate is normally
 *         included - with this option it is excluded.  This will reduce the
 *         size of the signed message but the verifier must have a copy of the
 *         signers certificate available locally (passed using the
 *         extracerts to
 *         openssl_pkcs7_verify for example).
 * @link http://www.php.net/manual/en/openssl.constants.php
 */
define ('PKCS7_NOCERTS', 2);

/**
 * Normally when a message is signed, a set of attributes are
 *         included which include the signing time and the supported symmetric
 *         algorithms.  With this option they are not included.
 * @link http://www.php.net/manual/en/openssl.constants.php
 */
define ('PKCS7_NOATTR', 256);

/**
 * Normally the input message is converted to "canonical" format
 *         which is effectively using CR and LF
 *          as end of line: as required by the S/MIME specification.  When this
 *          option is present, no translation occurs. This is useful when
 *          handling binary data which may not be in MIME format.
 * @link http://www.php.net/manual/en/openssl.constants.php
 */
define ('PKCS7_BINARY', 128);

/**
 * Don't try and verify the signatures on a message
 * @link http://www.php.net/manual/en/openssl.constants.php
 */
define ('PKCS7_NOSIGS', 4);
define ('OPENSSL_PKCS1_PADDING', 1);
define ('OPENSSL_SSLV23_PADDING', 2);
define ('OPENSSL_NO_PADDING', 3);
define ('OPENSSL_PKCS1_OAEP_PADDING', 4);
define ('OPENSSL_DEFAULT_STREAM_CIPHERS', "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:AES128:AES256:HIGH:!SSLv2:!aNULL:!eNULL:!EXPORT:!DES:!MD5:!RC4:!ADH");
define ('OPENSSL_CIPHER_RC2_40', 0);
define ('OPENSSL_CIPHER_RC2_128', 1);
define ('OPENSSL_CIPHER_RC2_64', 2);
define ('OPENSSL_CIPHER_DES', 3);
define ('OPENSSL_CIPHER_3DES', 4);
define ('OPENSSL_CIPHER_AES_128_CBC', 5);
define ('OPENSSL_CIPHER_AES_192_CBC', 6);
define ('OPENSSL_CIPHER_AES_256_CBC', 7);
define ('OPENSSL_KEYTYPE_RSA', 0);
define ('OPENSSL_KEYTYPE_DSA', 1);
define ('OPENSSL_KEYTYPE_DH', 2);

/**
 * This constant is only available when PHP is compiled with OpenSSL 0.9.8+.
 * @link http://www.php.net/manual/en/openssl.constants.php
 */
define ('OPENSSL_KEYTYPE_EC', 3);
define ('OPENSSL_RAW_DATA', 1);
define ('OPENSSL_ZERO_PADDING', 2);

/**
 * Whether SNI support is available or not.
 * @link http://www.php.net/manual/en/openssl.constants.php
 */
define ('OPENSSL_TLSEXT_SERVER_NAME', 1);

// End of openssl v.
?>
