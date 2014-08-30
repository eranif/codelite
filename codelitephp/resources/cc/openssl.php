<?php

// Start of openssl v.

/**
 * Frees a private key
 * @link http://www.php.net/manual/en/function.openssl-pkey-free.php
 * @param key resource <p>
 *       Resource holding the key.
 *      </p>
 * @return void 
 */
function openssl_pkey_free ($key) {}

/**
 * Generates a new private key
 * @link http://www.php.net/manual/en/function.openssl-pkey-new.php
 * @param configargs array[optional] <p>
 *       You can finetune the key generation (such as specifying the number of
 *       bits) using configargs.  See
 *       openssl_csr_new for more information about
 *       configargs.
 *      </p>
 * @return resource a resource identifier for the pkey on success, or false on
 *   error.
 */
function openssl_pkey_new (array $configargs = null) {}

/**
 * Gets an exportable representation of a key into a string
 * @link http://www.php.net/manual/en/function.openssl-pkey-export.php
 * @param key mixed <p>
 *      </p>
 * @param out string <p>
 *      </p>
 * @param passphrase string[optional] <p>
 *       The key is optionally protected by passphrase.
 *      </p>
 * @param configargs array[optional] <p>
 *       configargs can be used to fine-tune the export
 *       process by specifying and/or overriding options for the openssl
 *       configuration file.  See openssl_csr_new for more
 *       information about configargs.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function openssl_pkey_export ($key, &$out, $passphrase = null, array $configargs = null) {}

/**
 * Gets an exportable representation of a key into a file
 * @link http://www.php.net/manual/en/function.openssl-pkey-export-to-file.php
 * @param key mixed <p>
 *      </p>
 * @param outfilename string <p>
 *       Path to the output file.
 *      </p>
 * @param passphrase string[optional] <p>
 *       The key can be optionally protected by a
 *       passphrase.
 *      </p>
 * @param configargs array[optional] <p>
 *       configargs can be used to fine-tune the export
 *       process by specifying and/or overriding options for the openssl
 *       configuration file. See openssl_csr_new for more
 *       information about configargs.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function openssl_pkey_export_to_file ($key, $outfilename, $passphrase = null, array $configargs = null) {}

/**
 * Get a private key
 * @link http://www.php.net/manual/en/function.openssl-pkey-get-private.php
 * @param key mixed <p>
 *       key can be one of the following:
 *       
 *        a string having the format
 *          file://path/to/file.pem. The named file must
 *          contain a PEM encoded certificate/private key (it may contain both).
 * @param passphrase string[optional] <p>
 *       The optional parameter passphrase must be used
 *       if the specified key is encrypted (protected by a passphrase).
 *      </p>
 * @return resource a positive key resource identifier on success, or false on error.
 */
function openssl_pkey_get_private ($key, $passphrase = null) {}

/**
 * Extract public key from certificate and prepare it for use
 * @link http://www.php.net/manual/en/function.openssl-pkey-get-public.php
 * @param certificate mixed <p>
 *       certificate can be one of the following:
 *       
 *        an X.509 certificate resource
 * @return resource a positive key resource identifier on success, or false on error.
 */
function openssl_pkey_get_public ($certificate) {}

/**
 * Returns an array with the key details
 * @link http://www.php.net/manual/en/function.openssl-pkey-get-details.php
 * @param key resource <p>
 *       Resource holding the key.
 *      </p>
 * @return array an array with the key details in success or false in failure.
 *   Returned array has indexes bits (number of bits),
 *   key (string representation of the public key) and
 *   type (type of the key which is one of
 *   OPENSSL_KEYTYPE_RSA,
 *   OPENSSL_KEYTYPE_DSA,
 *   OPENSSL_KEYTYPE_DH,
 *   OPENSSL_KEYTYPE_EC or -1 meaning unknown).
 *  </p>
 *  <p>
 *   Depending on the key type used, additional details may be returned. Note that 
 *   some elements may not always be available.
 */
function openssl_pkey_get_details ($key) {}

