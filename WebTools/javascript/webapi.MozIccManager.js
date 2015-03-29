
/**
 * @brief The MozIccManager interface gives access to ICC related functionalities.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozIccManager
 */
function MozIccManager() {

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozIccManager/cardState
	 */
	this.oncardstatechange = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozIccManager/oniccinfochange
	 */
	this.oniccinfochange = '';

	/**
	 * @brief The sendStkResponse method is used to send a response back to ICC after an attempt to execute STK Proactive Command.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozIccManager/sendStkResponse
	 * @param command - A stkCommand object previously received from ICC through the stkcommand event.
	 * @param response - A stkReponse object to be sent to the ICC.
	 */
	this.sendStkResponse = function(command, response) {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozIccManager/sendStkMenuSelection
	 */
	this.sendStkMenuSelection = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozIccManager/sendStkTimerExpiration
	 */
	this.sendStkTimerExpiration = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozIccManager/sendStkEventDownload
	 */
	this.sendStkEventDownload = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozIccManager/updateContact
	 */
	this.updateContact = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozIccManager/iccOpenChannel
	 */
	this.iccOpenChannel = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozIccManager/iccExchangeAPDU
	 */
	this.iccExchangeAPDU = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozIccManager/iccCloseChannel
	 */
	this.iccCloseChannel = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozIccManager/getCardLock
	 */
	this.getCardLock = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozIccManager/unlockCardLock
	 */
	this.unlockCardLock = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozIccManager/setCardLock
	 */
	this.setCardLock = function() {};

}

