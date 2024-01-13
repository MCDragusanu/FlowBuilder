#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <filesystem>
class FileSystem;

class InvalidHandle : public std::exception {
public:
    InvalidHandle(const char* reason) :exception(reason) {};
};

enum FileExtension {
    TXT,
    CSV,
    FLOW
};

class FileHandle {
    friend class FileSystem;

    std::string m_fileName;

    FileExtension m_extensionType;

    virtual void writeToFile(const std::string&) = 0;
    virtual std::unique_ptr<std::string> getFileContent() const noexcept = 0;
    virtual void deleteFile() = 0;
    virtual void clearFileContent() = 0;

public:
    virtual ~FileHandle() = default;
    FileHandle(const char* name, FileExtension type)
        : m_fileName(name), m_extensionType(type) {}

    virtual bool isGood() = 0;

    const char* getFileName() const noexcept {
        return m_fileName.c_str();
    }



    std::string getRelativePath() const noexcept {
        std::stringstream ss;
        ss << m_fileName << getExtension(m_extensionType);

        return ss.str();
    }



    FileExtension getExtensionType() const noexcept {
        return m_extensionType;
    }
    static const char* getExtension(FileExtension type) {
        if (type == TXT) {
            return ".txt";
        }
        else if (type == CSV) {
            return ".csv";
        }
        else {
            return  ".flw";
        }
    }
};

class InMemoryFile : public FileHandle {
    std::stringstream m_buffer;

    void writeToFile(const std::string& buffer) override {


        m_buffer << buffer;
    }

    std::unique_ptr<std::string> getFileContent() const noexcept override {
        return std::make_unique<std::string>(m_buffer.str());
    }

    void deleteFile() override {
        m_buffer.clear();
    }
    void clearFileContent() override {
        m_buffer.clear();
    }

public:
    InMemoryFile(const char* fileName, FileExtension type, const char* buffer)
        : FileHandle(fileName, type), m_buffer(buffer) {}

    InMemoryFile(const char* fileName, FileExtension type)
        : FileHandle(fileName, type), m_buffer() {}

    bool isGood() override {
        return !m_buffer.bad();
    }
};

class FileSystem {

    static FileSystem* m_instance;
    std::vector<std::shared_ptr<FileHandle>> m_resources;
    std::string m_directory = std::string("C:\\tmp");

    std::shared_ptr<FileHandle> createNewFileHandle(const char* fileName, FileExtension extension) {
        auto newHandle = std::make_shared<InMemoryFile>(fileName, extension);

        return newHandle;
    }

    FileSystem() : m_resources() { }


    std::string sanitizeFileName(const std::string& fileName) {
        size_t dotPos = fileName.find_last_of('.');

        if (dotPos != std::string::npos) {
            // If a dot is found, remove the extension
            return fileName.substr(0, dotPos);
        }
        else {
            // If no dot is found, return the original filename
            return fileName;
        }
    }
public:

    std::shared_ptr<FileHandle> getFileHandle(const char* fileName, FileExtension extension) {
        try {
            auto where = std::find_if(m_resources.begin(), m_resources.end(), [fileName, extension](const auto& handle) {
                return handle->getFileName() == fileName && handle->getExtensionType() == extension;
                });

            if (where == m_resources.end()) {
                auto newHandle = createNewFileHandle(fileName, extension);
                m_resources.emplace_back(newHandle);
                return newHandle;
            }

            return *where;
        }
        catch (const std::exception& e) {
            std::cerr << e.what();
            return nullptr;
        }
    }

    bool saveFile(const FileHandle* handle) {
        // Check validity of handle
        if (handle == nullptr) {
            std::cerr << "File Handle is null";
            return false;
        }

        // Create and open new file stream 
        std::cout << "Current directory =" << m_directory;
        std::string filePath(m_directory + "\\" + handle->getRelativePath());

        std::cout << "\nCurrent Path = " << filePath << "\n";
        std::ofstream file(filePath);

        // Check if creation is successful
        if (!file.good() || !file.is_open()) {
            std::cerr << "Failed to open file stream for file " << m_directory << "\\" << handle->getRelativePath();
            return false;
        }

        auto content = handle->getFileContent();
        if (content == nullptr) {
            std::cerr << "File Content is empty";
            return true;
        }

        file << *content;  // Dereference the std::unique_ptr<std::string>

        return true;
    }


    bool writeToFile(FileHandle* handle, const std::string& buffer) {

        if (handle == nullptr) {
            std::cerr << "File Handle is null";
            return false;
        }

        if (buffer.empty()) {
            std::cerr << "Buffer is empty";
            return false;
        }

        if (!handle->isGood()) {
            std::cerr << "Cannot perform write action";
        }

        handle->writeToFile(buffer);
        return true;

    }

    static FileSystem* getInstance() {

        if (m_instance == nullptr) {
            m_instance = new FileSystem();
        }

        return m_instance;
    }

    bool fileAlreadyExistent(const char* fileName, FileExtension extension) {
        if (fileName == nullptr) {
            std::cerr << "FileName provided is null\n";
        }

        auto handle = std::find_if(m_resources.begin(), m_resources.end(), [fileName, extension](const std::shared_ptr<FileHandle>& handle) {
            return handle->getFileName() == std::string(fileName) && handle->getExtensionType() == extension;
            });

        return handle != m_resources.end();
    }
    bool clearFile(FileHandle* handle) {
        if (handle == nullptr) {
            std::cerr << "File Handle is null";
            return false;
        }
        handle->clearFileContent();
        return true;
    }

    std::string readFromInputFile(FileHandle* handle) {
        if (handle == nullptr) {
            std::cerr << "Handle provided is null\n";
            return std::string();
        }

        // Construct the full path to the file
        std::string path = m_directory + "\\" + sanitizeFileName(handle->getFileName()) +
            (handle->getExtensionType() == TXT ? std::string(".txt") : std::string(".csv"));

        std::ifstream file(path);

        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << path << std::endl;
            return std::string();
        }

        // Read the content from the file
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        // Close the file stream
        file.close();

        return content;
    }
};

FileSystem* FileSystem::m_instance = nullptr;