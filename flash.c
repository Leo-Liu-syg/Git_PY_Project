#include "flash.h"


/*************************************************************/
/* 全局变量定义（存储在 RAM 中，最终会写入 Flash 掉电保存）     */
/*************************************************************/
uint32_t remote_cur = 0;        /* 当前遥控器索引 */
uint32_t devices_cur = 0;        /* 当前设备索引 */
uint32_t sos_time = 4;           /* 报警持续时间：0=60s 1=5min 2=15min 3=30min 4=无限 */
uint32_t sos_mode = 0;           /* 报警模式：0=撤防 1=布防 */
uint32_t sos_urgency = 0;        /* 报警类型：0=紧急 1=正常 */
uint32_t speak_mode = 0;         /* 语音提示类型：0=火灾 1=紧急求助 2=安全 3=单纯报警 */

uint32_t remote[6] = {0};        /* 遥控器数据缓冲区（0号为索引，其余为数据） */
uint32_t devices1[30] = {0};     /* 设备组1数据缓冲区 */
uint32_t devices2[30] = {0};     /* 设备组2数据缓冲区 */

uint32_t data_buffer[32] = {0};  /* Flash 页写入临时缓冲区 */

/*************************************************************/
/* 函数功能：擦除指定 Flash 页                                */
/* 入口：flash_page_addr 要擦除的页地址                       */
/* 说明：Flash 写入前必须先擦除（将所有位写为 1）             */
/*************************************************************/
void flash_erase_pape(uint32_t flash_page_addr)
{
    /* 等待 Flash 空闲 */
    while (LL_FLASH_IsActiveFlag_BUSY(FLASH) == 1);

    /* 使能“操作完成中断” */
    LL_FLASH_EnableIT_EOP(FLASH);

    /* 使能页擦除模式 */
    LL_FLASH_EnablePageErase(FLASH);

    /* 设置要擦除的页地址 */
    LL_FLASH_SetEraseAddress(FLASH, flash_page_addr);

    /* 等待擦除完成 */
    while (LL_FLASH_IsActiveFlag_BUSY(FLASH) == 1);
    while (LL_FLASH_IsActiveFlag_EOP(FLASH) == 0);

    /* 清除操作完成标志 */
    LL_FLASH_ClearFlag_EOP(FLASH);

    /* 关闭中断 + 关闭擦除模式 */
    LL_FLASH_DisableIT_EOP(FLASH);
    LL_FLASH_DisablePageErase(FLASH);
}

/*************************************************************/
/* 函数功能：将 data_buffer 写入指定 Flash 页                 */
/* 入口：flash_page_addr 目标页地址                           */
/*************************************************************/
void flash_write_pape(uint32_t flash_page_addr)
{
    uint32_t *src = data_buffer;  /* 源数据指针 */

    /* 等待 Flash 空闲 */
    while (LL_FLASH_IsActiveFlag_BUSY(FLASH) == 1);

    /* 使能操作完成中断 */
    LL_FLASH_EnableIT_EOP(FLASH);

    /* 使能页编程模式 */
    LL_FLASH_EnablePageProgram(FLASH);

    /* 执行整页写入 */
    LL_FLASH_PageProgram(FLASH, flash_page_addr, src);

    /* 等待写入完成 */
    while (LL_FLASH_IsActiveFlag_BUSY(FLASH) == 1);
    while (LL_FLASH_IsActiveFlag_EOP(FLASH) == 0);

    /* 清除标志 + 关闭中断 + 关闭编程 */
    LL_FLASH_ClearFlag_EOP(FLASH);
    LL_FLASH_DisableIT_EOP(FLASH);
    LL_FLASH_DisablePageProgram(FLASH);
}

/*************************************************************/
/* 函数功能：Flash 页“擦除 + 写入”一体化操作                 */
/*************************************************************/
void flash_set_page(uint32_t flash_page_addr)
{
    LL_FLASH_Unlock(FLASH);        /* 解锁 Flash */
    flash_erase_pape(flash_page_addr);  /* 擦除页 */
    flash_write_pape(flash_page_addr);  /* 写入页 */
    LL_FLASH_Lock(FLASH);          /* 锁定 Flash */
}

