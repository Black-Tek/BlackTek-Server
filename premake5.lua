workspace "Black-Tek-Server"
   configurations { "Debug", "Release"}
   platforms { "64", "ARM64" }
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

      filter {"system:linux"}
      linkoptions { "-v" }
      filter {}


      filter { "system:linux", "architecture:ARM64" }
         -- Paths to vcpkg installed dependencies
         libdirs { "vcpkg_installed/arm64-linux/lib" }
         includedirs { "vcpkg_installed/arm64-linux/include" }
         links { "pugixml", "lua", "fmt", "mariadb", "cryptopp", "boost_iostreams", "zstd", "z", "curl", "tomlplusplus", "ssl", "crypto" }
      filter{}

      filter { "system:linux", "architecture:amd64" }
         -- Paths to vcpkg installed dependencies
         libdirs { "vcpkg_installed/x64-linux/lib" }
         includedirs { "vcpkg_installed/x64-linux/include" }
         links { "pugixml", "lua", "fmt", "mariadb", "cryptopp", "boost_iostreams", "zstd", "z", "curl", "tomlplusplus", "ssl", "crypto" }
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
