#ifndef EEU_CORE_H
#define EEU_CORE_H
#include <stdio.h>
#include <stddef.h>
int ee_resize_capacity(int current, int delta, int min_required);
int ee_read_config_line(FILE *stream, char *buffer, size_t size);
char *ee_resolve_name(const char *name);
#endif
