#include "Controller.h"

Controller::Controller(double kp, double ki, double kd):
  m_kp(kp), m_ki(ki), m_kd(kd), m_integ(0), m_diff(0),
  m_prevErr(100000), m_integThreshold(0), m_taskRunning(false), m_target(0)
{}

void Controller::setConstants(double kp, double ki, double kd){
  m_kp = kp;
  m_ki = ki;
  m_kd = kd;
}

void Controller::setTarget(int target){
  m_target = target;
}

int Controller::getTarget(){
  return m_target;
}

// void Controller::startTask(int (*errorFunction)(), void (*outputFunction)(int)){
//   if(m_taskRunning) killTask();
//   m_taskRunning = true;
//   m_loopTask = taskCreate(m_loopFunctionErr, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
// }
//
// void Controller::startTask(int (*valueFunction)(), void (*outputFunction)(int), int target){
//   if(m_taskRunning) killTask();
//   m_taskRunning = true;
// }
//
// void Controller::m_loopFunctionErr(void* args){
//   bool lastIsAuton = isAutonomous();
//   while(isAutonomous() == lastIsAuton){
//     int error = errorFunction();
//     int out = get(error);
//     outputFunction(out);
//     lastIsAuton = isAutonomous();
//     delay(20);
//   }
// }
//
// void Controller::m_loopFunctionTarget(void* args){
//   bool lastIsAuton = isAutonomous();
//   m_target = target;
//   while(isAutonomous() == lastIsAuton){
//     int error = valueFunction() - m_target;
//     int out = get(error);
//     outputFunction(out);
//     lastIsAuton = isAutonomous();
//     delay(20);
//   }
// }

bool Controller::killTask(){
  if(m_taskRunning){
    taskDelete(m_loopTask);
    m_taskRunning = false;
    return true;
  }
  return false;
}

int Controller::get(int error){
  double absError = (error < 0) ? -error : error;

  if(absError < m_integThreshold) m_integ += error;
  else m_integ = 0;

  if(m_prevErr == 100000) m_prevErr = error; // uninitialized

  m_diff = error - m_prevErr;

  int pwr = (error * m_kp) + (m_integ * m_ki) + (m_diff * m_kd);

  if(m_integThreshold != 0){
    if(abs(error) <= m_integThreshold) m_integ += error;
    else m_integ = 0;
  }
  else{
    if(abs(pwr) >= 127) m_integ += error;
    else m_integ = 0;
  }

  if(pwr >= 127){
    pwr = 127;
  }
  else if(pwr < -127){
    pwr = -127;
  }

  m_prevErr = error;

  return pwr;
}

bool Controller::isRunning(){
  return m_taskRunning;
}

void Controller::setIntegThreshold(int threshold){
  m_integThreshold = threshold;
}
