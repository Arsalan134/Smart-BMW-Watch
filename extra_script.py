Import("env")
import os
import glob

def remove_arm_asm_from_lvgl(source, target, env):
    pass

# Remove ARM-specific .S files from LVGL in libdeps before build
project_dir = env["PROJECT_DIR"]
pioenv = env["PIOENV"]
libdeps_dir = os.path.join(project_dir, ".pio", "libdeps", pioenv, "lvgl")

if os.path.isdir(libdeps_dir):
    for pattern in ["**/helium/*.S", "**/neon/*.S"]:
        for f in glob.glob(os.path.join(libdeps_dir, pattern), recursive=True):
            print(f"Removing ARM asm: {f}")
            os.remove(f)
