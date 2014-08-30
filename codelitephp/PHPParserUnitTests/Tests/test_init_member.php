<?php

class MyClas
{
	public $my_member;
	function foo() {
		$this->my_member = new SQLite("MyDB.db");
	}
}
