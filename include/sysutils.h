#ifndef OX_SYSUTILS_H
#define OX_SYSUTILS_H

namespace Ox
{
    inline bool file_exists(const std::string& filename)
    {
        DWORD attr = ::GetFileAttributes(filename.c_str());
        return attr != INVALID_FILE_ATTRIBUTES &&
               (attr & FILE_ATTRIBUTE_DIRECTORY) == 0;
    }

    inline bool dir_exists(const std::string& dir)
    {
        DWORD attr = ::GetFileAttributes(dir.c_str());
        return attr != INVALID_FILE_ATTRIBUTES &&
               (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }
}

#endif