/*************************************************************/
/* 函数功能：将【控制参数】保存到 Flash                       */
/* 包括：遥控器索引、设备索引、报警时间、模式、语音类型等     */
/*************************************************************/
void update_control()
{
    data_buffer[0] = remote_cur;
    data_buffer[1] = devices_cur;
    data_buffer[2] = sos_time;
    data_buffer[3] = sos_mode;
    data_buffer[4] = speak_mode;
    data_buffer[5] = sos_urgency;

    flash_set_page(FLASH_PAGE_CONTROL);
}

/*************************************************************/
/* 函数功能：将【遥控器数据】保存到 Flash                     */
/*************************************************************/
void update_remote()
{
    memcpy(data_buffer, remote, (6 * 4));  /* 复制遥控器数据到缓冲区 */
    flash_set_page(FLASH_PAGE_REMOTE);    /* 写入 Flash */
}

/*************************************************************/
/* 函数功能：将【两组设备数据】分别保存到 Flash               */
/*************************************************************/
void update_devices()
{
    memcpy(data_buffer, devices1, (30 * 4));
    flash_set_page(FLASH_PAGE_DEVICES1);

    memcpy(data_buffer, devices2, (30 * 4));
    flash_set_page(FLASH_PAGE_DEVICES2);
}

/*************************************************************/
/* 函数功能：恢复出厂设置（清空所有遥控器、设备数据）         */
/* 说明：将 Flash 全部写 0xFF，索引归零，重新保存             */
/*************************************************************/
void recovery()
{
    memset(remote, 0xff, (6 * 4));
    memset(devices1, 0xff, (30 * 4));
    memset(devices2, 0xff, (30 * 4));

    remote_cur = 0;
    devices_cur = 0;

    update_remote();
    update_devices();
    update_control();
}

/*************************************************************/
/* 函数功能：Flash 初始化 + 从 Flash 恢复所有数据到 RAM       */
/* 说明：上电时执行一次，把保存的配置读回变量中               */
/*************************************************************/
void flash_init()
{
    /* 使能时钟 */
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    /* 解锁 Flash + 配置 Flash 时序（24MHz） */
    LL_FLASH_Unlock(FLASH);
    LL_FLASH_TIMMING_SEQUENCE_CONFIG_24M();

    /* ==================================== */
    /* 从 Flash 读取【控制参数】到全局变量   */
    /* ==================================== */
    remote_cur = HW32_REG(FLASH_PAGE_CONTROL);
    if (remote_cur >= REMOTE_NUM) remote_cur = 0;  /* 非法值保护 */

    devices_cur = HW32_REG(FLASH_PAGE_CONTROL + (4 * 1));
    if (devices_cur >= DEVICE_NUM) devices_cur = 0;

    sos_time = HW32_REG(FLASH_PAGE_CONTROL + (4 * 2));
    if(sos_time > 4) sos_time = 4;

    sos_mode = HW32_REG(FLASH_PAGE_CONTROL + (4 * 3));
    if(sos_mode > 1) sos_mode = 0;

    speak_mode = HW32_REG(FLASH_PAGE_CONTROL + (4 * 4));
    if(speak_mode > 3) speak_mode = 0;

    sos_urgency = HW32_REG(FLASH_PAGE_CONTROL + (4 * 5));
    if(sos_urgency > 1) sos_urgency = 0;

    /* ==================================== */
    /* 从 Flash 读取【遥控器数据】           */
    /* ==================================== */
    for (uint8_t i = 0; i < REMOTE_NUM; i++)
    {
        remote[i] = HW32_REG(FLASH_PAGE_REMOTE + (4 * i));
    }

    /* ==================================== */
    /* 从 Flash 读取【两组设备数据】         */
    /* ==================================== */
    for (uint8_t i = 0; i < 30; i++)
    {
        devices1[i] = HW32_REG(FLASH_PAGE_DEVICES1 + (4 * i));
    }
    for (uint8_t i = 0; i < 30; i++)
    {
        devices2[i] = HW32_REG(FLASH_PAGE_DEVICES2 + (4 * i));
    }

    /* 锁定 Flash */
    LL_FLASH_Lock(FLASH);
}