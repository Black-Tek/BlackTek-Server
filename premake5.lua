workspace "Black-Tek-Server"
   configurations { "Debug", "Release"}
   platforms { "64", "ARM64", "ARM" }
   location ""
   editorintegration "On"

   project        "Black-Tek-Server"
      kind        "ConsoleApp"
      language    "C++"
      cppdialect  "C++20"
      targetdir   "%{wks.location}"
      objdir      "build/%{cfg.buildcfg}/obj"
      location    ""
      files { "src/**.cpp", "src/**.h" }
      flags {"MultiProcessorCompile"}
      enableunitybuild "On"
      intrinsics   "On"
      editandcontinue "Off"

      newoption {
         trigger     = "lua",
         description = "Specific Lua library to use. For example lua5.4. Useful if the packaged lua does not provide a symbolic liblua.so",
         value       = "libname",
         category    = "BlackTek", -- Group options together
         default     = "lua",
         allowed     = {
            {"lua", "Default"},
            {"lua5.4", "Use Lua 5.4"},
            {"lua5.3", "Use Lua 5.3"},
         }
      }

      newoption {
         trigger     = "custom-includes",
         description = "A comma separated list of custom include paths.",
         value       = "include paths",
         category    = "BlackTek", -- Group options together
      }

      newoption {
         trigger     = "custom-libs",
         description = "A comma separated list of custom library paths.",
         value       = "library paths",
         category    = "BlackTek", -- Group options together
      }

      newoption {
         trigger     = "verbose",
         description = "Show warnings during compilation.",
         category    = "BlackTek" -- Group options together
      }

      if _OPTIONS["custom-includes"] then
         includedirs { string.explode(_OPTIONS["custom-includes"], ",") }
      end

      if _OPTIONS["custom-libs"] then
         libdirs { string.explode(_OPTIONS["custom-libs"], ",") }
      end

      filter "configurations:Debug"
         defines { "DEBUG" }
         runtime "Debug"
         symbols "On"
         optimize "Debug"
         flags {"NoIncrementalLink"}
      filter {}

      filter "configurations:Release"
         defines { "NDEBUG" }
         runtime "Release"
         symbols "Off"
         optimize "Full"
      filter {}

      filter "platforms:64"
         architecture "x86_64"
      filter {}

      filter "platforms:ARM64"
         architecture "ARM64"
      filter {}

      filter "platforms:ARM"
         architecture "ARM"
      filter {}

      filter "system:not windows"
         buildoptions { "-Wall", "-Wextra", "-pedantic", "-pipe", "-fvisibility=hidden", "-Wno-unused-local-typedefs" }
         linkoptions{"-flto=auto"}
         flags {}
      filter {}

      filter "system:windows"
         openmp "On"
         characterset "MBCS"
         linkoptions {"/IGNORE:4099"}
         buildoptions {"/bigobj"}
         vsprops { VcpkgEnableManifest = "true" }
         symbolspath '$(OutDir)$(TargetName).pdb'
      filter {}

      filter "architecture:amd64"
	     vectorextensions "AVX"
      filter{}

      filter {"system:linux", "options:verbose"}
         linkoptions { "-v" }
         warnings "Extra"
      filter {}

      filter { "system:linux", "not options:verbose" }
         warnings "Off"
      filter {}

      filter { "system:linux", "architecture:ARM" }
         -- Paths to vcpkg installed dependencies
         libdirs { "vcpkg_installed/arm-linux/lib", "/usr/arm-linux-gnueabihf" }
         includedirs { "vcpkg_installed/arm-linux/include", "/usr/arm-linux-gnueabihf" }
      filter{}

      filter { "system:linux", "architecture:ARM64" }
         -- Paths to vcpkg installed dependencies
         libdirs { "vcpkg_installed/arm64-linux/lib", "/usr/arm-linux-gnueabi" }
         includedirs { "vcpkg_installed/arm64-linux/include", "/usr/arm-linux-gnueabi" }
      filter{}

      filter { "system:linux", "architecture:amd64" }
         -- Paths to vcpkg installed dependencies
         libdirs { "vcpkg_installed/x64-linux/lib" }
         includedirs { "vcpkg_installed/x64-linux/include" }
      filter{}

      filter "system:linux"
         -- Common Linux paths
         libdirs { "/usr/lib" }
         includedirs { "/usr/include", "/usr/include/lua5.*" }
         links { "pugixml", _OPTIONS["lua"], "fmt", "mariadb", "cryptopp", "boost_iostreams", "zstd", "z", "curl", "ssl", "crypto" }
      filter{}

      filter "toolset:gcc"
         buildoptions { "-fno-strict-aliasing" }
      filter {}

      filter "toolset:clang"
         buildoptions { "-Wimplicit-fallthrough", "-Wmove" }
      filter {}

      filter { "system:macosx", "action:gmake" }
         buildoptions { "-fvisibility=hidden" }   
      filter {}
