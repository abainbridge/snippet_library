#ifndef INCLUDED_MAPPED_FILE_H
#define INCLUDED_MAPPED_FILE_H


class MappedFile
{
private:
    void *m_file;

public:
    char const *m_data;
    unsigned    m_size;

    MappedFile();
    ~MappedFile();

    bool OpenFile(char const *fullPath);
    void CloseFile();
};


#endif
