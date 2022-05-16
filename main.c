#include <stdio.h>
#include <argp.h>
#include <stdbool.h>
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

struct Meta
{
    char *title;
};

void set_default_args(struct arguments *args)
{
    args->input_dir = ".";
    args->output_dir = "./build";
    args->verbose = false;
    args->force = false;
}

void get_args(int argc, char **argv, struct arguments *arguments)
{
    struct argp argp = {options, parse_opt, args_doc, doc};

    error_t rc = argp_parse(&argp, argc, argv, 0, 0, arguments);
    if (rc != 0)
    {
        printf("argp_parse failed: %s\n", strerror(rc));
        exit(rc);
    }
    if (arguments->verbose)
    {
        printf("input_dir: %s\n", arguments->input_dir);
        printf("output_dir: %s\n", arguments->output_dir);
        printf("verbose: %d\n", arguments->verbose);
        printf("force: %d\n", arguments->force);
    }
}

void parse_meta(char *input_dir, struct Meta *meta)
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
            meta->title = strtok(NULL, "=");
        }
    }
    fclose(fp);
    free(meta_file);
}

int main(int argc, char *argv[])
{
    struct arguments arguments;
    set_default_args(&arguments);
    get_args(argc, argv, &arguments);
    struct Meta meta;
    parse_meta(arguments.input_dir, &meta);
    if (arguments.verbose)
    {
        printf("title: %s\n", meta.title);
    }
    return 0;
}