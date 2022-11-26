include(ExternalProject)

# Commands to install POSIX-UEFI
# Creates header, lib, and crt0.o
list(APPEND BUILT_POSIX_UEFI_OBJS ${CMAKE_CURRENT_BINARY_DIR}/posix-uefi-prefix/src/posix-uefi/uefi/libuefi.a)
list(APPEND BUILT_POSIX_UEFI_OBJS ${CMAKE_CURRENT_BINARY_DIR}/posix-uefi-prefix/src/posix-uefi/uefi/crt_${CONFIG_ARCH}.o)
list(APPEND BUILT_POSIX_UEFI_OBJS ${CMAKE_CURRENT_BINARY_DIR}/posix-uefi-prefix/src/posix-uefi/uefi/uefi.h)
list(APPEND BUILT_POSIX_UEFI_OBJS ${CMAKE_CURRENT_BINARY_DIR}/posix-uefi-prefix/src/posix-uefi/uefi/elf_${CONFIG_ARCH}_efi.lds)
set(INSTALL_POSIX_UEFI_PATH ${CMAKE_CURRENT_BINARY_DIR}/posix-uefi-prefix/install/)
set(INSTALL_POSIX_UEFI_PATH ${INSTALL_POSIX_UEFI_PATH} PARENT_SCOPE)

message(STATUS ${CMAKE_CURRENT_BINARY_DIR})
ExternalProject_Add(posix-uefi
	GIT_REPOSITORY  https://gitlab.com/bztsrc/posix-uefi.git
	GIT_TAG ca2d3be4883394c419371645058e8e503fa786c2
	GIT_PROGRESS ON
	PATCH_COMMAND git apply ${CMAKE_SOURCE_DIR}/build/other/posix-uefi.patch > /dev/null 2>&1 || true
	CONFIGURE_COMMAND touch /dev/null
	BUILD_COMMAND make -C ../posix-uefi/uefi/ clean && make -C ../posix-uefi/uefi/ ARCH=${CONFIG_ARCH} libuefi.a crt_${CONFIG_ARCH}.o
	INSTALL_COMMAND mkdir -p ${INSTALL_POSIX_UEFI_PATH} && cp ${BUILT_POSIX_UEFI_OBJS} ${INSTALL_POSIX_UEFI_PATH} && mv ${INSTALL_POSIX_UEFI_PATH}/crt_${CONFIG_ARCH}.o ${INSTALL_POSIX_UEFI_PATH}/crt0.o && mv ${INSTALL_POSIX_UEFI_PATH}/elf_${CONFIG_ARCH}_efi.lds ${INSTALL_POSIX_UEFI_PATH}/link.ld
)