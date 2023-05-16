# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/BarzyKader/Documents/repos/sbgECom/build/_deps/argtable3-src"
  "C:/Users/BarzyKader/Documents/repos/sbgECom/build/_deps/argtable3-build"
  "C:/Users/BarzyKader/Documents/repos/sbgECom/build/_deps/argtable3-subbuild/argtable3-populate-prefix"
  "C:/Users/BarzyKader/Documents/repos/sbgECom/build/_deps/argtable3-subbuild/argtable3-populate-prefix/tmp"
  "C:/Users/BarzyKader/Documents/repos/sbgECom/build/_deps/argtable3-subbuild/argtable3-populate-prefix/src/argtable3-populate-stamp"
  "C:/Users/BarzyKader/Documents/repos/sbgECom/build/_deps/argtable3-subbuild/argtable3-populate-prefix/src"
  "C:/Users/BarzyKader/Documents/repos/sbgECom/build/_deps/argtable3-subbuild/argtable3-populate-prefix/src/argtable3-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/BarzyKader/Documents/repos/sbgECom/build/_deps/argtable3-subbuild/argtable3-populate-prefix/src/argtable3-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/BarzyKader/Documents/repos/sbgECom/build/_deps/argtable3-subbuild/argtable3-populate-prefix/src/argtable3-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
