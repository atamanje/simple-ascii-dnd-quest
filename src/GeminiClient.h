#pragma once
#include <string>
#include <nlohmann/json.hpp>

class GeminiClient {
public:
    // Initializes libcurl and loads the API key from GEMINI_API_KEY env var
    static bool Initialize();
    static void Cleanup();

    // Sends the conversation history to Gemini and returns the JSON text response
    static std::string GenerateContent(const std::string& systemInstruction, const nlohmann::json& contentsArray);

private:
    static std::string s_ApiKey;
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
};
