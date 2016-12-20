#pragma once

class File
{

public:

    File(const std::string &name);

    bool ReadWholeFile();

    const char *GetData() const;

    std::uint32_t GetDataSize() const;

private:

    std::string Name;

    std::vector<char> Buffer;

};