/**
 * Free key resource
 * @link http://www.php.net/manual/en/function.openssl-free-key.php
 * @param key_identifier resource <p>
 *      </p>
 * @return void 
 */
function openssl_free_key ($key_identifier) {}

/**
 * &Alias; <function>openssl_pkey_get_private</function>
 * @link http://www.php.net/manual/en/function.openssl-get-privatekey.php
 * @param key
 * @param passphrase[optional]
 */
function openssl_get_privatekey ($key, $passphrase) {}

/**
 * &Alias; <function>openssl_pkey_get_public</function>
 * @link http://www.php.net/manual/en/function.openssl-get-publickey.php
 * @param cert
 */
function openssl_get_publickey ($cert) {}

/**
 * Parse an X.509 certificate and return a resource identifier for
  it
 * @link http://www.php.net/manual/en/function.openssl-x509-read.php
 * @param x509certdata mixed <p>
 *      </p>
 * @return resource a resource identifier on success&return.falseforfailure;.
 */
function openssl_x509_read ($x509certdata) {}

/**
 * Free certificate resource
 * @link http://www.php.net/manual/en/function.openssl-x509-free.php
 * @param x509cert resource <p>
 *      </p>
 * @return void 
 */
function openssl_x509_free ($x509cert) {}

/**
 * Parse an X509 certificate and return the information as an array
 * @link http://www.php.net/manual/en/function.openssl-x509-parse.php
 * @param x509cert mixed <p>
 *      </p>
 * @param shortnames bool[optional] <p>
 *       shortnames controls how the data is indexed in the
 *       array - if shortnames is true (the default) then
 *       fields will be indexed with the short name form, otherwise, the long name
 *       form will be used - e.g.: CN is the shortname form of commonName.
 *      </p>
 * @return array The structure of the returned data is (deliberately) not
 *   yet documented, as it is still subject to change.
 */
function openssl_x509_parse ($x509cert, $shortnames = null) {}

/**
 * Verifies if a certificate can be used for a particular purpose
 * @link http://www.php.net/manual/en/function.openssl-x509-checkpurpose.php
 * @param x509cert mixed <p>
 *       The examined certificate.
 *      </p>
 * @param purpose int <p>
 *       <table>
 *        openssl_x509_checkpurpose purposes
 *        
 *         
 *          <tr valign="top">
 *           <td>Constant</td>
 *           <td>Description</td>
 *          </tr>
 *         
 *         
 *          <tr valign="top">
 *           <td>X509_PURPOSE_SSL_CLIENT</td>
 *           <td>Can the certificate be used for the client side of an SSL
 *           connection?</td>
 *          </tr>
 *          <tr valign="top">
 *           <td>X509_PURPOSE_SSL_SERVER</td>
 *           <td>Can the certificate be used for the server side of an SSL
 *           connection?</td>
 *          </tr>
 *          <tr valign="top">
 *           <td>X509_PURPOSE_NS_SSL_SERVER</td>
 *           <td>Can the cert be used for Netscape SSL server?</td>
 *          </tr>
 *          <tr valign="top">
 *           <td>X509_PURPOSE_SMIME_SIGN</td>
 *           <td>Can the cert be used to sign S/MIME email?</td>
 *          </tr>
 *          <tr valign="top">
 *           <td>X509_PURPOSE_SMIME_ENCRYPT</td>
 *           <td>Can the cert be used to encrypt S/MIME email?</td>
 *          </tr>
 *          <tr valign="top">
 *           <td>X509_PURPOSE_CRL_SIGN</td>
 *           <td>Can the cert be used to sign a certificate revocation list
 *           (CRL)?</td>
 *          </tr>
 *          <tr valign="top">
 *           <td>X509_PURPOSE_ANY</td>
 *           <td>Can the cert be used for Any/All purposes?</td>
 *          </tr>
 *         
 *        
 *       </table>
 *       These options are not bitfields - you may specify one only!
 *      </p>
 * @param cainfo array[optional] <p>
 *       cainfo should be an array of trusted CA files/dirs
 *       as described in Certificate
 *       Verification.
 *      </p>
 * @param untrustedfile string[optional] <p>
 *       If specified, this should be the name of a PEM encoded file holding
 *       certificates that can be used to help verify the certificate, although
 *       no trust is placed in the certificates that come from that file.
 *      </p>
 * @return int true if the certificate can be used for the intended purpose,
 *   false if it cannot, or -1 on error.
 */
