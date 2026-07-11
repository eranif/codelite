<?php

class MemberClass {}
class test_class_member_initialized {
    public $a = new MemberClass();
}

$cls = new test_class_member_initialized();
$cls->a-> // type of a is \MemberClass