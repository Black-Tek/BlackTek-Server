-- Workspace configuration for Black-Tek-Server
workspace "Black-Tek-Server"
    configurations { "Debug", "Release" }
    platforms { "64", "ARM64", "ARM" }
    location ""
    editorintegration "On"

-- Project configuration
project "Black-Tek-Server"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "%{wks.location}"
    objdir "build/%{cfg.buildcfg}/obj"
    location ""
    files { "src/**.cpp", "src/**.h" }
    flags { "MultiProcessorCompile" }
    enableunitybuild "On"
    intrinsics "On"
    editandcontinue "Off"

    -- Custom command-line options
    newoption {
        trigger     = "lua",
        description = "Specific Lua library to use (e.g., lua5.4).",
        value       = "libname",
        category    = "BlackTek", -- Group options together
        default     = "lua",
        allowed     = {
            { "lua", "Default" },
            { "lua5.4", "Use Lua 5.4" },
            { "lua5.3", "Use Lua 5.3" }
        }
    }

    newoption {
        trigger     = "custom-includes",
        description = "Comma-separated list of custom include paths.",
        value       = "include paths",
        category    = "BlackTek"
    }

    newoption {
        trigger     = "custom-libs",
        description = "Comma-separated list of custom library paths.",
        value       = "library paths",
        category    = "BlackTek"
    }

    newoption {
        trigger     = "verbose",
        description = "Enable verbose compilation warnings.",
        category    = "BlackTek"
    }

    -- Process custom include and library paths
    if _OPTIONS["custom-includes"] then
        includedirs { string.explode(_OPTIONS["custom-includes"], ",") }
    end

    if _OPTIONS["custom-libs"] then
        libdirs { string.explode(_OPTIONS["custom-libs"], ",") }
    end

    -- Configuration-specific settings
    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"
        optimize "Debug"
        flags { "NoIncrementalLink" }

    filter "configurations:Release"
        defines { "NDEBUG" }
        runtime "Release"
        symbols "Off"
        optimize "Full"

    -- Platform-specific settings
    filter "platforms:64"
        architecture "x86_64"

    filter "platforms:ARM64"
        architecture "ARM64"

    filter "platforms:ARM"
        architecture "ARM"

    -- Non-Windows system settings
    filter "system:not windows"
        buildoptions { "-Wall", "-Wextra", "-pedantic", "-pipe", "-fvisibility=hidden", "-Wno-unused-local-typedefs" }
        linkoptions { "-flto=auto" }

    -- Windows-specific settings
    filter "system:windows"
        openmp "On"
        characterset "MBCS"
        linkoptions { "/IGNORE:4099" }
        buildoptions { "/bigobj", "/utf-8" }
        vsprops { VcpkgEnableManifest = "true" }
        files {"resources.rc", "blackteklogo.ico"}
        symbolspath "$(OutDir)$(TargetName).pdb"

    -- Architecture-specific settings
    filter "architecture:x86_64"
        vectorextensions "AVX"

    -- Linux-specific settings
    filter { "system:linux", "options:verbose" }
        linkoptions { "-v" }
        warnings "Extra"

    filter { "system:linux", "not options:verbose" }
        warnings "Off"

    filter { "system:linux", "architecture:ARM" }
        libdirs { "vcpkg_installed/arm-linux/lib", "/usr/arm-linux-gnueabihf" }
        includedirs { "vcpkg_installed/arm-linux/include", "/usr/arm-linux-gnueabihf" }

    filter { "system:linux", "architecture:ARM64" }
        libdirs { "vcpkg_installed/arm64-linux/lib", "/usr/arm-linux-gnueabi" }
        includedirs { "vcpkg_installed/arm64-linux/include", "/usr/arm-linux-gnueabi" }

    filter { "system:linux", "architecture:x86_64" }
        libdirs { "vcpkg_installed/x64-linux/lib" }
        includedirs { "vcpkg_installed/x64-linux/include" }

    filter "system:linux"
        libdirs { "/usr/lib" }
        includedirs { "/usr/include", "/usr/include/lua5.*" }
        links { "pugixml", _OPTIONS["lua"], "fmt", "mariadb", "cryptopp", "boost_iostreams", "zstd", "z", "curl", "ssl", "crypto" }

    -- Toolset-specific settings
    filter "toolset:gcc"
        buildoptions { "-fno-strict-aliasing" }

    filter "toolset:clang"
        buildoptions { "-Wimplicit-fallthrough", "-Wmove" }

    -- macOS-specific settings
    filter { "system:macosx", "action:gmake" }
        buildoptions { "-fvisibility=hidden" }
