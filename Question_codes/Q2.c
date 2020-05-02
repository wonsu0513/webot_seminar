#include <stdio.h>
#include <stdlib.h>
#include <webots/robot.h>
#include <webots/motor.h>
#include <webots/keyboard.h>
#define TIME_STEP 64
#define MAX_SPEED 6.28

bool wonsu_zzang = false;


int main(int argc, char **argv)
{
   /* necessary to initialize webots stuff */
   wb_robot_init();
   // Setup motor devices
   WbDeviceTag left_motor = wb_robot_get_device("left wheel motor");
   WbDeviceTag right_motor = wb_robot_get_device("right wheel motor");
   
   // Setting the properties of each motor
   wb_motor_set_position(left_motor, INFINITY);
   wb_motor_set_position(right_motor, INFINITY);
   wb_motor_set_velocity(left_motor, 0);
   wb_motor_set_velocity(right_motor, 0);
   
   // Enable the keyboard function
   wb_keyboard_enable(TIME_STEP);
   int key_value = 0;
   

   while (wb_robot_step(TIME_STEP) != -1) {
      //Read a key value
	  key_value =  wb_keyboard_get_key(); 
      
	  //printf("%d \n", wb_keyboard_get_key()); // To check the pushed keyboard value 
	  
	  
	  // Move forward
      if (key_value == 315){
         wb_motor_set_velocity(left_motor, 0.3*MAX_SPEED);
         wb_motor_set_velocity(right_motor, 0.3*MAX_SPEED);
      }
	  
	  // Move backward
      else if(key_value == 317){
         wb_motor_set_velocity(left_motor, -0.3*MAX_SPEED);
         wb_motor_set_velocity(right_motor, -0.3*MAX_SPEED);
      }

      // Rotate ccw
      else if(key_value == 314){
         wb_motor_set_velocity(left_motor, -0.3*MAX_SPEED);
         wb_motor_set_velocity(right_motor, 0.3*MAX_SPEED);
      }
	  
	  // Rotate cw
      else if(key_value == 316){
         wb_motor_set_velocity(left_motor, 0.3*MAX_SPEED);
         wb_motor_set_velocity(right_motor, -0.3*MAX_SPEED);
      }
	  // Stop
      else{
         wb_motor_set_velocity(left_motor, 0);
         wb_motor_set_velocity(right_motor, 0);
      }         
   };
   
   wb_robot_cleanup();
   return 0;
} 		




