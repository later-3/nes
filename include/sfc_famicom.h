#ifndef SFC_FAMICOM_H
#define SFC_FAMICOM_H

#include "sfc_rom.h"
#include "sfc_code.h"

// StepFC扩展接口
typedef struct
{
    sfc_ecode (*load_rom)(void *, sfc_rom_info_t *); // ROM 加载器读取信息
    sfc_ecode (*free_rom)(void *, sfc_rom_info_t *); // ROM 加载器卸载
} sfc_interface_t;

struct sfc_famicom;

// StepFC: Mapper接口
typedef struct sfc_mapper
{
    sfc_ecode (*reset)(struct sfc_famicom *); // Mapper 重置
} sfc_mapper_t;

// FC 模拟器主体
typedef struct sfc_famicom
{
    void *argument;                    // 参数
    sfc_interface_t interfaces;        // 扩展接口
    sfc_mapper_t mapper;               // mapper 接口
    sfc_rom_info_t rom_info;           // ROM 信息
    uint8_t *prg_banks[0x10000 >> 13]; // 程序内存仓库(Bank)/窗口(Window)
    uint8_t save_memory[8 * 1024];     // 工作(work)/保存(save)内存
    uint8_t main_memory[2 * 1024];     // 主内存
} sfc_famicom_t;

// The SFC disassembly buf length
enum
{
    SFC_DISASSEMBLY_BUF_LEN2 = 48
};

// 指定地方反汇编
void sfc_fc_disassembly(uint16_t address, const struct sfc_famicom *famicom, char buf[SFC_DISASSEMBLY_BUF_LEN2]);

sfc_ecode sfc_famicom_init(struct sfc_famicom *famicom, void *argument, const sfc_interface_t *interfaces);
void sfc_famicom_uninit(struct sfc_famicom *);

#endif