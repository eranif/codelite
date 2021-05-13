## General
---
Node.js is a platform built on Chrome's JavaScript runtime for easily building fast, scalable network applications. 
Node.js uses an event-driven, non-blocking I/O model that makes it lightweight and efficient, perfect for data-intensive real-time applications that run across distributed devices. With Recent builds of CodeLite, it is possible to create, run and debug Node.js applications.

In short, Node.js is server side JavaScript engine

## The workspace
---

To fully maximize CodeLite's JavaScript capabilities, you first need to create a workspace. A Node.js workspace is simply a folder with a single JSON file `.workspace` which contains some metadata information about the workspace.
You can create a workspace for an existing source files or create a new workspace.

### Create a new workspace
---

- From the main menu, select `File` -> `New` -> `New workspace`
- Select `Node.js`
- In the dialog that shown, set the `Name` and the `Path` of the workspace
- Optionally, check the option to create the workspace on a separate directory

!!! Tip
    To create a workspace from an existing source code base, set the workspace path to your code base location
    and make sure you un-checked the option `Create the workspace in a separate directory`

!!! Tip
    Another way of creating a workspace from an existing code base: Open your file browser (Windows Explorer, Dolphin, Finder etc)
    drag the source code folder and drop it on the workspace view


## Hello World
---

### Creating and running `hello world` 
---

- Create a new workspace as described [here](#create-a-new-workspace)
- Add an empty file to the workspace by right clicking on the folder and selecting `New File`
- In the dialog that is shown, set the file name to `test.js`
- Paste the following content and save the file:

```JavaScript
function printHelloWorld() {
    console.log("Hello World");
}

printHelloWorld();
```

- Click ++ctrl+f5++ to execute it
- In the dialog that shown, click `OK`

### Debugging
----

- Click ++f5++ to launch the debugger dialog
- Accept the default values and click `OK`
- You are ready to debug, use the following table to get started with the debugger:

 Keyboard shortcut                           | Description
 ------------------------------------        |--------------
 ++f9++                                      | Toggle a breakpoint on the current line
 ++f5++                                      | Start or resume the debugger operation
 ++f10++                                     | Next line
 ++f11++                                     | Step into a function
 ++shift+f11++                               | Step out of a function
 Hover over a variable and click ++ctrl++    | Display the debugger tooltip
++shift+f5++                                 | Stop the debugger

!!! Tip
    All the above shortcuts can be changed from the [keyboard shortcuts dialog](../settings/keyboard_shortcuts.md)
