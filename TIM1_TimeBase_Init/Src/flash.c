/* 包含头文件 ------------------------------------------------------------------*/
#include "Flash.h"

/* 私有宏定义 ----------------------------------------------------------------*/
// 用户Flash操作起始地址
#define FLASH_USER_START_ADDR     0x08001000

/* 私有变量 -------------------------------------------------------------------*/
// 要写入Flash的数据数组（共64个32位数据）
uint32_t DATA[64] = {0x01010101, 0x23456789, 0x3456789A, 0x456789AB, 0x56789ABC, 0x6789ABCD, 0x789ABCDE, 0x89ABCDEF,
                     0x9ABCDEF0, 0xABCDEF01, 0xBCDEF012, 0xCDEF0123, 0xDEF01234, 0xEF012345, 0xF0123456, 0x01234567,
                     0x01010101, 0x23456789, 0x3456789A, 0x456789AB, 0x56789ABC, 0x6789ABCD, 0x789ABCDE, 0x89ABCDEF,
                     0x9ABCDEF0, 0xABCDEF01, 0xBCDEF012, 0xCDEF0123, 0xDEF01234, 0xEF012345, 0xF0123456, 0x01234567,
                     0x23456789, 0xaaaaaaaa, 0x55555555, 0x23456789, 0xaaaaaaaa, 0x55555555, 0x23456789, 0xaaaaaaaa,
                     0x23456789, 0xaaaaaaaa, 0x55555555, 0x23456789, 0xaaaaaaaa, 0x55555555, 0x23456789, 0xaaaaaaaa,
                     0x23456789, 0xaaaaaaaa, 0x55555555, 0x23456789, 0xaaaaaaaa, 0x55555555, 0x23456789, 0xaaaaaaaa,
                     0x23456789, 0xaaaaaaaa, 0x55555555, 0x23456789, 0xaaaaaaaa, 0x55555555, 0x23456789, 0xaaaaaaaa,
                    };




/**
  * @brief  擦除用户Flash区域（按页擦除）
  * @param  无
  * @retval 无
  */
static void Flash_Erase_process(void)
{
  uint32_t flash_program_start = FLASH_USER_START_ADDR ;                                /* 用户擦除页起始地址 */
  uint32_t flash_program_end = (FLASH_USER_START_ADDR + sizeof(DATA));                  /* 用户擦除页结束地址 */
  
  while (flash_program_start < flash_program_end)
  {
    /* 等待Flash不忙（BUSY=0） */
    while(LL_FLASH_IsActiveFlag_BUSY(FLASH)==1);

    /* 使能Flash操作完成中断（EOP） */
    LL_FLASH_EnableIT_EOP(FLASH);

    /* 使能页擦除模式 */
    LL_FLASH_EnablePageErase(FLASH);

    /* 设置要擦除的页地址 */
    LL_FLASH_SetEraseAddress(FLASH,flash_program_start);

    /* 等待Flash不忙 */
    while(LL_FLASH_IsActiveFlag_BUSY(FLASH)==1);

    /* 等待擦除操作完成（EOP=1） */
    while(LL_FLASH_IsActiveFlag_EOP(FLASH)==0);

    /* 清除操作完成标志位 */
    LL_FLASH_ClearFlag_EOP(FLASH);

    /* 关闭EOP中断 */
    LL_FLASH_DisableIT_EOP(FLASH);

    /* 关闭页擦除模式 */
    LL_FLASH_DisablePageErase(FLASH);
    
    flash_program_start += FLASH_PAGE_SIZE;                                           /* 指向下一页起始地址 */
  }
}

/**
  * @brief  向Flash写入数据
  * @param  无
  * @retval 无
  */
static void Flash_Write_process(void)
{
  uint32_t flash_program_start = FLASH_USER_START_ADDR ;                                /* Flash写入起始地址 */
  uint32_t flash_program_end = (FLASH_USER_START_ADDR + sizeof(DATA));                  /* Flash写入结束地址 */
  uint32_t *src = (uint32_t *)DATA;                                                     /* 指向数据数组的指针 */

  while (flash_program_start < flash_program_end)
  {
    /* 等待Flash不忙 */
    while(LL_FLASH_IsActiveFlag_BUSY(FLASH)==1);
    
    /* 使能EOP中断 */
    LL_FLASH_EnableIT_EOP(FLASH);

    /* 使能页编程模式 */
    LL_FLASH_EnablePageProgram(FLASH);

    /* 执行页编程（写入数据） */
    LL_FLASH_PageProgram(FLASH,flash_program_start,src);
    
    /* 等待Flash不忙 */
    while(LL_FLASH_IsActiveFlag_BUSY(FLASH)==1);
    
    /* 等待写入完成 */
    while(LL_FLASH_IsActiveFlag_EOP(FLASH)==0);
    
    /* 清除完成标志 */
    LL_FLASH_ClearFlag_EOP(FLASH);
   
    /* 关闭EOP中断 */
    LL_FLASH_DisableIT_EOP(FLASH);

    /* 关闭页编程模式 */
    LL_FLASH_DisablePageProgram(FLASH);
    
    flash_program_start += FLASH_PAGE_SIZE;                                           /* 指向下一页地址 */
    src += FLASH_PAGE_SIZE / 4;                                                       /* 指向下一组数据 */
  }
}

/**
  * @brief  检查Flash是否为空（全0xFFFFFFFF）
  * @param  无
  * @retval 无
  */
static void Flash_Check_If_Blank(void)
{
  uint32_t addr = 0;

  while (addr < sizeof(DATA))
  {
    // 读取地址数据，如果不是0xFFFFFFFF，说明非空，擦除失败
    if (0xFFFFFFFF != HW32_REG(FLASH_USER_START_ADDR + addr))
    {
      APP_ErrorHandler();
    }
    addr += 4;  // 按4字节（32位）检查
  }
}

/**
  * @brief  校验Flash写入的数据是否正确
  * @param  无
  * @retval 无
  */
static void Flash_Verify(void)
{
  uint32_t addr = 0;

  while (addr < sizeof(DATA))
  {
    // 对比：读取Flash数据 vs 原始数组数据
    if (DATA[addr / 4] != HW32_REG(FLASH_USER_START_ADDR + addr))
    {
      APP_ErrorHandler();  // 不一致则进入错误处理
    }
    addr += 4;
  }
}


void Flash_update_process(void)
{

  /* 解锁Flash（必须解锁才能擦写） */
  LL_FLASH_Unlock(FLASH);

  /* 配置Flash时序（24MHz系统时钟下使用） */
  LL_FLASH_TIMMING_SEQUENCE_CONFIG_24M();

  /* 擦除Flash */
  Flash_Erase_process();

  /* 检查Flash是否为空 */
  Flash_Check_If_Blank();

  /* 向Flash写入用户数据 */
  Flash_Write_process();

  /* 锁定Flash，禁止擦写操作 */
  LL_FLASH_Lock(FLASH);

  /* 校验写入的数据是否正确 */
  Flash_Verify();

}
/**
  * @brief  错误处理函数（出错时进入死循环）
  * @param  无
  * @retval 无
  */
void APP_ErrorHandler(void)
{
  /* 无限循环 */
  while (1)
  {
  }
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  断言失败时执行（报告文件名和行号）
  * @param  file：指向源文件名
  * @param  line：断言出错的行号
  * @retval 无
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* 可自定义打印错误信息：printf("参数错误：文件 %s 行号 %d\r\n", file, line) */
  
  /* 无限循环 */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/