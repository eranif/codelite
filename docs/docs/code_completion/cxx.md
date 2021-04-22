## Two types of engines
---

The C++ code completion in CodeLite, is powered by two open source tools:

- `clangd` a local daemon installed either by CodeLite (for `macOS` or `Windows`) or by the package manager (`Linux`). This daemon is clang's compiler implementation for the `Language Server Protocol`
- `ctags` a rough but efficient indexer tool

The `clangd` (the default completion engine) is discussed in details in the [Language Server Plugin](../plugins/lsp.md) section.

In this page we will cover the `ctags` code completion engine

## Ctags code completion
---

The code completion feature in CodeLite covers the followings:

- Auto completion of variables, classes, functions etc
- Go to definition / declaration
- Open resource of any type
- Hover tooltips
- File Outline 

## Setting Up `ctags` Code Completion
---

When you first create a workspace that contains C++ files, CodeLite will automatically scan for the following files:

- Any C/C++ files included in the workspace
- Any header files in the workspace
- Any file included by an `include` directive from a workspace file

The above file list is then parsed and the parsed information is then stored in the workspace directory under the folder `.codelite/<WORKSPACE-NAME>.db`
for later retrieval

Each time a file is saved, the database is updated automatically (only for the saved file)

## Code Completing
---

If possible, CodeLite will offers a completion list where it makes sense.

For example, given the below class:

```c++
class MyClass {
public:
    std::string m_name;
    std::string m_last_name;
};
```

And your code looks like this:

```c++
MyClass cls;
cls. // <-- code completion appears here
```

We can expect CodeLite to show the completion box with 2 options: `m_name` and `m_last_name`

### Configuring `ctags`
---

From the main menu bar, select `Settings` &#8594; `Code completion...` this brings the code completion dialog.

Select the `CTags` tab, which contains 3 child tabs:

* `Search paths`: this contains a list of global search paths to be added to the workspace folders. Usually, CodeLite will use this list to find any compiler header files e.g. `stdlib.h`
* `Exclude paths`: CodeLite will ignore any file found in these paths
* `Advanced` &#8594; `Tokens`: Contains list of tokens to be processed by `ctags`, usually, you want to add here macros which confuses `ctags`
* `Advanced` &#8594; `Types`: Contains  list of types that `ctags` should consider them as another type

### Tokens
---

There are 3 types of tokens that can be used here:

#### Type 1: simple tokens

This type takes the form of `x=y` syntax.

Lets say that you have the following code snippet:

```c++
CLASS myClass {
public:
    void foo(){};
};
```

The example above uses `CLASS` as a preprocessor macro which expands to something different for each platform. 
For instance, `CLASS` may be defined as `class __declspec(dllexport)` on Windows platforms while on macOS and Linux, it expands to `class`
Normally, the absence of the C++ keyword class would cause the source file to be incorrectly parsed by `ctags`

You can add the following entry to the `Tokens` list to resolve this:

```
CLASS=class
```

#### Type 2: Ignore entry

Many APIs are using functions / class decorators which confuses `ctags` parser, consider the following forward declaration line:

```c++
class WXDLLIMPEXP_FWD_RICHTEXT wxRichTextCtrl;
```

By parsing the above line, `ctags` will mistakenly count `wxRichTextCtrl` as a variable of type `WXDLLIMPEXP_FWD_RICHTEXT` rather than 
forward declaration of the class. To fix this, one need to add the following entry to the `Token` table:

```
WXDLLIMPEXP_FWD_RICHTEXT
```

By adding the above entry to the `Tokens` table `ctags` now knows to ignore this token, so the above code snippet is "seen" like this
by `ctags`:

```c++
 class wxRichTextCtrl;
```

#### Type 3: Composite replacement entry

Some macros are too complex and can not be resolved by using the simple `X=Y` or the 'ignore' entries. 
These kind of macros require the third type of token syntax: composite replacement

For example: most of the Python C API methods looks like this: 

```c++
PyAPI_FUNC(void*) PySomeFunctionName(PyObject*);
```

The syntax above is too complex for the `ctags` parser to understand that the above signature is actually:

```c++
void* PySomeFunctionName(PyObject*);
```

We could use the naive solution of adding the following entry:

```c++
PyAPI_FUNC(void*)=void*
```

However, there are many variations of the above:

```c++
PyAPI_FUNC(PyObject*)
PyAPI_FUNC(int)
...
```

For these case, we can use the following token entry:

```c++
PyAPI_FUNC(%0)=%0
```

Lets have a look at another example:

wxWidgets defines the macro `wxConstCast` like this: `#define wxConstCast(t, x) const_cast<t>(x)`
Using the composite replacement syntax, we can add an entry with the following form:

```c++
wxConstCast(%0, %1)=const_cast<%0>(%1)
```

!!! Important
    Adding new patterns to the `Tokens` tab requires a parsing of the workspace
    from: `Workspace` &#8594; `Parse workspace`
    
#### Types
---

Some C++ is too complex for `ctags` to handle. This is where you can help `ctags` the code a litte better
For example:

`std::queue::reference` definition is too complex for `ctags` to understand.
However, by looking at the code, we know that `std::queue::reference` is actually `_Tp` (the template parameter)

To help the parser, we can add the following entry:

```c++
std::queue::reference=_Tp
```

!!! Note
    The `Types` table already comes populated entries
    We recommend that you don't change this unless you really know what you are doing



