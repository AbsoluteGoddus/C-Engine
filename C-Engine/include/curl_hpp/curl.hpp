//
// Created by absol on 10/13/2024.
//

#ifndef C_ENGINE_CURL_HPP
#define C_ENGINE_CURL_HPP

#include <curl/curl.h>
#include <miniz/miniz.h>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

int LSX_LIBCURL_INIT_ = 0; // LibCurl is not initialized & is not in the process of being initialized.

size_t LUF_write_data(void* ptr, size_t size, size_t nmemb, std::ofstream* stream) {
    stream->write(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

void curl_init() {
    LSX_LIBCURL_INIT_ = 1; // LibCurl is not initialized, but is in the process of being initialized.
    curl_global_init(CURL_GLOBAL_ALL);
    LSX_LIBCURL_INIT_ = 2; // LibCurl is initialized.
}

void curl_cleanup() {
    curl_global_cleanup();
    LSX_LIBCURL_INIT_ = 0; // LibCurl is not initialized & is not in the process of being initialized.
}

void download(const std::string& output, const std::string& url) {
    if (LSX_LIBCURL_INIT_ != 2) {
        throw std::runtime_error("LibCurl is not initialized. Use curl_init() to initialize it.");
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize curl.");
    }

    std::ofstream file(output, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing.");
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, LUF_write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
    file.close();
}

// Downloads a .zip file, extracts it & runs an installer.exe file if it exists.
void install(const std::string& url, const std::string& output) {
    download(output, url);
}

bool unzip(const std::string& zipFilePath, const std::string& outputDir) {
    mz_zip_archive zipArchive;
    memset(&zipArchive, 0, sizeof(zipArchive));

    // Open the ZIP file
    if (!mz_zip_reader_init_file(&zipArchive, zipFilePath.c_str(), 0)) {
        std::cerr << "Failed to open ZIP file: " << zipFilePath << std::endl;
        return false;
    }

    // Create output directory if it doesn't exist
    if (!fs::exists(outputDir)) {
        fs::create_directories(outputDir);
    }

    // Iterate through the ZIP file entries
    int numFiles = mz_zip_reader_get_num_files(&zipArchive);
    for (int i = 0; i < numFiles; i++) {
        mz_zip_archive_file_stat fileStat;
        if (!mz_zip_reader_file_stat(&zipArchive, i, &fileStat)) {
            std::cerr << "Failed to get file info for file index " << i << std::endl;
            mz_zip_reader_end(&zipArchive);
            return false;
        }

        // Construct full output path using std::filesystem
        std::string outputPath = outputDir + "/" + fileStat.m_filename;
        fs::path outputFilePath(outputPath);

        // Check if it's a directory or file
        if (mz_zip_reader_is_file_a_directory(&zipArchive, i)) {
            fs::create_directories(outputFilePath); // Create directory
        } else {
            // Ensure parent directory exists before writing the file
            fs::create_directories(outputFilePath.parent_path());

            // Extract the file
            std::ofstream outFile(outputFilePath, std::ios::binary);
            if (!outFile.is_open()) {
                std::cerr << "Failed to open output file: " << outputPath << std::endl;
                mz_zip_reader_end(&zipArchive);
                return false;
            }

            size_t uncompSize = mz_zip_reader_extract_to_mem(&zipArchive, i, nullptr, 0, 0);
            std::vector<char> buffer(uncompSize);
            if (mz_zip_reader_extract_to_mem(&zipArchive, i, buffer.data(), buffer.size(), 0)) {
                outFile.write(buffer.data(), buffer.size());
            } else {
                std::cerr << "Failed to extract file: " << fileStat.m_filename << std::endl;
            }

            outFile.close();
        }
    }

    mz_zip_reader_end(&zipArchive); // Close the ZIP archive
    return true;
}

bool zip(const std::string& dirPath, const std::string& zipFilePath) {
    mz_zip_archive zipArchive;
    memset(&zipArchive, 0, sizeof(zipArchive));

    // Create the ZIP file
    if (!mz_zip_writer_init_file(&zipArchive, zipFilePath.c_str(), 0)) {
        std::cerr << "Failed to create ZIP file: " << zipFilePath << std::endl;
        return false;
    }

    // Recursively iterate through the directory
    for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
        if (entry.is_directory()) {
            // Skip directories as miniz handles only files
            continue;
        }

        std::string filePath = entry.path().string();
        std::string relativePath = fs::relative(entry.path(), dirPath).string(); // Get relative path for ZIP

        // Read the file content into a buffer
        std::ifstream inFile(filePath, std::ios::binary | std::ios::ate);
        if (!inFile.is_open()) {
            std::cerr << "Failed to open file for reading: " << filePath << std::endl;
            mz_zip_writer_end(&zipArchive);
            return false;
        }

        std::streamsize fileSize = inFile.tellg();
        inFile.seekg(0, std::ios::beg);
        std::vector<char> buffer(fileSize);
        if (!inFile.read(buffer.data(), fileSize)) {
            std::cerr << "Failed to read file: " << filePath << std::endl;
            mz_zip_writer_end(&zipArchive);
            return false;
        }

        // Add the file to the ZIP archive
        if (!mz_zip_writer_add_mem(&zipArchive, relativePath.c_str(), buffer.data(), buffer.size(), MZ_DEFAULT_COMPRESSION)) {
            std::cerr << "Failed to add file to ZIP: " << relativePath << std::endl;
            mz_zip_writer_end(&zipArchive);
            return false;
        }

        inFile.close();
    }

    // Finalize and close the ZIP archive
    if (!mz_zip_writer_finalize_archive(&zipArchive)) {
        std::cerr << "Failed to finalize ZIP archive." << std::endl;
        mz_zip_writer_end(&zipArchive);
        return false;
    }

    mz_zip_writer_end(&zipArchive); // Clean up
    return true;
}


#endif //C_ENGINE_CURL_HPP
