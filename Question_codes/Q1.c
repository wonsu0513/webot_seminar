#include <stdio.h>
#include <stdlib.h>

#include <webots/robot.h>
#include <webots/motor.h>

#define TIME_STEP 64
#define MAX_SPEED 6.28

bool wonsu_zzang = false;

int main(int argc, char **argv)
{
   /* necessary to initialize webots stuff */
   wb_robot_init();

   // Setting motor devices
   WbDeviceTag left_motor = wb_robot_get_device("left wheel motor");
   WbDeviceTag right_motor = wb_robot_get_device("right wheel motor");   
   
   // Setting the properties of each motor
   wb_motor_set_position(left_motor, INFINITY);
   wb_motor_set_position(right_motor, INFINITY);
   wb_motor_set_velocity(left_motor, 0);
   wb_motor_set_velocity(right_motor, 0);
   
   /* main loop
   * Perform simulation steps of TIME_STEP milliseconds
   * and leave the loop when the simulation is over
   */   
   while (wb_robot_step(TIME_STEP) != -1) {
      // Control a velocity of the left motor   
      wb_motor_set_velocity(left_motor, MAX_SPEED);
	  
      // Control a velocity of the right motor   
      wb_motor_set_velocity(right_motor, MAX_SPEED);   
  };

  /* Enter your cleanup code here */
  /* This is necessary to cleanup webots resources */
  wb_robot_cleanup();

  return 0;
}


