# README changes

This file documents the README layout changes made for the SELZ80Trainer project.

## Changed file

`README.md`

## Changes

### 1. Added a dedicated Screenshots section

The four screenshots that were previously displayed one after another were grouped
into a single `## Screenshots` section.

### 2. Changed the screenshot layout to a 2 x 2 grid

The screenshots are now displayed using an HTML table supported by GitHub Markdown.

Layout:

| Row | Left | Right |
|---|---|---|
| 1 | SEL Z80 Trainer | System 0 – 4-Bit Adder |
| 2 | CamelForth | CamelForth Screen |

### 3. Added titles above every screenshot

- `SEL Z80 Trainer`
- `System 0 – 4-Bit Adder`
- `CamelForth`
- `CamelForth Screen`

### 4. Preserved the existing image files

```text
docs/images/SELZ80Trainer.png
docs/images/selz80trainer-system0-4bit-adder.png
docs/images/selz80trainer-forth.png
docs/images/forth-screen.png
```

### 5. Removed the old duplicate screenshot section when present

The legacy screenshot section at the end of the README is removed when it matches
the previously documented duplicate block.

### 6. No build or project configuration is changed

This fix changes only README documentation.

## Suggested Git commit

```bash
git add README.md README_CHANGES.md
git commit -m "Improve README screenshot layout"
git push origin master
```
