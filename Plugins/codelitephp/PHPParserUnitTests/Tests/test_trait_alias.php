<?php
trait test_trait_alias_A {
    public function smallTalk() {
        echo 'a';
    }
    public function bigTalk() {
        echo 'A';
    }
}

trait test_trait_alias_B {
    public function smallTalk() {
        echo 'b';
    }
    public function bigTalk() {
        echo 'B';
    }
}

class Aliased_Talker {
    use test_trait_alias_A, test_trait_alias_B {
        test_trait_alias_B::smallTalk insteadof test_trait_alias_A;
        test_trait_alias_A::bigTalk insteadof test_trait_alias_B;
        test_trait_alias_B::bigTalk as talk;
    }
}

$a = new Aliased_Talker();

// suggest 3 members here
// smallTalk, bigTalk, talk
$a->