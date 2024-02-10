/**
 Credit to SomePineapple
 Code from: https://github.com/SomePineaple/Phantom/blob/main/src/utils/XUtils.h
 */

#ifndef xutils_hpp
#define xutils_hpp

#include <X11/extensions/XInput.h>
#include <X11/Xlib.h>
#include <X11/X.h>

namespace XUtils {
    class DeviceState {
    public:
        DeviceState();
        ~DeviceState();

        bool *buttonStates;
        bool *keyStates;
        int *valuatorStates;

        int numKeys;
        int numButtons;
        int numValuators;
    };

    extern int mouseDeviceIndex;
    extern int keyboardDeviceIndex;
    extern unsigned long mouseDeviceID;
    extern unsigned long keyboardDeviceID;
    extern bool isDeviceShit;

    void renderMouseSelector();
    void renderKeyboardSelector();

    DeviceState getDeviceState(Display *display, unsigned long deviceID);
    void clickMouseXEvent(int button, long delayMS);
}

#endif /* xutils_hpp */
