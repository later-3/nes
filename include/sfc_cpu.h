#ifndef SFC_CPU_H
#define SFC_CPU_H

#include <stdint.h>
#include <sfc_famicom.h>

/// NES的CPU将最后几个地址称为向量(vector)
enum sfc_cpu_vector {
    SFC_VERCTOR_NMI     = 0xFFFA,   // 不可屏蔽中断
    SFC_VERCTOR_RESET   = 0xFFFC,   // 重置CP指针地址
    SFC_VERCTOR_IRQBRK  = 0xFFFE,   // 中断重定向
};

uint8_t sfc_read_cpu_address(uint16_t, const struct sfc_famicom *);
void sfc_write_cpu_address(uint16_t, uint8_t, struct sfc_famicom *);

#endif