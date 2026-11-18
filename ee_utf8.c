#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#ifndef _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED 1
#endif

#include "ee_utf8.h"

#include <stddef.h>
#include <string.h>
#include <wchar.h>

/*
 * Return the byte length of one valid UTF-8 code point. Malformed bytes are
 * deliberately handled one at a time so that editing never consumes an
 * unrelated byte and the original file can be preserved byte-for-byte.
 */
int
ee_utf8_len(const unsigned char *s)
{
	unsigned char c, c1, c2, c3;

	c = s[0];
	if (c < 0x80)
		return (1);
	c1 = s[1];
	if (c >= 0xC2 && c <= 0xDF)
		return ((c1 >= 0x80 && c1 <= 0xBF) ? 2 : 1);
	if (c >= 0xE0 && c <= 0xEF) {
		if (c1 == 0 || s[2] == 0)
			return (1);
		c2 = s[2];
		if (c1 < 0x80 || c1 > 0xBF || c2 < 0x80 || c2 > 0xBF)
			return (1);
		if ((c == 0xE0 && c1 < 0xA0) || (c == 0xED && c1 >= 0xA0))
			return (1);
		return (3);
	}
	if (c >= 0xF0 && c <= 0xF4) {
		if (c1 == 0 || s[2] == 0 || s[3] == 0)
			return (1);
		c2 = s[2];
		c3 = s[3];
		if (c1 < 0x80 || c1 > 0xBF || c2 < 0x80 || c2 > 0xBF ||
		    c3 < 0x80 || c3 > 0xBF)
			return (1);
		if ((c == 0xF0 && c1 < 0x90) || (c == 0xF4 && c1 > 0x8F))
			return (1);
		return (4);
	}
	return (1);
}

/* Return the beginning of the preceding code point (or malformed byte). */
unsigned char *
ee_utf8_prev(const unsigned char *start, const unsigned char *ptr)
{
	int n;

	if (ptr <= start)
		return ((unsigned char *)start);
	/* Check only the four possible UTF-8 start positions. */
	for (n = 1; n <= 4 && (size_t)(ptr - start) >= (size_t)n; n++) {
		const unsigned char *candidate = ptr - n;
		/* Continuation bytes can only belong to a lead byte further back. */
		if ((candidate[0] & 0xC0) != 0x80 && ee_utf8_len(candidate) == n)
			return ((unsigned char *)candidate);
	}
	return ((unsigned char *)ptr - 1);
}

/* Return terminal-cell width for one code point in the active locale. */
int
ee_utf8_width(const unsigned char *s)
{
	wchar_t wc;
	mbstate_t mbs;
	int len, width;
	size_t n;

	len = ee_utf8_len(s);
	if (len == 1 && s[0] >= 0x80)
		return (1);
	memset(&mbs, 0, sizeof(mbs));
	n = mbrtowc(&wc, (const char *)s, (size_t)len, &mbs);
	if (n != (size_t)len)
		return (1);
	width = wcwidth(wc);
	return (width >= 0) ? width : 1;
}

/* Return the total terminal-cell width of a NUL-terminated UTF-8 string. */
int
ee_utf8_string_width(const unsigned char *s)
{
	const unsigned char *p;
	int width, len;

	width = 0;
	p = s;
	while (*p != '\0') {
		len = ee_utf8_len(p);
		width += ee_utf8_width(p);
		p += len;
	}
	return (width);
}
