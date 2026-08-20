# MDReader

A fast markdown viewer and editor for Windows, built with Qt 6 Quick.

Reading is the primary use: the rendered pane gets a centred reading column,
generous line height and a clickable outline. Editing is a plain source editor
with syntax highlighting, one keystroke away.

## Features

- **Reading view** with a document outline that tracks your position, a centred
  ~800px reading column and auto-hiding scrollbars
- **Source editor** with markdown syntax highlighting, list continuation and
  block indent/outdent
- **Split view** showing editor and preview side by side, with synced scrolling
- **Tabs**, one per open file — closing the last one leaves a blank document
  rather than quitting, and whatever was open comes back on the next launch
- **Crash-safe editing**: unsaved changes are kept on disk as you type, so a
  killed process or a forced reboot doesn't lose them
- **External change detection**: files are watched while open, and a banner
  offers to reload if something else changes one on disk
- **Single instance**: opening another file (or another copy of the app) hands
  the file to the already-running window instead of starting a second one
- **Extended markdown**: tables, task lists, footnotes, `==highlight==`,
  subscript, superscript, `:emoji:`, strikethrough and heading IDs
- **Fenced-code highlighting** for C, C++, C#, Python, JavaScript,
  PowerShell, Windows batch and shell (bash/zsh) snippets
- Independent reading and editor fonts, with fonts bundled for the ones that
  need to render correctly on any machine
- Dark and light themes, both defined in one file
- Find and replace, drag and drop, recent files, and a built-in markdown
  reference (F1) plus an in-app shortcut reference (Shift+F1)

## Shortcuts

| Shortcut | Action |
|---|---|
| `Ctrl+N` / `Ctrl+T` | New document / New tab |
| `Ctrl+O` | Open (multiple at once; drag and drop also works) |
| `Ctrl+S` / `Ctrl+Shift+S` | Save / Save As |
| `F5` | Reload from disk (discards unsaved changes in that tab) |
| `Ctrl+W` | Close tab |
| `Ctrl+Tab` / `Ctrl+Shift+Tab` | Next / previous tab |
| `Ctrl+1`–`Ctrl+8`, `Ctrl+9` | Go to tab 1–8, or the last tab |
| `Ctrl+E` | Toggle reading and source view |
| `Ctrl+\` | Toggle split view |
| `Ctrl+B` | Toggle the outline pane |
| `Ctrl+F` / `F3` / `Shift+F3` | Find / find next / find previous |
| `Ctrl+H` | Replace (editor only) |
| `Esc` | Close find |
| `Ctrl+ +` / `Ctrl+ -` / `Ctrl+0` | Font size up / down / reset |
| `F11` | Fullscreen |
| `F1` | Markdown reference |
| `Shift+F1` | Shortcut reference |

Middle-click and move the mouse up or down to autoscroll either pane;
middle-click a tab to close it.

## Building

Requires Qt 6.10 or later with the MinGW toolchain.

```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

To produce a Windows installer, build a Release configuration and run CPack
from the build directory:

```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
cpack -C Release -G NSIS --config build/CPackConfig.cmake
```

This uses the NSIS scripts in [packaging/](packaging/) to register `.md` file
associations. To change the colours, edit [qml/Theme.qml](qml/Theme.qml) — it's
the single source for both the dark and light palettes.
