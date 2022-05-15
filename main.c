#include <stdio.h>
#include <argp.h>
#include <stdbool.h>

const char *argp_program_version = "bluesky 0.1";
const char *argp_program_bug_address = "<m@stdwtr.io>";
static char *doc = "bluesky - a simple command line static site generator";
static char *args_doc = "INPUT_DIR OUTPUT_DIR";
static struct argp_option options[] = {
    {"verbose", 'v', 0, 0, "Produce verbose output"},
    {"force", 'f', 0, 0, "Force overwrite of existing files"},
    {"help", 'h', 0, 0, "Show this help"},
    {"version", 'V', 0, 0, "Show version"},
    {0}
};

struct arguments {
    char *input_dir;
    char *output_dir;
    bool verbose;
    bool force;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;
    switch (key) {
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
        if (state->arg_num >= 2) {
            argp_usage(state);
        }
        if (state->arg_num == 0) {
            arguments->input_dir = arg;
        } else {
            arguments->output_dir = arg;
        }
        break;
    case ARGP_KEY_END:
        if (state->arg_num < 2) {
            argp_usage(state);
        }
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

struct meta
{
    char *title;
};

int main(int argc, char *argv[])
{
    struct arguments arguments;
    struct argp argp = {options, parse_opt, args_doc, doc};
    arguments.input_dir = ".";
    arguments.output_dir = "./build";
    arguments.verbose = false;
    arguments.force = false;
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    if (arguments.verbose) {
        printf("input_dir: %s\n", arguments.input_dir);
        printf("output_dir: %s\n", arguments.output_dir);
        printf("verbose: %d\n", arguments.verbose);
        printf("force: %d\n", arguments.force);
    }
    return 0;
}