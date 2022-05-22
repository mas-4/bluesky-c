//
// Created by mas on 5/22/22.
//

#ifndef BLUESKY_RENDERING_H
#define BLUESKY_RENDERING_H


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

struct RenderedFile
{
    char *path;
    char *content;
};

void process_imports();
void process_files();
void write_files();

#endif //BLUESKY_RENDERING_H
