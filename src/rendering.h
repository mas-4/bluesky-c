//
// Created by mas on 5/22/22.
//

#ifndef BLUESKY_RENDERING_H
#define BLUESKY_RENDERING_H


struct RenderedFile
{
    char *path;
    char *content;
};

void process_files();
void write_files();
void free_rendered();

#endif //BLUESKY_RENDERING_H
