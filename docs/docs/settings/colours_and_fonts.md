## Themes
---

In addition to the built-in themes, you can add new themes.

## Installing new themes
---

CodeLite support 3 types of theme formats:

- `VSCode` themes - in the `JSON` format (this is different from the `JSONC` format!)
- [Eclipse themes][1]
- [Alacritty themes][2]

!!! Important
    `VSCode` theme creators usually are using `JSONC` (`JSON` with comments), which is not supported by CodeLite
    You will need to strip the comments in order to import it into CodeLite

To install a new theme:

- Download the theme file that you want to use and save it locally
    - For `VSCode`, the expected format is `JSON`
    - For eclipse theme, the expected format is `XML`
    - For `Alacritty` theme, the expected format is `YAML`

- From the menu bar, go to `Settings` &#8594; `Colours and Fonts...`
- Click the `Import` toolbar button

![Install it](colours_and_fonts_eclipse.png)

- If the import was successful, a notification message will be shown
- You can now select your new theme from the drop down box

## Tab Control Visualisation
---

On some platforms (Windows & macOS) CodeLite offers to ability to change the appearance of the tab control, as well
as their height and orientation

1. From the `Settings` &#8594; `Preferences` dialog, select the `Windows & Tabs` entry
2. Under the `Tabs` section, you can change the following:
    - Tab style
    - Tab height
    - Active tab marker colour
    - Workspace view tabs orientation
    - Output view tabs orientation

[1]: https://eclipse-color-themes.web.app/
[2]: https://github.com/alacritty/alacritty-theme
