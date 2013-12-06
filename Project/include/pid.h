#ifndef __PID__H__
#define __PID__H__

#define DIFF_SAMPLE_SIZE 4
#define PID_SUM_MAX 1500
#define PID_SUM_MIN 0

struct PIDController {
	int Kp, Kd, Ki;
	int deltaSum, diffSum, last, diffIndex;
	int diffHistory[DIFF_SAMPLE_SIZE];
};

void PID_Init(struct PIDController *pid, int kp, int ki, int kd, int initSum);
int PID_Update(struct PIDController *pid, int delta);

#endif /* __PID__H__ */