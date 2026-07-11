<?php

/**
 * global function that starts with 'p'
 */
function poof_global() {}

class test_cc_with_keywords {
    public function poof() {}
    public function bar() {
        p // cc should suggest here 2 entries: parent and poof_global
        
