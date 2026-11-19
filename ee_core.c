#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#include "ee_core.h"
#include <limits.h>
#include <errno.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int
ee_resize_capacity(int current, int delta, int min_required)
{
    int result;
    if (current < 1 || min_required < 1)
        return (-1);
    if (delta > 0 && current > INT_MAX - delta)
        return (-1);
    if (delta < 0 && delta < -current)
        return (-1);
    result = current + delta;
    if (result < min_required)
        return (-1);
    return (result);
}

int
ee_read_config_line(FILE *stream, char *buffer, size_t size)
{
    size_t length;
    int ch;
    if (stream == NULL || buffer == NULL || size < 2 || size > INT_MAX)
        return (-1);
    if (fgets(buffer, (int)size, stream) == NULL)
        return (0);
    length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[--length] = '\0';
    } else {
        ch = fgetc(stream);
        if (ch == '\r') {
            ch = fgetc(stream);
            if (ch != '\n' && ch != EOF) {
                while (ch != '\n' && ch != EOF)
                    ch = fgetc(stream);
                return (-1);
            }
        } else if (ch != '\n' && ch != EOF) {
            while (ch != '\n' && ch != EOF)
                ch = fgetc(stream);
            return (-1);
        }
    }
    length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\r')
        buffer[length - 1] = '\0';
    return (1);
}

static int
append_bytes(char **buffer, size_t *length, size_t *capacity,
    const char *source, size_t count)
{
    size_t needed, new_capacity;
    char *new_buffer;
    if (count > (size_t)-1 - *length - 1)
        return (-1);
    needed = *length + count + 1;
    if (needed > *capacity) {
        new_capacity = (*capacity == 0) ? 64 : *capacity;
        while (new_capacity < needed) {
            if (new_capacity > (size_t)-1 / 2) {
                new_capacity = needed;
                break;
            }
            new_capacity *= 2;
        }
        new_buffer = realloc(*buffer, new_capacity);
        if (new_buffer == NULL)
            return (-1);
        *buffer = new_buffer;
        *capacity = new_capacity;
    }
    if (count != 0)
        memcpy(*buffer + *length, source, count);
    *length += count;
    (*buffer)[*length] = '\0';
    return (0);
}

static char *
duplicate_range(const char *source, size_t length)
{
    char *copy;
    if (length == (size_t)-1)
        return (NULL);
    copy = malloc(length + 1);
    if (copy == NULL)
        return (NULL);
    memcpy(copy, source, length);
    copy[length] = '\0';
    return (copy);
}

char *
ee_resolve_name(const char *name)
{
    const char *cursor, *slash, *end, *variable_start;
    struct passwd *user;
    char *result, *variable, *value;
    size_t length, capacity, variable_length;
    int changed;

    if (name == NULL)
        return (NULL);
    result = NULL;
    length = capacity = 0;
    changed = 0;
    cursor = name;

    if (name[0] == '~' && (slash = strchr(name, '/')) != NULL) {
        if (slash == name + 1)
            user = getpwuid(getuid());
        else {
            variable = duplicate_range(name + 1,
                (size_t)(slash - (name + 1)));
            if (variable == NULL)
                return (NULL);
            user = getpwnam(variable);
            free(variable);
        }
        if (user != NULL) {
            if (append_bytes(&result, &length, &capacity, user->pw_dir,
                strlen(user->pw_dir)) == -1)
                goto allocation_error;
            cursor = slash;
            changed = 1;
        }
    }

    while (*cursor != '\0') {
        if (*cursor != '$') {
            end = strchr(cursor, '$');
            if (end == NULL)
                end = cursor + strlen(cursor);
            if (append_bytes(&result, &length, &capacity, cursor,
                (size_t)(end - cursor)) == -1)
                goto allocation_error;
            cursor = end;
            continue;
        }

        variable_start = cursor;
        cursor++;
        if (*cursor == '{') {
            cursor++;
            end = strchr(cursor, '}');
            if (end == NULL) {
                if (append_bytes(&result, &length, &capacity,
                    variable_start, strlen(variable_start)) == -1)
                    goto allocation_error;
                cursor += strlen(cursor);
                break;
            }
            variable_length = (size_t)(end - cursor);
            cursor = end + 1;
        } else {
            end = cursor;
            while (*end != '\0' && *end != '/' && *end != '$')
                end++;
            variable_length = (size_t)(end - cursor);
            cursor = end;
        }
        if (variable_length == 0) {
            if (append_bytes(&result, &length, &capacity, variable_start,
                (size_t)(cursor - variable_start)) == -1)
                goto allocation_error;
            continue;
        }
        variable = duplicate_range(
            (variable_start[1] == '{') ? variable_start + 2 :
            variable_start + 1, variable_length);
        if (variable == NULL)
            goto allocation_error;
        value = getenv(variable);
        free(variable);
        if (value != NULL) {
            if (append_bytes(&result, &length, &capacity, value,
                strlen(value)) == -1)
                goto allocation_error;
            changed = 1;
        } else if (append_bytes(&result, &length, &capacity,
            variable_start, (size_t)(cursor - variable_start)) == -1)
            goto allocation_error;
    }

    if (!changed) {
        free(result);
        return ((char *)name);
    }
    if (result == NULL) {
        result = strdup("");
        if (result == NULL)
            return (NULL);
    }
    return (result);

allocation_error:
    free(result);
    return (NULL);
}
