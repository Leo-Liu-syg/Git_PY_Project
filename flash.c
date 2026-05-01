#include "flash.h"
#include "main.h"

uint32_t remote_cur = 0;
uint32_t devices_cur = 0;
uint32_t sos_time = 4; //报警时间 0:60秒 1:5分钟 2:15分钟 3:30分钟 4:无限
uint32_t sos_mode = 0; //0:撤防 1:布防
uint32_t sos_urgency = 0; //0:紧急 1:正常
uint32_t speak_mode = 0; //0:火灾 1:紧急求助 2:安全 3:单纯报警

uint32_t remote[6] = {0};    // 0:cur other:data
uint32_t devices1[30] = {0}; // 0:cur other:data
uint32_t devices2[30] = {0}; // 0:cur other:data

uint32_t data_buffer[32] = {0};
void flash_erase_pape(uint32_t flash_page_addr)
{
    /* Wait Busy=0 */
    while (LL_FLASH_IsActiveFlag_BUSY(FLASH) == 1)
        ;

    /* Enable EOP */
    LL_FLASH_EnableIT_EOP(FLASH);

    /* Enable Page Erase */
    LL_FLASH_EnablePageErase(FLASH);

    /* Set Erase Address */
    LL_FLASH_SetEraseAddress(FLASH, flash_page_addr);

    /* Wait Busy=0 */
    while (LL_FLASH_IsActiveFlag_BUSY(FLASH) == 1)
        ;

    /* Wait EOP=1 */
    while (LL_FLASH_IsActiveFlag_EOP(FLASH) == 0)
        ;

    /* Clear EOP */
    LL_FLASH_ClearFlag_EOP(FLASH);

    /* Disable EOP */
    LL_FLASH_DisableIT_EOP(FLASH);

    /* Disable Page Erase */
    LL_FLASH_DisablePageErase(FLASH);
}

void flash_write_pape(uint32_t flash_page_addr)
{
    uint32_t *src = data_buffer;

    /* Wait Busy=0 */
    while (LL_FLASH_IsActiveFlag_BUSY(FLASH) == 1)
        ;

    /* Enable EOP */
    LL_FLASH_EnableIT_EOP(FLASH);

    /* Enable Program */
    LL_FLASH_EnablePageProgram(FLASH);

    /* Page Program */
    LL_FLASH_PageProgram(FLASH, flash_page_addr, src);

    /* Wait Busy=0 */
    while (LL_FLASH_IsActiveFlag_BUSY(FLASH) == 1)
        ;

    /* Wait EOP=1 */
    while (LL_FLASH_IsActiveFlag_EOP(FLASH) == 0)
        ;

    /* Clear EOP */
    LL_FLASH_ClearFlag_EOP(FLASH);

    /* Disable EOP */
    LL_FLASH_DisableIT_EOP(FLASH);

    /* Disable Program */
    LL_FLASH_DisablePageProgram(FLASH);
}

void flash_set_page(uint32_t flash_page_addr)
{
    LL_FLASH_Unlock(FLASH);
    flash_erase_pape(flash_page_addr);
    flash_write_pape(flash_page_addr);
    LL_FLASH_Lock(FLASH);
}

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

void update_remote()
{
    memcpy(data_buffer, remote, (6 * 4));
    flash_set_page(FLASH_PAGE_REMOTE);
}

void update_devices()
{
    memcpy(data_buffer, devices1, (30 * 4));
    flash_set_page(FLASH_PAGE_DEVICES1);
    memcpy(data_buffer, devices2, (30 * 4));
    flash_set_page(FLASH_PAGE_DEVICES2);
}

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

void flash_init()
{
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    LL_FLASH_Unlock(FLASH);
    LL_FLASH_TIMMING_SEQUENCE_CONFIG_24M();

    remote_cur = HW32_REG(FLASH_PAGE_CONTROL);
    if (remote_cur >= REMOTE_NUM)
        remote_cur = 0;
    devices_cur = HW32_REG(FLASH_PAGE_CONTROL + (4 * 1));
    if (remote_cur >= DEVICE_NUM)
        remote_cur = 0;
    sos_time = HW32_REG(FLASH_PAGE_CONTROL + (4 * 2));
    if(sos_time > 4)
        sos_time = 4;
    sos_mode = HW32_REG(FLASH_PAGE_CONTROL + (4 * 3));
    if(sos_mode > 1)
        sos_mode = 0;
    speak_mode = HW32_REG(FLASH_PAGE_CONTROL + (4 * 4));
    if(speak_mode > 3)
        speak_mode = 0;
    sos_urgency = HW32_REG(FLASH_PAGE_CONTROL + (4 * 5));
    if(sos_urgency > 1)
        sos_urgency = 0;

    for (uint8_t i = 0; i < REMOTE_NUM; i++)
    {
        remote[i] = HW32_REG(FLASH_PAGE_REMOTE + (4 * i));
    }

    for (uint8_t i = 0; i < 30; i++)
    {
        devices1[i] = HW32_REG(FLASH_PAGE_DEVICES1 + (4 * i));
    }
    for (uint8_t i = 0; i < 30; i++)
    {
        devices2[i] = HW32_REG(FLASH_PAGE_DEVICES2 + (4 * i));
    }

    LL_FLASH_Lock(FLASH);
}
