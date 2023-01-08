include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SEBL REQUIRED_VARS SEBL_INSTALL_PATH)

if(NOT SEBL_IMAGE_DIR)
	set(SEBL_IMAGE_DIR ${CMAKE_BINARY_DIR}/images)
endif()

if(EXISTS "${SEBL_INSTALL_PATH}/BOOTX64.efi")
	set(SEBL_BOOTBIN "${SEBL_INSTALL_PATH}/BOOTX64.efi")
elseif(EXISTS "${SEBL_INSTALL_PATH}/build/out/BOOTX64.efi")
	set(SEBL_BOOTBIN "${SEBL_INSTALL_PATH}/build/out/BOOTX64.efi")
elseif(EXISTS "${SEBL_INSTALL_PATH}/BOOTAA64.efi")
	set(SEBL_BOOTBIN "${SEBL_INSTALL_PATH}/BOOTAA64.efi")
elseif(EXISTS "${SEBL_INSTALL_PATH}/build/out/BOOTAA64.efi")
	set(SEBL_BOOTBIN "${SEBL_INSTALL_PATH}/build/out/BOOTAA64.efi")
else()
	message(FATAL_ERROR "Could not find SEBL bootloader at ${SEBL_INSTALL_PATH}")
endif()

function(sebl_target_create _target)
	add_custom_target(${_target}
	ALL
	COMMAND dd if=/dev/zero of=${_target} bs=512 count=93750 > /dev/null 2>&1
	COMMAND parted ${_target} -s -a minimal mklabel gpt
	COMMAND parted ${_target} -s -a minimal mkpart EFI FAT16 2048s 93716s
	COMMAND parted ${_target} -s -a minimal toggle 1 boot
	COMMAND dd if=/dev/zero of=part.img bs=512 count=91669 > /dev/null 2>&1
	COMMAND mformat -i part.img -h 32 -t 32 -n 64 -c 1
	COMMAND mkdir -p BOOT/UEFI/
	COMMAND mcopy -i part.img BOOT ::
	COMMAND mcopy -i part.img BOOT/UEFI ::/BOOT/
	COMMAND mcopy -i part.img ${SEBL_BOOTBIN} ::/BOOT/UEFI
	COMMAND mcopy -i part.img "$<TARGET_PROPERTY:${_target},SOURCES>" ::
	COMMAND dd if=part.img of=${_target} bs=512 count=91669 seek=2048 conv=notrunc > /dev/null 2>&1
	COMMAND rm part.img
	COMMAND mkdir -p ${SEBL_IMAGE_DIR}
	COMMAND cp ${_target} ${SEBL_IMAGE_DIR}
	SOURCES "${ARGN}"
)
endfunction()

function(sebl_target_sources _target)
	set_property(TARGET ${_target} APPEND PROPERTY SOURCES "${ARGN}")
endfunction()