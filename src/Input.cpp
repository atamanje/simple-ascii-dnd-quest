#include "Input.h"
#include <windows.h>
#include <cstring>

bool Input::s_CurrentKeys[256] = { false };
bool Input::s_PreviousKeys[256] = { false };

void Input::Update() {
    // Copy current state to previous state
    std::memcpy(s_PreviousKeys, s_CurrentKeys, sizeof(s_CurrentKeys));

    // Update current state for all 256 virtual keys
    for (int i = 0; i < 256; i++) {
        // GetAsyncKeyState returns a short. If the highest bit is set, the key is down.
        s_CurrentKeys[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    }
}

bool Input::IsKeyDown(int vKey) {
    if (vKey < 0 || vKey >= 256) return false;
    return s_CurrentKeys[vKey];
}

bool Input::IsKeyPressed(int vKey) {
    if (vKey < 0 || vKey >= 256) return false;
    return s_CurrentKeys[vKey] && !s_PreviousKeys[vKey];
}
