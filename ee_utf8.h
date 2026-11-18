/*
 * UTF-8 utilities used by ee.
 * Invalid or incomplete byte sequences are treated as individual bytes.
 */
#ifndef EE_UTF8_H
#define EE_UTF8_H

int ee_utf8_len(const unsigned char *s);
unsigned char *ee_utf8_prev(const unsigned char *start,
    const unsigned char *ptr);
int ee_utf8_width(const unsigned char *s);
int ee_utf8_string_width(const unsigned char *s);

#endif /* EE_UTF8_H */
