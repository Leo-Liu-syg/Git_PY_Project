#ifndef __SPEAK_H__
#define __SPEAK_H__

#include <stdint.h>

#include "main.h"

enum speak_58042
{
    SPEAK_STOP = 1,
    SPEAK_NORMAL,              // 正常模式切换成功
    SPEAK_URGENCY,             // 紧急模式切换成功
    SPEAK_STUDY_REMOTE,        // 学习遥控器
    SPEAK_STUDY_DETECTOR = 5,  // 学习探测器
    SPEAK_STUDY_SUCCESS,       // 学习成功
    SPEAK_WARNING,             // 警报声
    SPEAK_FIRE_WARNING,        // 发生火灾报警，请迅速按演习路线有序撤离+警报声
    SPEAK_URGENCY_WARNING,     // 发生紧急求助+警报声
    SPEAK_SECURE_WARNING = 10, // 安全报警，安全报警，请立即做好安全防护措施+警报声
    SPEAK_RECOVERY,            // 删除成功恢复出厂
    SPEAK_PLACE,               // 布防
    SPEAK_UNPLACE,             // 撤防
    SPEAK_CANCEL,              // 取消对码
};

void speak(uint8_t code);

#endif
