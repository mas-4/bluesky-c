
#include "main.h"
#include "raw_files.h"
#include "arguments.h"
#include "meta.h"

#include <string.h>

// find all indices of bluesky markers ("<bluesky-") in file
int find_all_bluesky_markers(char *file, int *indices)
{
    size_t indices_cap = 10;
    indices = malloc(sizeof(int) * indices_cap);
    int n_indices = 0;
    int i = 0;
    while (i < strlen(file))
    {
        char *ptr = strstr(&file[i], "<bluesky-");
        if (ptr != NULL)
        {
            if (n_indices == indices_cap)
            {
                indices_cap *= 2;
                indices = realloc(indices, sizeof(int) * indices_cap);
            }
            indices[n_indices] = (int) (ptr - file);
            n_indices++;
            i = indices[n_indices];
        } else
        {
            break;
        }
    }
    return n_indices;
}

// types of bluesky markers
// 0: <bluesky-include>
// 1: <bluesky-markdown>
// 2: <bluesky-template>
// 3: <bluesky-placeholder>

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

void teardown()
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

int main(int argc, char *argv[])
{
    get_args(argc, argv);
    parse_meta(args.input_dir);

    populate_file_list();

    // process_imports();
    // render_files();

    teardown();
    return 0;
}