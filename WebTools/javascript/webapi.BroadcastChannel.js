
/**
 * @brief The BroadcastChannel interface represents a named channel that any browsing context of a given origin can subscribe to. It allows communication between different documents (in different windows, tabs, frames or iframes) of the same origin. Messages are broadcasted via a message event fired at all BroadcastChannel objects listening to the channel.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/BroadcastChannel
 */
function BroadcastChannel() {

	/**
	 * @brief The read-only BroadcastChannel.name property returns a DOMString, which uniquely identifies the given channel with its name. This name is passed to the BroadcastChannel() constructor at creation time and is therefore read-only.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BroadcastChannel/name
	 */
	this.name = '';

	/**
	 * @brief The BroadcastChannel.onmessage event handler is a property that specifies the function to execute when a message event, of type MessageEvent, is received by this BroadcastChannel. Such an event is sent by the browser with a message broadcasted to the channel.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BroadcastChannel/onmessage
	 */
	this.onmessage = '';

	/**
	 * @brief The BroadcastChannel.postMessage() sends a message, which can be of any kind of Object, to each listener in any browser context with the same origin. The message is transmitted as a message event targeted at each BroadcastChannel bound to the channel.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BroadcastChannel/postMessage
	 */
	this.postMessage = function() {};

	/**
	 * @brief The BroadcastChannel.close() terminates the connection to the underlying channel, allowing the object to be garbage collected. This is a necessary step to perform as there is no other way for a browser to know that this channel is not needed anymore.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/BroadcastChannel/close
	 */
	this.close = function() {};

}