function openssl_x509_checkpurpose ($x509cert, $purpose, array $cainfo = null, $untrustedfile = null) {}

/**
 * Checks if a private key corresponds to a certificate
 * @link http://www.php.net/manual/en/function.openssl-x509-check-private-key.php
 * @param cert mixed <p>
 *       The certificate.
 *      </p>
 * @param key mixed <p>
 *       The private key.
 *      </p>
 * @return bool true if key is the private key that
 *   corresponds to cert, or false otherwise.
 */
function openssl_x509_check_private_key ($cert, $key) {}

/**
 * Exports a certificate as a string
 * @link http://www.php.net/manual/en/function.openssl-x509-export.php
 * @param x509 mixed 
 * @param output string <p>
 *       On success, this will hold the PEM.
 *      </p>
 * @param notext bool[optional] &note.openssl.param-notext;
 * @return bool Returns true on success or false on failure.
 */
function openssl_x509_export ($x509, &$output, $notext = null) {}

/**
 * Exports a certificate to file
 * @link http://www.php.net/manual/en/function.openssl-x509-export-to-file.php
 * @param x509 mixed 
 * @param outfilename string <p>
 *       Path to the output file.
 *      </p>
 * @param notext bool[optional] &note.openssl.param-notext;
 * @return bool Returns true on success or false on failure.
 */
function openssl_x509_export_to_file ($x509, $outfilename, $notext = null) {}

