
/**
 * @brief CSSStyleRule represents a single CSS style rule. It implements the CSSRule interface with a type value of 1 (CSSRule.STYLE_RULE).
 * @link https://developer.mozilla.org/en-US/docs/Web/API/CSSStyleRule
 */
function CSSStyleRule() {

	/**
	 * @brief The CSSRule.selectorText() method gets the textual representation of the selector for the rule set. This is implemented in a readonly manner; to set stylesheet rules dynamically, see Using dynamic styling information.
	 * @link https://developer.mozilla.org/en-US/docs/Web/API/CSSStyleRule/selectorText
	 */
	this.selectorText = '';

}

