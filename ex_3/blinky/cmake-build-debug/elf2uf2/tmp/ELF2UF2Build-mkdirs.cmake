# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/amerilain/pico/pico-sdk/tools/elf2uf2"
  "/Users/amerilain/Documents/ICT_docs/2nd_year/IoT_Connecting_Things/Assignments/ICT_Projects/ex_3/blinky/cmake-build-debug/elf2uf2"
  "/Users/amerilain/Documents/ICT_docs/2nd_year/IoT_Connecting_Things/Assignments/ICT_Projects/ex_3/blinky/cmake-build-debug/elf2uf2"
  "/Users/amerilain/Documents/ICT_docs/2nd_year/IoT_Connecting_Things/Assignments/ICT_Projects/ex_3/blinky/cmake-build-debug/elf2uf2/tmp"
  "/Users/amerilain/Documents/ICT_docs/2nd_year/IoT_Connecting_Things/Assignments/ICT_Projects/ex_3/blinky/cmake-build-debug/elf2uf2/src/ELF2UF2Build-stamp"
  "/Users/amerilain/Documents/ICT_docs/2nd_year/IoT_Connecting_Things/Assignments/ICT_Projects/ex_3/blinky/cmake-build-debug/elf2uf2/src"
  "/Users/amerilain/Documents/ICT_docs/2nd_year/IoT_Connecting_Things/Assignments/ICT_Projects/ex_3/blinky/cmake-build-debug/elf2uf2/src/ELF2UF2Build-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/amerilain/Documents/ICT_docs/2nd_year/IoT_Connecting_Things/Assignments/ICT_Projects/ex_3/blinky/cmake-build-debug/elf2uf2/src/ELF2UF2Build-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/amerilain/Documents/ICT_docs/2nd_year/IoT_Connecting_Things/Assignments/ICT_Projects/ex_3/blinky/cmake-build-debug/elf2uf2/src/ELF2UF2Build-stamp${cfgdir}") # cfgdir has leading slash
endif()
