/*
 * kmain.c
 *
 *  Created on: Jul 10, 2022
 *      Author: dgerthe
 */

#include <uefi.h>
#include <uefi_bootparams.h>
#include <uefi_elf.h>


int main (int argc, char **argv)
{
	// Disable the watchdog timer.  line might be removed later.
	BS->SetWatchdogTimer(0,0,0,NULL);

	efi_status_t status = EFI_SUCCESS;

	// Get screen in a known state
	ST->ConOut->ClearScreen(ST->ConOut);

	// Create our bootparams to pass to kernel
	bootparams_t* bps = uefi_new_bootparams();
	if(!bps) {
		return 1;
	}

	// Get memory map
	bootparam_t* mem_map = uefi_get_bootparam_mem_map();
	if(!mem_map){ // Mem map is required
		return 1;
	}
	status = uefi_add_bootparam(bps, mem_map);
	if(EFI_ERROR(status)){
		return 1;
	}

	elf_file_t elf = uefi_elf_load_from_path("kernel.elf");
	uefi_elf_load_and_jump(&elf, bps);

	for(;;);
	return 0;
}
