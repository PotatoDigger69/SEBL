/*
 * bootparams.h
 *
 *  Created on: Jul 16, 2022
 *      Author: dgerthe
 */

#pragma once

#pragma pack(push,1)

typedef struct _s_bootparam{
	uint16_t version;
	uint16_t type; // one of bootparam_type_t
	void* param;
	struct _s_bootparam* next_param;
} bootparam_t;

typedef struct {
	uint16_t bootparams_version;
	uint32_t bootparams_size;
	uint32_t num_params;
	bootparam_t* param_start;
} bootparams_t;

enum bootparam_type_t {
	bootparam_mem_map
};

typedef struct {
    uint32_t                type;
    efi_physical_address_t  phy_addr;
    efi_virtual_address_t   virt_addr;
    uint64_t                pages;
    uint64_t                attrs;
} bootparam_mem_descriptor_t;

typedef struct {
	uint16_t version;
	uint32_t num_descriptors;
	bootparam_mem_descriptor_t* descriptor_list;
} bootparam_mem_map_t;

#pragma pack(pop)
