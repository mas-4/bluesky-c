//
// Created by mas on 5/22/22.
//

#include "rendering.h"
#include "raw_files.h"
#include "arguments.h"
#include "imports.h"
#include <stdlib.h>
#include <libgen.h>

extern struct RawImport *raw_imports;
extern size_t n_raw_imports;
extern struct RawPage *raw_pages;
extern size_t n_raw_pages;
extern struct Arguments args;


struct RenderedFile *rendered_files = NULL;
size_t n_rendered_files = 0;
size_t cap_rendered_files = 0;

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
        FILE *f = fopen(rendered_file->path, "w");
        if (f == NULL)
        {
            fprintf(stderr, "Could not open file %s\n", rendered_file->path);
            fprintf(stderr, "Error: %s\n", strerror(errno));
            exit(1);
        }
        fclose(f);
    }
}

void free_rendered()
{
    for (int i = 0; i < n_rendered_files; i++)
    {
        free(rendered_files[i].content);
        free(rendered_files[i].path);
    }
    free(rendered_files);
}