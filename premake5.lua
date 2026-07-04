workspace "AsciiQuest"
    architecture "x64"
    startproject "AsciiQuest"

    configurations { "Debug", "Release" }

project "AsciiQuest"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"
    
    includedirs {
        "vendor/json/include"
    }

    files { "src/**.h", "src/**.cpp" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
