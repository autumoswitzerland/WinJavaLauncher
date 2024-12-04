/**
 * WinJavaLauncher - Main program.
 *
 * @file main.cpp
 * @brief Main application wrapper for running a jpackaged Windows application.
 *
 * This file contains the main application logic for extracting zip files and
 * handling resources. It defines functions for managing file extraction and
 * other file system operations.
 *
 * @author 2024 autumo Ltd. Switzerland, Michael Gasche
 * @date 2024-12-02
 * @version 1.0
 *
 * @license MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */


#include "resources.h"    // Ensure this includes the resource IDs
#include "miniz/miniz.h"  // Include the miniz header for zip functionality
#include <windows.h>
#include <string>
#include <stdexcept>      // For runtime_error
#include <fstream>
#include <iostream>
#include <filesystem>     // C++17 standard library to handle directories
#include <vector>

namespace fs = std::filesystem;


// In-memory execution; it is not implemented!
// true to enable in-memory execution, false to disable
constexpr bool IN_MEMORY_EXECUTION = false;

// Global debug control
constexpr bool DEBUG_MODE = false;

// File buffer size
constexpr size_t FILENAME_BUFFER_SIZE = 256;

// Debug macro
#define DEBUG_LOG(message) if (DEBUG_MODE) std::cout << "[DEBUG]: " << message << std::endl;


/**
 * @brief Print error messages to the standard error stream.
 *
 * This function is used to print error messages in a standardized way to
 * assist in debugging and logging application issues.
 *
 * @param message The error message to be displayed.
 */
void printErrorInfo(const std::string& message) {
    std::cerr << message << std::endl;  // Always print error messages to std::cerr
}

/**
 * @brief Deletes specified files and directories.
 *
 * This function deletes a zip file and its extracted directory if they exist
 * on the filesystem.
 *
 * @param zipPath Path to the zip file to delete.
 * @param extractDir Path to the directory to delete.
 */
void deleteFilesAndDirectories(const std::string& zipPath, const std::string& extractDir) {
    // Delete the zip file if it exists
    if (!zipPath.empty() && fs::exists(zipPath)) {
        try {
            if (!fs::remove(zipPath)) {
                printErrorInfo("Failed to delete zip file: " + zipPath);
            } else {
                DEBUG_LOG("Deleted zip file: " + zipPath);
            }
        } catch (const fs::filesystem_error& e) {
            printErrorInfo("Error deleting zip file: " + std::string(e.what()));
        }
    }

    // Delete the extracted directory if it exists
    if (!extractDir.empty() && fs::exists(extractDir)) {
        try {
            // Attempt to remove all contents of the directory
            std::uintmax_t removedCount = fs::remove_all(extractDir);
            if (removedCount == 0) {
                printErrorInfo("Failed to delete extracted directory: " + extractDir);
            } else {
                DEBUG_LOG("Deleted directory and its contents: " + extractDir);
            }
        } catch (const fs::filesystem_error& e) {
            printErrorInfo("Error deleting extracted directory: " + std::string(e.what()));
        }
    }
}

/**
 * @brief Extracts a resource from the executable into memory or a file.
 *
 * This function extracts a resource embedded within the executable using
 * Windows resource management functions. Depending on the configuration, it
 * either loads the resource into memory or writes it to a file.
 *
 * @param resourceID The identifier of the resource to extract.
 * @param resourceBuffer Buffer to store the resource data if extracted into memory (not used).
 * @param outputPath Path to the file where the resource will be saved (optional).
 */
