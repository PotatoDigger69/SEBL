#SEBL

## Installation
### CMake
Include the following CMake code in your file:
```
set(CONFIG_ARCH x86_64)
include(ExternalProject)
ExternalProject_Add(SEBL
	GIT_REPOSITORY https://github.com/PotatoDigger69/SEBL
	CONFIGURE_COMMAND ${CMAKE_COMMAND} --preset ${CONFIG_ARCH}-release -D CONFIG_COMPILER=clang
)
set(SEBL_INSTALL_DIR ${CMAKE_BINARY_DIR}/install/SEBL)
list(APPEND CMAKE_MODULE_PATH ${SEBL_INSTALL_DIR})
find_package(SEBL REQUIRED)

sebl_target_create(uefi.img file1_to_be_in_img.bin)
sebl_target_sources(uefi.img kernel.elf file2_to_be_in_img.bin)
```
