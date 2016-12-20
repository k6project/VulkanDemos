#include <Globals.h>

#include "NativeFile.h"

#include <cstdio>

File::File(const std::string &name)
    : Name(name)
{
}

bool File::ReadWholeFile()
{
    FILE *fp = fopen(Name.c_str(), "rb");
    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        long count = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        Buffer.resize(count, 0);
        fread(Buffer.data(), sizeof(char), count, fp);
        fclose(fp);
        return true;
    }
    return false;
}

const char * File::GetData() const
{
    return Buffer.data();
}

std::uint32_t File::GetDataSize() const
{
    return static_cast<std::uint32_t>(Buffer.size());
}
