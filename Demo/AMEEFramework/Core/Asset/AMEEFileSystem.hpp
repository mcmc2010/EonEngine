#ifndef __AMEE_FILESYSTEM_H__
#define __AMEE_FILESYSTEM_H__
#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace AMEE {

class FileSystem {
public:
    static FileSystem& Instance();

    void Initialize();

    void Mount(const std::string& LogicalPrefix, const std::string& PhysicalPath);

    std::string ResolvePath(const std::string& LogicalPath) const;
    std::string ReadText(const std::string& LogicalPath) const;
    bool Exists(const std::string& LogicalPath) const;
    bool IsInitialized() const { return m_Initialized; }

    void PrintMounts() const;

private:
    FileSystem() = default;

    struct MountPoint {
        std::string LogicalPrefix;
        std::string PhysicalPath;
    };

    std::vector<MountPoint> m_MountPoints;
    bool m_Initialized = false;

    static bool DirectoryExists(const std::string& Path);
    static std::string GetCurrentDir();
    static std::string GetBundleResourcesPath();
};

} // namespace AMEE

#endif // __AMEE_FILESYSTEM_H__