/**
 * Exports a PKCS#12 Compatible Certificate Store File to variable.
 * @link http://www.php.net/manual/en/function.openssl-pkcs12-export.php
 * @param x509 mixed 
 * @param out string <p>
 *       On success, this will hold the PKCS#12.
 *      </p>
 * @param priv_key mixed <p>
 *      Private key component of PKCS#12 file.
 *      </p>
 * @param pass string <p>
 *      Encryption password for unlocking the PKCS#12 file.
 *      </p>
 * @param args array[optional] <p>
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function openssl_pkcs12_export ($x509, &$out, $priv_key, $pass, array $args = null) {}

/**
 * Exports a PKCS#12 Compatible Certificate Store File
 * @link http://www.php.net/manual/en/function.openssl-pkcs12-export-to-file.php
 * @param x509 mixed 
 * @param filename string <p>
 *       Path to the output file.
 *      </p>
 * @param priv_key mixed <p>
 *      Private key component of PKCS#12 file.
 *      </p>
 * @param pass string <p>
 *      Encryption password for unlocking the PKCS#12 file.
 *      </p>
 * @param args array[optional] <p>
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function openssl_pkcs12_export_to_file ($x509, $filename, $priv_key, $pass, array $args = null) {}

/**
 * Parse a PKCS#12 Certificate Store into an array
 * @link http://www.php.net/manual/en/function.openssl-pkcs12-read.php
 * @param pkcs12 string <p>
 *      </p>
 * @param certs array <p>
 *       On success, this will hold the Certificate Store Data.
 *      </p>
 * @param pass string <p>
 *      Encryption password for unlocking the PKCS#12 file.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function openssl_pkcs12_read ($pkcs12, array &$certs, $pass) {}

/**
 * Generates a CSR
 * @link http://www.php.net/manual/en/function.openssl-csr-new.php
 * @param dn array <p>
 *       The Distinguished Name to be used in the certificate.
 *      </p>
 * @param privkey resource <p>
 *       privkey should be set to a private key that was
 *       previously generated by openssl_pkey_new (or
 *       otherwise obtained from the other openssl_pkey family of functions).
 *       The corresponding public portion of the key will be used to sign the
 *       CSR.
 *      </p>
 * @param configargs array[optional] <p>
 *       By default, the information in your system openssl.conf
 *       is used to initialize the request; you can specify a configuration file
 *       section by setting the config_section_section key of
 *       configargs.  You can also specify an alternative
 *       openssl configuration file by setting the value of the
 *       config key to the path of the file you want to use.
 *       The following keys, if present in configargs
 *       behave as their equivalents in the openssl.conf, as
 *       listed in the table below.
 *       <table>
 *        Configuration overrides
 *        
 *         
 *          <tr valign="top">
 *           <td>configargs key</td>
 *           <td>type</td>
 *           <td>openssl.conf equivalent</td>
 *           <td>description</td>
 *          </tr>
 *         
 *         
 *          <tr valign="top">
 *           <td>digest_alg</td>
 *           <td>string</td>
 *           <td>default_md</td>
 *           <td>Selects which digest method to use</td>
 *          </tr>
 *          <tr valign="top">
 *           <td>x509_extensions</td>
 *           <td>string</td>
 *           <td>x509_extensions</td>
 *           <td>Selects which extensions should be used when creating an x509
 *           certificate</td>
 *          </tr>
 *          <tr valign="top">
 *           <td>req_extensions</td>
 *           <td>string</td>
 *           <td>req_extensions</td>
 *           <td>Selects which extensions should be used when creating a CSR</td>
 *          </tr>
 *          <tr valign="top">
 *           <td>private_key_bits</td>
 *           <td>integer</td>
 *           <td>default_bits</td>
 *           <td>Specifies how many bits should be used to generate a private
 *            key</td>
 *          </tr>
 *          <tr valign="top">
 *           <td>private_key_type</td>
 *           <td>integer</td>
 *           <td>none</td>
 *           <td>Specifies the type of private key to create.  This can be one
 *            of OPENSSL_KEYTYPE_DSA,
 *            OPENSSL_KEYTYPE_DH or
 *            OPENSSL_KEYTYPE_RSA.
 *            The default value is OPENSSL_KEYTYPE_RSA which
 *            is currently the only supported key type.
 *           </td>
 *          </tr>
 *          <tr valign="top">
 *           <td>encrypt_key</td>
 *           <td>boolean</td>
 *           <td>encrypt_key</td>
 *           <td>Should an exported key (with passphrase) be encrypted?</td>
 *          </tr>
 *          <tr valign="top">
 *           <td>encrypt_key_cipher</td>
 *           <td>integer</td>
 *           <td>none</td>
 *           <td>
 *            One of cipher constants.
 *           </td>
 *          </tr>
 *         
 *        
 *       </table>
 *      </p>
 * @param extraattribs array[optional] <p>
 *       extraattribs is used to specify additional
 *       configuration options for the CSR.  Both dn and
 *       extraattribs are associative arrays whose keys are
 *       converted to OIDs and applied to the relevant part of the request.
 *      </p>
 * @return mixed the CSR.
 */
function openssl_csr_new (array $dn, &$privkey, array $configargs = null, array $extraattribs = null) {}

/**
 * Exports a CSR as a string
 * @link http://www.php.net/manual/en/function.openssl-csr-export.php
 * @param csr resource <p>
 *      </p>
 * @param out string <p>
 *      </p>
 * @param notext bool[optional] &note.openssl.param-notext;
 * @return bool Returns true on success or false on failure.
 */
function openssl_csr_export ($csr, &$out, $notext = null) {}

/**
 * Exports a CSR to a file
 * @link http://www.php.net/manual/en/function.openssl-csr-export-to-file.php
 * @param csr resource <p>
 *      </p>
 * @param outfilename string <p>
 *       Path to the output file.
 *      </p>
 * @param notext bool[optional] &note.openssl.param-notext;
 * @return bool Returns true on success or false on failure.
 */
function openssl_csr_export_to_file ($csr, $outfilename, $notext = null) {}

