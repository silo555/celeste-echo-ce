# Build CEleste Echo

## Native calculator build

Install the official [CE C/C++ toolchain](https://github.com/CE-Programming/toolchain/releases). This project is built in CI with v11.2, which matches the inherited APIs. Put its `bin` directory on PATH.

From the repository root:

```sh
make gfx
make
python3 tools/verify.py
```

The output is `bin/CECHO.8xp`. `make gfx` regenerates the atlas and palette from `src/gfx/atlas.png` and `convimg.yaml`. The icon is already supplied as `icon.png`; its editable vector source is `assets/icon.svg`.

On Apple silicon, use an official toolchain that runs on your architecture. The older v11.2 macOS package may need an existing Intel runtime; newer toolchains may require API compatibility changes. Do not silently alter calculator firmware or Windows/macOS security settings to build this app.

## Host tests

A normal C++ compiler and Python 3 are required:

```sh
make gfx
python3 tools/test.py
```

`tools/test.py` finds `c++`, `clang++`, or `g++`, or uses `CXX`. It compiles the record model and copies the actual practice/records implementation into an isolated test directory with fake keypad, drawing, and AppVar APIs. Warnings are errors. Tests generate `build/tests/results.json` and a selector layout PPM.

With Pillow installed, `python3 tools/render_preview.py` converts that layout into documentation PNGs. This image is a host-rendered layout preview, not a calculator/emulator screenshot. Native 8x8 header glyphs are approximated by the fixture; list glyphs, room pixels, positions, highlighting and PB values come from the real draw calls and source data.

## CI and packaging

The private repository's GitHub Actions workflow installs the official v11.2 Linux toolchain, regenerates graphics, runs native host tests, builds the calculator executable, checks its TI file structure and core physics hashes, and packages the result. Artifacts stay in this private repository.

`python3 tools/package.py` creates `dist/CEleste-Echo-install.zip` from the built executable and the official CE runtime-library group. The ZIP contains installation instructions, test notes, licenses, and SHA256 checksums.

The application source is not sent to a public hosted compiler. Windows Application Control blocked the local C++ compiler during development, so the native build runs in the explicitly requested private GitHub repository.
