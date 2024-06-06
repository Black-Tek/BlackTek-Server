workspace "Black-Tek-Server"
   configurations { "Debug", "Release"}
   platforms { "64" }
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
      flags {"LinkTimeOptimization", "MultiProcessorCompile"}
      vectorextensions "AVX"
      enableunitybuild "On"
      architecture "amd64"
      intrinsics   "On"

      filter "configurations:Debug"
         defines { "DEBUG" }
         symbols "On"
         optimize "Debug"
      filter {}

      filter "configurations:Release"
         defines { "NDEBUG" }
         symbols "On"
         optimize "Speed"
      filter {}

      filter "system:not windows"
         buildoptions { "-Wall", "-Wextra", "-pedantic", "-pipe", "-fvisibility=hidden", "-Wno-unused-local-typedefs" }
         linkoptions{"-flto=auto"}
      filter {}

      filter "system:windows"
         openmp "On"
         characterset "MBCS"
         debugformat "c7"
         linkoptions {"/IGNORE:4099"}
         vsprops { VcpkgEnableManifest = "true" }
      filter {}

      filter "system:Unix"
         -- Paths to vcpkg installed dependencies
         libdirs { "vcpkg_installed/x64-linux/lib" }
         includedirs { "vcpkg_installed/x64-linux/include" }
         links { "pugixml", "lua", "fmt", "ssl", "mariadb", "cryptopp", "crypto", "boost_iostreams", "zstd", "z" }
      filter{}

      filter "toolset:gcc"
         buildoptions { "-fno-strict-aliasing" }
         buildoptions {"-std=c++20"}
      filter {}

      filter "toolset:clang"
         buildoptions { "-Wimplicit-fallthrough", "-Wmove" }
      filter {}

      filter { "system:macosx", "action:gmake" }
         buildoptions { "-fvisibility=hidden" }   
      filter {}