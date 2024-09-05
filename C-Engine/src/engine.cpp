#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <windows.h>
#include <filesystem>
#include <curl/curl.h>
#include <minizip/unzip.h>

#include "../include/CFG.hpp"

namespace fs = std::filesystem;

using _av = std::vector<std::any>;

void createDirectoryIfNotExists(const std::string& path) {
    fs::path dirPath(path);
    if (!fs::exists(dirPath)) {
        fs::create_directories(dirPath);
    }
}

void createFileIfNotExists(const std::string& path) {
    namespace fs = std::filesystem;

    // Check if the file exists
    if(fs::exists(path)) {
        return;
    }

    // Ensure parent directories exist
    fs::path filePath(path);
    if(!fs::create_directories(filePath.parent_path())) {
        throw std::runtime_error("Unable to create file: " + path);
    }

    // Attempt to create the file
    std::ofstream file(path, std::ios::out);
    if(file.is_open()) {
        file.close(); // Close the file after creation
        return;
    }

    throw std::runtime_error("Unable to create file: " + path);
}

void writeFile(const std::string& filePath, const std::string& data) {
    std::ofstream outFile(filePath, std::ios::binary);
    if (outFile) {
        outFile.write(data.data(), data.size());
    } else {
        throw std::runtime_error("Failed to open file for writing: " + filePath);
    }
}

void unzip(const std::string& outputDir, const std::string& path) {
    unzFile zipFile = unzOpen(path.c_str());
    if (zipFile == nullptr) {
        throw std::runtime_error("Failed to open ZIP file: " + path);
    }

    int result = unzGoToFirstFile(zipFile);
    if (result != UNZ_OK) {
        unzClose(zipFile);
        throw std::runtime_error("Failed to go to first file in ZIP: " + path);
    }

    do {
        char filename[256];
        unz_file_info fileInfo;

        result = unzGetCurrentFileInfo(zipFile, &fileInfo, filename, sizeof(filename), nullptr, 0, nullptr, 0);
        if (result != UNZ_OK) {
            unzClose(zipFile);
            throw std::runtime_error("Failed to get file info: " + path);
        }

        std::string fullPath = outputDir + "/" + filename;

        if (filename[strlen(filename) - 1] == '/') {
            // Directory
            createDirectoryIfNotExists(fullPath);
        } else {
            // File
            createDirectoryIfNotExists(fs::path(fullPath).parent_path().string());

            result = unzOpenCurrentFile(zipFile);
            if (result != UNZ_OK) {
                unzClose(zipFile);
                throw std::runtime_error("Failed to open file in ZIP: " + path);
            }

            std::string fileData(fileInfo.uncompressed_size, '\0');
            result = unzReadCurrentFile(zipFile, fileData.data(), fileInfo.uncompressed_size);
            if (result < 0) {
                unzCloseCurrentFile(zipFile);
                unzClose(zipFile);
                throw std::runtime_error("Failed to read file in ZIP: " + path);
            }
            unzCloseCurrentFile(zipFile);

            writeFile(fullPath, fileData);
        }

        result = unzGoToNextFile(zipFile);
    } while (result == UNZ_OK);

    unzClose(zipFile);
}


// Function to write data to a file
size_t WriteCallback(void* ptr, size_t size, size_t nmemb, void* stream) {
    std::ofstream* out = static_cast<std::ofstream*>(stream);
    out->write(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

// Function to download a file from a URL
bool installFile(const std::string& destination, const std::string& URL) {

    CURL* curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        createFileIfNotExists(destination);
        std::ofstream outFile(destination, std::ios::binary);
        if (!outFile.is_open()) {
            std::cerr << "Unable to open file for writing: " << destination << std::endl;
            return false;
        }

        curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outFile);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0"); // Adding User-Agent
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "Curl download error: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
        outFile.close();
        return (res == CURLE_OK);
    } else {
        std::cerr << "Failed to initialize CURL." << std::endl;
    }
    return false;
}

void UpdatePathVariable(const std::string& newValue) {
    // Open the registry key where the user PATH is stored
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Environment", 0, KEY_READ | KEY_WRITE, &hKey) != ERROR_SUCCESS) {
        std::cerr << "Failed to open registry key.\n";
        exit(4);
    }

    // Get the current value of PATH
    char currentPath[32767];
    DWORD bufferSize = sizeof(currentPath);
    if (RegQueryValueExA(hKey, "PATH", nullptr, nullptr, reinterpret_cast<LPBYTE>(currentPath), &bufferSize) != ERROR_SUCCESS) {
        std::cerr << "Failed to read current PATH value.\n";
        RegCloseKey(hKey);
        exit(5);
    }

    std::string newPath = currentPath;
    newPath += ";";
    newPath += newValue;

    // Update the PATH variable in the registry
    if (RegSetValueExA(hKey, "PATH", 0, REG_EXPAND_SZ, reinterpret_cast<const BYTE*>(newPath.c_str()), newPath.size() + 1) != ERROR_SUCCESS) {
        std::cerr << "Failed to set new PATH value.\n";
        RegCloseKey(hKey);
        exit(6);
    }

    // Notify the system about the environment variable change
    SendMessageTimeoutA(HWND_BROADCAST, WM_SETTINGCHANGE, 0, reinterpret_cast<LPARAM>("Environment"), SMTO_ABORTIFHUNG, 5000, nullptr);

    std::cout << "PATH updated successfully." << std::endl;
    RegCloseKey(hKey);
}

