
/**
 * @brief The SubtleCrypto interface represents a set of cryptographic primitives. It is available via the Crypto.subtle properties available in a window context (via Window.crypto).
 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto
 */
function SubtleCrypto() {

	/**
	 * @brief The SubtleCrypto.encrypt() method returns a Promise of the encrypted data corresponding to the cleartext, algorithm and key given as parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/encrypt
	 */
	this.encrypt() = '';

	/**
	 * @brief The SubtleCrypto.decrypt() method returns a Promise of the cleartext corresponding to the ciphertext, algorithm and key given as parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/decrypt
	 */
	this.decrypt() = '';

	/**
	 * @brief The SubtleCrypto.sign() method returns a Promise containing the signature corresponding to the text, algorithm and key given as parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/sign
	 */
	this.sign() = '';

	/**
	 * @brief The SubtleCrypto.verify() method returns a Promise of a Boolean value indicating if the signature given as parameter matches the text, algorithm and key also given as parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/verify
	 */
	this.verify() = '';

	/**
	 * @brief The SubtleCrypto.digest() method returns a Promise of a digest generated from the hash function and text given as parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/digest
	 */
	this.digest() = '';

	/**
	 * @brief The SubtleCrypto.generateKey() method returns a Promise of a newly generated CryptoKey, for symmetrical algorithms, or a CryptoKeyPair, containing two newly generated keys, for asymmetrical algorithm, that matches the algorithm, the usages and the extractability given as parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/generateKey
	 */
	this.generateKey() = '';

	/**
	 * @brief The SubtleCrypto.deriveKey() method returns a Promise of a newly generated CryptoKey derivated from a master key and a specific algorithm given as parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/deriveKey
	 */
	this.deriveKey() = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/deriveBits
	 */
	this.deriveBits() = '';

	/**
	 * @brief The SubtleCrypto.importKey() method returns a Promise of the CryptoKey generated from the data given in parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/importKey
	 */
	this.importKey() = '';

	/**
	 * @brief The SubtleCrypto.exportKey() method returns a Promise of the key encrypted in the requested format. If the key is not extractable, that is if CryptoKey.extractable returns false, the promise fails with an InvalidAccessError exception; it the format is unknown, the promive fails with a NotSupported exception.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/exportKey
	 */
	this.exportKey() = '';

	/**
	 * @brief The SubtleCrypto.wrapKey() method returns a Promise of a wrapped symmetric key for usage (transfer, storage) in unsecure environments. The wrapped buffer returned is in the format given in parameters, and contained the key wrapped by the give wrapping key with the given algorithm.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/wrapKey
	 */
	this.wrapKey() = '';

	/**
	 * @brief The SubtleCrypto.wrapKey() method returns a Promise of a CryptoKey corresponding to the wrapped key given in parameter.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/unwrapKey
	 */
	this.unwrapKey() = '';

	/**
	 * @brief The SubtleCrypto.encrypt() method returns a Promise of the encrypted data corresponding to the cleartext, algorithm and key given as parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/encrypt
	 */
	this.encrypt = function() {};

	/**
	 * @brief The SubtleCrypto.decrypt() method returns a Promise of the cleartext corresponding to the ciphertext, algorithm and key given as parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/decrypt
	 */
	this.decrypt = function() {};

	/**
	 * @brief The SubtleCrypto.sign() method returns a Promise containing the signature corresponding to the text, algorithm and key given as parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/sign
	 */
	this.sign = function() {};

	/**
	 * @brief The SubtleCrypto.verify() method returns a Promise of a Boolean value indicating if the signature given as parameter matches the text, algorithm and key also given as parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/verify
	 */
	this.verify = function() {};

	/**
	 * @brief The SubtleCrypto.digest() method returns a Promise of a digest generated from the hash function and text given as parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/digest
	 */
	this.digest = function() {};

	/**
	 * @brief The SubtleCrypto.generateKey() method returns a Promise of a newly generated CryptoKey, for symmetrical algorithms, or a CryptoKeyPair, containing two newly generated keys, for asymmetrical algorithm, that matches the algorithm, the usages and the extractability given as parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/generateKey
	 */
	this.generateKey = function() {};

	/**
	 * @brief The SubtleCrypto.deriveKey() method returns a Promise of a newly generated CryptoKey derivated from a master key and a specific algorithm given as parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/deriveKey
	 */
	this.deriveKey = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/deriveBits
	 */
	this.deriveBits = function() {};

	/**
	 * @brief The SubtleCrypto.importKey() method returns a Promise of the CryptoKey generated from the data given in parameters.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/importKey
	 */
	this.importKey = function() {};

	/**
	 * @brief The SubtleCrypto.exportKey() method returns a Promise of the key encrypted in the requested format. If the key is not extractable, that is if CryptoKey.extractable returns false, the promise fails with an InvalidAccessError exception; it the format is unknown, the promive fails with a NotSupported exception.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/exportKey
	 */
	this.exportKey = function() {};

	/**
	 * @brief The SubtleCrypto.wrapKey() method returns a Promise of a wrapped symmetric key for usage (transfer, storage) in unsecure environments. The wrapped buffer returned is in the format given in parameters, and contained the key wrapped by the give wrapping key with the given algorithm.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/wrapKey
	 */
	this.wrapKey = function() {};

	/**
	 * @brief The SubtleCrypto.wrapKey() method returns a Promise of a CryptoKey corresponding to the wrapped key given in parameter.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/unwrapKey
	 */
	this.unwrapKey = function() {};

}

