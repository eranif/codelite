
/**
 * @brief An OscillatorNode is created using the AudioContext.createOscillator method. It always has exactly one output and no inputs, both with the same amount of channels. Its basic property defaults (see AudioNode for definitions) are:
 * @link https://developer.mozilla.org/en-US/docs/Web/API/OscillatorNode
 */
function OscillatorNode() {

	/**
	 * @brief An a-rate AudioParam.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/OscillatorNode/frequency
	 */
	this.frequency = '';

	/**
	 * @brief The following example shows basic usage of an AudioContext to create an oscillator node. For an applied example, check out our Violent Theremin demo (see app.js for relevant code).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/OscillatorNode/start
	 * @param when - An optional double representing the time in seconds after the current time when the oscillator should start. If a value is not included, it defaults to 0, meaning that it starts playing immediately.
	 */
	this.start = function(when) {};

	/**
	 * @brief The following example shows basic usage of an AudioContext to create an oscillator node. For an applied example, check out our Violent Theremin demo (see app.js for relevant code).
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/OscillatorNode/stop
	 * @param when - An optional double representing the time in seconds after the current time when the oscillator should stop. If a value is not included, it defaults to 0, meaning that it stops playing immediately.
	 */
	this.stop = function(when) {};

	/**
	 * @brief void.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/OscillatorNode/setPeriodicWave
	 */
	this.setPeriodicWave = function() {};

}

