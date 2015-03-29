
/**
 * @brief Provides support for sending and managing both MMS and SMS messages on a device with WebSMS.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileMessageManager
 */
function MozMobileMessageManager() {

	/**
	 * @brief The delete method is used to delete a given message.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileMessageManager/delete
	 * @param message - The id of the message to delete or the message itself as a MozSmsMessage or MozMmsMessage object.
	 */
	this.ozMobileMessageManager.delete(param)

	/**
	 * @brief The getMessage method is used to access a given SMS message.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileMessageManager/getMessage
	 * @param id - The id of the message to retrieve.
	 */
	this.ozMobileMessageManager.getMessage(id)

	/**
	 * @brief The getMessages method is used to access a list of messages.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileMessageManager/getMessages
	 * @param filter - A MozSmsFilter object used to filter the messages to retrieve.
	 * @param reverse - A boolean indicating if the order to access the messages is reversed (true) or not (false).
	 */
	this.ozMobileMessageManager.getMessages(filter, reverseOrder)

	/**
	 * @brief The getSegmentInfoForText method is used to get the information necessary to create a multi-part SMS for a given text.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileMessageManager/getSegmentInfoForText
	 * @param text - A string representing the text to send as a potential multi-part SMS.
	 */
	this.ozMobileMessageManager.getSegmentInfoForText(text)

	/**
	 * @brief The markMessageRead method is used to change the read state of a given message.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileMessageManager/markMessageRead
	 * @param id - The id of the message to mark as read or unread.
	 * @param isRead - A boolean indicating if the message should be marked as read (true) or unread (false).
	 */
	this.ozMobileMessageManager.markMessageRead(id, isRead)

	/**
	 * @brief The send method is used to send an SMS message to one or more receivers.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileMessageManager/send
	 * @param receivers - This parameter can be a string (representing the phone number of the receiver) or an Array of string to have the message sent to more than one receiver.
	 * @param message - A string representing the message to send.
	 */
	this.ozMobileMessageManager.send(number, message)

	/**
	 * @brief The sendMMS method is used to send an MMS message to one or more receivers.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileMessageManager/sendMMS
	 * @param param - A configuration object which is an MmsParameters object
	 * @param receivers - An array of strings, with each string representing the phone number of a receiver.
	 * @param subject - A string representing the subject of the MMS.
	 * @param smil - A stringified version of the SMIL document defining the MMS.
	 * @param attachments - An array of Attachment objects used by the SMIL document.
	 * @param id - The unique id of the attachment.
	 * @param location - A string representing the name/location of the attachment within the SMIL content of the MMS.
	 * @param content - A Blob representing the content of the attachment.
	 */
	this.ozMobileMessageManager.sendMMS(param)

	/**
	 * @brief The getThreads method is used to iterate through a list of MozMobileMessageThread objects. Each of those objects represents a conversation between participants.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileMessageManager/getThreads
	 */
	this.getThreads = function() {};

	/**
	 * @brief The retrieveMMS method is used to access a given MMS message.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozMobileMessageManager/retrieveMMS
	 * @param id - The id of the message to retrieve.
	 */
	this.ozMobileMessageManager.retrieveMMS(id)

}

