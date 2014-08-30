<?php

class Bar
{
	public $a;
	public $b;
}

function foo() {
	$a = new Bar();
	return $a;
}

function Baz() {
	return foo();
}
