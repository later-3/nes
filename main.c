#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sfc_cpu.h"
#include "sfc_famicom.h"

#define NES_PATH "./resources/nestest.nes"

int main()
{
    sfc_ecode code;
    struct sfc_famicom famicom;
    memset(&famicom, 0, sizeof(struct sfc_famicom));

    code = sfc_famicom_init(&famicom, NES_PATH, NULL);
    if (code != SFC_ERROR_OK) {
        printf("famicom init failed\n");
        goto err;
    }

    printf("ROM: PRG-ROM: %d x 16kb   CHR-ROM %d x 8kb   Mapper: %03d\n", (int)famicom.rom_info.count_prgrom16kb, 
            (int)famicom.rom_info.count_chrrom_8kb, (int)famicom.rom_info.mapper_number);

      // V0 - NMI
    uint16_t v0 = sfc_read_cpu_address(SFC_VERCTOR_NMI + 0, &famicom);
    v0 |= sfc_read_cpu_address(SFC_VERCTOR_NMI + 1, &famicom) << 8;
    // V1 - RESET
    uint16_t v1 = sfc_read_cpu_address(SFC_VERCTOR_RESET + 0, &famicom);
    v1 |= sfc_read_cpu_address(SFC_VERCTOR_RESET + 1, &famicom) << 8;
    // V2 - IRQ/BRK
    uint16_t v2 = sfc_read_cpu_address(SFC_VERCTOR_IRQBRK + 0, &famicom);
    v2 |= sfc_read_cpu_address(SFC_VERCTOR_IRQBRK + 1, &famicom) << 8;

    printf(
        "ROM: NMI: $%04X  RESET: $%04X  IRQ/BRK: $%04X\n",
        (int)v0, (int)v1, (int)v2
    );
    
    getchar();
err:
    sfc_famicom_uninit(&famicom);
    return code;
}