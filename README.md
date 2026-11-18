# eeu — UTF-8 Easy Editor

[Русская версия](README.ru.md)

`eeu` is an independently buildable UTF-8 version of the Easy Editor (`ee`)
source distributed with FreeBSD 15.1. It keeps the small, menu-driven interface
of the original editor while adding UTF-8-aware input, display, cursor movement,
editing, search, and a Russian message catalog.

The original `/usr/bin/ee` and `/usr/bin/ree` are not replaced. The restricted
variant is installed separately as `reeu`.

## Features

- UTF-8 input and display through wide-character `ncursesw`.
- Cursor movement, Backspace, Delete, and prompt editing operate on UTF-8 code
  points rather than individual bytes.
- Screen positioning accounts for `wcwidth()` terminal-cell widths, including
  wide CJK code points and combining marks.
- Case-insensitive search uses the active locale's wide-character case mapping.
- File contents are kept as bytes. Opening and saving do not transcode or
  normalize file contents; malformed UTF-8 bytes are preserved.
- Russian menus, help, prompts, status text, and editor messages are supplied
  through the standard `catgets()` message-catalog interface.
- Editor commands and configuration keywords remain English. Existing
  `.init.ee` configuration files remain compatible.
- `reeu` runs the restricted mode and does not permit file operations or shell
  escapes, matching the original `ree` behavior.

## Unicode scope and limitations

The editor moves and deletes by Unicode code point, not by extended grapheme
cluster. A letter followed by combining marks, or an emoji sequence joined from
several code points, may require multiple cursor movements or deletions. The
rendering and width of complex emoji also depend on the terminal emulator and
its Unicode-width tables. This is not a claim of full grapheme-cluster editing.

For invalid UTF-8 data, the editor preserves the original bytes when saving and
uses a replacement glyph for display where possible.

## Requirements

On FreeBSD, the build uses the wide-character curses/terminfo libraries from
the system and `gencat` from the base system. A UTF-8 locale is required when
running the editor with UTF-8 text. For the Russian interface, the
`ru_RU.UTF-8` locale and its message catalog must be available. The default
build and test locale is the selected message locale (`ru_RU.UTF-8`); set
`UTF8_LOCALE` explicitly if you need another installed UTF-8 locale.

The native build file is the BSD `Makefile`. `GNUmakefile` is provided for GNU
make environments that have the `ncursesw` development headers and library.

## Build and install on FreeBSD

From the project directory:

```sh
make clean
make
make test
sudo make install
```

The default prefix is `/usr/local`. To install under a separate prefix, such as
`/opt/eeu`, set `PREFIX` explicitly:

```sh
make PREFIX=/opt/eeu NLS_LOCALE=ru_RU.UTF-8 UTF8_LOCALE=ru_RU.UTF-8
make test PREFIX=/opt/eeu NLS_LOCALE=ru_RU.UTF-8 UTF8_LOCALE=ru_RU.UTF-8
make install PREFIX=/opt/eeu NLS_LOCALE=ru_RU.UTF-8 UTF8_LOCALE=ru_RU.UTF-8
```

With `PREFIX=/opt/eeu`, the executable files are installed under
`/opt/eeu/bin/`, and the Russian catalog is installed as:

```text
/opt/eeu/share/nls/ru_RU.UTF-8/eeu
```

The catalog name is deliberately `eeu`, not `ee`. On FreeBSD the installed
catalog file has no `.cat` suffix.

## Russian interface in tcsh

Add the following to `~/.tcshrc` when using the `/opt/eeu` prefix:

```tcsh
setenv PATH "/opt/eeu/bin:${PATH}"
setenv NLSPATH "/opt/eeu/share/nls/%L/%N"
setenv LANG ru_RU.UTF-8
setenv LC_CTYPE ru_RU.UTF-8
setenv LC_MESSAGES ru_RU.UTF-8
```

If `LC_ALL` is set, it overrides the individual locale variables. Leave it
unset or set it to a suitable UTF-8 locale. Then reload the shell settings and
refresh the command hash:

```tcsh
source ~/.tcshrc
rehash
```

Start the editor with:

```tcsh
eeu filename.txt
```

For the English interface, use an English `LC_MESSAGES` locale. If the Russian
catalog cannot be found, the built-in English messages are used.

## Tests

`make test` checks the UTF-8 helper module and opens the Russian message catalog
by its basename (`eeu`) through `NLSPATH`, which is the same lookup mechanism
used by the editor. The tests cover UTF-8 sequence validation, code-point
boundaries, terminal-cell widths, selected Russian UI messages, and the fact
that command words and confirmation keys remain English.

The test suite is not a substitute for interactive testing in a real FreeBSD
terminal. After building, verify keyboard input, menus, resizing, search,
editing, and saving using the terminal emulator you normally use.

## Version and source

This release is `eeu 1.0.0`. The upstream Easy Editor source version used as the
base is `1.5.2`, from the FreeBSD 15.1 source archive supplied for this fork.
See [Changes](Changes) for the release notes and [UPSTREAM.md](UPSTREAM.md) for
provenance and attribution.

## License

The original Easy Editor source is distributed under the two-clause BSD-style
license reproduced in [LICENSE](LICENSE). Original copyright and attribution
notices are retained in the source files.