/**
 * Sign a CSR with another certificate (or itself) and generate a certificate
 * @link http://www.php.net/manual/en/function.openssl-csr-sign.php
 * @param csr mixed <p>
 *       A CSR previously generated by openssl_csr_new.
 *       It can also be the path to a PEM encoded CSR when specified as
 *       file://path/to/csr or an exported string generated
 *       by openssl_csr_export.
 *      </p>
 * @param cacert mixed <p>
 *       The generated certificate will be signed by cacert.
 *       If cacert is &null;, the generated certificate
 *       will be a self-signed certificate.
 *      </p>
 * @param priv_key mixed <p>
 *       priv_key is the private key that corresponds to
 *       cacert.
 *      </p>
 * @param days int <p>
 *       days specifies the length of time for which the
 *       generated certificate will be valid, in days.
 *      </p>
 * @param configargs array[optional] <p>
 *       You can finetune the CSR signing by configargs.
 *       See openssl_csr_new for more information about
 *       configargs.
 *      </p>
 * @param serial int[optional] <p>
 *       An optional the serial number of issued certificate. If not specified
 *       it will default to 0.
 *      </p>
 * @return resource an x509 certificate resource on success, false on failure.
 */
function openssl_csr_sign ($csr, $cacert, $priv_key, $days, array $configargs = null, $serial = null) {}

/**
 * Returns the subject of a CERT
 * @link http://www.php.net/manual/en/function.openssl-csr-get-subject.php
 * @param csr mixed 
 * @param use_shortnames bool[optional] 
 * @return array 
 */
function openssl_csr_get_subject ($csr, $use_shortnames = null) {}

/**
 * Returns the public key of a CERT
 * @link http://www.php.net/manual/en/function.openssl-csr-get-public-key.php
 * @param csr mixed 
 * @param use_shortnames bool[optional] 
 * @return resource 
 */
function openssl_csr_get_public_key ($csr, $use_shortnames = null) {}

/**
 * Computes a digest
 * @link http://www.php.net/manual/en/function.openssl-digest.php
 * @param data string <p>
 *       The data.
 *      </p>
 * @param method string <p>
 *       The digest method.
 *      </p>
 * @param raw_output bool[optional] <p>
 *       Setting to true will return as raw output data, otherwise the return
 *       value is binhex encoded.
 *      </p>
 * @return string the digested hash value on success&return.falseforfailure;.
 */
function openssl_digest ($data, $method, $raw_output = null) {}

/**
 * Encrypts data
 * @link http://www.php.net/manual/en/function.openssl-encrypt.php
 * @param data string <p>
 *       The data.
 *      </p>
 * @param method string <p>
 *       The cipher method.
 *      </p>
 * @param password string <p>
 *       The password.
 *      </p>
 * @param raw_output bool[optional] <p>
 *       Setting to true will return as raw output data, otherwise the return
 *       value is base64 encoded.
 *      </p>
 * @param iv string[optional] <p>
 *       A non-NULL Initialization Vector.
 *      </p>
 * @return string the encrypted string on success&return.falseforfailure;.
 */
function openssl_encrypt ($data, $method, $password, $raw_output = null, $iv = null) {}

/**
 * Decrypts data
 * @link http://www.php.net/manual/en/function.openssl-decrypt.php
 * @param data string <p>
 *       The data.
 *      </p>
 * @param method string <p>
 *       The cipher method.
 *      </p>
 * @param password string <p>
 *       The password.
 *      </p>
 * @param raw_input bool[optional] <p>
 *       Setting to true will take a raw encoded string,
 *       otherwise a base64 string is assumed for the
 *       data parameter.
 *      </p>
 * @param iv string[optional] <p>
 *       A non-NULL Initialization Vector. 
 *      </p>
 * @return string The decrypted string on success&return.falseforfailure;.
 */
function openssl_decrypt ($data, $method, $password, $raw_input = null, $iv = null) {}

/**
 * Gets the cipher iv length
 * @link http://www.php.net/manual/en/function.openssl-cipher-iv-length.php
 * @param method string <p>
 *      The method.
 *     </p>
 * @return int the cipher length on success, or false on failure.
 */
