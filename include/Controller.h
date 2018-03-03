#ifndef CONTROL_H_
#define CONTROL_H_

// #include <API.h>

// typedef void * Controller;
//
// Controller initController(double kp, double ki, double kd);

// class Controller{
// private:
//   double m_kp;
//   double m_ki;
//   double m_kd;
//   double m_integ;
//   double m_diff;
//   double m_prevErr;
//   double m_integThreshold;
//   bool m_taskRunning;
//   int m_target;
//   TaskHandle m_loopTask;
//
//   static void m_loopFunctionErr(void* args);
//   static void m_loopFunctionTarget(void* args);
//   static int m_staticGet(Controller* object);
//   // static array of bools to avoid motor interference
//   // static bool tasksRegister;
// public:
//   Controller(double kp, double ki, double kd);
//   ~Controller();
//
//   void startTask(int (*errorFunction)(), void (*outputFunction)(int));
//   void startTask(int (*valueFunction)(), void (*outputFunction)(int), int target);
//   // void startTask(int (*valueFunction)(void* args), void* args, void (*outputFunction)(int), int target);
//   bool killTask();
//   bool isRunning();
//   void setTarget(int target);
//   int getTarget();
//   void setIntegThreshold(int threshold);
//
//   // void startTask(double (*valueFunction)(), double target);
//   void setConstants(double kp, double ki, double kd);
//   static int get(int error);
//
// };
//
#endif
