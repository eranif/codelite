<?php

namespace NS_Test1\NS_Sub_1\NS_Sub_2;

class ClassInNS
{
    public function foo() {}
    public function bar() {}
    public function baz() {}
}

$_ClassInNS = new ClassInNS();                                      // 3 matches
$_ClassInNSFullPath = new \NS_Test1\NS_Sub_1\NS_Sub_2\ClassInNS();  // 2 matches

$dbhPDO_Bad = new PDO(); // 0 matches
$dbhPDO = new \PDO(); // 100 matches
