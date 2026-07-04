#pragma once
#include <windows.h>
#include <vector>

class Renderer {
public:
    Renderer(int width, int height);
    ~Renderer();

    void Clear();
    void DrawChar(int x, int y, char c, WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    void DrawString(int x, int y, const char* str, WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    void Present();

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    int m_Width;
    int m_Height;
    HANDLE m_ConsoleHandle;
    std::vector<CHAR_INFO> m_Buffer;
    SMALL_RECT m_WriteRect;
};
