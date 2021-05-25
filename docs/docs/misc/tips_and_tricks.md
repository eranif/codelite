# Tips and tricks
---

## Multiple Insertion Points
---

If you do Ctrl+left click with the mouse, you get another caret at that point. Repeating this results in another caret each time. 
Why? Because each acts as an insertion point for typing. So if you want to add "bar" to several words, just ++ctrl+left-button++ at 
the end of each, then type `bar`. Do an ordinary click to exit or click ++esc++. Yes, you could do this in other ways too e.g. 
Find/Replace, but it's there if you want it.


Since CodeLite `7.0` there's another way to do something similar in a multi-line selection: `Edit` &#8594; `Split` Selection into Lines, 
++ctrl+shift+l++ by default, will put a caret at the end of each line in the selection; again this lets you add or delete to 
multiple lines at once. It's less flexible than ++ctrl+left-button++ as it's only the end of lines, and only for continuous blocks 
of lines; but for a 50-line block it's faster to type ++ctrl+shift+l++ once than to ++ctrl+left-button++ 50 times. 

## Block Select
---

You can select a rectangular block of text with the ++alt++ and ++shift++ keys. With them pressed, use either the ++left-button++ or the 
cursor-keys to extend the selection. The caret changes size to match the height of the selection. One way this can be useful 
is to create an empty block-selection at (e.g.) the beginning of several lines. Letters typed will then be duplicated on each line. 

## Build Order for Multiple Custom Projects
---

This came from this [forum post][1] You can't set a build order for a custom build project. However you can work around 
this by creating a standard project without any source files, and make this the main build project. As this is a standard 
project it will have a build order, which you can adjust to order the custom project as you wish.


## wxWidgets & C++11
---

Do you want to use `C++11` features and wxWidgets (e.g. lambda expressions) with CodeLite? See [this forum thread][2]

## Find Resource
---

CodeLite has lots of ways of locating things:

Keyboard shortcut | What does it do
------------------|-------------------
++ctrl+shift+f++  | find in files
++ctrl+shift+o++  | open the outline view for the current editor
++ctrl+shift+r++  | find any class, function, macro, file etc in the current workspace

See [here][3] for a more complete list

## Code Folding
---

How many lines of source-code may your files contain? Some people like lots e.g. the `wxRichTextBuffer` source file is over 
13 thousand lines! Seeing its individual functions is hard. One solution (yes, there are others) is to fold the code. 
`View` &#8594; `Toggle all Folds` will fold all the functions, so that they occupy only a couple of lines. You can then 
unfold just the bits you are interested in.

Toggle all Folds doesn't do exactly quite that. Instead: 
- it ignores a single fold that would collapse the whole file, as would happen with `#ifndef FOOH` ... `#endif` in most headers. 
- it folds lazily: internal folds are ignored.

This is usually what you (or at least I) want. However since CodeLite 5.0 there's an extra option: Toggle Every Fold in 
Selection. This does what it says: only the selected code is touched, and it affects internal folds too. So if you want a 
particular function to have all its contents folded, select it, Toggle Every Fold in Selection, then unfold the function 
itself with Toggle Current Fold (or use the mouse). You can then unfold whichever individual areas you like. 

## Mimetypes (Linux specific)
---

CodeLite is 'associated' with the mimetypes `application/x-codelite-workspace` and `application/x-codelite-project`. 
This means that files ending in `.workspace` or `.project` can automatically be opened into CodeLite. However CodeLite 
probably won't be the first-choice application for doing this. To make it so, either use your desktop environment's Settings 
mechanism; or do it by hand: add to the file `~/.local/share/applications/mimeapps.list` (create the file if necessary) 

```bash
[Added Associations]
application/x-codelite-project=codelite.desktop;
application/x-codelite-workspace=codelite.desktop;
```

## Can't change Theme?
---

From a gitter discussion: 

```
Q: "...it is not changing the theme and every time i open it it asks me for fixing the theme and restarting the program"
A: "There is an issue with the Windows accounts that have names with non ASCII characters (like ı, ü, ş, ğ). CodeLite can't create config file properly. For example there aren't any themes to select only default one."
```


[1]: https://codelite.org/forum/viewtopic.php?f=11&t=1109
[2]: https://codelite.org/forum/viewtopic.php?f=11&t=1798
[3]: /settings/keyboard_shortcuts