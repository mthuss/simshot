# simshot

simple screenshot program that's essentially a wrapper to slurp+grim so that I can have a pretty little file save dialogue whenever I want to save a screenshot.

Currently only available as a cli tool intended for use with WM keybinds. Will probably add a little UI in the future.

## Compiling

You can install the app by running
```bash
meson setup builddir
meson compile -C builddir
sudo meson install -C builddir
```

You can use it with the following flags:
```bash
-f, --full          Take full screenshot
-s, --selection     Select screenshot area
-S, --save          Save to file
-C, --clip          Copy to clipboard
```