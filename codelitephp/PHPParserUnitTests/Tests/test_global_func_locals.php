<?php
    namespace MyNS;
    
	$v1 = new DatabaseManager();

	function foo($place_holder, $selected, &$content) {
		// global variable
		global $v1;
		// local variable with simple string assignment
		$v2 = "string";
		
		// class allocation
		$v3 = new SQLite("string");
		// assignment with HEREDOC
		$v4 =<<<EOF
<li><a href="$BASE_URL/$tab_name/Main/" target="_self" title="$tab_name"><span>$tab_name</span></a></li>
EOF;
		// call method
		$v5 = method("");
		// Dont close the scope, or else the locals will be discarded
	