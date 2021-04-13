# Getting started

## The setup wizard
---

When you start CodeLite for the first time, the setup wizard is loaded.
The setup wizard allows you configure the following:

- Development profile:
    - Default : everything is enabled
    - C/C++ development: CodeLite is tuned for C/C++ development, i.e. all non C/C++ plugins are disabled
    - Web Development: all non web related plugins are disabled
- Setup compilers: CodeLite will attempt do auto detect all installed compilers on your computer and configure them for you
- Customise colours: select the colouring theme
- Whitespace and indentation: basic editing configuration

!!! note "TIP"
    You can always bring the setup wizard from main menu bar: `Help` &#8594; `Run the setup wizard`


## `hello world` program
---

1. Create an empty workspace: from the menu bar, select: `File` &#8594; `New` &#8594; `New workspace` and select `C++`
2. In the next dialog, set the `Workspace path` and `Workspace Name` fields
3. Add new project to the workspace: from the menu bar, select: `File` &#8594; `New` &#8594; `New project`
4. Fill the dialog similar to the dialog shown below (your compiler name is probably different the one shown in screenshot)

![New Project Dialog](img/hello_world/new_project.png)

5. Click the `OK` button
6. CodeLite will generate a sample `main.cpp` file
7. Hit ++f7++ to build the project
8. Hit ++ctrl+f5++ to launch the program
