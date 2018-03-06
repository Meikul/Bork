#ifndef PID_H_
#define PID_H_

#include <API.h>


typedef void * PID;
typedef int(*IntFunction)();
// typedef int(*IntFunction)(unsigned char);

/**
 * Creates pid controller
 * @param  kp
 * @param  ki
 * @param  kd
 * @param  input inputFunction pointer
 * @return       PID
 */
PID initPid(double kp, double ki, double kd, int process, int initTarget);

/**
 * Gains
 * @param pid controller
 * @param kp
 * @param ki
 * @param kd
 */
void setGains(PID pid, double kp, double ki, double kd);

/**
 * Gets current target
 * @param pid controller
 */
int getTarget(PID pid);

/**
 * Sets target of specified PID
 * @param controller PID to set
 * @param target     set point
 */
void setTarget(PID pid, int target);

/**
 * Calculates pid output
 * @return output
 */
int calculatePid(PID pid, int process);


#endif
