//
// Created by mas on 5/21/22.
//

#ifndef BLUESKY_RAW_FILES_H
#define BLUESKY_RAW_FILES_H

#define _XOPEN_SOURCE 700   /* See feature_test_macros(7) */
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#include "constants.h"
#include "arguments.h"

#include <ftw.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>

#ifndef USE_FDS
#define USE_FDS 15
#endif

struct RawPage
{
    char *name;
    char *path;
    char *content;
    size_t content_size;
};

struct RawImport
{
    char *name;
    char *path;
    char *content;
    size_t content_size;
};

void populate_file_list();

void free_raw_files();

#endif //BLUESKY_RAW_FILES_H
