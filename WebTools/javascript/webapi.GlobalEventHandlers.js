
/**
 * @brief The GlobalEventHandlers interface describes the event handlers common to several interfaces like HTMLElement, Document, Window, or WorkerGlobalScope for Web Workers. Each of these interfaces can implement more event handlers.
 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers
 */
function GlobalEventHandlers() {

	/**
	 * @brief An event handler for abort events sent to the window. (Not available with Firefox 2 or Safari)
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onabort
	 */
	this.onabort = '';

	/**
	 * @brief The onblur property returns the onBlur event handler code, if any, that exists on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onblur
	 */
	this.onblur = '';

	/**
	 * @brief An event handler for runtime script errors.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onerror
	 */
	this.onerror = '';

	/**
	 * @brief The onfocus property returns the onFocus event handler code on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onfocus
	 */
	this.onfocus = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/oncancel
	 */
	this.oncancel = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/oncanplay
	 */
	this.oncanplay = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/oncanplaythrough
	 */
	this.oncanplaythrough = '';

	/**
	 * @brief The onchange property sets and returns the event handler for the change event.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onchange
	 */
	this.onchange = '';

	/**
	 * @brief The onclick property returns the click event handler code on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onclick
	 */
	this.onclick = '';

	/**
	 * @brief An event handler for close events sent to the window. (Not available with Firefox 2 or Safari)
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onclose
	 */
	this.onclose = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/oncontextmenu" title='An event handler property for right-click events on the window. Unless the default behavior is prevented (see examples below on how to do this), the browser context menu will activate (though IE8 has a bug with this and will not activate the context menu if a contextmenu event handler is defined). Note that this event will occur with any non-disabled right-click event and does not depend on an element possessing the "contextmenu" attribute.'><code>GlobalEventHandlers.oncontextmenu</code></a></dt>
 <dd>Is an <a class=
	 */
	this.oncuechange = '';

	/**
	 * @brief The ondblclick property returns the onDblClick event handler code on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/ondblclick
	 */
	this.ondblclick = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/ondrag
	 */
	this.ondrag = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/ondragend
	 */
	this.ondragend = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/ondragenter
	 */
	this.ondragenter = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/ondragexit
	 */
	this.ondragexit = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/ondragleave
	 */
	this.ondragleave = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/ondragover
	 */
	this.ondragover = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/ondragstart
	 */
	this.ondragstart = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/ondrop
	 */
	this.ondrop = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/ondurationchange
	 */
	this.ondurationchange = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onemptied
	 */
	this.onemptied = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onended
	 */
	this.onended = '';

	/**
	 * @brief An event handler for the input event on the window. The input event is raised when an &lt;input> element value changes.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/oninput
	 */
	this.oninput = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/oninvalid
	 */
	this.oninvalid = '';

	/**
	 * @brief The onkeydown property returns the onKeyDown event handler code on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onkeydown
	 */
	this.onkeydown = '';

	/**
	 * @brief The onkeypress property sets and returns the onKeyPress event handler code for the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onkeypress
	 */
	this.onkeypress = '';

	/**
	 * @brief The onkeyup property returns the onKeyUp event handler code for the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onkeyup
	 */
	this.onkeyup = '';

	/**
	 * @brief An event handler for the load event of a window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onload
	 */
	this.onload = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onloadeddata
	 */
	this.onloadeddata = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onloadedmetadata
	 */
	this.onloadedmetadata = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onloadstart
	 */
	this.onloadstart = '';

	/**
	 * @brief The onmousedown property returns the onmousedown event handler code on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onmousedown
	 */
	this.onmousedown = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onmouseenter
	 */
	this.onmouseenter = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onmouseleave
	 */
	this.onmouseleave = '';

	/**
	 * @brief The onmousemove property returns the mousemove event handler code on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onmousemove
	 */
	this.onmousemove = '';

	/**
	 * @brief The onmouseout property returns the onMouseOut event handler code on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onmouseout
	 */
	this.onmouseout = '';

	/**
	 * @brief The onmouseover property returns the onMouseOver event handler code on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onmouseover
	 */
	this.onmouseover = '';

	/**
	 * @brief The onmouseup property returns the onMouseUp event handler code on the current element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onmouseup
	 */
	this.onmouseup = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onmousewheel
	 */
	this.onmousewheel = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandler/onmozfullscreenchange
	 */
	this.onpause = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onplay
	 */
	this.onplay = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onplaying
	 */
	this.onplaying = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onpointerdown
	 */
	this.onpointerdown = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onpointermove
	 */
	this.onpointermove = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onpointerup
	 */
	this.onpointerup = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onpointercancel
	 */
	this.onpointercancel = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onpointerover
	 */
	this.onpointerover = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onpointerout
	 */
	this.onpointerout = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onpointerenter
	 */
	this.onpointerenter = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onpointerleave
	 */
	this.onpointerleave = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onpointerlockchange
	 */
	this.onprogress = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onratechange
	 */
	this.onratechange = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onreset
	 */
	this.onreset = '';

	/**
	 * @brief An event handler for scroll events on element.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onscroll
	 */
	this.onscroll = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onseeked
	 */
	this.onseeked = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onseeking
	 */
	this.onseeking = '';

	/**
	 * @brief An event handler for the select event on the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onselect
	 */
	this.onselect = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onshow
	 */
	this.onshow = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onsort
	 */
	this.onstalled = '';

	/**
	 * @brief An event handler for the submit event on the window.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onsubmit
	 */
	this.onsubmit = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onsuspend
	 */
	this.onsuspend = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/ontimeupdate
	 */
	this.ontimeupdate = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onvolumechange
	 */
	this.onvolumechange = '';

	/**
	 * @brief The documentation about this has not yet been written; please consider contributing!
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/GlobalEventHandlers/onwaiting
	 */
	this.onwaiting = '';

}

