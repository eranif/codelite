#include <stdio.h>

struct Foo
{
    struct Tada
    {
        int bar;
    };

    int Bar(Tada baz);
};

int Foo::Bar(Tada