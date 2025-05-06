Import("env")
import os

# Make PlatformIO use our custom stm32f1x.cfg script
def before_upload(source, target, env):
    # Get the OpenOCD script directory
    openocd_dir = os.path.expanduser("~/.platformio/packages/tool-openocd/openocd/scripts")

    # Backup the original target cfg if not already done
    original_cfg = os.path.join(openocd_dir, "target/stm32f1x.cfg")
    backup_cfg = os.path.join(openocd_dir, "target/stm32f1x.cfg.backup")

    # Only create backup if it doesn't exist yet
    if not os.path.exists(backup_cfg) and os.path.exists(original_cfg):
        print("Creating backup of original stm32f1x.cfg")
        import shutil
        shutil.copy(original_cfg, backup_cfg)

    # Copy our modified cfg file if it exists
    custom_cfg = os.path.join(env.subst("${PROJECT_DIR}"), "scripts/stm32f1x.cfg")
    if os.path.exists(custom_cfg):
        print("Temporarily using custom stm32f1x.cfg with support for 0x2ba01477 chip ID")
        # We don't actually need to copy it since we're using upload_flags to specify it directly

# Register the callback
env.AddPreAction("upload", before_upload)
