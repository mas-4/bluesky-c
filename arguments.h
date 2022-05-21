//
// Created by mas on 5/21/22.
//

#ifndef BLUESKY_ARGUMENTS_H
#define BLUESKY_ARGUMENTS_H


#include <argp.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct Arguments
{
    char *input_dir;
    char *output_dir;
    bool verbose;
    bool force;
};

void get_args(int argc, char **argv);

#endif //BLUESKY_ARGUMENTS_H
