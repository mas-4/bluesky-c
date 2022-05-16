#define _XOPEN_SOURCE 700   /* See feature_test_macros(7) */
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <argp.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include <ftw.h>

#ifndef USE_FDS
#define USE_FDS 15
#endif

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

struct Meta
{
    char *title;
};

struct arguments args = {
        .input_dir = ".",
        .output_dir = "./build",
        .verbose = false,
        .force = false,
};

struct Meta meta = {
        .title = "",
};

char **files = NULL;
int files_count = 0;
int files_capacity = 0;

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


int add_file(const char *fpath, const struct stat *sb,
             int typeflag, struct FTW *ftwbuf)
{
    if (typeflag != FTW_F)
    {
        return 0;
    }
    if (strcmp(&fpath[ftwbuf->base], "meta") == 0)
    {
        return 0;
    }
    if (files_count == files_capacity)
    {
        files_capacity *= 2;
        files = realloc(files, files_capacity * sizeof(char *));
    }
    files[files_count] = strdup(fpath);
    files_count++;
    return 0;
}

void populate_file_list()
{
    files_count = 0;
    files_capacity = 10;
    files = malloc(files_capacity * sizeof(char *));
    nftw(args.input_dir, add_file, 10, FTW_PHYS);
    if (args.verbose)
    {
        printf("\n*** files ***\n");
        printf("files_count: %d\n", files_count);
        for (int i = 0; i < files_count; i++)
        {
            printf("%s\n", files[i]);
        }
    }
}

void teardown()
{
    for (int i = 0; i < files_count; i++)
    {
        free(files[i]);
    }
    free(files);
}

int main(int argc, char *argv[])
{
    get_args(argc, argv);
    parse_meta(args.input_dir);
    populate_file_list();

    teardown();
    return 0;
}