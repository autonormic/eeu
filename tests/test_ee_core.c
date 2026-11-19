#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#include "../ee_core.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
test_resize_capacity(void)
{
    assert(ee_resize_capacity(1010, -500, 501) == 510);
    assert(ee_resize_capacity(100, 20, 101) == 120);
    assert(ee_resize_capacity(100, -20, 80) == 80);
    assert(ee_resize_capacity(100, -99, 2) == -1);
    assert(ee_resize_capacity(INT_MAX, 1, 1) == -1);
    assert(ee_resize_capacity(10, INT_MIN, 1) == -1);
}

static void
test_config_lines(void)
{
    char buffer[8];
    FILE *stream;
    stream = tmpfile();
    assert(stream != NULL);
    assert(fputs("CASE\r\nFINAL", stream) >= 0);
    rewind(stream);
    assert(ee_read_config_line(stream, buffer, sizeof(buffer)) == 1);
    assert(strcmp(buffer, "CASE") == 0);
    assert(ee_read_config_line(stream, buffer, sizeof(buffer)) == 1);
    assert(strcmp(buffer, "FINAL") == 0);
    assert(ee_read_config_line(stream, buffer, sizeof(buffer)) == 0);
    fclose(stream);

    stream = tmpfile();
    assert(stream != NULL);
    assert(fputs("123456789012345\nNEXT\n", stream) >= 0);
    rewind(stream);
    assert(ee_read_config_line(stream, buffer, sizeof(buffer)) == -1);
    assert(ee_read_config_line(stream, buffer, sizeof(buffer)) == 1);
    assert(strcmp(buffer, "NEXT") == 0);
    fclose(stream);

    stream = tmpfile();
    assert(stream != NULL);
    assert(fputs("1234567\nOK\n", stream) >= 0);
    rewind(stream);
    assert(ee_read_config_line(stream, buffer, sizeof(buffer)) == 1);
    assert(strcmp(buffer, "1234567") == 0);
    assert(ee_read_config_line(stream, buffer, sizeof(buffer)) == 1);
    assert(strcmp(buffer, "OK") == 0);
    fclose(stream);
}

static void
test_path_expansion(void)
{
    char *path;
    static const char missing[] = "$EEU_VARIABLE_THAT_DOES_NOT_EXIST/file";
    assert(setenv("EEU_TEST_PATH", "/tmp/eeu-test", 1) == 0);
    path = ee_resolve_name("prefix/$EEU_TEST_PATH/file");
    assert(path != NULL);
    assert(strcmp(path, "prefix//tmp/eeu-test/file") == 0);
    free(path);
    path = ee_resolve_name("${" "EEU_TEST_PATH}/file");
    assert(path != NULL);
    assert(strcmp(path, "/tmp/eeu-test/file") == 0);
    free(path);
    path = ee_resolve_name(missing);
    assert(path == missing);
}

int
main(void)
{
    test_resize_capacity();
    test_config_lines();
    test_path_expansion();
    puts("Core regression tests passed");
    return (0);
}
