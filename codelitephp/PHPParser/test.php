<?php

const BASE_URL = "http://codelite.org";
$NAME = "Eran Ifrah";
class PageRenderer
{
	/** private helper method
	 */
	private function _replaceTabPlaceHolder($place_holder, $selected, &$content) {
		global $BASE_URL;
		$tab_name = $this->tab_templates[$place_holder];
		if($selected) {
			$replacement =<<<EOF
<li><a href="$BASE_URL/$tab_name/Main/" target="_self" title="$tab_name" class="selected"><span>$tab_name</span></a></li>
EOF;
			$content = str_replace($place_holder,
						$replacement,
						$content);
		} else {
			$replacement =<<<EOF
<li><a href="$BASE_URL/$tab_name/Main/" target="_self" title="$tab_name"><span>$tab_name</span></a></li>
EOF;
			$content = str_replace($place_holder,
						$replacement,
						$content);
		}
	}

	function foo() {
	}
}

$SecondGlobal = "Second Global";
$ThirdGlobal = 3;