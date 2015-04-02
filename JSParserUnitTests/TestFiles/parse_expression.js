function Array() {
    this.count = 0;
    this.allocated = 4;
    this.elements = [];
}

function MyClass() {
    /* @var Array */
    this.items = 0;
}

var v = new MyClass();
v.