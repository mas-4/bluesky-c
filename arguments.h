//
// Created by mas on 5/21/22.
//

#ifndef BLUESKY_ARGUMENTS_H
#define BLUESKY_ARGUMENTS_H


#include <argp.h>
#include <string.h>
#include <stdlib.h>

const char *argp_program_version = "bluesky 0.1";
const char *argp_program_bug_address = "<m@stdwtr.io>";
static char *doc = "bluesky - a simple command line static site generator";
static char *args_doc = "INPUT_DIR OUTPUT_DIR";
static struct argp_option options[] = {
        {"verbose", 'v', 0, 0, "Produce verbose output"},
        {"force",   'f', 0, 0, "Force overwrite of existing files"},
        {"help",    'h', 0, 0, "Show this help"},
        {"version", 'V', 0, 0, "Show version"},
        {0}
};

struct arguments
{
    char *input_dir;
    char *output_dir;
    bool verbose;
    bool force;
};

struct arguments args = {
        .input_dir = ".",
        .output_dir = "./build",
        .verbose = false,
        .force = false,
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;
    switch (key)
    {
        case 'v':
            arguments->verbose = true;
            break;
        case 'f':
            arguments->force = true;
            break;
        case 'h':
            argp_state_help(state, stdout, ARGP_HELP_LONG);
            break;
        case 'V':
            printf("%s\n", argp_program_version);
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num >= 2)
            {
                argp_usage(state);
            }
            if (state->arg_num == 0)
            {
                arguments->input_dir = arg;
            } else
            {
                arguments->output_dir = arg;
            }
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 2)
            {
                argp_usage(state);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

void get_args(int argc, char **argv)
{
    struct argp argp = {options, parse_opt, args_doc, doc};

    error_t rc = argp_parse(&argp, argc, argv, 0, 0, &args);
    if (rc != 0)
    {
        printf("argp_parse failed: %s\n", strerror(rc));
        exit(rc);
    }
    if (args.verbose)
    {
        printf("*** args ***\n");
        printf("input_dir: %s\n", args.input_dir);
        printf("output_dir: %s\n", args.output_dir);
        printf("verbose: %d\n", args.verbose);
        printf("force: %d\n", args.force);
    }
}

#endif //BLUESKY_ARGUMENTS_H
