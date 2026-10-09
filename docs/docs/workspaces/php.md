# General
---

As of CodeLite 7.0, PHP support in CodeLite has been extended to provide a fully workable PHP environment for editing and building a PHP application or a simple website.

The PHP plugin supports (but is not limited to) the following:

- Code Completion
- Debugging capabilities via [XDebug][1]
- Syntax checking
- Simplified project management
- Class wizard
- Remote synchronizing over SFTP
- Re-factoring 

## Concepts
---

### The Workspace file
---
CodeLite's PHP workspace file contains the following information:

- List of project files
- Metadata (workspace file format version and other info needed by CodeLite) 

Only one PHP workspace can be loaded at a time within CodeLite; however, each workspace may contain multiple projects.
Each workspace is represented on the file system as a single file with the extension `.workspace`

### The Project files
--

Each PHP project corresponds to a real directory on the file system (i.e. there are no Virtual Directories as there are with standard C++ CodeLite projects).

A project may contain sub folders, each corresponding to a dir in the matching position on the file system. However you don't have to include every real dir; 
you may define filters to define which dirs and files you wish to view (e.g. `.git`, `.svn` etc) in the project settings.

!!! Important
    Projects can't be nested: a project tree can't contain another project.
    
## Getting Started by an example
---

### Create your first workspace
---
- Create an empty workspace: `File` &#8594; `New` &#8594; `New workspace` and select `PHP`
- Choose the workspace `Path`, for the `Name` set it to `test_php` and click `OK`

### Add your first project

* Right click on the PHP workspace icon in the tree view and select `Create a new project` to start the new project wizard the wizard will guide you through the new project process by gathering the following information:

| Project property  | Description |
|-------------------|-------------|
| `Project name`      | The name of the project |
| `Project location`  | the project location. This is where CodeLite will create the `.project` file|
| `Project type`      | Is this a web project or command line? |
| `PHP executable`    | Path to `php` executable (`php.exe` on Windows)|
| `Code completion folders` | Extra locations where CodeLite parser can find PHP files for code completion purposes|

For the purpose of our example, set the `Project name` to `HelloWorldPHP`

* Your tree view should look similar to this:

![PHP Demo](php_tree_view_1.png)

* Create a new file: `hello_world.php` by right clicking on the `HelloWorldPHP` folder in the tree view and choose `New file...`
* In the dialog that pops, set the file name to `hello_world.php`
* Paste the below code onto the newly created `hello_world.php` file:

```php
<?php

class MyClass {
    public function __construct() {}
    public function hello_world() {
        echo "Hello world";
    }
}

$my_class = new MyClass();
$my_class->hello_world();
```

* Run your code by clicking ++ctrl+f5++
* The `Run project` will show, keep the default values and click `OK`
* You should see a CodeLite terminal with the `Hello World` message shown

![Console Hello World](php_hello_world_console.png)

## Syntax checking
---

Syntax checking is provided by the PHP language server ([`phpantom_lsp`][6]).
It reports syntax errors and runs [PHP_CodeSniffer][2], [PHPMD][3] and [PHPStan][7] when they are found in the project's `vendor/bin` folder or in the `PATH`.

When an issue is detected a marker will be placed to the left of the affected line, hover the marker to view a description of the issue

See [Language Server Plugin][8] for how to install and enable `phpantom_lsp`.
The linters are configured in the project's `.phpantom.toml` file and in the tools' own configuration files (for example `phpcs.xml` or `phpstan.neon`).

## Code refactoring
---

Refactoring (rename, extract method, optimize `use` statements and more) is provided by the PHPantom language server.
See [Language Server Plugin][8] for how to install and enable `phpantom_lsp`.

- Rename a symbol with the `Rename` entry of the editor context menu
- Right click and choose `Code actions...` (or press `Alt-Enter`) to list the refactorings available for the caret position or the selection

[1]: /debuggers/xdebug
[2]: https://github.com/squizlabs/PHP_CodeSniffer
[3]: https://github.com/phpmd/phpmd
[6]: https://github.com/PHPantom-dev/phpantom_lsp
[7]: https://phpstan.org/
[8]: /plugins/lsp/#phpantom_lsp-php
