cmake_minimum_required(VERSION 3.23.2)
project (SEBL)

enable_language(C ASM)

set(CONFIG_ARCH "x86_64" CACHE STRING "The architecture to build for")
set_property(CACHE CONFIG_ARCH PROPERTY STRINGS "x86_64" "aarch64") 
set(CONFIG_COMPILER "clang" CACHE STRING "Compiler flavor to use.  Can also override by directly specifying CMAKE_C_COMPILER... etc.")
set_property(CACHE CONFIG_COMPILER PROPERTY STRINGS "clang" "gcc")

set(CONFIG_DEFAULT_KERNEL_PATH "kernel.elf" CACHE STRING "Default path the bootloader looks for to find the kernel.")

add_subdirectory(third_party)


#if using clang environment, change linker
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
	set(CMAKE_LINKER "lld")
	set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_LINKER} <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -out:<TARGET> <LINK_LIBRARIES>")
endif()

file(GLOB_RECURSE UEFI_CORE_SOURCES 
					src/*.c
)

if(CONFIG_ARCH STREQUAL "x86_64")
	set(BOOTBIN "BOOTX64.EFI")
elseif(CONFIG_ARCH STREQUAL "aarch64")
	set(BOOTBIN "BOOTAA64.EFI")
endif()

add_executable(${BOOTBIN} ${UEFI_CORE_SOURCES})
add_dependencies(${BOOTBIN} posix-uefi)
target_include_directories(${BOOTBIN} PRIVATE 
	${INSTALL_POSIX_UEFI_PATH}/
	${CMAKE_CURRENT_SOURCE_DIR}/src/		
)
target_include_directories(${BOOTBIN} PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/include/
)
file(GLOB_RECURSE BOOTLOADER_PUBLIC_HEADERS include/*.h)
set_target_properties(${BOOTBIN} PROPERTIES PUBLIC_HEADER "${BOOTLOADER_PUBLIC_HEADERS}")

target_link_libraries(${BOOTBIN} 
							PRIVATE 
								${INSTALL_POSIX_UEFI_PATH}/libuefi.a
								${INSTALL_POSIX_UEFI_PATH}/crt0.o
)

target_compile_options(${BOOTBIN} PRIVATE
	-fshort-wchar 
	-fno-strict-aliasing 
	-ffreestanding 
	-fno-stack-protector 
	-fno-stack-check
)

target_compile_definitions(${BOOTBIN} PRIVATE
	__${CONFIG_ARCH}__
)
#x86_64 specific compile options
if(CONFIG_ARCH STREQUAL "x86_64")
	target_compile_options(${BOOTBIN} PRIVATE
		-mno-red-zone
	)
	target_compile_definitions(${BOOTBIN} PRIVATE
		HAVE_USE_MS_ABI
	)
endif()

#Compiler specific build options
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
	target_compile_options(${BOOTBIN} PRIVATE 
		--target=${CONFIG_ARCH}-pc-win32-coff 
		-Wno-builtin-requires-header 
		-Wno-incompatible-library-redeclaration 
		-Wno-long-long
	)
	target_link_options(${BOOTBIN} PRIVATE
		-flavor link
		-subsystem:efi_application 
		-nodefaultlib 
		-dll 
		-entry:uefi_init
	)
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	#TODO
else()
  message(FATAL_ERROR "Invalid C compiler ID: ${CMAKE_C_COMPILER_ID}")
endif()

install(TARGETS ${BOOTBIN} 
	RUNTIME
		DESTINATION "${CMAKE_INSTALL_PREFIX}"
	PUBLIC_HEADER
		DESTINATION "${CMAKE_INSTALL_PREFIX}"
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/build/other/SEBL.cmake
			DESTINATION "${CMAKE_INSTALL_PREFIX}"
)
	