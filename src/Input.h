#pragma once

class Input {
public:
    // Call this once per frame in the Engine loop
    static void Update();

    // Returns true if the key is currently held down
    static bool IsKeyDown(int vKey);

    // Returns true only on the first frame the key is pressed (useful for menus)
    static bool IsKeyPressed(int vKey);

private:
    static bool s_CurrentKeys[256];
    static bool s_PreviousKeys[256];
};
