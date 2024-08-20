//
// Created by absol on 7/22/2024.
//

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <minizip/unzip.h>
#include <curl/curl.h>

#include "../include/CFG.hpp"

namespace fs = std::filesystem;

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
    createDirectoryIfNotExists(filePath.parent_path().string());

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

using av = std::vector<std::any>;

std::vector<std::any> ac(std::any a) {
    return std::any_cast<std::vector<std::any>>(a);
}

Node getExtensionInfo(const std::string& name) {
    std::ifstream file("C:/C-Engine/Extensions/" + name + "/extension.cfg");
    if (file.is_open()) {
        Node root;
        std::stringstream content;
        content << file.rdbuf();
        engine::cfg_parse(content.str(), root);
        return root;
    }
    Node root;
    root.pushValue("Error", "ERR_NOT_FOUND");

    return {};
}

std::string operator"" _s(const char *s) {
    return {s};
}

std::string operator"" _s(const char *s, unsigned long long) {
    return {s};
}


Node std_application_cfg(const std::string& name, const std::string& description, const std::string& author) {
    Node root;
    Node required("required");
    Node info("info");

    info.pushValue("Application_Name", std::any(av{name}));
    info.pushValue("Version", std::any(av{0, 0, 0, 1}));
    info.pushValue("Description", std::any(av{description}));
    info.pushValue("Author", std::any(av{author}));

    root << info;
    root << required;

    return root;
}

class Command {
public:
    std::string command;
    std::string args;

    explicit Command(const std::string& str) {
        command = str.substr(0, str.find(' '));
        args = str.substr(str.find(' ') + 1, str.size() - 1);
    }
};

/**
 * Creates a file if it doesn't already exist
 *
 * @param path The path to the file
 * @param content If the file doesn't exist it creates it and puts content into it
 */
void createFile(const std::string& path, const std::string& content) {
    fs::path p(path);

    // Check if the path exists and is not a directory
    if (!fs::exists(p)) {
        // Create the file
        std::ofstream fileStream(p);

        // Write the content to the file
        if (fileStream.is_open()) {
            fileStream << content;
            fileStream.close();
        } else {
            throw std::runtime_error("Failed to create file");
        }
    } else if (fs::is_directory(p)) {
        return;
    }
    // If the file already exists, do nothing
}

std::string getFile(const fs::path& p) {
    std::ifstream file(p);
    std::stringstream tmp;
    tmp << file.rdbuf();
    return tmp.str();
}

int main(int argc, char** argv) {
    if (argc < 2) {
        throw std::invalid_argument("Wrong usage. argc should be grater than two");
    }
    std::string cmd;
    for (int i = 1; i < argc; i++) {
        cmd += (i != 1) ? " " + std::string(argv[i]) : std::string(argv[i]);
    }
    Command c(cmd);

    // Handle the arguments
    if (c.command == "-new") {
        std::cout << "Creating new Application..." << std::endl;
        std::string name;
        std::string author;
        std::string description;
        std::cout << "Please enter Application Name" << std::endl;
        std::cin >> name;
        std::cout << "Please enter description:\n";
        std::cin >> description;
        std::cout << "Please enter author:\n";
        std::cin >> author;
        createFile("application.cfg", engine::cfg_serialize(std_application_cfg(name, description, author), 0));
    } else if (c.command == "-build") {
        std::ifstream config("application.cfg");
        std::stringstream ss;
        ss << config.rdbuf();
        Node root;
        engine::cfg_parse(ss.str(), root);
        av build_files = std::any_cast<av>(root["build"]("filesystem"));
        av required_extensions = std::any_cast<av>(root["required"]);

        for (auto e : required_extensions) {
            Node _root;
            root = getExtensionInfo(std::any_cast<std::string>(e));
            if (root.contains("Error")) {
                throw std::runtime_error("Error, while getting required extensions: " + std::any_cast<std::string>(ac(root("Error"))[0]));
            }


        }


    } else if (c.command == "-get") { // Gets all the required extensions
        Node root;
        engine::cfg_parse(getFile("application.cfg"), root);
        for (auto e : root["root"]["required"].getValues()) {
            std::cout << "[GET] | Installing: " << e.first << "...\n";
            createDirectoryIfNotExists("C-Engine/Extensions/" + e.first + "/");
            if (!installFile("C-Engine/Extensions/" + e.first + "/data_unpacked.zip", std::any_cast<std::string>(std::any_cast<av>(e.second)[0]))) {
                throw std::runtime_error("[GET / ERROR] | Error while installing: " + e.first);
            }
            std::cout << "[GET] | Unpacking...\n";
            unzip("C-Engine/Extensions/" + e.first + "/", "C-Engine/Extensions/" + e.first + "/data_unpacked.zip");
            std::cout << "[GET] | Success\n";
        }
    }
    return 0;
}