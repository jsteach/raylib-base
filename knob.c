#define KNOB_IMPLEMENTATION
#include "knob.h"

static const char *raylib_modules[] = {
    "rcore",
    "raudio",
    "rglfw",
    "rmodels",
    "rshapes",
    "rtext",
    "rtextures",
    "utils",
};
// #define RAYLIB_PATH "./raylib/raylib-4.5.0"
#define RAYLIB_PATH "./raylib/raylib-5.0"

void init_cmd(Knob_Cmd* cmd){
    knob_cmd_append(cmd, "./Tools/windows_x64/zig.exe","cc");
    knob_cmd_append(cmd, "-static");
    knob_cmd_append(cmd, "--debug", "-std=c11", "-fno-sanitize=undefined","-fno-omit-frame-pointer");
    knob_cmd_append(cmd, "-DPLATFORM_DESKTOP");
    knob_cmd_append(cmd, "-target");
    knob_cmd_append(cmd, "x86_64-windows");
    knob_cmd_append(cmd, "-I"RAYLIB_PATH"/src/external/glfw/include");
    knob_cmd_append(cmd, "-I"RAYLIB_PATH"/src");
    knob_cmd_append(cmd, "-lkernel32","-lwinmm", "-lgdi32","-lopengl32");
    for (size_t i = 0; i < KNOB_ARRAY_LEN(raylib_modules); ++i) {
        knob_cmd_append(cmd,knob_temp_sprintf(RAYLIB_PATH"/src/%s.c", raylib_modules[i]));
    }
    knob_cmd_append(cmd,"./src/main.c");
}
bool build_game(void)
{
    bool result = true;
    Knob_Cmd cmd = {0};
    Knob_Procs procs = {0};

    if (!knob_mkdir_if_not_exists("./Deployment")) {
        knob_return_defer(false);
    }
    cmd.count = 0;
    init_cmd(&cmd);
    knob_cmd_append(&cmd,"./src/lab1_entrypoint.c",);
    knob_cmd_append(&cmd, "-o", "./Deployment/game.exe");
    if (!knob_cmd_run_sync(cmd)) knob_return_defer(false);
    // Ajouter pour chaque labo:
    // cmd.count = 0;
    // init_cmd(&cmd);
    // knob_cmd_append(&cmd,"./src/laboX_entrypoint.c",);
    // knob_cmd_append(&cmd, "-o", "./Deployment/game.exe");
    // if (!knob_cmd_run_sync(cmd)) knob_return_defer(false);
    
defer:
    knob_cmd_free(cmd);
    knob_da_free(procs);
    return result;
}

int main( int argc, char** argv){
    if (!build_game()) return 1;
}