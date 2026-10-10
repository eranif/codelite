# PHP
---

PHP code is edited in a [File System Workspace][4]. The PHP language server ([`phpantom_lsp`][6]) provides code completion, navigation, syntax checking, formatting and refactoring, and the PHP plugin provides debugging with [XDebug][1].

## Set up
---

- Create a File System Workspace for the folder with your code: `File` &#8594; `New` &#8594; `New workspace` and select `File System Workspace`
- Install and enable `phpantom_lsp`, see [Language Server Plugin][8]
- To debug, select `XDebug` as the debugger in the workspace settings, see [XDebug][1]

## Old PHP workspaces
---

CodeLite no longer creates PHP workspaces. When an old PHP workspace is opened, CodeLite converts it to a File System Workspace:

- The workspace folder becomes the root folder. Projects outside of this folder are left out
- The file types and the exclude folders of all projects are merged
- The debugger is set to `XDebug`. The XDebug settings of the active project and the breakpoints are kept in `.codelite/xdebug.json`
- The SFTP settings become the remote target of the workspace
- The old workspace file is kept as `<name>.workspace.php-backup`. The project files (`.phprj`) are not changed

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

- Rename a symbol with the `Rename symbol` entry of the editor context menu
- Right click and choose `Code actions...` (or press `Alt-Enter`) to list the refactorings available for the caret position or the selection

[1]: /debuggers/xdebug
[2]: https://github.com/PHPCSStandards/PHP_CodeSniffer
[3]: https://github.com/phpmd/phpmd
[4]: /workspaces/file_system
[6]: https://github.com/PHPantom-dev/phpantom_lsp
[7]: https://phpstan.org/
[8]: /plugins/lsp/#phpantom_lsp-php
