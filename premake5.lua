workspace "AsciiQuest"
    architecture "x64"
    startproject "AsciiQuest"

    configurations { "Debug", "Release" }

-- Project: Curl (build from source)
project "Curl"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    
    targetdir "bin/%{cfg.buildcfg}/%{prj.name}"
    
    includedirs {
        "vendor/curl/include",
        "vendor/curl/lib"
    }

    files {
        "vendor/curl/lib/**.c",
        "vendor/curl/lib/**.h",
    }
    
    removefiles {
        "vendor/curl/lib/ldap.c",
        "vendor/curl/lib/ldaps.c",
        "vendor/curl/lib/openldap.c",
        "vendor/curl/lib/vtls/openssl.c",
        "vendor/curl/lib/vtls/mbedtls.c",
        "vendor/curl/lib/vtls/wolfssl.c",
    }

    defines {
        "BUILDING_LIBCURL",
        "CURL_STATICLIB",
        "USE_SCHANNEL",
        "USE_WINDOWS_SSPI",
        "USE_WIN32_IDN",
        "CURL_DISABLE_LDAP",
        "CURL_DISABLE_LDAPS",
        "WANT_IDN_PROTOTYPES",
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "system:windows"
        systemversion "latest"

project "AsciiQuest"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}/%{prj.name}"
    
    includedirs {
        "vendor/json/include",
        "vendor/curl/include"
    }

    files { "src/**.h", "src/**.cpp" }

    links {
        "Curl",
        "ws2_32",
        "crypt32",
        "wldap32",
        "secur32",
        "normaliz",
        "advapi32"
    }

    filter "system:windows"
        defines { "CURL_STATICLIB" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
