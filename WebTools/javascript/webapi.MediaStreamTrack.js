
/**
 * @brief The MediaStream interface represents a stream of media content. A stream consists of several tracks, like video or audio tracks.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/MediaStreamTrack
 */
function MediaStreamTrack() {

	/**
	 * @brief The read-only property MediaStreamTrack.enabled returns a Boolean with a value of true if the track is enabled, that is allowed to render the media source stream; or false if it is disabled, that is not rendering the media source stream but silence and blackness. If the track has been disconnected, this value can be changed but has no more effect.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MediaStreamTrack/enabled
	 */
	this.enabled = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MediaStreamTrack/getConstraints
	 */
	this.getConstraints = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MediaStreamTrack/applyConstraints
	 */
	this.applyConstraints = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MediaStreamTrack/getSettings
	 */
	this.getSettings = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MediaStreamTrack/getCapabilities
	 */
	this.getCapabilities = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MediaStreamTrack/clone
	 */
	this.clone = function() {};

	/**
	 * @brief The MediaStreamTrack.stop() method stops playing the source associated to the track, both the source and the track are deassociated. The track state is set to ended.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/MediaStreamTrack/stop
	 */
	this.stop = function() {};

}

