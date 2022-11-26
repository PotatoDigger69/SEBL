/*
 * uefi_elf.h
 *
 *  Created on: Jul 18, 2022
 *      Author: dgerthe
 */

#ifndef SRC_BOOT_UEFI_UEFI_ELF_H_
#define SRC_BOOT_UEFI_UEFI_ELF_H_

#include <bootparams.h>

typedef struct {
	uintn_t filesize;
	void* buffer;
} elf_file_t;

//! Loads elf file into buffer via filesystem
//! @param[in] path  the file path to load the elf file from
//! @return elf file handle to be passed to other functions
extern elf_file_t uefi_elf_load_from_path(const char* path);

//! Loads the given elf file into memory and jumps to it
//! @param[in] file  The file handle of file in memory to operate on
extern void uefi_elf_load_and_jump(const elf_file_t* file, const bootparams_t* bp);

#endif /* SRC_BOOT_UEFI_UEFI_ELF_H_ */
