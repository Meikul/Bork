#include "Controller.h"

// Controller::Controller(){
//   Controller(0,0,0);
// }

Controller::Controller(double kp):
  m_kp(kp), m_ki(0), m_kd(0), m_integ(0), m_diff(0),
  m_prevErr(1000), m_integThreshold(100), m_taskRunning(false)
{}

Controller::Controller(double kp, double kd):
  m_kp(kp), m_ki(0), m_kd(kd), m_integ(0), m_diff(0),
  m_prevErr(1000), m_integThreshold(100), m_taskRunning(false)
{}

Controller::Controller(double kp, double ki, double kd):
  m_kp(kp), m_ki(ki), m_kd(kd), m_integ(0), m_diff(0),
  m_prevErr(1000), m_integThreshold(100), m_taskRunning(false)
{}

void Controller::setConstants(double kp, double ki, double kd){
  m_kp = kp;
  m_ki = ki;
  m_kd = kd;
}


int Controller::get(int error){
  double absError = (error < 0) ? -error : error;

  if(absError < m_integThreshold) m_integ += error;
  else m_integ = 0;

  if(m_prevErr == 1000) m_prevErr = error; // uninitialized

  m_diff = error - m_prevErr;

  int pwr = (error * m_kp) + (m_integ * m_ki) + (m_diff * m_kd);

  if(pwr >= 127){
    pwr = 127;
    m_integ = 0;
  }
  else if(pwr < -127){
    pwr = -127;
    m_integ = 0;
  }
  else{
    m_integ += error;
  }


  m_prevErr = error;

  return pwr;
}

bool Controller::isRunning(){
  return m_taskRunning;
}
