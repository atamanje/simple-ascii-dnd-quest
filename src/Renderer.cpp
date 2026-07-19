#include "Renderer.h"
#include <string>

Renderer::Renderer(int width, int height) : m_Width(width), m_Height(height) {
    m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    
    // Attempt to resize console and buffer
    COORD size = { (short)m_Width, (short)m_Height };
    SMALL_RECT rect = { 0, 0, (short)(m_Width - 1), (short)(m_Height - 1) };
    
    SetConsoleWindowInfo(m_ConsoleHandle, TRUE, &rect);
    SetConsoleScreenBufferSize(m_ConsoleHandle, size);
    
    m_Buffer.resize(m_Width * m_Height);
    m_WriteRect = { 0, 0, (short)(m_Width - 1), (short)(m_Height - 1) };
    
    // Hide cursor
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(m_ConsoleHandle, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(m_ConsoleHandle, &cursorInfo);
    
    Clear();
}

Renderer::~Renderer() {
}

void Renderer::Clear() {
    for (int i = 0; i < m_Width * m_Height; i++) {
        m_Buffer[i].Char.AsciiChar = ' ';
        m_Buffer[i].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }
}

void Renderer::DrawChar(int x, int y, char c, WORD color) {
    if (x >= 0 && x < m_Width && y >= 0 && y < m_Height) {
        m_Buffer[y * m_Width + x].Char.AsciiChar = c;
        m_Buffer[y * m_Width + x].Attributes = color;
    }
}

void Renderer::DrawString(int x, int y, const char* str, WORD color) {
    int cx = x;
    int cy = y;
    while (*str) {
        if (*str == '\n') {
            cy++;
            cx = x;
        } else {
            DrawChar(cx, cy, *str, color);
            cx++;
        }
        str++;
    }
}

void Renderer::DrawStringWrapped(int x, int y, const char* str, int maxWidth, WORD color) {
    int cx = x;
    int cy = y;
    std::string word;
    
    auto printWord = [&]() {
        if (cx + (int)word.length() > maxWidth) {
            cy++;
            cx = x;
        }
        for (char c : word) {
            DrawChar(cx++, cy, c, color);
        }
        word.clear();
    };

    while (*str) {
        if (*str == '\n') {
            printWord();
            cy++;
            cx = x;
        } else if (*str == ' ') {
            printWord();
            if (cx < maxWidth) {
                DrawChar(cx++, cy, ' ', color);
            } else {
                cy++;
                cx = x;
            }
        } else {
            word += *str;
        }
        str++;
    }
    printWord();
}

void Renderer::Present() {
    COORD bufferSize = { (short)m_Width, (short)m_Height };
    COORD bufferCoord = { 0, 0 };
    WriteConsoleOutputA(m_ConsoleHandle, m_Buffer.data(), bufferSize, bufferCoord, &m_WriteRect);
}
