/*
 * uefi_bootparams.h
 *
 *  Created on: Jul 17, 2022
 *      Author: dgerthe
 */

#ifndef SRC_BOOT_UEFI_UEFI_BOOTPARAMS_H_
#define SRC_BOOT_UEFI_UEFI_BOOTPARAMS_H_

#include <bootparams.h>

extern bootparams_t* uefi_new_bootparams(void);
extern efi_status_t uefi_add_bootparam(bootparams_t* params, bootparam_t* to_add);
extern bootparam_t* uefi_get_bootparam_mem_map(void);
extern bootparam_t* uefi_get_bootparam_rsdp(void);

#endif /* SRC_BOOT_UEFI_UEFI_BOOTPARAMS_H_ */
