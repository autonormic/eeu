# Upstream provenance

This project is a derivative work based on the Easy Editor (`ee`) source archive
provided for FreeBSD 15.1. The original source identifies its upstream version
as 1.5.2 and credits Hugh Mahon. The original copyright notices are retained in
`ee.c`, and the upstream two-clause BSD-style license is reproduced in `LICENSE`.

The `eeu` fork changes the terminal interface to wide-character `ncursesw`,
adds UTF-8-aware editing helpers, adds a Russian message catalog, and installs
under distinct command names (`eeu` and `reeu`). It deliberately does not replace
the system `ee` or `ree` commands and preserves the original `.init.ee`
configuration naming for compatibility.

The standalone build uses `ee.c`, `ee_utf8.c`, and `ee_utf8.h`. The legacy
`new_curse` implementation from the original source is not part of the `eeu`
build; the program uses the system wide-character curses implementation.
