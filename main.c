#include <stdio.h>
#include "sfc_famicom.h"

int main()
{
    sfc_ecode code;
    sfc_famicom_t famicom;
    const char *nes_path = "/home/later/code/fc/resources/nestest.nes";

    code = sfc_famicom_init(&famicom, (void *)nes_path, NULL);
    if (code != SFC_ERROR_OK) {
        printf("famicom init failed\n");
        goto err;
    }

    printf("ROM: PRG-ROM: %d x 16kb   CHR-ROM %d x 8kb   Mapper: %03d\n",
        (int)famicom.rom_info.count_prgrom16kb, (int)famicom.rom_info.count_chrrom_8kb, (int)famicom.rom_info.mapper_number);
    getchar();

err:
    sfc_famicom_uninit(&famicom);
    return code;
}