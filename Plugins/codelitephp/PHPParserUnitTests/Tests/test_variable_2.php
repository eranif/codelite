<?php

class test_variable_2_return_value {

    public function return_value_1(){
    }

    public function return_value_2(){
    }

    public function return_value_3(){
    }
}

class test_variable_2 {
    protected $name = "test_variable_2";
    
    /** @return test_variable_2_return_value */
    public function foo(){
    }

    public function func_with_args($a, $b){
    }
    
    /**
     * @param $name
     */
    public function setName($name){
        $this->name = $name;
    }
    
    /**
     * @return $name
     */
    public function getName(){
        return $this->name;
    }
}

$a = new test_variable_2();
$a->func_with_args($a->foo(), $a->