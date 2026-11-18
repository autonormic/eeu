#include "../ee_utf8.h"

#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

static void
check_len(const unsigned char *s, int want)
{
	assert(ee_utf8_len(s) == want);
}

int
main(void)
{
	static const unsigned char ascii[] = "A";
	static const unsigned char cyrillic[] = "\320\226"; /* Ж */
	static const unsigned char cjk[] = "\347\225\214"; /* 界 */
	static const unsigned char combining[] = "\314\201"; /* U+0301 */
	static const unsigned char emoji[] = "\360\237\230\200"; /* 😀 */
	static const unsigned char invalid_overlong[] = "\300\200";
	static const unsigned char invalid_surrogate[] = "\355\240\200";
	static const unsigned char invalid_too_large[] = "\364\220\200\200";
	static const unsigned char truncated[] = { 0xE2, 0x82, 0x00 };
	static const unsigned char orphan_continuation[] = { 0x80, 0x00 };
	static const unsigned char mixed_invalid[] = { 0xE2, 0x82, 'X', 0x80, 0x00 };
	static const unsigned char text[] = "A\320\226\347\225\214Z";
	const unsigned char *end;

	if (setlocale(LC_ALL, "") == NULL || MB_CUR_MAX < 4) {
		fprintf(stderr, "test requires a UTF-8 locale (for example LC_ALL=C.UTF-8)\n");
		return (77);
	}

	check_len(ascii, 1);
	check_len(cyrillic, 2);
	check_len(cjk, 3);
	check_len(combining, 2);
	check_len(emoji, 4);
	check_len(invalid_overlong, 1);
	check_len(invalid_surrogate, 1);
	check_len(invalid_too_large, 1);
	check_len(truncated, 1);
	check_len(orphan_continuation, 1);
	check_len(mixed_invalid, 1);

	assert(ee_utf8_width(ascii) == 1);
	assert(ee_utf8_width(cyrillic) == 1);
	assert(ee_utf8_width(cjk) == 2);
	assert(ee_utf8_width(combining) == 0);
	assert(ee_utf8_width(emoji) >= 1);
	assert(ee_utf8_width(invalid_overlong) == 1);
	assert(ee_utf8_width(orphan_continuation) == 1);

	/* Invalid bytes must be traversed individually without skipping neighbors. */
	end = mixed_invalid + 4;
	assert(ee_utf8_prev(mixed_invalid, end) == mixed_invalid + 3);
	assert(ee_utf8_prev(mixed_invalid, mixed_invalid + 3) == mixed_invalid + 2);
	assert(ee_utf8_prev(mixed_invalid, mixed_invalid + 2) == mixed_invalid + 1);
	assert(ee_utf8_prev(mixed_invalid, mixed_invalid + 1) == mixed_invalid);

	end = text + strlen((const char *)text);
	assert(ee_utf8_prev(text, end) == text + 6); /* Z */
	assert(ee_utf8_prev(text, text + 6) == text + 3); /* 界 */
	assert(ee_utf8_prev(text, text + 3) == text + 1); /* Ж */
	assert(ee_utf8_prev(text, text + 1) == text);
	assert(ee_utf8_prev(text, text) == text);

	/* Terminal-cell widths used by Russian and other translated menus. */
	assert(ee_utf8_string_width((const unsigned char *)"Главное меню") == 12);
	assert(ee_utf8_string_width((const unsigned char *)"界") == 2);
	assert(ee_utf8_string_width((const unsigned char *)"е\xCC\x88") == 1);

	puts("UTF-8 tests passed");
	return (0);
}
