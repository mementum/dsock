-- A solution contains projects, and defines the available configurations
solution "dsock"
   configurations { "Release" }
 
   -- A project defines one build target
   project "dsock"
      kind "StaticLib"
      language "C++"
      files { "src/include/dsock/*.hpp", "src/*.cpp" }
      includedirs { "src/include" }
 
      configuration "Release"
         flags { "Optimize" }

   project "testing"
      kind "ConsoleApp"
      language "C++"
      files { "testing/*.hpp", "testing/*.cpp" }
      includedirs { "src/include" }
      links { "dsock", "wsock32", "winmm" }
 
      configuration "Release"
         flags { "Optimize" } 