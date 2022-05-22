
#include "main.h"
#include "raw_files.h"
#include "arguments.h"
#include "meta.h"
#include "rendering.h"

#include <string.h>

extern struct Arguments args;


void teardown()
{
    free_meta();
    free_raw_files();
    free_rendered();
}

int main(int argc, char *argv[])
{
    get_args(argc, argv);
    parse_meta(args.input_dir);

    populate_file_list();

    process_imports();
    process_files();
    write_files();

    teardown();
    return 0;
}