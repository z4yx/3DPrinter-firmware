/*
 * =====================================================================================
 *
 *       Filename:  hostctrl.c
 *
 *    Description:  上位机控制请求处理
 *
 *        Version:  
 *        Created:  
 *       Revision:  
 *       Compiler:  
 *
 *         Author:  zhangyuxiang
 *   Organization:  
 *
 * =====================================================================================
 */

#include "common.h"
#include "systick.h"
#include "hostctrl.h"

#define CMD_BUF_LEN 8
#define PARAM_BUF_LEN 8

enum {PARSE_INITIAL, PARSE_CMD, PARSE_PARAM};
static uint8_t parse_stage;
static bool cmd_received;
static char cmd_buf[CMD_BUF_LEN], param_buf[PARAM_BUF_LEN];

void HostCtrl_Init(void)
{
	// USART_RxInt_Config(true);
	parse_stage = PARSE_INITIAL;
	cmd_received = false;
}

bool HostCtrl_GetCmd(char **p_cmd, char **p_param)
{
	if(cmd_received){
		*p_param = param_buf;
		*p_cmd = cmd_buf;
	}
	return cmd_received;
}

//指令已经处理完,准备接收下一指令
void HostCtrl_CmdProcessed()
{
	cmd_received = false;
}

static void parse_host_cmd(uint8_t byte)
{
	static int cmd_buf_i = 0, param_buf_i = 0;
	switch(parse_stage){
		case PARSE_INITIAL:
			//如果有尚未处理的指令,则丢弃本次指令
			if(byte == '!' && !cmd_received){
				parse_stage = PARSE_CMD;
				cmd_buf_i = param_buf_i = 0;
			}
			break;
		case PARSE_CMD:
			if('A'<=byte && byte<='Z'){
				if(cmd_buf_i < CMD_BUF_LEN-2)
					cmd_buf[cmd_buf_i++] = byte;
			}else if('#' == byte){
				cmd_buf[cmd_buf_i] = '\0';
				// cmd_received = true;
				parse_stage = PARSE_PARAM;
			}else{
				//无效指令
				parse_stage = PARSE_INITIAL;
			}
			break;
		case PARSE_PARAM:
			if('\r' == byte || '\n' == byte){
				param_buf[param_buf_i] = '\0';
				cmd_received = true;
				parse_stage = PARSE_INITIAL;
			}else{
				if(param_buf_i < PARAM_BUF_LEN-2)
					param_buf[param_buf_i++] = byte;
			}
			break;
	}
}

void HostCtrl_Task(void)
{
	if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != SET)
		return;
	uint8_t byte = USART_getchar();
	parse_host_cmd(byte);
}

void HostCtrl_Interrupt(void)
{
	uint8_t byte = USART_getchar();
	USART_putchar(byte);
	USART_ClearITPendingBit(USART1, USART_FLAG_RXNE);
}