function openssl_cipher_iv_length ($method) {}

/**
 * Generate signature
 * @link http://www.php.net/manual/en/function.openssl-sign.php
 * @param data string <p>
 *      </p>
 * @param signature string <p>
 *       If the call was successful the signature is returned in
 *       signature.
 *      </p>
 * @param priv_key_id mixed <p>
 *      </p>
 * @param signature_alg int[optional] <p>
 *       For more information see the list of Signature Algorithms.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function openssl_sign ($data, &$signature, $priv_key_id, $signature_alg = null) {}

/**
 * Verify signature
 * @link http://www.php.net/manual/en/function.openssl-verify.php
 * @param data string <p>
 *      </p>
 * @param signature string <p>
 *      </p>
 * @param pub_key_id mixed <p>
 *      </p>
 * @param signature_alg int[optional] <p>
 *       For more information see the list of Signature Algorithms.
 *      </p>
 * @return int 1 if the signature is correct, 0 if it is incorrect, and
 *   -1 on error.
 */
function openssl_verify ($data, $signature, $pub_key_id, $signature_alg = null) {}

/**
 * Seal (encrypt) data
 * @link http://www.php.net/manual/en/function.openssl-seal.php
 * @param data string <p>
 *      </p>
 * @param sealed_data string <p>
 *      </p>
 * @param env_keys array <p>
 *      </p>
 * @param pub_key_ids array <p>
 *      </p>
 * @param method string[optional] 
 * @return int the length of the sealed data on success, or false on error.
 *   If successful the sealed data is returned in
 *   sealed_data, and the envelope keys in
 *   env_keys.
 */
function openssl_seal ($data, &$sealed_data, array &$env_keys, array $pub_key_ids, $method = null) {}

/**
 * Open sealed data
 * @link http://www.php.net/manual/en/function.openssl-open.php
 * @param sealed_data string <p>
 *      </p>
 * @param open_data string <p>
 *       If the call is successful the opened data is returned in this
 *       parameter.
 *      </p>
 * @param env_key string <p>
 *      </p>
 * @param priv_key_id mixed <p>
 *      </p>
 * @param method string[optional] 
 * @return bool Returns true on success or false on failure.
 */
function openssl_open ($sealed_data, &$open_data, $env_key, $priv_key_id, $method = null) {}

/**
 * Verifies the signature of an S/MIME signed message
 * @link http://www.php.net/manual/en/function.openssl-pkcs7-verify.php
 * @param filename string <p>
 *       Path to the message.
 *      </p>
 * @param flags int <p>
 *       flags can be used to affect how the signature is
 *       verified - see PKCS7 constants
 *       for more information.
 *      </p>
 * @param outfilename string[optional] <p>
 *       If the outfilename is specified, it should be a
 *       string holding the name of a file into which the certificates of the
 *       persons that signed the messages will be stored in PEM format.
 *      </p>
 * @param cainfo array[optional] <p>
 *       If the cainfo is specified, it should hold
 *       information about the trusted CA certificates to use in the verification
 *       process - see certificate
 *       verification for more information about this parameter.
 *      </p>
 * @param extracerts string[optional] <p>
 *       If the extracerts is specified, it is the filename
 *       of a file containing a bunch of certificates to use as untrusted CAs.
 *      </p>
 * @param content string[optional] <p>
 *       You can specify a filename with content that will
 *       be filled with the verified data, but with the signature information
 *       stripped.
 *      </p>
 * @return mixed true if the signature is verified, false if it is not correct
 *   (the message has been tampered with, or the signing certificate is invalid),
 *   or -1 on error.
 */
function openssl_pkcs7_verify ($filename, $flags, $outfilename = null, array $cainfo = null, $extracerts = null, $content = null) {}

