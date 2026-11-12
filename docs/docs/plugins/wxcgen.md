# wxCrafter, wxcgen and the AI skill
---

## About wxCrafter
---

wxCrafter is a RAD tool for designing wxWidgets user interfaces: dialogs, frames, panels, wizards, menus, toolbars and
all the common controls. It is available as a CodeLite plugin and as a standalone application.
For a step-by-step tutorial, see the [wxCrafter plugin page](wxcrafter.md).

A wxCrafter design is stored in a **`.wxcp` file** (JSON). From it, wxCrafter generates:

- **Base classes** (C++): all the controls, sizers and event connections. These files are overwritten on every
  generation and carry the banner `Do not modify this file by hand!`
- **Derived classes**: generated once, next to the `.wxcp` file. This is where your code goes, including the event
  handlers
- **Bitmaps code**: every bitmap used in the design is converted into C++ code, so it is available at runtime
- **XRC** (optional)

!!! note
    Never edit the generated base classes. Change the design (the `.wxcp` file) and generate the code again.

## wxcgen
---

`wxcgen` is the command line code generator of wxCrafter. It reads `.wxcp` files and generates the same code as the
`Generate code` button of the designer, without a display and without a UI. Use it from build scripts, CI, or
anywhere you do not want to open the designer.

```
wxcgen [options] <file>.wxcp [<file2>.wxcp ...]
```

Option | Description
-------|------------
`-o <dir>`, `--output <dir>` | Output folder of the base classes
`--verbose` | Print the output folders and the generated files
`--bitmaps-in-base-class-file` | Generate the bitmaps code inside the base class source file. By default, it is written to its own `*_bitmaps.cpp` file
`--install-skill-local` | Install the wxCrafter [AI skill](#ai-skill) into `<current directory>/.claude/skills/wxcrafter`. No input file is needed
`--install-skill-global` | Install the AI skill into `<home directory>/.claude/skills/wxcrafter`, for all projects. No input file is needed
`-v`, `--version` | Print the version and exit
`-h`, `--help` | Print the usage and exit

`wxcgen` exits with `0` on success, and with `1` if any file could not be read or generated.

### Output folder

The base classes are generated next to the `.wxcp` file, unless an output folder is found. The folder is selected using
the first match of:

1. The `-o` option
2. The environment variable `WXCGEN_FOLDER_MAP=<base_dir>=<target_folder>`. Several entries are separated by `:` on
   Unix and by `;` on Windows
3. The same `WXCGEN_FOLDER_MAP=<base_dir>=<target_folder>` line, in a `.wxcrafter-environment` file (one `NAME=VALUE`
   per line), searched in the `.wxcp` folder and then in each of its parent folders

If the `.wxcp` file is under `<base_dir>`, the base classes are generated in `<target_folder>` plus the relative path of
the `.wxcp` folder. Derived class files are always generated next to the `.wxcp` file.

For example, this `.wxcrafter-environment` file at the root of the source tree generates the base classes of
`src/dialogs/my.wxcp` into `build/gen/dialogs`:

```
WXCGEN_FOLDER_MAP=/home/me/project/src=/home/me/project/build/gen
```

### `--verbose` output

For each input file, after a successful generation, `wxcgen` prints:

```
Base class output directory:<full-path>
Base class files:<comma separated list of files>
Subclass output directory:<full-path>
Subclass files:<comma separated list of files>
```

### CMake

CodeLite's CMake files provide the `wxcrafter_target` macro, which runs `wxcgen` as part of the build:

```cmake
wxcrafter_target(MyDialogs my_dialogs.wxcp my_dialogs.cpp my_dialogs.hpp)
```

The base classes are generated under the build folder, in the same relative path as the `.wxcp` file has in the source
folder, and the bitmaps code is generated inside the base class source file. The macro sets
`WXCRAFTER_<Name>_OUTPUTDIR` and `WXCRAFTER_<Name>_OUTPUTS`, which you can use to add the generated files to your
target.

## AI skill
---

wxCrafter can generate an **AI skill**: a folder that teaches an AI coding agent (such as Claude Code) how to create and
edit wxCrafter projects. With it, you can ask the agent to add a dialog, a control or an event handler, and it will edit
the `.wxcp` file and run `wxcgen`, instead of asking you to do it in the designer.

### Installing the skill

For Claude Code, `wxcgen` installs the skill where Claude Code looks for it:

```bash
# Only for one project: run it at the project root
cd <project>
wxcgen --install-skill-local      # <project>/.claude/skills/wxcrafter

# For all your projects
wxcgen --install-skill-global     # ~/.claude/skills/wxcrafter
```

Both switches can be used together. Existing files are overwritten, so run the command again after upgrading
wxCrafter, to refresh the reference.

The installed `wxcrafter` folder contains:

```
wxcrafter/
  SKILL.md                 the entry point: when to use the skill and the edit, generate, verify workflow
  references/
    index.md               how to use the reference, and one line per control
    format.md              the .wxcp JSON layout, how it is loaded, and an example
    common.md              what every control has: styles, sizer flags, properties and events
    <category>.md          the controls of one category: sizers, containers, menus and toolbars, ...
```

The reference is **generated from wxCrafter itself**: for every control it lists the `m_type` ID, window styles,
sizer flags, properties (label, type, default value and choices) and events. It always matches the wxCrafter version
that generated it. It is split into small files so that an agent reads only the controls it needs.

### Using the skill

Ask the agent in plain words, for example:

> Add a "Cancel" button next to the OK button in `my_dialog.wxcp` and connect its click event.

The skill instructs the agent to:

1. Edit the `.wxcp` file with the smallest possible change
2. Run `wxcgen --verbose` to validate it and regenerate the code
3. Review the diff, and make sure the event handlers exist in the derived class

`wxcgen` must be installed and available in the `PATH` of the agent.
