---
title: MDReader stress test
author: test fixture
date: 2026-08-05
---

# MDReader stress test

This file exists to be opened, scrolled, edited and re-rendered. It exercises
every construct the app claims to support, plus a few it deliberately does not,
so the failures are visible rather than silent.

The YAML block above is front matter. The outline pane should **not** list it as
a heading.

**What to watch while you read:**

- The outline pane should list every heading below, indented by level.
- Scrolling should stay smooth; this file is long on purpose.
- In split view, typing anywhere should not make the preview jump or flash.
- Every section marked ⚠️ is a known limitation, not a bug to report.

---

## 1. Headings

The six ATX levels follow. Each should be progressively smaller, accent
coloured, and each should appear in the outline.

# Level 1 heading
## Level 2 heading
### Level 3 heading
#### Level 4 heading
##### Level 5 heading
###### Level 6 heading

Setext headings use underlines instead of hashes and should behave identically:

Setext level one
================

Setext level two
----------------

### Heading with a custom ID {#custom-anchor}

The `{#custom-anchor}` should be hidden in the reading view, not printed as
literal text.

---

## 2. Emphasis and inline marks

Ordinary paragraph text for comparison, so the coloured runs below have
something to be measured against.

- **Bold with asterisks** and __bold with underscores__
- *Italic with asterisks* and _italic with underscores_
- ***Bold and italic together*** and ___the underscore form___
- ~~Strikethrough~~ using double tildes
- `inline code` in the middle of a sentence
- ==Highlighted text== with a marker-pen background
- Escaped characters: \*not italic\*, \_not italic\_, \`not code\`
- Nested: **bold containing *italic* inside it**
- Nested: *italic containing `code` inside it*

A line ending in two spaces forces a hard break.  
This sentence should start on a new line but stay in the same paragraph.

A backslash at the end works too.\
So should this one.

Subscript and superscript are translated to Unicode before parsing, because the
markdown parser claims single tildes for strikethrough:

- Water is H~2~O and carbon dioxide is CO~2~
- Euler: e^i^ and squares like x^2^ + y^2^
- ⚠️ Only characters that exist as Unicode sub/superscripts convert. `x^abc^`
  has no Unicode form and should stay as written.

Emoji shortcodes: :smile: :rocket: :warning: :book: :bug:

---

## 3. Lists

### Unordered

- First item
- Second item
    - Nested one level
    - Another nested item
        - Nested two levels
        - And a sibling
- Third item with a much longer body so that it wraps across more than one line
  and you can check that the continuation lines align with the text rather than
  with the bullet
- Item with `code`, **bold**, and a [link](https://example.com) inside it

### Ordered

1. First
2. Second
   1. Nested ordered
   2. Second nested
      1. Third level
3. Third
7. A wrong number, which CommonMark renumbers on render

### Mixed

1. Ordered parent
   - Unordered child
   - Another child
     1. Ordered grandchild
2. Back to the parent level

### Task lists

- [x] A completed task
- [ ] An outstanding task
- [x] Another completed one
- [ ] A task with **bold**, `code` and a [link](https://example.com)
- [ ] A very long task item that wraps onto a second line, so you can check the
      checkbox stays aligned with the first line rather than drifting
- Plain item with no checkbox, mixed into the same list
- [ ] Back to a checkbox after a plain item

### Loose vs tight

A tight list has no blank lines between items:

- tight one
- tight two

A loose list has blank lines, and each item becomes its own paragraph:

- loose one

- loose two

---

## 4. Links and images

### Links

- Inline: [Markdown Guide](https://www.markdownguide.org)
- With a title: [CommonMark](https://commonmark.org "The specification")
- Reference style: [Qt documentation][qt-docs]
- Bare autolink: <https://www.qt.io>
- Relative to this file: [the theming notes](../docs/THEMING.md)
- Anchor within the document: [jump to tables](#6-tables)

[qt-docs]: https://doc.qt.io "Qt Documentation"

### Images

A relative path, resolved against this file's own location. The app's icon lives
one directory up:

![The MDReader icon](../icons/mdreader.ico)

The same image with a title attribute:

![Icon again](../icons/mdreader.ico "MDReader")

A deliberately missing file. This should render as nothing at all — no
placeholder, no error, and above all no hang:


⚠️ `file://` with two slashes used to freeze the app for several seconds while
Windows tried to resolve it as a network share. It is now rewritten to three
slashes before parsing, so the line below should render harmlessly:


