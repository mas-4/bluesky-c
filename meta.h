//
// Created by mas on 5/21/22.
//

#ifndef BLUESKY_META_H
#define BLUESKY_META_H

#include <string.h>
#include <malloc.h>
#include <stdlib.h>


struct Meta
{
    char *title;
};

struct Meta meta = {
        .title = "",
};

void parse_meta(char *input_dir)
{
    char *meta_file = malloc(strlen(input_dir) + strlen("/meta") + 1);
    strcpy(meta_file, input_dir);
    strcat(meta_file, "/meta");
    FILE *fp = fopen(meta_file, "r");
    if (fp == NULL)
    {
        printf("Could not open meta\n");
        exit(1);
    }
    char *line = NULL;
    size_t len = 0;
    while ((getline(&line, &len, fp)) != -1)
    {
        if (strstr(line, "title") != NULL)
        {
            strtok(line, "="); // ignore title
            meta.title = strtok(NULL, "=");
        }
    }
    if (args.verbose)
    {
        printf("\n*** meta ***\n");
        printf("title: %s\n", meta.title);
    }
    fclose(fp);
    free(meta_file);
}

#endif //BLUESKY_META_H
