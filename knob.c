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

#ifdef _WIN32
#define LIB_EXT ".lib"
#define ZIG_PATH "./Tools/windows_x64/zig.exe"
#else
#define ZIG_PATH "./Tools/linux_x64/zig"
#define LIB_EXT ".a"
#endif

bool build_raylib(Knob_Cmd* end_cmd)
{
    bool result = true;
    Knob_Cmd cmd = {0};
    // Knob_File_Paths object_files = {0};

    if (!knob_mkdir_if_not_exists("./build/raylib")) {
        knob_return_defer(false);
    }

    Knob_Procs procs = {0};

    const char *build_path = "./build/raylib";

    if (!knob_mkdir_if_not_exists(build_path)) {
        knob_return_defer(false);
    }

    for (size_t i = 0; i < KNOB_ARRAY_LEN(raylib_modules); ++i) {
        const char *input_path = knob_temp_sprintf(RAYLIB_PATH"/src/%s.c", raylib_modules[i]);
        const char *output_path = knob_temp_sprintf("%s/%s.o", build_path, raylib_modules[i]);
        output_path = knob_temp_sprintf("%s/%s.o", build_path, raylib_modules[i]);
        knob_cmd_append(end_cmd,output_path);

        if (knob_needs_rebuild(output_path, &input_path, 1)) {
            cmd.count = 0;
            knob_cmd_append(&cmd, ZIG_PATH,"cc");
            knob_cmd_append(&cmd, "--debug", "-std=c11", "-fno-sanitize=undefined","-fno-omit-frame-pointer");
            knob_cmd_append(&cmd, "-target");
            knob_cmd_append(&cmd, "x86_64-windows");
            knob_cmd_append(&cmd, "-DPLATFORM_DESKTOP","-fPIC");
            knob_cmd_append(&cmd, "-I"RAYLIB_PATH"/src/external/glfw/include");
            knob_cmd_append(&cmd, "-I"RAYLIB_PATH"/src");
            knob_cmd_append(&cmd, "-c", input_path);
            knob_cmd_append(&cmd, "-o", output_path);

            Knob_Proc proc = knob_cmd_run_async(cmd,NULL,NULL);
            knob_da_append(&procs, proc);
        }
    }
    cmd.count = 0;

    if (!knob_procs_wait(procs)) knob_return_defer(false);

defer:
    knob_cmd_free(cmd);
    return result;
}

#define LAB_NAME "entrypoint"
bool build_game(void)
{
    bool result = true;
    Knob_Cmd cmd = {0};
    Knob_Procs procs = {0};

    if (!knob_mkdir_if_not_exists("./Deployment")) {
        knob_return_defer(false);
    }
    if (!knob_mkdir_if_not_exists("./build")) {
        knob_return_defer(false);
    }
    knob_cmd_append(&cmd, ZIG_PATH,"c++");
    knob_cmd_append(&cmd, "-static");
    knob_cmd_append(&cmd, "--debug", "-std=c++11", "-fno-sanitize=undefined","-fno-omit-frame-pointer");
    knob_cmd_append(&cmd, "-I"RAYLIB_PATH"/src");
    build_raylib(&cmd);
    knob_cmd_append(&cmd,knob_temp_sprintf("./src/%s.cpp",LAB_NAME),"-o","./Deployment/game.exe");
    knob_cmd_append(&cmd, "-lkernel32","-lwinmm", "-lgdi32","-lopengl32");
    knob_cmd_append(&cmd,"./src/main.cpp");
    if (!knob_cmd_run_sync(cmd)) knob_return_defer(false);
    
defer:
    knob_cmd_free(cmd);
    knob_da_free(procs);
    return result;
}

int main( int argc, char** argv){
    if (!build_game()) return 1;
}