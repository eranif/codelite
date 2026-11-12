#include "metadata_md.h"

#include "JSON.h"
#include "Properties/property_base.h"
#include "events_database.h"
#include "wxc_widget.h"
#include "wxgui_defs.h"

#include <algorithm>
#include <map>
#include <memory>
#include <wx/arrstr.h>

namespace
{
struct FlagInfo {
    wxString name;
    bool on = false; // the default state
};

struct PropInfo {
    wxString label;    // The JSON "m_label" (used verbatim, including the trailing ':' if there is one)
    wxString jsonType; // The JSON "type"
    wxString def;      // The default value, as displayed by the property
    wxString options;  // For "choice" properties: the options, in order
};

struct EventInfo {
    wxString name;
    wxString eventClass;
};

struct ControlInfo {
    int id = 0;   // The ID used to create the control == the "m_type" of the JSON
    int type = 0; // The type reported by the created control (differs from `id` for aliases)
    int kind = 0; // CONTROL_TYPE
    wxString wxClass;
    bool topWindow = false;
    bool sizer = false;
    bool container = false;
    bool windowParent = false;
    bool eventHandler = false;
    std::vector<FlagInfo> styles;
    std::vector<FlagInfo> sizerFlags;
    std::vector<PropInfo> props;
    std::vector<EventInfo> events;
};

struct CommonInfo {
    std::vector<FlagInfo> styles;
    std::vector<FlagInfo> sizerFlags;
    std::vector<PropInfo> props;
};

struct Category {
    const char* file;
    const char* title;
    const char* what;
};

// The categories, in the order they appear in the index
const Category kCategories[] = {
    {"top-level-windows.md",
     "Top-level windows",
     "dialogs, frames, panels, wizards, popup windows: the entries of the `windows` array"},
    {"sizers.md", "Sizers", "sizers, spacers, the standard dialog button sizer and its buttons"},
    {"controls.md", "Controls", "buttons, text, choice, checkbox, sliders, pickers, ... and any other simple control"},
    {"containers.md",
     "Containers and books",
     "panels, scrolled window, notebooks and other book controls with their pages, splitter, collapsible pane, AUI "
     "manager"},
    {"lists-trees-grids.md",
     "Lists, trees, grids and data views",
     "list control, data view controls, tree list, grid, property grid and their columns"},
    {"menus-toolbars.md", "Menus, toolbars and status bar", "menu bar, menus and items, tool bars and their tools, status bar"},
    {"ribbon.md", "Ribbon", "ribbon bar, pages, panels, button bars, tool bars and galleries"},
    {"non-visual.md", "Non-visual items", "timer, image list, bitmap, task bar icon"},
};
constexpr int kCategoryCount = sizeof(kCategories) / sizeof(kCategories[0]);

int CategoryOf(const ControlInfo& ci)
{
    if (ci.topWindow) {
        return 0;
    }
    switch (ci.kind) {
    case TYPE_SIZER:
    case TYPE_SPACER:
    case TYPE_STD_BUTTON_SIZER:
    case TYPE_STD_BUTTON:
        return 1;
    case TYPE_CONTAINER:
    case TYPE_BOOK:
    case TYPE_WIZARD_PAGE:
    case TYPE_NOTEBOOK_PAGE:
    case TYPE_SPLITTERWIN_PAGE:
    case TYPE_SPLITTER_WINDOW:
    case TYPE_COLOLAPSIBLEPANE:
    case TYPE_COLOLAPSIBLEPANE_PANE:
    case TYPE_AUI_MGR:
        return 3;
    case TYPE_LIST_CTRL:
    case TYPE_LIST_CTRL_COL:
    case TYPE_DV_WITH_COLUMNS:
    case TYPE_DV_LISTCTRLCOL:
    case TYPE_GRID:
    case TYPE_GRID_CHILD:
    case TYPE_TREE_LIST_CTRL:
    case TYPE_TREE_LIST_CTRL_COL:
    case TYPE_PG_MGR:
    case TYPE_PG_PROPERTY:
        return 4;
    case TYPE_MENUBAR:
    case TYPE_MENU:
    case TYPE_SUBMENU:
    case TYPE_MENUITEM:
    case TYPE_TOOLBAR:
    case TYPE_TOOLITEM:
    case TYPE_STATUSABR:
    case TYPE_AUITOOLBAR:
        return 5;
    case TYPE_RIBBON_BAR:
    case TYPE_RIBBON_PAGE:
    case TYPE_RIBBON_PANEL:
    case TYPE_RIBBON_BUTTONBAR:
    case TYPE_RIBBON_BUTTON:
    case TYPE_RIBBON_TOOLBAR:
    case TYPE_RIBBON_TOOL:
    case TYPE_RIBBON_GALLERY:
    case TYPE_RIBBON_GALLERY_ITEM:
        return 6;
    case TYPE_TIMER:
    case TYPE_BITMAP:
    case TYPE_IMGLIST:
    case TYPE_TASKBARICON:
        return 7;
    default:
        return 2;
    }
}

wxString Num(int value)
{
    wxString s;
    s << value;
    return s;
}

// Inline code (empty string for an empty value)
wxString Code(const wxString& s)
{
    if (s.IsEmpty()) {
        return wxEmptyString;
    }
    wxString r = s;
    r.Replace("\r", "");
    r.Replace("\n", "\\n");
    r.Replace("`", "'");
    wxString out;
    out << "`" << r << "`";
    return out;
}

// "`a`, `b` (on)"
wxString FlagList(const std::vector<FlagInfo>& flags)
{
    wxString s;
    for (const auto& f : flags) {
        if (!s.IsEmpty()) {
            s << ", ";
        }
        s << "`" << f.name << "`";
        if (f.on) {
            s << "*";
        }
    }
    return s;
}

ControlInfo Collect(int id, wxcWidget& w, std::map<wxString, wxString>& shapes)
{
    ControlInfo ci;
    ci.id = id;
    ci.type = w.GetType();
    ci.kind = w.GetWidgetType();
    ci.wxClass = w.GetWxClassName();

    ci.topWindow = w.IsTopWindow();
    ci.sizer = w.IsSizer();
    ci.container = w.IsContainer();
    ci.windowParent = w.IsValidParent();
    ci.eventHandler = w.IsEventHandler();

    for (const auto& [_, info] : w.GetStyles()) {
        if (info.style_name == "0") {
            continue; // the separator between the common styles and the control specific ones
        }
        ci.styles.push_back({info.style_name, info.is_set});
    }

    for (const auto& [_, info] : w.GetSizerFlags()) {
        ci.sizerFlags.push_back({info.style_name, info.is_set});
    }

    for (const auto& [_, prop] : w.GetProperties()) {
        if (!prop || prop->GetType() == PT_CATEGORY) {
            continue;
        }
        JSONItem json = prop->Serialize();
        if (!json.isObject()) {
            continue;
        }

        PropInfo pi;
        pi.label = prop->GetLabel();
        pi.jsonType = json.namedObject("type").toString();
        pi.def = prop->GetValue();
        if (pi.jsonType == "choice") {
            const wxArrayString options = prop->GetOptions();
            for (size_t i = 0; i < options.GetCount(); ++i) {
                if (i != 0) {
                    pi.options << ", ";
                }
                pi.options << options[i];
            }
        }
        if (shapes.count(pi.jsonType) == 0) {
            shapes.emplace(pi.jsonType, json.format(false));
        }
        ci.props.push_back(std::move(pi));
    }

    for (const auto& [_, cd] : w.GetControlEvents().GetEvents()) {
        ci.events.push_back({cd.GetEventName(), cd.GetEventClass()});
    }
    return ci;
}

bool HasFlag(const std::vector<FlagInfo>& flags, const wxString& name)
{
    return std::any_of(flags.begin(), flags.end(), [&](const FlagInfo& f) { return f.name == name; });
}

// The styles / sizer flags / properties that every control has. They are written once, to keep the files small
CommonInfo ComputeCommon(const std::vector<ControlInfo>& all)
{
    CommonInfo common;
    if (all.empty()) {
        return common;
    }

    // The reference control, used for the default values of the common items
    const ControlInfo* ref = &all.front();
    for (const auto& ci : all) {
        if (ci.type == ID_WXBUTTON) {
            ref = &ci;
            break;
        }
    }

    for (const auto& s : ref->styles) {
        if (std::all_of(all.begin(), all.end(), [&](const ControlInfo& ci) { return HasFlag(ci.styles, s.name); })) {
            common.styles.push_back(s);
        }
    }

    for (const auto& s : ref->sizerFlags) {
        if (std::all_of(
                all.begin(), all.end(), [&](const ControlInfo& ci) { return HasFlag(ci.sizerFlags, s.name); })) {
            common.sizerFlags.push_back(s);
        }
    }

    for (const auto& p : ref->props) {
        const bool everywhere = std::all_of(all.begin(), all.end(), [&](const ControlInfo& ci) {
            return std::any_of(ci.props.begin(), ci.props.end(), [&](const PropInfo& other) {
                return other.label == p.label && other.jsonType == p.jsonType;
            });
        });
        if (everywhere) {
            common.props.push_back(p);
        }
    }
    return common;
}

// Return the entries of `flags` that are not in `common`, or that have a different default state
std::vector<FlagInfo> DiffFlags(const std::vector<FlagInfo>& flags, const std::vector<FlagInfo>& common)
{
    std::vector<FlagInfo> out;
    for (const auto& f : flags) {
        const auto it =
            std::find_if(common.begin(), common.end(), [&](const FlagInfo& c) { return c.name == f.name; });
        if (it == common.end() || it->on != f.on) {
            out.push_back(f);
        }
    }
    return out;
}

// Return the properties of `props` that are not in `common`, or that have a different default value
std::vector<PropInfo> DiffProps(const std::vector<PropInfo>& props, const std::vector<PropInfo>& common)
{
    std::vector<PropInfo> out;
    for (const auto& p : props) {
        const auto it = std::find_if(common.begin(), common.end(), [&](const PropInfo& c) {
            return c.label == p.label && c.jsonType == p.jsonType;
        });
        if (it == common.end() || it->def != p.def) {
            out.push_back(p);
        }
    }
    return out;
}

// - `Label:` = `default`           (a "string" property: the type is omitted)
// - `Label:` bool = `1`
// - `Label:` choice = `x` (options: a, b)
void AppendProps(wxString& md, const std::vector<PropInfo>& props)
{
    for (const auto& p : props) {
        md << "- " << Code(p.label);
        if (p.jsonType != "string") {
            md << " " << p.jsonType;
        }
        if (!p.def.IsEmpty()) {
            md << " = " << Code(p.def);
        }
        if (!p.options.IsEmpty()) {
            md << " (options: " << p.options << ")";
        }
        md << "\n";
    }
}

// - `wxEVT_X` wxCommandEvent
void AppendEvents(wxString& md, const std::vector<EventInfo>& events)
{
    for (const auto& e : events) {
        md << "- " << Code(e.name) << " " << e.eventClass << "\n";
    }
}

// ---------------------------------------------------------------------------------------------------------------
// format.md: the file format and its load semantics (as implemented by wxcWidget::Serialize/UnSerialize and
// wxcProjectMetadata)
// ---------------------------------------------------------------------------------------------------------------

wxString BuildFormat(const std::map<wxString, wxString>& shapes)
{
    wxString md;
    md << R"md(# The `.wxcp` file format

A `.wxcp` file is a JSON document with two members:

```json
{
  "metadata": { ... project settings ... },
  "windows":  [ <widget>, <widget>, ... ]
}
```

Every entry of `windows` is a top-level window (dialog, frame, panel, wizard, popup window, ...). Each widget owns its
children in `m_children`, so the file is a tree.

## Project `metadata`

| Key | Type | Default | Meaning |
|---|---|---|---|
| `m_generatedFilesDir` | string | `""` | Output directory of the generated base classes (relative to the `.wxcp` file) |
| `m_objCounter` | int | `0` | Counter used to generate unique control names (`m_button1`, `m_button2`, ...) |
| `m_includeFiles` | string array | `[]` | Extra header files, emitted as `#include "<file>"` lines at the top of the generated header |
| `m_bitmapFunction` | string | `""` | Name of the generated bitmap-initialisation function (generated when empty) |
| `m_bitmapsFile` | string | `""` | Name of the generated bitmaps source file (generated when empty) |
| `m_GenerateCodeTypes` | int | `1` | Bit mask: `1` = generate C++, `2` = generate XRC, `3` = both |
| `m_outputFileName` | string | `""` | Base name of the generated files (defaults to the `.wxcp` file name) |
| `m_firstWindowId` | int | `10000` | First value of the generated window ID `enum` |
| `m_useEnum` | bool | `true` | Generate the window IDs as an `enum` |
| `m_useUnderscoreMacro` | bool | `true` | Wrap the generated strings with `_()` (when `false`, `wxT()` is used) |
| `m_addHandlers` | bool | `true` | Register `wxBitmapXmlHandler` in the generated bitmaps code |

## A widget

| Key | Type | Meaning |
|---|---|---|
| `m_type` | int | **The control ID** (see `index.md`). A widget with an unknown `m_type` is silently skipped |
| `proportion` | int | Sizer proportion (`0` when missing) |
| `border` | int | Sizer border in pixels (`5` when missing) |
| `gbSpan` | string | `"rows,cols"`, only meaningful inside a `wxGridBagSizer`. Always write it (`"1,1"`): the loader sets an empty value when it is missing |
| `gbPosition` | string | `"row,col"`, only meaningful inside a `wxGridBagSizer`. Always write it (`"0,0"`) |
| `m_styles` | string array | The window styles that are **on** |
| `m_sizerFlags` | string array | The sizer flags that are **on** |
| `m_properties` | array | The properties (see below). `null` entries are category placeholders and can be omitted |
| `m_events` | array | The events connected to a handler (see below) |
| `m_children` | array | The child widgets |
| `wxAuiPaneInfo` | object | Only for widgets that are an AUI pane (children of a `wxAuiManager`): the pane layout |

**Load semantics** (what wxCrafter and `wxcgen` do when they read the file):

* `m_styles` and `m_sizerFlags` are **reset** first: anything that is not listed is **off**. The `*` marks ("on by
  default") in the other files describe a newly created control in the designer, they are *not* applied when loading a
  file. For example, to get a button with all four borders, `m_sizerFlags` must contain `wxALL`.
* A property that is not in `m_properties` keeps the default value of the control. Properties are matched by their
  `m_label` only; an unknown label is silently ignored.
* `m_events` replaces the events of the control.
* The `Name:` property is the C++ member name. It must be unique inside its top-level window. The name of a top-level
  window is the name of the generated base class.

## Property objects

Every property is an object with a `type`, an `m_label` and one or more value members. Copy the `m_label` verbatim
from the control's file, including a trailing `:` when there is one. The shapes, by `type`:

)md";

    for (const auto& [type, example] : shapes) {
        md << "* `" << type << "`: `" << example << "`\n";
    }
    md << "\n"
       << "For a `choice` property, `m_selection` is the 0-based index into `m_options`, and `m_options` must be "
          "written in full (the loader replaces the options with the ones in the file). A `bool` property is stored as "
          "a JSON `true` / `false` (shown as `1` / `0` in the other files). Most other values are strings.\n\n";

    md << R"md(## Event objects

```json
{ "m_eventName": "wxEVT_COMMAND_BUTTON_CLICKED", "m_eventClass": "wxCommandEvent",
  "m_functionNameAndSignature": "OnOk(wxCommandEvent& event)" }
```

`m_eventName` and `m_eventClass` come from the control's events list (or from the common events in `common.md`).
`m_functionNameAndSignature` is the handler declaration, `Name(EventClass& event)`.

## Minimal example

)md";

