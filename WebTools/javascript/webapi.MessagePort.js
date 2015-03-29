
/**
 * @brief The MessagePort interface of the Channel Messaging API represents one of the two ports of a MessageChannel, allowing sending of messages from one port and listening out for them arriving at the other.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/MessagePort
 */
function MessagePort() {

	/**
	 * @brief The postMessage() method of the MessagePort interface sends a message from the port, and optionally, transfers ownership of objects to other browsing contexts.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MessagePort/postMessage
	 * @param message - The message you want to send through the channel. This can be of any basic data type. Multiple data items can be sent as an array.
	 * @param [, transfer] - (optional) Objects to be transferred — these objects have their ownership transferred to the receiving browsing context, so are no longer usable by the sending browsing context.
	 */
	this.essagePort.postMessage

	/**
	 * @brief The start() method of the MessagePort interface starts the sending of messages queued on the port. This method is only needed when using EventTarget.addEventListener; it is implied when using MessageChannel.onmessage.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MessagePort/start
	 */
	this.essagePort.start

	/**
	 * @brief The close() method of the MessagePort interface disconnects the port, so it is no longer active. This stops the flow of messages to that port.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MessagePort/close
	 */
	this.essagePort.close

}

