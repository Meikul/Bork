#ifndef PID_H_
#define PID_H_

#include <API.h>


typedef void * PID;
typedef int(*InputFunction)();
typedef void(*OutputFunction)(int);
// typedef int(*InputFunction)(unsigned char);

/**
 * Creates pid controller
 * @param  kp
 * @param  ki
 * @param  kd
 * @param  process    initial input
 * @param  initTarget initial target
 * @return            PID
 */
PID initPid(double kp, double ki, double kd, int process, int initTarget);

/**
 * Creates pid controller with input function
 * @param  kp
 * @param  ki
 * @param  kd
 * @param  input      input function
 * @param  initTarget initial target
 * @return            PID
 */
PID initPid(double kp, double ki, double kd, InputFunction input, int initTarget);

/**
 * Creates pid controller with input function
 * @param  kp
 * @param  ki
 * @param  kd
 * @param  input      input function
 * @param  output     output function
 * @param  initTarget initial target
 * @return            PID
 */
PID initPid(double kp, double ki, double kd, InputFunction input, OutputFunction output, int initTarget);


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
 * Sets integral limits
 * @param pidptr     pid
 * @param innerLimit inner limit for integral to accumulate
 * @param outerLimit outer limit for integral to reset
 */
void setIntegLimits(PID pidptr, int innerLimit, int outerLimit);

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

/**
 * Calculates pid output
 * @return output
 */
int calculatePid(PID pid);

/**
 * Set input function
 * @param pid
 * @param input
 */
void setInputFunction(PID pid, InputFunction input);

/**
 * Set output function
 * @param pid
 * @param output
 */
void setOutputFunction(PID pid, OutputFunction output);

/**
 * Starts running pid in task
 * @param pid controller
 */
void startTask(PID pid);

/**
 * Starts running pid in task
 * @param  pid    controller
 * @param  output function to output to
 */
void startTask(PID pid, OutputFunction output);

/**
 * Starts running pid in task
 * @param  pid    controller
 * @param  input  function to get process variable from
 * @param  output function to output to
 */
void startTask(PID pidptr, InputFunction input, OutputFunction output);

/**
 * Pause pid task
 * @param pid controller
 */
void pauseTask(PID pid);

/**
 * Resume pid task
 * @param pid
 */
void resumeTask(PID pid);

/**
 * Kill pid task
 * @param pid controller
 */
void killTask(PID pid);


#endif
