
/**
 * @brief Implements properties from its parent, AudioContext.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/OfflineAudioContext
 */
function OfflineAudioContext() {

	/**
	 * @brief In this simple example, we declare both an AudioContext and an OfflineAudioContext object. We use the AudioContext to load an audio track via XHR (AudioContext.decodeAudioData), then the OfflineAudioContext to render the audio into an AudioBufferSourceNode and play the track through. After the offline audio graph is set up, you need to render it to an AudioBuffer using OfflineAudioContext.startRendering.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/OfflineAudioContext/oncomplete
	 */
	this.oncomplete = '';

	/**
	 * @brief The startRendering() method of the OfflineAudioContext Interface starts rendering the audio graph, taking into account the current connections and the current scheduled changes.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/OfflineAudioContext/startRendering
	 */
	this.startRendering = function() {};

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/OfflineAudioContext/startRendering_(promise)
	 */
	this.fflineAudioContext.startRendering_(promise)

}

