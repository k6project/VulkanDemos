#pragma once

namespace VK
{
    class Device;
}

class VKClearScreen
{

public:

    void Init(VK::Device &devices);

    void Render(float dt);

    void Shutdown();

private:

    struct ImplDetails;

    ImplDetails *Impl = nullptr;

    VK::Device *Device = nullptr;

};
