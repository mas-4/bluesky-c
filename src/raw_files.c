//
// Created by mas on 5/21/22.
//

#include "raw_files.h"

struct RawPage *raw_pages;
size_t n_raw_pages;
size_t raw_pages_capacity;

struct RawImport *raw_imports;
size_t n_raw_imports;
size_t raw_imports_capacity;

extern struct Arguments args;

void save_page(char *page_path)
{
    if (n_raw_pages == raw_pages_capacity)
    {
        raw_pages_capacity += 10;
        raw_pages = realloc(raw_pages, raw_pages_capacity * sizeof(char *));
    }
    raw_pages[n_raw_pages].path = page_path;
    // strip path and extension
    char *page_name = strdup(strrchr(page_path, '/') + 1);
    char *extension = strrchr(page_name, '.');
    extension[0] = '\0';
    raw_pages[n_raw_pages].name = page_name;

    FILE *fp = fopen(page_path, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Could not open %s\n", page_path);
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    raw_pages[n_raw_pages].content_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    raw_pages[n_raw_pages].content = malloc(
            raw_pages[n_raw_pages].content_size
    );
    fread(
            raw_pages[n_raw_pages].content,
            raw_pages[n_raw_pages].content_size,
            1,
            fp
    );
    fclose(fp);
    n_raw_pages++;
}

void save_import(char *import_path)
{
    if (n_raw_imports == raw_imports_capacity)
    {
        raw_imports_capacity += 10;
        raw_imports = realloc(raw_imports, raw_imports_capacity * sizeof(char *));
    }
    raw_imports[n_raw_imports].path = import_path;
    char *page_name = strdup(strrchr(import_path, '/') + 1);
    char *extension = strrchr(page_name, '.');
    extension[0] = '\0';
    raw_imports[n_raw_imports].name = page_name;
    // strip path and extension
    FILE *fp = fopen(import_path, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Could not open %s\n", import_path);
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    raw_imports[n_raw_imports].content_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    raw_imports[n_raw_imports].content = malloc(
            raw_imports[n_raw_imports].content_size
    );
    fread(
            raw_imports[n_raw_imports].content,
            raw_imports[n_raw_imports].content_size,
            1,
            fp
    );
    fclose(fp);
    n_raw_imports++;
}

int add_file(
        const char *fpath,
        const struct stat *sb,
        int typeflag,
        struct FTW *ftwbuf
)
{
    if (typeflag != FTW_F)
    {
        return 0;
    }
    if (strcmp(&fpath[ftwbuf->base], META_FILE) == 0)
    {
        return 0;
    }
    if (strstr(fpath, IMPORTS_DIR) != NULL)
    {
        save_import(strdup(fpath));
        return 0;
    }
    save_page(strdup(fpath));
    return 0;
}

void populate_file_list()
{
    raw_pages_capacity = 10;
    raw_pages = malloc(raw_pages_capacity * sizeof(struct RawPage));
    raw_imports_capacity = 10;
    raw_imports = malloc(raw_imports_capacity * sizeof(struct RawImport));
    nftw(args.input_dir, add_file, 10, FTW_PHYS);
    if (args.verbose)
    {
        printf("\n*** imports ***\n");
        printf("n_imports: %d\n", (int) n_raw_imports);
        for (int i = 0; i < n_raw_imports; i++)
        {
            printf("%s: %s\n", raw_imports[i].name, raw_imports[i].path);
        }
        printf("\n*** pages ***\n");
        printf("n_pages: %d\n", (int) n_raw_pages);
        for (int i = 0; i < n_raw_pages; i++)
        {
            printf("%s: %s\n", raw_pages[i].name, raw_pages[i].path);
        }
    }
}

void free_raw_files()
{
    printf("%d imports\n", (int)n_raw_imports);
    for (int i = 0; i < n_raw_imports; i++)
    {
        free(raw_imports[i].path);
        free(raw_imports[i].name);
        free(raw_imports[i].content);
    }
    free(raw_imports);
    for (int i = 0; i < n_raw_pages; i++)
    {
        free(raw_pages[i].path);
        free(raw_pages[i].name);
        free(raw_pages[i].content);
    }
    free(raw_pages);
}