/**
 * Decrypts an S/MIME encrypted message
 * @link http://www.php.net/manual/en/function.openssl-pkcs7-decrypt.php
 * @param infilename string <p>
 *      </p>
 * @param outfilename string <p>
 *       The decrypted message is written to the file specified by
 *       outfilename.
 *      </p>
 * @param recipcert mixed <p>
 *      </p>
 * @param recipkey mixed[optional] <p>
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function openssl_pkcs7_decrypt ($infilename, $outfilename, $recipcert, $recipkey = null) {}

/**
 * Sign an S/MIME message
 * @link http://www.php.net/manual/en/function.openssl-pkcs7-sign.php
 * @param infilename string <p>
 *      </p>
 * @param outfilename string <p>
 *      </p>
 * @param signcert mixed <p>
 *      </p>
 * @param privkey mixed <p>
 *      </p>
 * @param headers array <p>
 *       headers is an array of headers that
 *       will be prepended to the data after it has been signed (see
 *       openssl_pkcs7_encrypt for more information about
 *       the format of this parameter).
 *      </p>
 * @param flags int[optional] <p>
 *       flags can be used to alter the output - see PKCS7 constants.
 *      </p>
 * @param extracerts string[optional] <p>
 *       extracerts specifies the name of a file containing
 *       a bunch of extra certificates to include in the signature which can for
 *       example be used to help the recipient to verify the certificate that you used.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function openssl_pkcs7_sign ($infilename, $outfilename, $signcert, $privkey, array $headers, $flags = null, $extracerts = null) {}

/**
 * Encrypt an S/MIME message
 * @link http://www.php.net/manual/en/function.openssl-pkcs7-encrypt.php
 * @param infile string <p>
 *      </p>
 * @param outfile string <p>
 *      </p>
 * @param recipcerts mixed <p>
 *       Either a lone X.509 certificate, or an array of X.509 certificates.
 *      </p>
 * @param headers array <p>
 *       headers is an array of headers that
 *       will be prepended to the data after it has been encrypted.
 *      </p>
 *      <p>
 *       headers can be either an associative array
 *       keyed by header name, or an indexed array, where each element contains
 *       a single header line.
 *      </p>
 * @param flags int[optional] <p>
 *       flags can be used to specify options that affect
 *       the encoding process - see PKCS7
 *       constants.
 *      </p>
 * @param cipherid int[optional] <p>
 *       One of cipher constants.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function openssl_pkcs7_encrypt ($infile, $outfile, $recipcerts, array $headers, $flags = null, $cipherid = null) {}

/**
 * Encrypts data with private key
 * @link http://www.php.net/manual/en/function.openssl-private-encrypt.php
 * @param data string <p>
 *      </p>
 * @param crypted string <p>
 *      </p>
 * @param key mixed <p>
 *      </p>
 * @param padding int[optional] <p>
 *       padding can be one of
 *       OPENSSL_PKCS1_PADDING,
 *       OPENSSL_NO_PADDING.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function openssl_private_encrypt ($data, &$crypted, $key, $padding = null) {}

/**
 * Decrypts data with private key
 * @link http://www.php.net/manual/en/function.openssl-private-decrypt.php
 * @param data string <p>
 *      </p>
 * @param decrypted string <p>
 *      </p>
 * @param key mixed <p>
 *       key must be the private key corresponding that
 *       was used to encrypt the data.  
 *      </p>
 * @param padding int[optional] <p>
 *       padding can be one of
 *       OPENSSL_PKCS1_PADDING,
 *       OPENSSL_SSLV23_PADDING,
 *       OPENSSL_PKCS1_OAEP_PADDING,
 *       OPENSSL_NO_PADDING.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function openssl_private_decrypt ($data, &$decrypted, $key, $padding = null) {}

/**
 * Encrypts data with public key
 * @link http://www.php.net/manual/en/function.openssl-public-encrypt.php
 * @param data string <p>
 *      </p>
 * @param crypted string <p>
 *       This will hold the result of the encryption.
 *      </p>
 * @param key mixed <p>
 *       The public key.
 *      </p>
 * @param padding int[optional] <p>
 *       padding can be one of
 *       OPENSSL_PKCS1_PADDING,
 *       OPENSSL_SSLV23_PADDING,
 *       OPENSSL_PKCS1_OAEP_PADDING,
 *       OPENSSL_NO_PADDING.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function openssl_public_encrypt ($data, &$crypted, $key, $padding = null) {}

/**
 * Decrypts data with public key
 * @link http://www.php.net/manual/en/function.openssl-public-decrypt.php
 * @param data string <p>
 *      </p>
 * @param decrypted string <p>
 *      </p>
 * @param key mixed <p>
 *       key must be the public key corresponding that
 *       was used to encrypt the data.  
 *      </p>
 * @param padding int[optional] <p>
 *       padding can be one of
 *       OPENSSL_PKCS1_PADDING,
 *       OPENSSL_NO_PADDING.
 *      </p>
 * @return bool Returns true on success or false on failure.
 */
