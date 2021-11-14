﻿#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sfc_famicom.h"
#include "sfc_mapper.h"

union nes_header
{
    uint32_t u32;
    uint8_t id[4];
};

// 声明一个随便(SB)的函数指针类型
typedef void (*sfc_funcptr_t)();

int sfc_is_right_nes(uint32_t id)
{
    union nes_header head;
    head.id[0] = 'N';
    head.id[1] = 'E';
    head.id[2] = 'S';
    head.id[3] = '\x1A';
    return head.u32 == id ? 0 : 1;
}

// 加载默认测试ROM
sfc_ecode sfc_load_default_rom(void *arg, sfc_rom_info_t *info)
{
    int res;
    assert(info->data_prgrom == NULL && "FREE FIRST");
    FILE *const file = fopen(arg, "rb");

    if (file == NULL) {
        return SFC_ERROR_FILE_NOT_FOUND;
    }

    sfc_ecode code = SFC_ERROR_ILLEGAL_FILE;
    // 读取文件头
    sfc_nes_header_t nes_header;
    if (!fread(&nes_header, sizeof(nes_header), 1, file)) {
        printf("read nes file failed\n");
        goto err;
    }

    res = sfc_is_right_nes(nes_header.id);
    if (res) {
        printf("wrong nes file\n");
        goto err;
    }
    const size_t size1 = 16 * 1024 * nes_header.count_prgrom16kb;
    const size_t size2 = 8 * 1024 * nes_header.count_chrrom_8kb;
    uint8_t *const ptr = (uint8_t *)malloc(size1 + size2);
    // 内存申请成功
    if (ptr == NULL) {
        code = SFC_ERROR_OUT_OF_MEMORY;
        goto err;
    }
    // TODO: 实现Trainer
    // 跳过Trainer数据
    if (nes_header.control1 & SFC_NES_TRAINER) {
        fseek(file, 512, SEEK_CUR);
    }
    
    // 这都错了就不关我的事情了
    fread(ptr, size1 + size2, 1, file);

    // 填写info数据表格
    info->data_prgrom = ptr;
    info->data_chrrom = ptr + size1;
    info->count_prgrom16kb = nes_header.count_prgrom16kb;
    info->count_chrrom_8kb = nes_header.count_chrrom_8kb;
    info->mapper_number = (nes_header.control1 >> 4) | (nes_header.control2 & 0xF0);
    info->vmirroring = (nes_header.control1 & SFC_NES_VMIRROR) > 0;
    info->four_screen = (nes_header.control1 & SFC_NES_4SCREEN) > 0;
    info->save_ram = (nes_header.control1 & SFC_NES_SAVERAM) > 0;
    assert(!(nes_header.control1 & SFC_NES_TRAINER) && "unsupported");
    assert(!(nes_header.control2 & SFC_NES_VS_UNISYSTEM) && "unsupported");
    assert(!(nes_header.control2 & SFC_NES_Playchoice10) && "unsupported");
    code = SFC_OK;
err:
    fclose(file);
    return code;
}

sfc_ecode sfc_free_default_rom(void *arg, sfc_rom_info_t *info)
{
    if (info->data_prgrom == NULL) {
        goto out;
    }

    free(info->data_prgrom);
    info->data_prgrom = NULL;
out:
    return SFC_OK;
}

// StepFC: 加载ROM
sfc_ecode sfc_load_new_rom(sfc_famicom_t *famicom)
{
    // 先释放旧的ROM
    sfc_ecode code = famicom->interfaces.free_rom(famicom->argument, &famicom->rom_info);
    // 清空数据
    memset(&famicom->rom_info, 0, sizeof(famicom->rom_info));
    // 载入ROM
    if (code == SFC_OK) {
        code = famicom->interfaces.load_rom(famicom->argument, &famicom->rom_info);
    }

    // 载入新的Mapper
    if (code == SFC_OK) {
        code = sfc_load_mapper(famicom, famicom->rom_info.mapper_number);
    }
    // 首次重置
    if (code == SFC_OK) {
        famicom->mapper.reset(famicom);
    }
    return code;
}

sfc_ecode sfc_famicom_init(sfc_famicom_t *famicom, void *argument, const sfc_interface_t *interfaces)
{
    assert(famicom && "bad famicom");
    famicom->argument = argument;
    // 载入默认接口
    famicom->interfaces.load_rom = sfc_load_default_rom;
    famicom->interfaces.free_rom = sfc_free_default_rom;

    // 初步BANK
    famicom->prg_banks[0] = famicom->main_memory;
    famicom->prg_banks[3] = famicom->save_memory;

    // 提供了接口
    if (interfaces == NULL) {
        goto out;
    }

    const int count = sizeof(*interfaces) / sizeof(interfaces->load_rom);
    // 复制有效的函数指针
    // UB: C标准并不一定保证sizeof(void*)等同sizeof(fp) (非冯体系)
    //     所以这里声明了一个sfc_funcptr_t
    sfc_funcptr_t *const func_src = (sfc_funcptr_t *)interfaces;
    sfc_funcptr_t *const func_des = (sfc_funcptr_t *)&famicom->interfaces;
    for (int i = 0; i != count; ++i) {
        if (func_src[i]) {
            func_des[i] = func_src[i];
        }
    }

out:
    return sfc_load_new_rom(famicom);
    // return famicom->interfaces.load_rom(argument, &famicom->rom_info);
}

// StepFC: 反初始化famicom
void sfc_famicom_uninit(sfc_famicom_t *famicom)
{
    famicom->interfaces.free_rom(famicom->argument, &famicom->rom_info);
}