std::string getFile(fs::path p) {

    std::ifstream file(p);

    if (!file.is_open()) {
        std::cerr << "File: " << p.filename() << ", Not found at: " << p.string() << "!";
        exit(4);
    }

    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::vector<fs::path> getSubdirectories(const fs::path& directory) {
    std::vector<fs::path> subdirs;

    // Iterate over the directory entries
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (fs::is_directory(entry.path())) {
            subdirs.push_back(entry.path());
        }
    }

    return subdirs;
}

std::vector<std::any> av(std::any v) {
    return std::any_cast<std::vector<std::any>>(v);
}

void installExtension(const std::string& url) {
    std::cout << "Installing extension at: " << url << "...\n";
    std::cout << "Downloading...\n";
    installFile("C:/C-Engine/Build/Extensions/tmp/data.zip", url);
    std::cout << "Success\n";
    std::cout << "Unpacking...\n";
    unzip("C:/C-Engine/Build/Extensions/tmp/", "C:/C-Engine/Build/Extensions/tmp/data.zip");
    std::cout << "Success\n";
    std::cout << "Copying...\n";
    Node root;
    for (auto dir : getSubdirectories("C:/C-Engine/Build/Extensions/tmp/")) {
        std::cout << "\tSearching for \"extension.cfg\" in: " << dir.string() << "\n";
        engine::cfg_parse(getFile(dir.string() + "/extension.cfg"), root);
        std::cout << "\tSuccess\n";
        auto extName = std::any_cast<std::string>(av(root["Info"]("Name"))[0]);
        fs::create_directories("C:/C-Engine/Build/Extensions/" + extName);
        const auto copyOptions = fs::copy_options::overwrite_existing
                                 | fs::copy_options::recursive;
        std::cout << "\tPreparing...\n";
        fs::remove_all("C:/C-Engine/Build/Extensions/" + extName);
        std::cout << "\tSuccess\n";
        fs::copy(dir.string(), "C:/C-Engine/Build/Extensions/" + extName, copyOptions);
        std::cout << "\tCopied " << extName << " at: " << dir.string() << " to: " << "C:/C-Engine/Build/Extensions/" + extName << "\n";
    }
    std::cout << "Success\n";
    std::cout << "Cleanup...\n";
    fs::remove_all("C:/C-Engine/Build/Extensions/tmp");
    std::cout << "Success\n";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "No arguments provided\nCorrect usage:\nengine <args>\n";
        exit(2);
    }

    std::vector<std::string> args(argv, argv + argc);

    if (args[1] == "-setPath") {
        if (argc < 3) {  // Checking if the version argument is provided
            std::cerr << "No version provided.\nCorrect usage:\nengine -setPath <version>\n";
            exit(2);
        }

        // Building the version-specific path
        std::string versionPath = "C:/C-Engine/Build/Base/C-Engine-" + args[2];
        if (!fs::exists(versionPath)) {
            std::cerr << "No such version: " + args[2] + "\n";
            exit(3);
        }

        // Add the directory (not the executable) to the PATH
        std::string binPath = versionPath + R"(\C-Engine\bin)";
        UpdatePathVariable(binPath);

    } else if (args[1] == "-hello") {
        std::cout << "Hello, World!\n";
    } else if (args[1] == "-install") {
        if (argc < 3) {
            std::cerr << "No version provided.\nCorrect usage:\nengine -install <URL>\n";
            exit(2);
        }

        installExtension(args[2]);
    } else if (args[1] == "-build") {
        Node root;
        engine::cfg_parse(getFile(fs::current_path().string() + "/application.cfg"), root);
        if (root.size() <= 1) {
            std::cerr << "No application.cfg found in: " << fs::current_path().string() << "\n";
            exit(4);
        }
        std::cout << "Building...\n";
        std::cout << "Success\n";
    } else if (args[1] == "-new") {
        Node root;
        Node application;
        Node _info("Info");

        std::string title;

        std::cout << "Title: ";
        std::cin >> title;
        std::cout << std::endl;
        std::cout << "Searching for user.cfg...\n";
        engine::cfg_parse(getFile("C:/C-Engine/user.cfg"), root);
        std::cout << "Success\n";
        std::cout << "Pushing Author as: " << std::any_cast<std::string>(root["Ingo"]("Tag"))[0] << "...\n";
        _info.pushValue("Author", _av{root["Info"]("Tag")});
        std::cout << "Success\n";
        _info.pushValue("Title", _av{title});
        _info.pushValue("Version", _av{0, 0, 1, 0});
        application << _info;
        std::cout << "Generating {application.cfg} at: " << fs::current_path().string() << "/application.cfg...\n";
        std::ofstream file("application.cfg", std::ios::app);
        file << engine::cfg_serialize(application, 0);
        std::cout << "Success\n";
    } else if (args[1] == "-setup") {
        std::string tag;
        std::cout << "Please enter Tag: ";
        std::cin >> tag;
        std::cout << "\n";
        std::fstream file("C:/C-Engine/user.cfg", std::ios::out | std::ios::in | std::ios::app);
        file.clear();
        Node Info("Info");
        Info.pushValue("Author", _av{tag});
    }

    return EXIT_SUCCESS;
}
