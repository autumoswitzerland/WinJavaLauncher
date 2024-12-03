# Wrapper Application Resource Preparation

This document explains the resources required for the wrapper application, which serves as a launcher for jpackaged applications. These resources are defined in the `src/resources.rc` file and must be prepared in specific ways for the build process to work.

## Required Resources

### 1. **App Executable**
   - **Resource Identifier**: `IDR_APP_EXECUTABLE`
   - **Description**: This is the main executable file for your jpackaged application. The executable name is defined by the user and can be anything you choose.
   - **File Location**: `app/your-app.exe`
   - **Instructions**:
     - Replace `your-app.exe` with the actual executable for your jpackaged app.
     - Ensure the executable is correctly located in the `app/` directory.

### 2. **App Icon**
   - **Resource Identifier**: `IDR_APP_ICON`
   - **Description**: This is the icon associated with the wrapper executable. It should be in `.ico` format.
   - **File Location**: `app/icon.ico`
   - **Instructions**:
     - Ensure your app has a valid `.ico` file for the icon.
     - Place the icon file in the `app/` directory.

### 3. **App Contents (`app.zip`)**
   - **Resource Identifier**: `IDR_APP_CONTENTS`
   - **Description**: This zip file contains the core files for your application (e.g., `.jar` files, configuration files).
   - **File Location**: `app/app.zip`
   - **Instructions**:
     - Zip the necessary application files (e.g., `.jar` files) into `app.zip`.
     - Ensure the correct internal structure for your application inside the zip file.

### 4. **Runtime Contents (`runtime.zip`)**
   - **Resource Identifier**: `IDR_RUNTIME_CONTENTS`
   - **Description**: This zip file contains runtime dependencies, such as the Java Runtime Environment (JRE) or any other required libraries.
   - **File Location**: `app/runtime.zip`
   - **Instructions**:
     - Zip any required runtime libraries or JRE files into `runtime.zip`.
     - This could include a specific version of JRE or any other necessary dependencies for your application.

## Directory Structure

For the build process to work, the directory structure should look like this:

    /app
    ├── app.zip          # Core app files
    ├── icon.ico         # App icon
    ├── runtime.zip      # Runtime dependencies
    └── your-app.exe     # Executable of your jpackaged app

Ensure that each resource is placed in the correct directory as shown above. The name of the executable (`your-app.exe`) is fully customizable, and the resource definitions in the `src/resources.rc` file should reflect the name you choose.

## Important Notes

- **File Paths**: The paths in the `src/resources.rc` file must match the actual location of your resources. If you move or rename any files, update the `src/resources.rc` file accordingly.
- **Executable Naming**: The name of the executable file is completely customizable. The `src/resources.rc` file will reference the executable's name, so be sure to update the file with your chosen name. For example:
  - If your executable is called `MyCustomApp.exe`, ensure the `IDR_APP_EXECUTABLE` identifier points to `app/MyCustomApp.exe`.
- **App Contents**: Ensure that `app.zip` contains the necessary files for your jpackaged app. This can include `.jar` files, configuration files, and any other resources needed for your application to run.

## Preparation Checklist

1. Prepare and zip the necessary application files into `app.zip` and place it in the `app` directory.
2. Zip any required runtime dependencies into `runtime.zip` and place it in the `app` directory.
3. Place the application icon (`icon.ico`) in the `app` directory.
4. Place your app's executable (e.g., `your-app.exe`) in the `app` directory.
5. Verify that the file paths in `src/resources.rc` correspond to the actual locations of the resources.

Once the resources are correctly prepared and the files are in place, you can proceed with building the wrapper application.
