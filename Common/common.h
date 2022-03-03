///
///
/// 控制系统命令定义
/// 命令头组成如下
/// Command ID (2 字节) + Package Length (2 字节) + IP (4 字节) + Parameter (任意长度)
///
///

#include <stdint.h>

// 被动注册 （服务器 4 字节地址 + 2 字节端口 ）
const uint16_t CMD_PASSIVE_REGISTRATE                       = 0X3001;
// 被动注册回复 (客户端 4 字节地址)
const uint16_t CMD_PASSIVE_REGISTRATE_RESPONSE              = 0X3002;

// 启动目标程序（无参数，目标程序目录由配置文件确定）
const uint16_t CMD_START_PROGRAM                            = 0X3003;
// 启动目标程序回复 （ 任意字节消息 ）
const uint16_t CMD_START_PROGRAM_RESPONSE                   = 0X3004;

// 关闭目标程序（无参数，目标程序目录由配置文件确定）
const uint16_t CMD_STOP_PROGRAM                             = 0X3005;
// 关闭目标程序回复（ 任意字节消息 ）
const uint16_t CMD_STOP_PROGRAM_RESPONSE                    = 0X3006;

// 发送键盘按键点击（按键系统标志（ 1 字节 ））（例子：VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)）
const uint16_t CMD_KEYBOARD_PRESS                           = 0X3007;
const uint16_t CMD_KEYBOARD_PRESS_RESPONSE                  = 0X3008;

// 发送鼠标移动（鼠标移动位置，绝对位置；4 字节组成，前两字节表示 X ,后两个字节表示 Y）
const uint16_t CMD_MOVE_MOUSE                               = 0X3009;
const uint16_t CMD_MOVE_MOUSE_RESPONSE                      = 0X3010;

// 发送鼠标单击（无参数）
const uint16_t CMD_CLICK_MOUSEM                             = 0X3011;
const uint16_t CMD_CLICK_MOUSEM_RESPONSE                    = 0X3012;

// 关闭计算机 （关机延迟时间（秒数）（ 2 字节 ））
const uint16_t CMD_SHUTDOWN_COMPUTER                        = 0X3013;
// 关闭计算机回复( 1 字节状态 )
const uint16_t CMD_SHUTDOWN_COMPUTER_RESPONSE               = 0X3014;

// 服务端心跳检测
const uint16_t CMD_HEARTBEAT_DETECTION                      = 0X3015;
// 服务端心跳检测回复
const uint16_t CMD_HEARTBEAT_DETECTION_RESPONSE             = 0X3016;
