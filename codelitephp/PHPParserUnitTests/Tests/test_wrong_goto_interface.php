<?php
interface ifoo {
    public function bar_baz();
}

class foo implements ifoo {
    public function bar_baz() {
        return 1;
    }
}
$a = new foo();
$a->