function openssl_public_decrypt ($data, &$decrypted, $key, $padding = null) {}

/**
 * Gets available digest methods
 * @link http://www.php.net/manual/en/function.openssl-get-md-methods.php
 * @param aliases bool[optional] <p>
 *       Set to true if digest aliases should be included within the
 *       returned array.
 *      </p>
 * @return array An array of available digest methods.
 */
function openssl_get_md_methods ($aliases = null) {}

/**
 * Gets available cipher methods
 * @link http://www.php.net/manual/en/function.openssl-get-cipher-methods.php
 * @param aliases bool[optional] <p>
 *       Set to true if cipher aliases should be included within the
 *       returned array.
 *      </p>
 * @return array An array of available cipher methods.
 */
function openssl_get_cipher_methods ($aliases = null) {}

/**
 * Computes shared secret for public value of remote DH key and local DH key
 * @link http://www.php.net/manual/en/function.openssl-dh-compute-key.php
 * @param pub_key string <p>
 *       Public key
 *      </p>
 * @param dh_key resource <p>
 *       DH key
 *      </p>
 * @return string computed key on success&return.falseforfailure;.
 */
function openssl_dh_compute_key ($pub_key, $dh_key) {}

/**
 * Generate a pseudo-random string of bytes
 * @link http://www.php.net/manual/en/function.openssl-random-pseudo-bytes.php
 * @param length int <p>
 *       The length of the desired string of bytes. Must be a positive integer. PHP will
 *       try to cast this parameter to a non-null integer to use it. 
 *      </p>
 * @param crypto_strong bool[optional] <p>
 *       If passed into the function, this will hold a boolean value that determines
 *       if the algorithm used was "cryptographically strong", e.g., safe for usage with GPG, 
 *       passwords, etc. true if it did, otherwise false
 *      </p>
 * @return string the generated &string; of bytes on success, &return.falseforfailure;.
 */
function openssl_random_pseudo_bytes ($length, &$crypto_strong = null) {}

/**
 * Return openSSL error message
 * @link http://www.php.net/manual/en/function.openssl-error-string.php
 * @return string an error message string, or false if there are no more error
 *   messages to return.
 */
function openssl_error_string () {}

define ('OPENSSL_VERSION_TEXT', "OpenSSL 0.9.8o 01 Jun 2010");
define ('OPENSSL_VERSION_NUMBER', 9470207);
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
define ('OPENSSL_CIPHER_RC2_40', 0);
define ('OPENSSL_CIPHER_RC2_128', 1);
define ('OPENSSL_CIPHER_RC2_64', 2);
define ('OPENSSL_CIPHER_DES', 3);
define ('OPENSSL_CIPHER_3DES', 4);
define ('OPENSSL_KEYTYPE_RSA', 0);
define ('OPENSSL_KEYTYPE_DSA', 1);
define ('OPENSSL_KEYTYPE_DH', 2);
define ('OPENSSL_KEYTYPE_EC', 3);

/**
 * Whether SNI support is available or not.
 * @link http://www.php.net/manual/en/openssl.constants.php
 */
define ('OPENSSL_TLSEXT_SERVER_NAME', 1);

// End of openssl v.
?>
