workspace "Rimfrost"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Game"


outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"


include "Rimfrost/vendor/imgui"

project "Rimfrost"
    location "Rimfrost"
    kind "StaticLib"
    staticruntime "on"
    language "C++"
    cppdialect "C++20"
    systemversion "latest"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "rfpch.hpp"
    pchsource "Rimfrost/src/rfpch.cpp"

    files 
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/imgui/backends/imgui_impl_dx11.h",
        "%{prj.name}/vendor/imgui/backends/imgui_impl_dx11.cpp",
        "%{prj.name}/vendor/imgui/backends/imgui_impl_win32.h",
        "%{prj.name}/vendor/imgui/backends/imgui_impl_win32.cpp",
        "%{prj.name}/src/**.hlsl"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/src/**",
        "%{prj.name}/vendor/**",
        "external_headers/",
    }

    defines
    {
        "_UNICODE",
        "UNICODE",
    }

    links
    {
        "ImGui"
    }


    filter { "configurations:Debug" }
        defines { "_DEBUG", "DEBUG" }
        runtime "Debug"
        symbols "on"

    filter { "configurations:Release" }
       defines { "_NDEBUG", "NDEBUG" }
       runtime "Release"
       optimize "on"

    filter {"files:**.hlsl"}
        flags {"ExcludeFromBuild"}

    filter {"files:Rimfrost/vendor/**.cpp"}
        flags {"NoPCH"}




project "Game"
    location "Game"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++20"
    systemversion "latest"
    staticruntime "on"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files 
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "Rimfrost/src",
        "Rimfrost/src/**",
        "Rimfrost/src/Rimfrost",
        "Rimfrost/vendor/imgui",
    }

    defines
    {
        "_UNICODE",
        "UNICODE",
    }

    links
    {
        "Rimfrost"
    }

    filter { "configurations:Debug" }
        defines { "_DEBUG", "DEBUG" }
        symbols "On"

    filter { "configurations:Release" }
       defines { "_NDEBUG", "NDEBUG" }
       optimize "On"


