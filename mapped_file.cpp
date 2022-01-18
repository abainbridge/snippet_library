#include "lib/universal_include.h"

#include "mapped_file.h"


MappedFile::MappedFile()
{
    m_file = 0;
    m_data = NULL;
    m_size = 0;
}


MappedFile::~MappedFile()
{
    CloseFile();
}


#ifdef _WIN32

bool MappedFile::OpenFile(char const *fullPath)
{
    HANDLE tmpHandle = CreateFile(fullPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (tmpHandle == INVALID_HANDLE_VALUE)
        goto err;

    m_size = GetFileSize(tmpHandle, NULL);
    if (m_size == 0)
        return true;

    m_file = CreateFileMapping(tmpHandle, NULL, PAGE_READONLY, 0, 0, NULL);
    if (!m_file)
        goto err;

    CloseHandle(tmpHandle);
    tmpHandle = NULL;

    m_data = (char*)MapViewOfFile(m_file, FILE_MAP_READ, 0, 0, 0);
    if (!m_data)
        goto err;

    return true;

err:
    if (tmpHandle)
        CloseHandle(tmpHandle);
    if (m_file)
        CloseHandle(m_file);

    m_data = NULL;
    m_file = 0;
    m_size = 0;

    return false;
}


void MappedFile::CloseFile()
{
    if (m_file)
    {
        UnmapViewOfFile(m_data);
        CloseHandle(m_file);
        m_data = NULL;
        m_file = 0;
        m_size = 0;
    }
}

#else

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> // mmap() is defined in this header
#include <fcntl.h>


bool MappedFile::OpenFile(char const *fullPath)
{
    int f = open(fullPath, O_RDONLY);
    if (f < 0)
        return false;

    struct stat statbuf;
    if (fstat(f, &statbuf) < 0)
        goto err;
    m_size = statbuf.st_size;

    m_data = (char const *)mmap(0, m_size, PROT_READ, MAP_SHARED, f, 0);
    if (m_data == MAP_FAILED)
        goto err;

    close(f);
    return true;

err:
    close(f);
    return false;
}


void MappedFile::CloseFile()
{
    munmap((void *)m_data, m_size);
}


#endif
