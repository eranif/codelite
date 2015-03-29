
/**
 * @brief The CryptoKey interface represents a cryptographic key derived from a specific key algorithm.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/CryptoKey
 */
function CryptoKey() {

	/**
	 * @brief The CryptoKey.extractable read-only property that indicates if the raw material key can be extracted, for example for archiving it.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CryptoKey/type" title='The CryptoKey.type read-only property that indicates the type of the key: if it is the key for a symmetric algorithm ("secret") or for an asymmetric algorithm ("public" or "private", depending of its purpose).'><code>CryptoKey.type</code></a></dt>
 <dd>Returns an enumerated value representing the type of the key, a secret key (for symmetric algorithm), a public or a private key (for an asymmetric algorithm)</dd>
 <dt><a href=
	 */
	this.extractable = '';

	/**
	 * @brief The CryptoKey.algorithm read-only property is an opaque value containing all the information about the algorithm related to the key.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CryptoKey/algorithm
	 */
	this.algorithm = '';

	/**
	 * @brief The CryptoKey.usages read-only property is an array of enumerated that indicates the purposes of the key.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CryptoKey/usages
	 */
	this.usages = '';

}

