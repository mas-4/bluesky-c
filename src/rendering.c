//
// Created by mas on 5/22/22.
//

#include "rendering.h"
#include "raw_files.h"
#include "arguments.h"
#include <stdlib.h>
#include <libgen.h>

extern struct RawImport *raw_imports;
extern size_t n_raw_imports;
extern struct RawPage *raw_pages;
extern size_t n_raw_pages;
extern struct Arguments args;

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

struct RenderedFile *rendered_files = NULL;
size_t n_rendered_files = 0;
size_t cap_rendered_files = 0;

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
            .name = raw_import->name,
            .content = raw_import->content,
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

char *process_file(char *content)
{
    char *new_content = strdup(content);
    int next_marker;
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
            case IT_TEMPLATE:
                break;
            case IT_MARKDOWN:
                break;
            case IT_PLACEHOLDER:
                break;
            default:
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
    return new_content;
}

char *parse_out_path(char *file_path)
{
    char *bname = basename(file_path);
    int slash = args.output_dir[strlen(args.output_dir) - 1] != '/';
    char *output_file_path = malloc(
            strlen(args.output_dir)
            + slash
            + strlen(bname)
            + 1);
    strcpy(output_file_path, args.output_dir);
    if (slash != 0)
    {
        strcat(output_file_path, "/");
    }
    strcat(output_file_path, bname);
    return output_file_path;
}

void add_rendered_file(struct RawPage *raw_page, char *content)
{
    if (cap_rendered_files == 0)
    {
        cap_rendered_files = 10;
        rendered_files = malloc(
                sizeof(struct RenderedFile) * cap_rendered_files);
    } else if (n_rendered_files == cap_rendered_files)
    {
        cap_rendered_files *= 2;
        rendered_files = realloc(rendered_files, sizeof(struct RenderedFile) *
                                                 cap_rendered_files);
    }
    struct RenderedFile *rendered_file = &rendered_files[n_rendered_files];
    rendered_file->content = content;
    char *path = parse_out_path(raw_page->path);
    rendered_file->path = path;
    n_rendered_files++;
}

void process_imports()
{
    for (int i = 0; i < n_raw_imports; i++)
    {
        struct RawImport *raw_import = &raw_imports[i];
        if (strncmp(raw_import->name, "_", 1) == 0)
        {
            parse_include(raw_import);
        }
    }
}

void process_files()
{
    for (int i = 0; i < n_raw_pages; i++)
    {
        char *content = raw_pages[i].content;
        char *processed_content = process_file(content);
        add_rendered_file(&raw_pages[i], processed_content);
    }
}

// https://stackoverflow.com/questions/2336242/recursive-mkdir-system-call-on-unix
static void _mkdir(const char *dir) {
    char tmp[256];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++)
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    mkdir(tmp, S_IRWXU);
}


void write_files()
{
    for (int i = 0; i < n_rendered_files; i++)
    {
        struct RenderedFile *rendered_file = &rendered_files[i];
        char *tmp = strdup(rendered_file->path);
        char *dir_path = dirname(tmp);
        _mkdir(dir_path);
        free(tmp);
        printf("Writing %s\n", rendered_file->path);
        FILE *f = fopen(rendered_file->path, "w");
        if (f == NULL)
        {
            fprintf(stderr, "Could not open file %s\n", rendered_file->path);
            fprintf(stderr, "Error: %s\n", strerror(errno));
            exit(1);
        }
        fprintf(f, "%s", rendered_file->content);
        fclose(f);
    }
}