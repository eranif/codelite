
/**
 * @brief The MozContact interface is used to describe a single contact in the device's contact database.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozContact
 */
function MozContact() {

	/**
	 * @brief The id property is a unique string representing the contact within the device's contact database.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/id
	 */
	this.name = '';

	/**
	 * @brief The honorificPrefix property is a list of all the possible honorific prefix usable for the contact.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/honorificPrefix
	 */
	this.honorificPrefix = '';

	/**
	 * @brief The givenName property is a list of all the possible given names usable for the contact.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/givenName
	 */
	this.givenName = '';

	/**
	 * @brief The additionalName property is a list of any additional names (other than given names or family names) usable for the contact.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/additionalName
	 */
	this.additionalName = '';

	/**
	 * @brief The familyName property is a list of all the possible family names usable for the contact.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/familyName
	 */
	this.familyName = '';

	/**
	 * @brief The honorificSuffix property is a list of all the possible honorific suffixes usable for the contact.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/honorificSuffix
	 */
	this.honorificSuffix = '';

	/**
	 * @brief The nickname property is a list of all the possible nicknames for the contact.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/nickname
	 */
	this.nickname = '';

	/**
	 * @brief The email property is a list of all the possible e-mails for the contact.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/email
	 */
	this.email = '';

	/**
	 * @brief The photo property is a list of images for the contact.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/photo
	 */
	this.photo = '';

	/**
	 * @brief The url property is a list of all the possible URLs for the contact.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/url
	 */
	this.url = '';

	/**
	 * @brief The category property is a list of all the different categories the contact is associated with.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/category
	 */
	this.category = '';

	/**
	 * @brief The adr property is a list of all the possible addresses for the contact.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/adr
	 */
	this.adr = '';

	/**
	 * @brief The tel property is a list of all the possible phone numbers for the contact.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/tel
	 */
	this.tel = '';

	/**
	 * @brief The org property is a list of all the possible organizations the contact is affiliated with.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/org
	 */
	this.org = '';

	/**
	 * @brief The jobTitle property is a list of all the possible job titles of the contact.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/jobTitle
	 */
	this.jobTitle = '';

	/**
	 * @brief The bday property represents the birthday date of the contact.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/bday
	 */
	this.bday = '';

	/**
	 * @brief The note property is a list of arbitrary notes related to the contact.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/note
	 */
	this.note = '';

	/**
	 * @brief The impp property is a list of all the possible Instant Messaging addresses for the contact.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/impp
	 */
	this.impp = '';

	/**
	 * @brief The anniversary property represents the anniversary date of the contact (which can be different than its birthday date).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/anniversary
	 */
	this.anniversary = '';

	/**
	 * @brief The sex property represents the actual sex of the contact.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/sex
	 */
	this.sex = '';

	/**
	 * @brief The genderIdentity property represents the actual gender identity of the contact (which can be different than its actual sex).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/genderIdentity
	 */
	this.genderIdentity = '';

	/**
	 * @brief The key property is a list of all the possible encryption keys usable with the contact.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/key
	 */
	this.key = '';

	/**
	 * @brief The init method is used to initialize the mozContact object using a configuration object.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/mozContact/init
	 * @param param - A configuration object which properties are one or more of the properties allow for mozContact. Note that id, published and update will be ignored as they are automatically generated and overridden when the contact is stored.
	 */
	this.init = function(param) {};

}

