/*
 * uefi_bootparams.c
 *
 *  Created on: Jul 17, 2022
 *      Author: dgerthe
 */


#include <uefi.h>
#include <uefi_bootparams.h>

bootparams_t* uefi_new_bootparams(void) {
	efi_status_t status = EFI_SUCCESS;
	void* bootparams_buffer;
	status = BS->AllocatePool(
			EfiLoaderData,
			sizeof(bootparams_t),
			&bootparams_buffer
	);

	if(EFI_ERROR(status)){
		ST->ConOut->OutputString(ST->ConOut, L"Error allocating storage for bootparams\n");
		return NULL;
	}
	bootparams_t* bootparams = bootparams_buffer;
	bootparams->bootparams_version = 1;
	bootparams->bootparams_size = sizeof(bootparams_t);
	bootparams->num_params = 0;
	bootparams->param_start = NULL;
	return bootparams;
}

efi_status_t uefi_add_bootparam(bootparams_t* params, bootparam_t* to_add) {
	if(!params || !to_add){
		return EFI_BAD_BUFFER_SIZE;
	}
	if(!params->param_start){
		params->param_start = to_add;
	} else {
		bootparam_t* curr_param = params->param_start;
		while(curr_param->next_param){
			curr_param = curr_param->next_param;
		}
		curr_param->next_param = to_add;
	}
	params->num_params++;
	return EFI_SUCCESS;
}
bootparam_t* uefi_get_bootparam_mem_map(void)
{
	efi_status_t status = EFI_SUCCESS;
		efi_memory_descriptor_t* memory_map = NULL;
		uint32_t version = 0;
		uintn_t map_key = 0;
		uintn_t descriptor_size = 0;
		uintn_t memory_map_size = 0;

		status = BS->GetMemoryMap(&memory_map_size, memory_map, &map_key, &descriptor_size, &version);
		if(status != EFI_BUFFER_TOO_SMALL){
			ST->ConOut->OutputString(ST->ConOut, L"Error on EFI first GetMemoryMap call\n");
			return NULL;
		}

		uintn_t encompassing_size = memory_map_size + (2*descriptor_size);
		status = BS->AllocatePool(EfiLoaderData, encompassing_size, (void**)(&memory_map));
		if(EFI_ERROR(status)){
			ST->ConOut->OutputString(ST->ConOut, L"Error allocating storage for EFI memory map\n");
			return NULL;
		}
		memory_map_size = encompassing_size;

		status = BS->GetMemoryMap(&memory_map_size, memory_map, &map_key, &descriptor_size, &version);
		if(EFI_ERROR(status)){
			ST->ConOut->OutputString(ST->ConOut, L"Error getting EFI memory map\n");
			return NULL;
		}

		void* bootparam_buffer = NULL;
		status = BS->AllocatePool(
				EfiLoaderData,
				sizeof(bootparam_t) + sizeof(bootparam_mem_map_t) + memory_map_size,
				&bootparam_buffer);
		if(EFI_ERROR(status)){
			ST->ConOut->OutputString(ST->ConOut, L"Error allocating storage for memory map bootparam\n");
			goto dealloc_memmap_error;
		}
		bootparam_t* bootparam = bootparam_buffer;
		bootparam->type = bootparam_mem_map;
		bootparam->version = 1;
		bootparam->param = bootparam+1;
		bootparam->next_param = NULL;

		bootparam_mem_map_t* bp_mem_map = bootparam->param;
		bp_mem_map->version = 1;
		bp_mem_map->num_descriptors = (memory_map_size / descriptor_size);
		bp_mem_map->descriptor_list = (bootparam_mem_descriptor_t*)(bp_mem_map+1);

		for(int i = 0; i < (memory_map_size / descriptor_size); i++){
				efi_memory_descriptor_t* efi_desc =
						(efi_memory_descriptor_t*) (((char*)memory_map)+(descriptor_size * i));

				bootparam_mem_descriptor_t* bootparam_desc = bp_mem_map->descriptor_list + i;

				bootparam_desc->type = efi_desc->Type;
				bootparam_desc->phy_addr = efi_desc->PhysicalStart;
				bootparam_desc->virt_addr = efi_desc->VirtualStart;
				bootparam_desc->pages = efi_desc->NumberOfPages;
				bootparam_desc->attrs = efi_desc->Attribute;
		}

		BS->FreePool((void*)memory_map);
		return bootparam;

		dealloc_memmap_error:
		BS->FreePool((void*)memory_map);
		return NULL;
}

bootparam_t* uefi_get_bootparam_rsdp(void){
	return NULL; // TODO: implement
}
