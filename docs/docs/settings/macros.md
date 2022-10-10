# Macros
---

Macros are special place holders used by CodeLite. The macros are honored by most of CodeLite plugins and its core.

Below is a list of supported macros by CodeLite:

 Macro name | Description
 -----------|-------------
 `$(ProjectPath)`|Current project folder path
 `$(WorkspaceName)`|The current workspace name
 `$(WorkspaceConfiguration)`|The workspace configuration (e.g. `Debug`)
 `$(WorkspacePath)`|The workspace path. For remote workspaces, this expands to the remote workspace path
 `$(ProjectName)`|Current project name
 `$(IntermediateDirectory)`|The location where CodeLite places object files (`.o`)
 `$(ConfigurationName)`|The current **project** configuration name. e.g. (`Debug`)
 `$(OutDir)`|the location where CodeLite places the build artifacts
 `$(ProjectFiles)`|Space delimited list of project files, relative to the project
 `$(ProjectFilesAbs)`|Space delimited list of project files using absolute path
 `$(CurrentFileName)`|The current file name: `/path/to/file.cpp` &#8594; `file`
 `$(CurrentFilePath)`|The current file path: `/path/to/file.cpp` &#8594; `/path/to`
 `$(CurrentFileExt)`|The current file extension: `/path/to/file.cpp` &#8594; `cpp`
 `$(CurrentFileFullPath)`|The current file full path: `/path/to/file.cpp` &#8594; `/path/to/file.cpp`
 `$(User)`| The current user (e.g. Linux login id)
 `$(Date)`| The current date
 `$(CodeLitePath)`| CodeLite installation folder
 `$(CurrentSelection)`| Returns the selected string in the editor
 `$(OutputDirectory)`| An alias to `$(OutDir)`
 `$(ProjectOutputFile)`| The C++ project artifact name
 `$(SSH_AccountName)`| When a remote workspace is loaded, this expands to the current account name connected
 `$(SSH_Host)`| When a remote workspace is loaded, this expands to the current host connected
 `$(SSH_User)`| When a remote workspace is loaded, this expands to the SSH'd user