**Spacing check:** the images above should sit in normal paragraph rhythm. If
there is a large empty gap underneath one, the line-height fix has regressed.

---

## 5. Code

Inline `code`, `a_longer_identifier`, and `--flags --with --dashes`.

A fenced block with a language tag:

```cpp
// Colour detection, from markdowndocument.cpp
const bool isMono = isCode || existing.hasProperty(QTextFormat::FontFixedPitch);
const bool isBold = existing.fontWeight() > QFont::Normal;

if (heading > 0 && m_headingColor.isValid()) {
    overlay.setForeground(m_headingColor);
} else if (existing.isAnchor() && m_linkColor.isValid()) {
    overlay.setForeground(m_linkColor);
    overlay.setFontUnderline(false);
}
```

A fenced block with no language:

```
plain text in a fence
    with indentation preserved
        and more of it
```

A block with lines long enough to test horizontal behaviour:

```python
def a_function_with_a_very_long_signature(first_argument, second_argument, third_argument, fourth_argument, fifth_argument=None):
    return {"key": "a value long enough that the line will not fit in the reading column at any sensible window width"}
```

An indented code block, four spaces, no fence:

    indented code block
    second line
    third line

A fence containing what looks like markdown, which must not be rendered:

```markdown
# This heading is code, not a heading
- This bullet is code
**This is not bold**
| not | a | table |
```

---

## 6. Tables

A plain table:

| Element | Syntax | Supported |
|---|---|---|
| Heading | `# text` | Yes |
| Bold | `**text**` | Yes |
| Table | `\| a \| b \|` | Yes |
| Footnote | `[^1]` | Rendered by the app |

Alignment markers — left, centre, right:

| Left | Centre | Right |
|:---|:---:|---:|
| a | b | c |
| longer cell | longer cell | longer cell |
| 1 | 22 | 333 |

Formatting inside cells:

