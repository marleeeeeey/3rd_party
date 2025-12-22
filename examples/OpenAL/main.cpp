#include <alc.h>
#include <iostream>

int main() {
    ALCdevice* device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to open OpenAL device" << std::endl;
        return 1;
    }

    std::cout << "OpenAL device opened successfully!" << std::endl;

    // Check device name
    const ALCchar* name = alcGetString(device, ALC_DEVICE_SPECIFIER);
    std::cout << "Using device: " << name << std::endl;

    alcCloseDevice(device);
    return 0;
}