# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/GameEngine/Dependencies/glm-src"
  "D:/GameEngine/Dependencies/glm-build"
  "D:/GameEngine/Dependencies/glm-subbuild/glm-populate-prefix"
  "D:/GameEngine/Dependencies/glm-subbuild/glm-populate-prefix/tmp"
  "D:/GameEngine/Dependencies/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp"
  "D:/GameEngine/Dependencies/glm-subbuild/glm-populate-prefix/src"
  "D:/GameEngine/Dependencies/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/GameEngine/Dependencies/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/GameEngine/Dependencies/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
