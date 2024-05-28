#include "Input.h"
#include <Windows.h>
#include <chrono>
#include <thread>

using namespace std::chrono;
using namespace std::this_thread;
using namespace std::chrono_literals;

void Input::PressKey(HKey key)
{
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = key;
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;
    input.ki.wVk = 0;
    input.ki.dwFlags = KEYEVENTF_SCANCODE;
    SendInput(1, &input, sizeof(INPUT));
    input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}