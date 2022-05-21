#include <stdbool.h>

enum ImportType
{
    IT_INCLUDE,
    IT_TEMPLATE,
    IT_MARKDOWN,
    IT_PLACEHOLDER
};

char const *const import_type_identifiers[] = {
    [IT_INCLUDE] = "<bluesky-include",
    [IT_TEMPLATE] = "<bluesky-template",
    [IT_MARKDOWN] = "<bluesky-markdown",
    [IT_PLACEHOLDER] = "<bluesky-placeholder"
};

int n_import_types = sizeof(import_type_identifiers) / sizeof(char const *);

struct Import
{
    char *path;
    char *name;
    char *content;
    enum ImportType type;
};

struct RenderedFiles
{
    char *fname;
    char *content;
};