void extractResource(UINT resourceID, std::vector<char>& resourceBuffer, const std::string& outputPath) {
    DEBUG_LOG("Starting resource extraction. Resource ID: " + std::to_string(resourceID));

    // Get the current module handle (the executable itself)
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule == NULL) {
        printErrorInfo("Failed to get module handle!");
        return;
    }
    DEBUG_LOG("Module handle obtained.");

    HRSRC hResInfo = FindResource(hModule, MAKEINTRESOURCE(resourceID), RT_RCDATA);
    if (hResInfo == NULL) {
        printErrorInfo("Resource not found!");
        return;
    }
    DEBUG_LOG("Resource found.");

    DWORD dwSize = SizeofResource(hModule, hResInfo);
    if (dwSize == 0) {
        printErrorInfo("Resource size is 0!");
        return;
    }
    DEBUG_LOG("Resource size determined.");

    HGLOBAL hResData = LoadResource(hModule, hResInfo);
    if (hResData == NULL) {
        printErrorInfo("Failed to load resource!");
        return;
    }
    DEBUG_LOG("Resource loaded successfully.");

    // Lock the resource and get the pointer to the data
    const void* pData = LockResource(hResData);
    if (pData == NULL) {
        printErrorInfo("Failed to lock resource!");
        return;
    }
    DEBUG_LOG("Resource locked into memory.");

    if (IN_MEMORY_EXECUTION) {
        // In-memory extraction: Store resource data in a buffer (do not write to disk)
        resourceBuffer.assign(reinterpret_cast<const char*>(pData), reinterpret_cast<const char*>(pData) + dwSize);
        //data.assign((char*)pData, (char*)pData + dwSize);
        DEBUG_LOG("Resource loaded into memory. Size: " + std::to_string(dwSize));
    } else {
        // Write the resource data (zip or executable) to disk if output path is specified
        if (!outputPath.empty()) {
            std::ofstream outFile(outputPath, std::ios::binary);
            if (outFile.is_open()) {
                outFile.write(reinterpret_cast<const char*>(pData), dwSize);
                if (outFile.fail()) {
                    printErrorInfo("Failed to write resource to file!");
                } else {
                    DEBUG_LOG("Resource extracted to " + outputPath);
                }
                outFile.close();
            } else {
                printErrorInfo("Failed to open output file for writing.");
            }
        } else {
            printErrorInfo("No output path specified for disk extraction.");
        }
    }

    DEBUG_LOG("Resource extraction completed.");
}

/**
 * @brief Extracts the contents of a zip file to a specified directory.
 *
 * This function unzips a zip archive and extracts its contents to a given
 * directory using the `miniz` library. It also cleans up the zip file after extraction.
 *
 * @param zipPath Path to the zip file to be extracted.
 * @param extractDir Directory where the zip contents will be extracted.
 */
void unzipFile(const std::string& zipPath, const std::string& extractDir) {
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));

    // Open the zip file
    if (!mz_zip_reader_init_file(&zip, zipPath.c_str(), 0)) {
        printErrorInfo("Error opening zip file: " + zipPath);
        return;
    }

    // Get the number of files in the zip archive
    int num_files = mz_zip_reader_get_num_files(&zip);
    for (int i = 0; i < num_files; ++i) {
        // Get filename of the file in the zip archive
        char filename[FILENAME_BUFFER_SIZE];
        if (!mz_zip_reader_get_filename(&zip, i, filename, sizeof(filename))) {
            printErrorInfo("Error getting filename from zip: " + zipPath);
            continue;
        }

        fs::path filePath = fs::path(extractDir) / filename;

        // Check if it's a directory or file
        // Create directories first if necessary
        if (filename[strlen(filename) - 1] == '\\' || filename[strlen(filename) - 1] == '/') {
            // It's a directory, create it if it doesn't exist
            if (!fs::create_directories(filePath)) {
                printErrorInfo("Error creating directory: " + filePath.string());
            } else {
                DEBUG_LOG("Directory created: " + filePath.string());
            }
        } else {
            // It's a file, extract it
            if (!mz_zip_reader_extract_to_file(&zip, i, filePath.string().c_str(), 0)) {
                printErrorInfo("Error extracting file: " + filePath.string());
                mz_zip_reader_end(&zip);
                return;
            } else {
                DEBUG_LOG("Extracted: " + filePath.string());
            }
        }
    }

    mz_zip_reader_end(&zip);

    if (num_files == 0) {
        printErrorInfo("No files found in the zip archive: " + zipPath);
    } else {
        DEBUG_LOG("Unzip operation completed for: " + zipPath);
    }

    // Remove the zip file after extraction
    try {
        if (fs::exists(zipPath)) {
            fs::remove(zipPath);
            DEBUG_LOG("Zip file removed: " + zipPath);
        }
    } catch (const fs::filesystem_error& e) {
        printErrorInfo("Error removing zip file: " + zipPath + " - " + e.what());
    }
}

/**
 * @brief Executes a program using in-memory data.
 *
 * This placeholder function simulates the execution of an application loaded
 * entirely into memory. It demonstrates the intended usage for in-memory execution.
 * It is not implemented!
 *
 * @param exeData Executable data in memory.
 * @param appData Application data in memory.
 * @param runtimeData Runtime data in memory.
 * @return True if execution succeeds, false otherwise.
 */
