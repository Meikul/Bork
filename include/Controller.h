#ifndef CONTROL_H_
#define CONTROL_H_

#include <API.h>

class Controller{
private:
  double m_kp;
  double m_ki;
  double m_kd;
  double m_integ;
  double m_diff;
  double m_prevErr;
  double m_integThreshold;
  bool m_taskRunning;
  // static array of bools to avoid motor interference
  // static bool tasksRegister;
public:
  Controller(double kp);
  Controller(double kp, double kd);
  Controller(double kp, double ki, double kd);
  // void startTask(double (*errorFunction)());
  // void startTask(int (*errorFunction)());
  void startTask(int (*valueFunction)(), void (*outputFunction)(int), int target);
  void startTask(int (*valueFunction)(void* args), void* args, void (*outputFunction)(int), int target);
  bool killTask();
  bool isRunning();

  // void startTask(double (*valueFunction)(), double target);
  void setConstants(double kp, double ki, double kd);
  int get(int error);

};

#endif
