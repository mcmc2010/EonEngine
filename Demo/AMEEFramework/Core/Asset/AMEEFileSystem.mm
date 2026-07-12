#include "AMEEFileSystem.hpp"
#include "../Log/AMEELog.hpp"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>

#if defined(__APPLE__)
#import <Foundation/Foundation.h>
#endif

namespace AMEE {

AMEE_SINGLETON_IMPL(FileSystem)

void FileSystem::Initialize()
{
    if (m_Initialized) return;

#if defined(__APPLE__)
    {
        // 1. Bundle Resources/Assets (production)
        NSString* resPath = [[NSBundle mainBundle] resourcePath];
        if (resPath) {
            std::string bundlePath = [resPath UTF8String];
            if (!bundlePath.empty() && bundlePath.back() != '/') {
                bundlePath += '/';
            }
            Mount("Assets", bundlePath + "Assets");
        }
    }
#endif

    {
        // 2. Working directory Assets (dev, non-sandbox)
        std::string cwd = GetCurrentDir();
        if (!cwd.empty()) {
            Mount("Assets", cwd + "Assets");
        }
    }

    // 3. AMEE_ASSETS_PATH env var (dev, sandbox workaround)
    {
        const char* env = ::getenv("AMEE_ASSETS_PATH");
        if (env && env[0] != '\0') {
            std::string envPath(env);
            if (!envPath.empty() && envPath.back() != '/') envPath += '/';
            Mount("Assets", envPath + "Assets");
        }
    }

    m_Initialized = true;
    AMEE_LOG_INFO("FileSystem", "Initialized with %zu mount point(s)", m_MountPoints.size());
}

void FileSystem::Mount(const std::string& LogicalPrefix, const std::string& PhysicalPath)
{
    if (PhysicalPath.empty()) return;

    std::string normalized = PhysicalPath;
    if (normalized.back() != '/') normalized += '/';

    for (const auto& mp : m_MountPoints) {
        if (mp.LogicalPrefix == LogicalPrefix && mp.PhysicalPath == normalized) {
            return;
        }
    }

    m_MountPoints.push_back({LogicalPrefix, normalized});
    AMEE_LOG_INFO("FileSystem", "Mount '%s' -> '%s'", LogicalPrefix.c_str(), normalized.c_str());
}

std::string FileSystem::ResolvePath(const std::string& LogicalPath) const
{
    for (auto it = m_MountPoints.rbegin(); it != m_MountPoints.rend(); ++it) {
        if (LogicalPath.compare(0, it->LogicalPrefix.size(), it->LogicalPrefix) == 0) {
            std::string relative = LogicalPath.substr(it->LogicalPrefix.size());
            if (!relative.empty() && relative.front() == '/') {
                relative = relative.substr(1);
            }
            std::string full = it->PhysicalPath + relative;

            struct stat st;
            if (::stat(full.c_str(), &st) == 0) {
                return full;
            }
        }
    }

    struct stat st;
    if (::stat(LogicalPath.c_str(), &st) == 0) {
        return LogicalPath;
    }

    return {};
}

std::string FileSystem::ReadText(const std::string& LogicalPath) const
{
    std::string physical = ResolvePath(LogicalPath);
    if (physical.empty()) {
        AMEE_LOG_ERROR("FileSystem", "File not found: %s", LogicalPath.c_str());
        return {};
    }

    std::ifstream file(physical, std::ios::in);
    if (!file.is_open()) {
        AMEE_LOG_ERROR("FileSystem", "Cannot open file: %s", physical.c_str());
        return {};
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

bool FileSystem::Exists(const std::string& LogicalPath) const
{
    return !ResolvePath(LogicalPath).empty();
}

void FileSystem::PrintMounts() const
{
    for (const auto& mp : m_MountPoints) {
        AMEE_LOG_INFO("FileSystem", "  %s -> %s", mp.LogicalPrefix.c_str(), mp.PhysicalPath.c_str());
    }
}

bool FileSystem::DirectoryExists(const std::string& Path)
{
    struct stat st;
    return ::stat(Path.c_str(), &st) == 0 && (st.st_mode & S_IFDIR);
}

std::string FileSystem::GetCurrentDir()
{
    char buf[PATH_MAX];
    if (!::getcwd(buf, sizeof(buf))) return {};
    std::string cwd(buf);
    if (!cwd.empty() && cwd.back() != '/') cwd += '/';
    return cwd;
}

std::string FileSystem::GetBundleResourcesPath()
{
#if defined(__APPLE__)
    NSString* path = [[NSBundle mainBundle] resourcePath];
    if (path) {
        std::string result = [path UTF8String];
        if (!result.empty() && result.back() != '/') result += '/';
        return result;
    }
#endif
    return {};
}

} // namespace AMEE
