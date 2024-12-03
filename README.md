
# WinJavaLauncher - Wrapper Application Build and Packaging for Windows

This repository provides a build system to create a wrapper application for a jpackaged app. The wrapper application launches the jpackaged executable and includes all necessary resources, such as app files, icons, and runtime libraries. If you build the wrapper application with `make.sh` script, you only receive a single executable and portable file for a packaged Java application, which temporarily creates the app and runtime directory in the same location at runtime and deletes them again after execution.

## Overview

The project uses a `make.sh` to automate the process of building and packaging the wrapper application. It integrates various resources (such as the executable, runtime dependencies, and icons) into a resource file (`src/resources.rc`), which is used by the wrapper application.

### Key Resources:
- **App Executable**: The main executable file of the jpackaged application (can be any name).
- **App Icon**: The icon to be associated with the wrapper executable.
- **App Contents**: A zip file containing the core files of your jpackaged app.
- **Runtime Contents**: A zip file containing any required runtime dependencies (e.g., JRE).

All these resources are packaged and used by the `make.sh` to create a functional wrapper.

## `make.sh`

The `make.sh` script is used to build the wrapper application. It does the following:

1. **Compiles the Resources**: It compiles the `src/resources.rc` file that contains all necessary resource definitions (like icons, app contents, etc.).
2. **Builds the Wrapper Executable**: It creates a wrapper executable that, when run, launches the jpackaged application.
3. **Customizable Variables**: The script allows users to define custom variables, such as the name of the jpackaged executable.

### How to Define the Executable Name

The name of the wrapper application's executable can be defined by setting the `WRAPPER_APP_EXE` variable. This variable determines the name of the output wrapper executable and is defined in the `make.sh`.

#### Example:

To define the wrapper executable name as `MyWrapperApp.exe`, set the `WRAPPER_APP_EXE` variable in your terminal or within the script:

```bash
export WRAPPER_APP_EXE="MyWrapperApp.exe"
```

Alternatively, you can modify the `make.sh` directly to set the executable name.

### Build Instructions

1. **Prepare Resources**: Ensure that all necessary resources (e.g., `app.zip`, `icon.ico`, `runtime.zip`, and your jpackaged executable) are placed in the `app/` directory.

   Your directory structure should look like this:

        /app
        ├── app.zip
        ├── icon.ico
        ├── runtime.zip
        └── your-app.exe

    Read [Wrapper Application Resource Preparation](app/README.md) for detailed instructions.

2. **Run the Build Script**: Once the resources are in place, run the `make.sh` to build the wrapper application:
   ```bash
   ./Makefile.sh
   ```

3. **Check the Output**: The wrapper executable will be created using the name defined in the `WRAPPER_APP_EXE` variable. The executable will include all the necessary resources and be ready to launch your jpackaged application.

### Customizing the Build

You can customize the build process by modifying the `make.sh`:
- Set the `WRAPPER_APP_EXE` variable to change the name of the output executable.
- Modify the `src/resources.rc` file to include additional resources if needed.

## Directory Structure

The expected directory structure for your project is as follows:

```
WinJavaLauncher/
├── src/
│   ├── main.cpp         # Entry point of the wrapper application
│   ├── resources.rc     # Resource definitions (icon, zips, etc.)
│   ├── resources.h      # Header file for resource handling
│   └── miniz/           # Embedded zipping library
│       ├── miniz.c
│       └── miniz.h
├── build/               # Compiled outputs
│   └── (Generated files like EXEs and intermediate object files)
├── app/                 # App-specific resources to bundle
│   ├── app.zip          # Main app archive
│   ├── runtime.zip      # JRE runtime archive
│   └── icon.ico         # App icon
│   └── your-app.exe     # Your jpackaged Windows app
├── makefile.sh          # Build script
├── README.md            # Documentation
└── LICENSE.md           # License file
```

## Important Notes

- **Executable Naming**: The executable name is fully customizable. You can define it by setting the `WRAPPER_APP_EXE` variable, either in your terminal session or within the `make.sh`.
- **Resource Paths**: Ensure that all resources (like `app.zip`, `icon.ico`, etc.) are placed in the `app/` directory and that the paths in the `src/resources.rc` file match their actual locations.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE.md) file for more details.
