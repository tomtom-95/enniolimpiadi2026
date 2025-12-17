# Building on Windows

## Prerequisites (one-time setup)

### 1. Install a C Compiler

Download [w64devkit](https://github.com/skeeto/w64devkit/releases) (portable MinGW-w64, no installer required):

1. Download the latest `w64devkit-x.x.x.zip`
2. Extract to `C:\w64devkit`
3. Add `C:\w64devkit\bin` to your system PATH:
   - Press `Win + R`, type `sysdm.cpl`, press Enter
   - Go to **Advanced** tab → **Environment Variables**
   - Under "User variables", select **Path** → **Edit** → **New**
   - Add `C:\w64devkit\bin`
   - Click OK to save

4. Verify installation by opening a new PowerShell window:
   ```powershell
   gcc --version
   ```

### 2. Install raylib

1. Go to [raylib releases](https://github.com/raysan5/raylib/releases)
2. Download `raylib-X.X_win64_mingw-w64.zip` (use the mingw-w64 version)
3. Extract to `C:\raylib`

Your folder structure should look like:
```
C:\raylib\
├── include\
│   └── raylib.h
└── lib\
    └── libraylib.a
```

## Building the Project

Open a command prompt in the project directory and run:

```cmd
build.bat release
```

Available commands:
- `build.bat release` - Build optimized release executable
- `build.bat debug` - Build with debug symbols
- `build.bat clean` - Remove build directory

The executable will be created in the `build\` folder.

## Troubleshooting

### "gcc is not recognized"

Make sure `C:\w64devkit\bin` is in your PATH and you opened a **new** command prompt after adding it.

### "cannot find -lraylib" or missing header errors

Verify raylib is extracted to `C:\raylib` with the correct folder structure. If you installed it elsewhere, edit the `RAYLIB_PATH` variable in `build.bat`.
