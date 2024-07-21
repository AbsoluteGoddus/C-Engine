#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <curl/curl.h>
#include <minizip/unzip.h>

namespace fs = std::filesystem;

void createDirectoryIfNotExists(const std::string& path) {
    fs::path dirPath(path);
    if (!fs::exists(dirPath)) {
        fs::create_directories(dirPath);
    }
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
    // Create directories if they do not exist
    fs::path destPath(destination);
    if (!fs::exists(destPath.parent_path())) {
        if (!fs::create_directories(destPath.parent_path())) {
            std::cerr << "Unable to create directories: " << destPath.parent_path() << std::endl;
            return false;
        }
    }

    CURL* curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
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

int main(int argc, char** argv) {
    std::string src = "https://github.com/AbsoluteGoddus/C-Engine/archive/refs/tags/";
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <version>" << std::endl;
        return 1;
    }

    for (auto &p : fs::directory_iterator("C:/C-Engine/Build/")) {
        if (fs::is_directory(p) && p.path().filename() == argv[1]) {
            std::cout << "Version [" << argv[1] << "] is already installed. Build base to it? [Y/N]" << std::endl;
            std::string tmp;
            std::cin >> tmp;
            if (tmp == "Y") {
                std::cout << "Clearing Data..." << std::endl;
                for (auto &e : fs::directory_iterator("C:/C-Engine/Build/Base/")) {
                    fs::remove(e);
                }
                unzip("C:/C-Engine/Build/Base", "C:/C-Engine/Build/" + std::string(argv[1]) + "/src/data.zip");
                std::cout << "Unzipping data..." << std::endl;
                std::cout << "Build Successful" << std::endl;
                return 0;
            } else {
                std::cout << "Reinstall Version [" << argv[1] << "]? [Y/N]" << std::endl;
                std::cin >> tmp;
                if (tmp == "N") {
                    return 0;
                }
            }
        }
    }

    src += argv[1];
    src += ".zip";
    std::string destination = "C:/C-Engine/Build/" + std::string(argv[1]) + "/src/data.zip";
    if (installFile(destination, src)) {
        std::cout << "Downloaded file from " << src << " to " << destination << std::endl;
    } else {
        std::cerr << "Failed to download file from " << src << std::endl;
        return -1;
    }

    std::cout << "Unzipping data..." << std::endl;
    unzip("C:/C-Engine/Build/Base", "C:/C-Engine/Build/" + std::string(argv[1]) + "/src/data.zip");
    std::cout << "Build Successful" << std::endl;

    return 0;
}
