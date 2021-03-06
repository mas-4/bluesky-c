//
// Created by mas on 5/22/22.
//

#include "imports.h"
#include "constants.h"
#include "raw_files.h"
#include <stdlib.h>
#include <string.h>

char const *const import_type_identifiers[] = {
        [IT_INCLUDE] = "<bluesky-include",
        [IT_TEMPLATE] = "<bluesky-template",
        [IT_MARKDOWN] = "<bluesky-markdown",
        [IT_PLACEHOLDER] = "<bluesky-placeholder"
};
int n_import_types = sizeof(import_type_identifiers) / sizeof(char const *);

struct Import *imports = NULL;
size_t n_imports = 0;
size_t cap_imports = 0;

extern int n_raw_imports;
extern struct RawImport *raw_imports;

enum ImportType identify_import(char *str)
{
    for (int i = 0; i < n_import_types; i++)
    {
        if (strncmp(str,
                    import_type_identifiers[i],
                    strlen(import_type_identifiers[i])) == 0)
        {
            return i;
        }
    }
    return -1;
}

int find_next_bluesky_import(char *file)
{
    char *ptr = strstr(file, BLUESKY_IDENTIFIER);
    if (ptr == NULL)
    {
        return -1;
    }
    return (int)(ptr - file);
}

void add_import(struct Import *import)
{
    if (cap_imports == 0)
    {
        cap_imports = 10;
        imports = malloc(sizeof(struct Import) * cap_imports);
    } else if (n_imports == cap_imports)
    {
        cap_imports *= 2;
        imports = realloc(imports, sizeof(struct Import) * cap_imports);
    }
    imports[n_imports] = *import;
    n_imports++;
}

void parse_include(struct RawImport *raw_import)
{
    struct Import import = {
            .name = strdup(raw_import->name),
            .content = strdup(raw_import->content),
            .type = IT_INCLUDE
    };
    add_import(&import);
}

struct Import *get_import(char *name, enum ImportType type)
{
    for (int i = 0; i < n_imports; i++)
    {
        struct Import *import = &imports[i];
        if (import->type != type)
        {
            continue;
        }
        if (strcmp(import->name, name) == 0)
        {
            return import;
        }
    }
    return NULL;
}

char *process_include(char *marker, char *content)
{
    char *name_str = "name=\"";
    char *name = strdup(strstr(marker, name_str));
    if (name == NULL)
    {
        fprintf(stderr, "No name specified for include\n");
        fprintf(stderr, "Content: %s\n", content);
        exit(1);
    }
    name += strlen(name_str);
    char *end_name = strchr(name, '"');
    if (end_name == NULL)
    {
        fprintf(stderr, "No end quote for name in include\n");
        fprintf(stderr, "Content: %s\n", content);
        exit(1);
    }
    *end_name = '\0';
    struct Import *include = get_import(name, IT_INCLUDE);
    if (include == NULL)
    {
        fprintf(stderr, "No include with name %s\n", name);
        fprintf(stderr, "Content: %s\n", content);
        exit(1);
    }
    char *new_content = malloc(strlen(include->content) + strlen(content) + 1);
    strncpy(new_content, content, marker - content);
    strcat(new_content, include->content);
    char *after_marker = strstr(marker, ">") + 1;
    strcat(new_content, after_marker);
    return new_content;
}

void append_slot(struct Import *import, struct slot *slot)
{
if (import->slots == NULL)
    {
        import->slots = malloc(sizeof(struct slot) * 10);
        import->slot_cap = 10;
    } else
    {
        if (import->n_slots == import->slot_cap)
        {
            import->slot_cap *= 2;
            import->slots = realloc(import->slots, sizeof(struct slot) * import->slot_cap);
        }
    }
    import->slots[import->n_slots] = *slot;
    import->n_slots++;
}

void process_slot(struct Import *import, char *marker, char *content)
{
    char *name_str = "name=\"";
    char *name = strdup(strstr(marker, name_str));
    if (name == NULL)
    {
        fprintf(stderr, "No name specified for slot\n");
        fprintf(stderr, "Content: %s\n", content);
        exit(1);
    }
    name += strlen(name_str);
    char *end_name = strchr(name, '"');
    if (end_name == NULL)
    {
        fprintf(stderr, "No end quote for name in slot\n");
        fprintf(stderr, "Content: %s\n", content);
        exit(1);
    }
    *end_name = '\0';
    struct slot slot = {
            .name = strdup(name),
            .start = marker,
            .end = content
    };
    append_slot(import, &slot);
}

void parse_template(struct RawImport *raw_import)
{
    char *content = raw_import->content;

    struct Import import = {
            .name = strdup(raw_import->name),
            .content = strdup(raw_import->content),
            .type = IT_TEMPLATE
    };
    char *new_content = strdup(content);
    enum ImportType next_marker;
    char *head = new_content;
    while ((next_marker = find_next_bluesky_import(head)) != -1)
    {
        char *marker = head + next_marker;
        size_t offset = marker - new_content;
        enum ImportType type = identify_import(marker);
        char *tmp = new_content;
        switch (type)
        {
            case IT_INCLUDE:
            {
                tmp = process_include(marker, new_content);
                break;
            }
            case IT_PLACEHOLDER:
            {
                process_slot(&import, marker, new_content);
            }
            case IT_MARKDOWN:
            case IT_TEMPLATE:
            {
                fprintf(stderr, "Not implemented for templates.\n");
                exit(1);
            }
            default:
                fprintf(stderr, "Unknown import type\n");
                break;
        }
        if (tmp != new_content)
        {
            free(new_content);
            marker = NULL;
            new_content = tmp;
        }
        head = new_content + offset + 1;
    }
    add_import(&import);
}

void process_imports()
{
    // todo: sort imports by name, underscores first

    for (int i = 0; i < n_raw_imports; i++)
    {
        struct RawImport *raw_import = &raw_imports[i];
        if (strncmp(raw_import->name, "_", 1) == 0)
        {
            parse_include(raw_import);
        }
    }
    for (int i = 0; i < n_raw_imports; i++)
    {
        struct RawImport *raw_import = &raw_imports[i];
        if (strncmp(raw_import->name, "_", 1) == 0)
        {
            parse_template(raw_import);
        }
    }
}

void free_imports()
{
    for (int i = 0; i < n_imports; i++)
    {
        struct Import *import = &imports[i];
        free(import->name);
        free(import->content);
        free(import->indices);
        for (int j = 0; j < import->n_slots; j++)
        {
            free(import->slots[j].name);
        }
        if (import->slots != NULL)
        {
            free(import->slots);
        }
    }
    free(imports);
}