| Kind | Example | Note |
|---|---|---|
| Bold | **strong** | should be bold |
| Italic | *emphasis* | should be italic |
| Code | `monospace` | tinted background |
| Link | [Qt](https://www.qt.io) | should be clickable |
| Strike | ~~gone~~ | struck through |
| Highlight | ==marked== | marker background |

A wide table, to test what happens when it will not fit:

| ID | Name | Category | Status | Owner | Created | Updated | Priority | Notes |
|---|---|---|---|---|---|---|---|---|
| 1 | First row | Alpha | Open | Ada | 2026-01-01 | 2026-02-01 | High | Some text |
| 2 | Second row | Beta | Closed | Grace | 2026-01-15 | 2026-03-12 | Low | More text |
| 3 | Third row | Gamma | Open | Alan | 2026-02-20 | 2026-04-02 | Medium | Longer note here |

A table with an empty cell and uneven columns:

| A | B | C |
|---|---|---|
| filled |  | filled |
| filled | filled |  |

**Table checks:** header row shaded, alternating row stripes, one-pixel borders
that meet at the corners, and cell padding that does not collapse.

---

## 7. Blockquotes

> A single-level blockquote. It should be indented, tinted, and set apart from
> the surrounding prose.

> A blockquote spanning
> several source lines that
> join into one paragraph.

> The outer level.
>
> > A nested quote inside it.
> >
> > > And a third level, which is where most renderers give up.

> A quote containing other things:
>
> - a list item
> - another item
>
> ```js
> // and a code block
> const x = 1;
> ```
>
> | and | a table |
> |---|---|
> | a | b |

---

## 8. Horizontal rules

Three ways to write the same rule:

---

***

___

---

## 9. Definition lists

Definition lists are not CommonMark; the app renders them itself.

Markdown
: A plain-text formatting syntax.

CommonMark
: A strongly specified dialect of Markdown.

md4c
: The C parser Qt uses to import markdown documents.

---

## 10. Footnotes

Footnotes are not CommonMark either. Here is a reference[^1], and a second
one[^note] with a longer label.

[^1]: The first footnote's text.
[^note]: A footnote with a named label rather than a number.

---

## 11. Raw HTML

⚠️ Support for inline HTML is whatever Qt's importer does with it, which is not
the same as a browser.

<strong>Bold via HTML</strong> and <em>italic via HTML</em> inline.

<div align="center">
A block-level HTML element.
</div>

An HTML comment follows this line and should not be visible.

<!-- invisible comment -->

---

## 12. Unicode and long lines

Accents and scripts: café, naïve, Straße, Ångström, Москва, 日本語, العربية, עברית.

Symbols: → ← ↑ ↓ ⇒ ⇔ ∀ ∃ ∈ ∉ ∑ ∏ √ ∞ ≈ ≠ ≤ ≥ ± × ÷ ° ‰ § ¶ † ‡ • … — –

Box drawing, which needs a monospace font to line up:

```
┌─────────────┬─────────────┐
│ Left cell   │ Right cell  │
├─────────────┼─────────────┤
│ Another     │ And another │
└─────────────┴─────────────┘
```

A paragraph with no line breaks at all, to check wrapping in the reading column: Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.

---

## 13. Edge cases

An empty heading:

###

A heading immediately followed by another:

### First
### Second

A list immediately after a paragraph with no blank line between them:
- does this become a list
- or stay part of the paragraph

Emphasis at a word boundary: snake_case_identifier should **not** be italic in
the middle.

A line that is only whitespace follows this one.
   
Trailing pipes on a table row:

| a | b |
|---|---|
| c | d |

Characters that need escaping in tables: `\|` inside a cell.

| Column | Value |
|---|---|
| Pipe | `a \| b` |

---

## 14. A long tail for scrolling

The sections below exist to make the document long enough that scrolling,
the outline, find, and the split-view sync all have something to work with.

### Section 14.1

Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer nec odio.
Praesent libero. Sed cursus ante dapibus diam. Sed nisi. Nulla quis sem at nibh
elementum imperdiet. Duis sagittis ipsum. Praesent mauris.

### Section 14.2

Fusce nec tellus sed augue semper porta. Mauris massa. Vestibulum lacinia arcu
eget nulla. Class aptent taciti sociosqu ad litora torquent per conubia nostra,
per inceptos himenaeos.

- A point
- Another point
- A third point

### Section 14.3

Curabitur sodales ligula in libero. Sed dignissim lacinia nunc. Curabitur
tortor. Pellentesque nibh. Aenean quam. In scelerisque sem at dolor.

```bash
# a shell block for variety
find . -name '*.md' -print0 | xargs -0 wc -l | sort -n
```

### Section 14.4

Maecenas mattis. Sed convallis tristique sem. Proin ut ligula vel nunc egestas
porttitor. Morbi lectus risus, iaculis vel, suscipit quis, luctus non, massa.

| Metric | Value |
|---|---|
| Lines | many |
| Words | more |

### Section 14.5

Fusce ac turpis quis ligula lacinia aliquet. Mauris ipsum. Nulla metus metus,
ullamcorper vel, tincidunt sed, euismod in, nibh.

> A closing quote, so the document does not end on a table.

### Section 14.6

Quisque volutpat condimentum velit. Class aptent taciti sociosqu ad litora
torquent per conubia nostra, per inceptos himenaeos. Nam nec ante.

- [ ] One last task
- [x] And one that is done

---

## 15. Find and replace fodder

The word **needle** appears several times in this section so that find has
something to count. needle. Needle. NEEDLE. needless — which should **not**
match a whole-word search, but this app matches substrings, so it will.

Here is a needle in a table:

| Item | Contains |
|---|---|
| Row one | needle |
| Row two | nothing |

And a needle inside code, which the reading pane still finds: `needle`.

---

## 16. Code blocks as panels

Every fenced block below should be one rounded panel with even padding, a
caption at the top left naming its language, and a copy button at the top
right. The button should be faint until the block is hovered. Copying should
put the code on the clipboard **without** the fence lines and without a
trailing blank line.

```python
def greet(name):
    return f"hello, {name}"
```

A fence with no language: the panel and the button stay, the caption goes.

```
no language on this fence
```

An unknown language still captions, because the name is taken from the fence
rather than from a list of languages the app knows:

```klingon
nuqneH
```

Indented four spaces instead of fenced. Same panel, no caption:

    indented, not fenced
    second line

Tilde fences are equivalent to backtick fences:

~~~ruby
puts "tildes work too"
~~~

A single line, to check the panel does not collapse:

```sh
ls
```

A block whose lines are longer than the reading column. These should **wrap**
inside the panel rather than being cut off at the right edge — losing the end
of a command is worse than a soft wrap:

```powershell
Copy-Item -LiteralPath "include\Config\ApiKeys.h.example" -Destination "include\Config\ApiKeys.h" -Force -ErrorAction Stop
```

And one long unbroken run with nowhere natural to wrap:

```
supercalifragilisticexpialidociousXsupercalifragilisticexpialidociousXsupercalifragilisticexpialidociousXsupercalifragilisticexpialidocious
```

Two blocks back to back, to check the panels do not merge into one another:

```json
{ "first": true }
```

```json
{ "second": true }
```

A block immediately under a heading, where the heading's bottom margin meets
the panel's top padding:

### Heading directly above a block

```c
int main(void) { return 0; }
```

A fence holding fences, written with four backticks:

````markdown
```python
this inner fence is content, not markup
```
````

A block with blank lines inside it, and trailing blank lines before the closing
fence. The panel should include the inner blanks; copying should not pick up
the trailing ones:

```text
first line

third line after a blank


```

### 16.1 Supported syntax palettes

Each block below should use the same syntax colours in the source editor and
the rendered reading pane. Check keywords, types, strings, numbers, comments,
function or command names, variables, and command options. Unknown and
unlabelled fences above should remain plain.

#### C

```c
#include <stdio.h>

/* A block comment can continue
   across another line. */
static int add(int left, int right)
{
    const char *label = "total";
    printf("%s: %d\n", label, left + right);
    return left + right + 42;
}
```

#### C++

```cpp
#include <iostream>
#include <string>

class Greeter final {
public:
    explicit Greeter(std::string name) : m_name(std::move(name)) {}

    void greet() const
    {
        // A normal line comment.
        std::cout << "Hello, " << m_name << '!' << std::endl;
    }

private:
    std::string m_name;
};

int main()
{
    Greeter greeter("MDReader");
    greeter.greet();
    return 0;
}
```

#### C sharp

```csharp
using System;

public sealed class Report
{
    private const int MaxItems = 42;

    public string Format(string title, int count)
    {
        // Interpolated strings should still read as strings.
        return $"{title}: {Math.Min(count, MaxItems)}";
    }
}
```

#### Python

```python
class Greeter:
    """A triple-quoted string can continue
    across another line."""

    def __init__(self, name: str):
        self.name = name

    async def greet(self, count: int = 3):
        # Keywords, built-in types, calls, strings and numbers.
        for index in range(count):
            print(f"{index}: hello, {self.name}")
```

#### JavaScript

```javascript
class Greeter {
    constructor(name) {
        this.name = name;
    }

    async greet(count = 3) {
        /* A block comment can continue
           across another line. */
        const message = `hello,
${this.name}`;
        return Promise.resolve(Array(count).fill(message));
    }
}
```

#### PowerShell

```powershell
param([string]$SourcePath, [int]$Limit = 10)

<# A PowerShell block comment can continue
   across another line. #>
$message = @"
Copying from $SourcePath
"@

Get-ChildItem -LiteralPath $SourcePath -File |
    Select-Object -First $Limit |
    ForEach-Object { Write-Output "$message: $($_.Name)" }
```

#### Linux terminal

```linux
#!/usr/bin/env bash

name="MDReader"
count=3

# Commands, options, variables, strings and numbers.
for index in $(seq 1 "$count"); do
    printf '%s: hello, %s\n' "$index" "$name"
done

find ./samples -type f -name '*.md' | head -n 10
```

#### Windows terminal

```terminal
@echo off
setlocal
set "SOURCE_DIR=samples"
set "LIMIT=10"

rem Commands, variables, strings, numbers and control-flow keywords.
if not exist "%SOURCE_DIR%" exit /b 1
for %%F in ("%SOURCE_DIR%\*.md") do call :show "%%~fF"
exit /b 0

:show
echo File: %~1
exit /b 0
```

⚠️ A fenced block inside a list item, which Qt's importer does not keep inside
the item. The panel may sit at the left margin instead of indented with its
bullet:

- A list item with code under it:

  ```js
  const inList = true;
  ```

- The item after it.

⚠️ A fenced block inside a blockquote, same caveat:

> Quoted text, then code:
>
> ```yaml
> quoted: true
> ```

---

## 17. Headings with inline code

A code span inside a heading should stay at the heading's size and weight and
gain its chip. It should not shrink to body size, and it should not turn the
colour inline code uses in a paragraph — a heading stays one colour.

# `h1_all_code`
## `h2_all_code`
### `h3_all_code`
#### `h4_all_code`
##### `h5_all_code`
###### `h6_all_code`

Mixed headings, where plain words and a code span sit side by side and have to
line up on the same baseline at the same size:

### 3.1 `NodeTypeDefinition`

## Configuring `mdReader` for `--verbose` output

Compare against a heading with no code at all, which is the reference for the
size the ones above should match:

### 3.1 NodeTypeDefinition

Other things nested in a heading, for comparison:

### A heading with **bold** and *italic*

### A heading that is a [link with `code` in it](https://example.com)

---

## 18. Links inside this document

Each entry below points at a heading in this file. Clicking one should scroll
to that heading, not open a browser.

1. [The Shape of the Headings section](#1-headings)
2. [Emphasis and inline marks](#2-emphasis-and-inline-marks)
3. [Code](#5-code)
4. [Unicode and long lines](#12-unicode-and-long-lines)
5. [Edge cases](#13-edge-cases)
6. [Code blocks as panels](#16-code-blocks-as-panels)

Punctuation is dropped rather than turned into separators, so the commas and
the full stop in the heading below do not appear in its anchor:

### 18.1 Scope, Storage Duration, and Lifetime

[Link to the heading above](#181-scope-storage-duration-and-lifetime)

Plus signs go the same way, which is why a C++ heading collapses to a bare `c`:

### 18.2 Writing C++ Programs

[Link to the C++ heading](#182-writing-c-programs)

A heading containing a code span takes its anchor from the words, backticks and
all discarded:

### 18.3 The `NodeType` enum

[Link to the code heading](#183-the-nodetype-enum)

Two headings with identical text. The first keeps the plain anchor and the
second is numbered from one, the way GitHub does it:

### Repeated heading

Content under the first.

### Repeated heading

Content under the second.

[Link to the first](#repeated-heading) and
[link to the second](#repeated-heading-1).

An explicit `{#id}` wins over the derived anchor, and section 1 declared one:

[Link to the custom anchor](#custom-anchor)

Links that should **not** navigate anywhere in this file:

- [A dead anchor](#no-such-heading-exists) — should do nothing at all. It must
  not open the browser on this file, which is what a fragment falling through
  to the system handler would do.
- [A real external link](https://commonmark.org) — should open the browser.
- [A relative file link](./kitchen-sink.md) — should open the browser, since it
  is not a fragment.

An anchor that needs percent-decoding before it will match. Accented letters
survive into the anchor, and a generator encodes them on the way into the link:

### 18.4 Café Résumé

[Percent-encoded anchor](#184-caf%C3%A9-r%C3%A9sum%C3%A9) and the same target
[written literally](#184-café-résumé). Both should land on the heading above.

---

*End of the stress test. If you got here with everything rendered and no
freezes, the app is in good shape.*
