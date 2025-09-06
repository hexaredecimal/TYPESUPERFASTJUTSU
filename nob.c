#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define BUILD_PATH "./build/"

int main(int argc, char** argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  if (!nob_mkdir_if_not_exists(BUILD_PATH))
    return 1;

  Nob_Cmd cmd = {0};
  nob_cmd_append(&cmd, "g++");
  nob_cmd_append(&cmd, "-std=c++17");
  nob_cmd_append(&cmd, "main.cpp");
  nob_cmd_append(&cmd, "ini.c");
  nob_cmd_append(&cmd, "-o");
  nob_cmd_append(&cmd, BUILD_PATH"typesuperfast");
  nob_cmd_append(&cmd, "-lX11");
  nob_cmd_append(&cmd, "-lXtst");
  nob_cmd_append(&cmd, "-lXext");
  nob_cmd_append(&cmd, "-lraylib");
  nob_cmd_append(&cmd, "-lm");

  if (!nob_cmd_run_sync_and_reset(&cmd))
    return 1;
}
