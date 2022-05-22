//
// Created by mas on 5/22/22.
//

#ifndef BLUESKY_IMPORTS_H
#define BLUESKY_IMPORTS_H

#include "raw_files.h"

enum ImportType
{
    IT_INCLUDE,
    IT_TEMPLATE,
    IT_MARKDOWN,
    IT_PLACEHOLDER
};


struct Import
{
    char *path;
    char *name;
    char *content;
    int *indices;
    enum ImportType type;
};

char *process_include(char *marker, char *content);
int find_next_bluesky_import(char *file);
enum ImportType identify_import(char *str);
void parse_include(struct RawImport *raw_import);
void free_imports();

#endif //BLUESKY_IMPORTS_H
