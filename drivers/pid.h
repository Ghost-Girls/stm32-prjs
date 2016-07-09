#ifndef _PID_H_
#define _PID_H_


#include "conf.h"

//-----constants -----------------------------------
#define IG_MAX 400.0
#define I_MAX 200.0
#define PG_MAX 800.0
//#define DG_MAX 400.0
#define PID_OUT_MAX 1200.0
#define PID_Z_MAX	500
#define PID_Z_MIN 180
#define MAX_GYRO_ERROR 350.0
#define MAX_TARGET_ANGLE_M2 60.0//20.0*2
#define MAX_TARGET_ANGLE		30.0//20.0*2
#define MAX_TARGET_RATE_M2 200.0//100.0*2
#define MAX_TARGET_RATE 	 100.0
#define TARGET_Z_RATE 150
//------- Quad mode --------
#define _QUAD_X_
//#define _QUAD_PLUS_
//--------------------------
//-------------------------------------------------

//------- variables -------------------------------
extern uint8_t page,pid_pt,pid_temp[9],read_request;
extern int temp_data[9];
extern int temp_gain_vl;

extern float gyro_x_errorI,gyro_y_errorI;
extern float gyro_x_error,last_gyro_x_error,delta_gyro_x_error,x_rate_temp;
extern float gyro_y_error,last_gyro_y_error,delta_gyro_y_error,y_rate_temp;
extern float gyro_z_error,last_gyro_z_error,delta_gyro_z_error,pid_z_out_temp;

extern float imu_x_error,imu_x_target,last_imu_x_error,imu_x_errorI;
extern float imu_y_error,imu_y_target,last_imu_y_error,imu_y_errorI;

extern float Pg_temp,Ig_temp,Dg_temp;
extern float P_imu_temp,I_imu_temp,D_imu_temp;

extern float PID_x,PID_y,PID_z;
extern float loop_time;
//-------------------------------------------------


//------------- functions -------------------------
void PID_x_update(void);
void PID_y_update(void);
void PID_z_update(void);
void Set_pid_gain_value(void);
void Get_pid_gain_value(void);
void Send_pid_gain(void);
void MORTOR_output(void);
//-------------------------------------------------
#endif
