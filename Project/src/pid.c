/*
 * =====================================================================================
 *
 *       Filename:  pid.c
 *
 *    Description:  PID控制算法
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
#include "pid.h"

void PID_Init(struct PIDController *pid, int kp, int ki, int kd, int initSum)
{
	for (int i = 0; i < DIFF_SAMPLE_SIZE; ++i)
	{
		pid->diffHistory[i] = 0;
	}
	pid->Kp = kp;
	pid->Ki = ki;
	pid->Kd = kd;
	pid->deltaSum = initSum;
	pid->diffIndex = pid->last = pid->diffSum = 0;
}

//根据当前差值计算输出值
int PID_Update(struct PIDController *pid, int delta)
{
	int output = 0, diff;

	pid->deltaSum += delta;
	if(pid->deltaSum > PID_SUM_MAX)
		pid->deltaSum = PID_SUM_MAX;
	if(pid->deltaSum < PID_SUM_MIN)
		pid->deltaSum = PID_SUM_MIN;

	diff = delta - pid->last;
	pid->last = delta;

	pid->diffSum += diff;
	pid->diffSum -= pid->diffHistory[pid->diffIndex];
	
	pid->diffHistory[pid->diffIndex] = diff;
	pid->diffIndex = (pid->diffIndex + 1) % DIFF_SAMPLE_SIZE;

	DBG_MSG("P: %d, I: %d, D: %d", delta, pid->deltaSum, pid->diffSum);

	
	output += pid->Kp * delta;
	output += pid->Ki * pid->deltaSum;
	output += pid->Kd * pid->diffSum;

	return output;
}
