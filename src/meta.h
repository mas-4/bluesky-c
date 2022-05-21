//
// Created by mas on 5/21/22.
//

#ifndef BLUESKY_META_H
#define BLUESKY_META_H

#include <malloc.h>


struct Meta
{
    char *title;
};

void parse_meta(char *input_dir);

void free_meta();

#endif //BLUESKY_META_H
