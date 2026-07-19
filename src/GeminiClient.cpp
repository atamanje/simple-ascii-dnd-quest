#include "GeminiClient.h"
#include <curl/curl.h>
#include <iostream>
#include <cstdlib>

using json = nlohmann::json;

std::string GeminiClient::s_ApiKey = "";

bool GeminiClient::Initialize() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
#pragma warning(disable: 4996) 
    const char* envKey = std::getenv("GEMINI_API_KEY");
#pragma warning(default: 4996)
    
    if (envKey) {
        s_ApiKey = envKey;
        return true;
    }
    return false;
}

void GeminiClient::Cleanup() {
    curl_global_cleanup();
}

size_t GeminiClient::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* str = static_cast<std::string*>(userp);
    str->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

std::string GeminiClient::GenerateContent(const std::string& systemInstruction, const nlohmann::json& contentsArray) {
    if (s_ApiKey.empty()) {
        std::cerr << "GEMINI_API_KEY not set!" << std::endl;
        return "";
    }

    CURL* curl = curl_easy_init();
    if (!curl) return "";

    std::string url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-3.5-flash:generateContent?key=" + s_ApiKey;
    
    json payload;
    
    if (!systemInstruction.empty()) {
        payload["system_instruction"]["parts"] = json::array({{{"text", systemInstruction}}});
    }
    
    payload["contents"] = contentsArray;
    
    // Force the model to return JSON matching our structure
    payload["generationConfig"]["responseMimeType"] = "application/json";

    std::string payloadStr = payload.dump();
    std::string responseBuffer;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadStr.c_str());
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);
    
    // Standard SSL verify config
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

    CURLcode res = curl_easy_perform(curl);
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "cURL failed: " << curl_easy_strerror(res) << std::endl;
        return "";
    }

    try {
        json responseJson = json::parse(responseBuffer);
        if (responseJson.contains("candidates") && responseJson["candidates"].size() > 0) {
            auto& candidate = responseJson["candidates"][0];
            if (candidate.contains("content") && candidate["content"].contains("parts")) {
                return candidate["content"]["parts"][0]["text"].get<std::string>();
            }
        } else if (responseJson.contains("error")) {
             std::cerr << "Gemini API Error: " << responseJson["error"].dump() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse Gemini response: " << e.what() << "\nResponse Data:\n" << responseBuffer << std::endl;
    }

    return "";
}