    wxString example;
    example << R"md(```json
{
  "metadata": { "m_generatedFilesDir": "", "m_objCounter": 1, "m_GenerateCodeTypes": 1 },
  "windows": [
    {
      "m_type": @DIALOG@, "proportion": 0, "border": 5, "gbSpan": "1,1", "gbPosition": "0,0",
      "m_styles": ["wxDEFAULT_DIALOG_STYLE"], "m_sizerFlags": [],
      "m_properties": [
        { "type": "string", "m_label": "Name:",  "m_value": "MyDialogBase" },
        { "type": "string", "m_label": "Title:", "m_value": "My dialog" }
      ],
      "m_events": [],
      "m_children": [
        {
          "m_type": @BOXSIZER@, "proportion": 0, "border": 5, "gbSpan": "1,1", "gbPosition": "0,0",
          "m_styles": [], "m_sizerFlags": ["wxEXPAND"],
          "m_properties": [ { "type": "string", "m_label": "Name:", "m_value": "boxSizer1" } ],
          "m_events": [],
          "m_children": [
            {
              "m_type": @BUTTON@, "proportion": 0, "border": 5, "gbSpan": "1,1", "gbPosition": "0,0",
              "m_styles": [], "m_sizerFlags": ["wxALL"],
              "m_properties": [
                { "type": "string", "m_label": "Name:",  "m_value": "m_buttonOk" },
                { "type": "string", "m_label": "Label:", "m_value": "OK" }
              ],
              "m_events": [
                { "m_eventName": "wxEVT_COMMAND_BUTTON_CLICKED", "m_eventClass": "wxCommandEvent",
                  "m_functionNameAndSignature": "OnOk(wxCommandEvent& event)" }
              ],
              "m_children": []
            }
          ]
        }
      ]
    }
  ]
}
```

The IDs (`@DIALOG@` dialog, `@BOXSIZER@` box sizer, `@BUTTON@` button) are the ones of this wxCrafter build.
Run `wxcgen <file>.wxcp` to validate a file and generate the code from it.
)md";
    example.Replace("@DIALOG@", Num(ID_WXDIALOG));
    example.Replace("@BOXSIZER@", Num(ID_WXBOXSIZER));
    example.Replace("@BUTTON@", Num(ID_WXBUTTON));
    md << example;
    return md;
}

// SKILL.md: the entry point of the skill. Generic: it knows nothing about the project that uses wxCrafter
wxString BuildSkill()
{
    wxString md;
    md << R"md(---
name: wxcrafter
description: Create and edit wxCrafter GUI projects (*.wxcp files) for wxWidgets applications, and generate the C++ / XRC code from them with wxcgen. Use when asked to add, change or remove dialogs, frames, panels, controls, sizers, menus, toolbars or event handlers that are defined in a .wxcp file.
---

# wxCrafter

wxCrafter is a RAD tool that designs wxWidgets user interfaces. A design is stored in a `.wxcp` file (JSON).
`wxcgen` is its command line code generator: it reads a `.wxcp` file and writes the C++ base classes (and optionally
XRC). The generated files are overwritten on every run and carry the banner "Do not modify this file by hand!".

**Change the `.wxcp` file, then regenerate. Never edit the generated files.**

## Workflow

1. Find the `.wxcp` file and the files generated from it: `wxcgen --verbose <file>.wxcp` prints the output
   directories and the generated files (base class files and subclass files).
2. Read `references/index.md`, then `references/format.md` (the JSON layout and the load semantics: read it before
   writing any JSON). Then read `references/common.md` and only the category files of the controls you use.
   Everything is in the reference: the `m_type` ID, styles, sizer flags, property labels, defaults and events.
3. Edit the `.wxcp` file with the smallest possible change. Keep its formatting and indentation. To add a control,
   copy a sibling widget of the same type and adjust it, rather than writing one from scratch. Names (`Name:`) must be
   unique inside the top-level window.
4. Run `wxcgen --verbose <file>.wxcp`. It must exit with 0. The reference tells you what each value means; if
   `wxcgen` fails or silently drops a widget, re-check `m_type` and the `m_label` spelling.
5. Review the diff: the `.wxcp` file must only contain the intended change, and the generated files must change
   accordingly.
6. Event handlers: the generated base class declares them, and the subclass (the "Inherited Class", written next to
   the `.wxcp` file) is where they are implemented. After adding an event, check that the handler exists in the
   subclass and add it there if it is missing, using the signature from `m_functionNameAndSignature`.

## wxcgen

`wxcgen [options] <file>.wxcp...`

| Option | Meaning |
|---|---|
| `-o <dir>` | Output directory of the base classes |
| `--verbose` | Print the output directories and the generated files |
| `--bitmaps-in-base-class-file` | Put the bitmaps code in the base class source file (default: its own `_bitmaps.cpp`) |
| `-v` | Print the version |

Without `-o`, the output directory comes from the environment variable `WXCGEN_FOLDER_MAP=<base_dir>=<target_folder>`
(several entries are separated by `:` on Unix and `;` on Windows), then from the same line in a
`.wxcrafter-environment` file in the `.wxcp` folder or one of its parents. Otherwise the base classes are written next
to the `.wxcp` file. Subclass files are always written next to the `.wxcp` file.

`wxcgen` must be installed (it is part of wxCrafter). If it is not available, tell the user instead of editing the
generated files.

The reference was installed by `wxcgen --install-skill-local` or `wxcgen --install-skill-global`: run the same
command again after upgrading wxCrafter.
)md";
    return md;
}

wxString BuildCommon(const CommonInfo& common, const std::vector<EventInfo>& commonEvents)
{
    wxString md;
    md << "# Common items\n\n"
       << "These apply to **every** control. The category files only list what is specific to a control, or what has "
          "a different default.\n\n";
    md << "## Window styles (`m_styles`)\n\n" << FlagList(common.styles) << "\n\n";
    md << "## Sizer flags (`m_sizerFlags`)\n\n"
       << "Used when the control is placed inside a sizer. `*` = on by default in the designer (not applied when "
          "loading a file).\n\n"
       << FlagList(common.sizerFlags) << "\n\n";
    md << "## Properties\n\n";
    AppendProps(md, common.props);
    md << "\n## Events\n\n"
       << "Available on every control that handles events (see the *no events* mark in the category files).\n\n";
    AppendEvents(md, commonEvents);
    return md;
}

// The heading of a control, and its body (what is specific to the control). Bodies are compared, so that a control
// that is identical to an earlier one can be written as "same as <class>"
struct ControlText {
    wxString heading;
    wxString body;
};

ControlText BuildControl(const ControlInfo& ci, const CommonInfo& common)
{
    ControlText text;
    text.heading << "## " << ci.id << " " << ci.wxClass << "\n";
    if (ci.type != ci.id) {
        text.heading << "(alias: creates type " << ci.type << ")\n";
    }

    wxString& md = text.body;

    // Only what an author needs: where the control can go and whether it takes events
    wxString traits;
    if (ci.topWindow) {
        traits << "top-level, ";
    }
    if (ci.sizer) {
        traits << "sizer, ";
    }
    if (ci.container) {
        traits << "container (needs a sizer), ";
    }
    if (!ci.windowParent) {
        traits << "cannot be a window parent, ";
    }
    if (!ci.eventHandler) {
        traits << "no events, ";
    }
    if (!traits.IsEmpty()) {
        traits.RemoveLast(2);
        md << traits << "\n";
    }

    const auto styles = DiffFlags(ci.styles, common.styles);
    if (!styles.empty()) {
        md << "Styles: " << FlagList(styles) << "\n";
    }

    const auto sizerFlags = DiffFlags(ci.sizerFlags, common.sizerFlags);
    if (!sizerFlags.empty()) {
        md << "Sizer flags: " << FlagList(sizerFlags) << "\n";
    }

    const auto props = DiffProps(ci.props, common.props);
    if (!props.empty()) {
        md << "Properties:\n";
        AppendProps(md, props);
    }

    if (!ci.events.empty()) {
        md << "Events:\n";
        AppendEvents(md, ci.events);
    }
    return text;
}

} // namespace

std::vector<MetadataFile> BuildMetadataMarkdown(const wxString& version)
{
    // Instantiate all the controls and gather their metadata
    std::vector<ControlInfo> controls;
    std::map<wxString, wxString> shapes;
    for (int id = ID_FIRST_CONTROL; id < ID_LAST_CONTROL; ++id) {
        std::unique_ptr<wxcWidget> w{wxcWidget::Create(id)};
        if (!w) {
            continue;
        }
        controls.push_back(Collect(id, *w, shapes));
    }
    const CommonInfo common = ComputeCommon(controls);

    std::vector<EventInfo> commonEvents;
    for (const auto& [_, cd] : wxcWidget::GetCommonEvents().GetEvents()) {
        commonEvents.push_back({cd.GetEventName(), cd.GetEventClass()});
    }

    std::vector<MetadataFile> files;

    // index.md
    {
        wxString md;
        md << "# wxCrafter reference\n\n"
           << "Generated by `wxcgen` (wxCrafter " << version << "). Do not edit: regenerate it.\n\n"
           << "How to write or edit a wxCrafter project file (`*.wxcp`). Read only what you need:\n\n"
           << "1. `format.md`: the JSON layout, the load semantics and an example. Read it first.\n"
           << "2. `common.md`: the styles, sizer flags, properties and events that every control has.\n"
           << "3. The category file of each control you use (the table below says which one). It lists, for the "
              "control, its styles, sizer flags, properties (`m_label`, JSON type, default, options) and events, "
              "without what is in `common.md`.\n\n"
           << "`m_type` is the control ID written in the JSON. A control is looked up by its wx class or `m_type`.\n\n"
           << "## Categories\n\n";
        for (int c = 0; c < kCategoryCount; ++c) {
            md << "* `" << kCategories[c].file << "`: " << kCategories[c].what << "\n";
        }
        md << "\n## Controls\n\n"
           << "| `m_type` | wx class | File |\n"
           << "|---|---|---|\n";
        for (const auto& ci : controls) {
            md << "| " << ci.id << " | " << ci.wxClass << " | " << kCategories[CategoryOf(ci)].file << " |\n";
        }
        files.push_back({"references/index.md", md});
    }

    files.push_back({"SKILL.md", BuildSkill()});
    files.push_back({"references/format.md", BuildFormat(shapes)});
    files.push_back({"references/common.md", BuildCommon(common, commonEvents)});

    // One file per category
    for (int c = 0; c < kCategoryCount; ++c) {
        wxString md;
        md << "# " << kCategories[c].title << "\n\n"
           << "`*` = on by default in the designer (not applied when loading a file, see `format.md`). A property "
              "without a type is a `string`. What every control has is in `common.md`.\n";
        bool any = false;
        std::vector<std::pair<wxString, wxString>> seen; // body -> wx class of the first control that has it
        for (const auto& ci : controls) {
            if (CategoryOf(ci) != c) {
                continue;
            }
            any = true;
            const ControlText text = BuildControl(ci, common);
            md << "\n" << text.heading;
            const auto it = std::find_if(
                seen.begin(), seen.end(), [&](const std::pair<wxString, wxString>& s) { return s.first == text.body; });
            if (!text.body.IsEmpty() && it != seen.end()) {
                md << "Same as " << it->second << ".\n";
            } else {
                md << text.body;
                seen.emplace_back(text.body, ci.wxClass);
            }
        }
        if (any) {
            files.push_back({wxString("references/") + kCategories[c].file, md});
        }
    }
    return files;
}