bool executeFromMemory(const std::vector<char>& exeData, const std::vector<char>& appData, const std::vector<char>& runtimeData) {
    // Placeholder function
    return true;
}

/**
 * @brief Retrieves the executable name from resources.
 *
 * This function uses Windows resource management to load a string resource
 * that contains the name of the executable to be launched.
 *
 * @return The name of the executable as a string.
 */
std::string getExecutable() {
    // Get the current module handle
    HMODULE hModule = GetModuleHandle(NULL);
    char buffer[MAX_PATH];

    // Load the resource string
    LoadStringA(hModule, IDS_EXECUTABLE, buffer, MAX_PATH);

    return std::string(buffer);
}

/**
 * @brief Main entry point of the application.
 *
 * The `main` function handles the initialization, resource extraction, and
 * process execution. It also manages fallback behavior for in-memory execution.
 *
 * @return 0 if successful, 1 otherwise.
 */
int main() {

    if (IN_MEMORY_EXECUTION) {
        // In-memory extraction
        std::vector<char> appData;
        extractResource(IDR_APP_CONTENTS, appData, "");

        std::vector<char> runtimeData;
        extractResource(IDR_RUNTIME_CONTENTS, runtimeData, "");

        std::vector<char> exeData;
        extractResource(IDR_APP_EXECUTABLE, exeData, "");

        // Execute extracted executable from memory
        if (!executeFromMemory(exeData, appData, runtimeData)) {
            printErrorInfo("Execution failed.");
            return 1;
        }
    } else {
        // get jpackage executable
        std::string exeFile = getExecutable();

    	// Create the directory for the extraction ->
        // Remove extension from the executable name (assuming .exe extension)
        std::string runDir = exeFile.substr(0, exeFile.find_last_of("."));
        // Replace invalid characters in the directory name with underscores
		for (auto& ch : runDir) {
			if (!std::isalnum(ch) && ch != '-' && ch != '_') {
				ch = '_';
			}
		}

        // Create the directory if it doesn't exist (for disk extraction)
        if (!fs::exists(runDir)) {
            try {
                if (!fs::create_directory(runDir)) {
                    throw std::runtime_error("Failed to create directory: " + runDir);
                }
            } catch (const std::exception& e) {
                printErrorInfo(e.what());
                return 1;  // Exit the program if directory creation fails
            }
            DEBUG_LOG("Run directory created: " + runDir);
        }

        // Empty buffer for file extraction
        std::vector<char> emptyBuf;

        // Extract resources and unzip files here...
        extractResource(IDR_APP_CONTENTS, emptyBuf, runDir + "\\app.zip");
        extractResource(IDR_RUNTIME_CONTENTS, emptyBuf, runDir + "\\runtime.zip");
        extractResource(IDR_APP_EXECUTABLE, emptyBuf, runDir + "\\" + exeFile);

        unzipFile(runDir + "\\app.zip", runDir);
        unzipFile(runDir + "\\runtime.zip", runDir);

        deleteFilesAndDirectories(runDir + "\\app.zip", "");
        deleteFilesAndDirectories(runDir + "\\runtime.zip", "");

        STARTUPINFO si = {0};
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi = {0};

        // Launch the executable
        std::string fullPath = runDir + "\\" + exeFile;
        // Attempt to launch the process
        if (CreateProcess(
                fullPath.c_str(),    // Path to the executable
                NULL,                // No command-line arguments
                NULL,                // Default security attributes
                NULL,                // Default thread security attributes
                FALSE,               // No inheritance of handles
                CREATE_NO_WINDOW,    // Ensures no window is created
                NULL,                // Use the parent's environment
                runDir.c_str(),      // Working directory
                &si,                 // Startup information
                &pi)) {              // Process information
            DEBUG_LOG("Process launched successfully: " + fullPath);
            WaitForSingleObject(pi.hProcess, INFINITE);

            // Close process and thread handles
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        } else {
            printErrorInfo("Failed to launch " + exeFile + ". Error Code: " + std::to_string(GetLastError()));
        }

        // Cleanup: Delete temporary directory
        DEBUG_LOG("Deleting temporary directory...");

        // Delete the "run" directory
        deleteFilesAndDirectories("", runDir);

        // Log completed cleanup
        DEBUG_LOG("Cleanup completed.");
    }

    return 0;
}
