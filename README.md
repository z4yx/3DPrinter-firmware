基于STM32单片机的3D打印机控制器固件代码

###目录结构

 - Libraries: STM32固件函数库3.5
 - Project/include: 工程头文件目录
 - Project/src: 工程源代码目录
 - Project/src/fatfs: FatFs文件系统库
 - Project/src/usb_library: USB设备库
 - Project/src/lowlevel: 底层设备操作封装
 
###组件说明
详细参数及细节请参考源文件

######ADC (adc.h)
A/D转换器操作封装(使用ADC1)

- ADC_Config: ADC1初始化，指定通道数。注意，初始化之后必须分别配置每个通道。
- ADC_Channel_Config: 配置一个ADC通道
- ADC_Start: 校准并启动ADC转换
- ADC_Read_Value: 读取最近一次转换结果

#####Common (common.h)
项目的公共函数和参数定义

- DBG_MSG: 通过串口输出调试信息
- ERR_MSG: 通过串口输出错误信息
- RCC_GPIOClockCmd: 开启某个GPIO区域的时钟(用于简化代码)
- Timer_16bit_Calc: 根据频率(Hz)计算TIM的参数

#####Fan (fanControl.h)
挤出器散热风扇控制

- Fan_Config: I/O端口配置
- Fan_Enable: 开启或关闭风扇

#####FileManager (gfiles.h)
G代码文件管理

- FileManager_Init: 初始化文件系统及SD卡
- FileManager_GetCardInfo: 获取存储卡信息
- FileManager_SDCardAvailable: 存储卡是否被使用
- FileManager_ListGFiles: 列举SD卡根目录中的G代码文件
- FileManager_OpenGcode: 打开某个G代码文件
- FileManager_Close: 关闭已经打开的文件
- FileManager_GetLine: 从当前打开文件中读取一行,包含换行符

#####LED (led.h)
实现板载LED的控制,常用于指示状态

- LED_Config: I/O端口配置
- LED_Enable: 点亮或关闭LED

#####LimitSwitch (limitSwitch.h)
限位开关检测,开关编号见枚举量

- LimitSwitch_Config: I/O端口配置
- LimitSwitch_Pressed: 获取限位开关状态

#####MAX6675 (max6675.h)
MAX6675热电偶转换芯片读取

- MAX6675_Config: MAX6675相关引脚初始化
- MAX6675_Read_Value: 获取温度整数值，如果热电偶断开，返回-1

#####Move (move.h)
运动控制模块,轴编号见枚举量

- Move_Init: 初始化函数,包括电机及限位开关初始化
- Move_XYZ_Ready: 检测三轴均处于待命状态
- Move_Home: 启动回原点过程
- Move_RelativeMove: 三轴相对移动及挤出器旋转,相对位移量
- Move_AbsoluteMove: 三轴绝对移动及挤出器旋转,绝对位置
- Move_SetCurrentPos: 强制设定当前所在位置

#####PWM (pwmOutput.h)
PWM输出控制模块，使用TIM2定时器，实现两组加热控制

- PWM_Init: 定时器及I/O初始化,指定PWM频率
- PWM_Channel: 设置某个通道的占空比

#####SysTick (systick.h)
系统滴答定时器，产生系统标准时间，实现延时等待

- SysTick_Init: 嘀嗒定时器初始化
- GetSystemTick: 获取系统时间,即自系统启动以来,经过的毫秒数
- Delay_ms: 毫秒级延时，误差±1ms
- Delay_us: 微秒级延时

#####USART (usart.h)
USART收发与调试输出

- USART_Config: 配置串口
- USART_putchar: 通过串口发送一个字节
- USART_getchar: 通过串口接收一个字节
- USART_ReadInt: 读入一个整数值
- USART_printf: 通过串口发送格式化字符串

#####USBDevice (usb.h)
USB存储设备相关函数

- USBDevice_Config: 配置USB
- USBDevice_PlugIn: 检测USB是否插入
- USBDevice_Connect: 与主机连接
- USBDevice_Disconnect: 与主机断开连接


###参数调整

- hardwareDef.h: 所有硬件连接相关宏定义
- configure.h: 程序中可调参数汇总

###硬件调试步骤

1. 连接串口并确认有调试信息输出
2. 测试限位开关被触发后,程序是否相应
3. 根据机械设计,修改电机与导轨相关参数
4. 测试电机是否按指定位移运动,方向是否正确
5. 启动加热组件,检测升温情况,查看温度传感器数据是否正确
6. 测试存储卡能否读取文件,G代码解析是否正确
7. 运行简单G代码,观察回原点,加热,挤出等过程是否正常
8. 根据打印效果继续调整工艺参数(温度,进给等)

###FAQ

#####功率驱动管为何发热严重
检查电路连接是否正确,有无短路.所驱动的负载功率是否过大,有没有超过额定电压.

#####热电偶度数不稳定或错误的原因
热电偶产生的信号非常微弱(数百微伏量级),在远距离传输中极易受到干扰.因此热电偶连线必须用屏蔽线,并保证屏蔽层与电源地相连.
