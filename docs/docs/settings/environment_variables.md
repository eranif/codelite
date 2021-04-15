# Environment variables
---

From `Settings` &#8594; `Environment Variables` you can define environment variables for CodeLite.
These environment variables are not applied system wide and are only applied to processes launched by
CodeLite (e.g. the build process, executing your program, opening a terminal etc)

## Using pre-defined set
---

This method allows you to organize your environment variables in groups.
You can select which set to use in the C++ workspace

To create a new set:

1. Click on the `New Set...` button
2. Give it a name and click `OK`

## Setting global environment variables
---

Lets explain this with a simple example:
You want to a add new environment name `NEW_HOME` pointing to a different location
And in addition, you want to define another variable name `ORIG_HOME` pointing to the OS home directory.


For Linux, this translates into: 

```bash
NEW_HOME=$(HOME)/new_home
ORIG_HOME=$(HOME)
```

On Windows the syntax is the **same**, we just need to use the correct environment variable name (and use Windows backslash):

```bash
NEW_HOME=$(HOMEPATH)\new_home
ORIG_HOME=$(HOMEPATH)
```

!!! Important
    CodeLite uses the `$` syntax on all platforms
    
## Exporting environment variables
---

CodeLite offers the ability to export the environment variables defined here into an OS specific script (`bat` or `.sh`)
by clicking on the `Export` button