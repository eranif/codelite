
/**
 * @brief The MozSocial object, returned by the navigator.mozSocial property, is available within the social media provider's panel to provide functionality it may need.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozSocial
 */
function MozSocial() {

	/**
	 * @brief Indicates whether or not the social media provider's content sidebar is currently visible.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozSocial/isVisible
	 */
	this.isVisible = '';

	/**
	 * @brief Closes the currently open social panel on the parent social media provider panel.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozSocial/closePanel
	 */
	this.closePanel = function() {};

	/**
	 * @brief Performs some, possibly platform-specific, action designed to get the user's attention. This might involve playing a sound effect, flashing the screen, or presenting some other form of effect.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozSocial/getAttention
	 */
	this.getAttention = function() {};

	/**
	 * @brief Returns a reference to the service worker for the social media service. Once you have this, you use the port attribute to call the postMessage() method and communicate with the background worker. These may be one of the standard social messages, or custom messages for the service.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozSocial/getWorker
	 */
	this.getWorker = function() {};

	/**
	 * @brief Opens a chat window, anchored to the bottom of the browser window. Each chat window is typically a one-on-one chat, but this functionality may vary depending on the provider.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozSocial/openChatWindow
	 * @param toUrl - The URL to which to open the chat connection.
	 * @param callback - An object to receive callback notifications on the connection. The callback receives as an input a reference to the chat window. See Implementing chat functionality for details.
	 */
	this.openChatWindow = function(toUrl, callback) {};

	/**
	 * @brief Opens a flyout panel attached to the social media provider's sidebar at a given vertical offset.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MozSocial/openPanel
	 * @param toUrl - The URL to the content to present in the panel.
	 * @param offset - The vertical offset, in pixels, at which to position the flyout panel.
	 * @param callback - An object to receive callback notifications on the connection. The first argument to the callback is a reference to the window object of  the flyout.
	 */
	this.openPanel = function(toUrl, offset, callback) {};

}

