#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <webots/robot.h>
#include <webots/motor.h>
#include <webots/keyboard.h>
#include <webots/distance_sensor.h>
#include <webots/supervisor.h>
#include <webots/mouse.h>


#define TIME_STEP 64
#define MAX_SPEED 6.28

bool wonsu_zzang = false;


int main(int argc, char **argv)
{
   wb_robot_init();
   // Robot setup
   WbNodeRef mybot = wb_supervisor_node_get_from_def("e-puck");
   
  // Robot Setup to read current positions
   WbFieldRef translationField = wb_supervisor_node_get_field(mybot, "translation");
   WbFieldRef rotationField = wb_supervisor_node_get_field(mybot, "rotation");

  // Setting motor devices
   WbDeviceTag left_motor = wb_robot_get_device("left wheel motor");
   WbDeviceTag right_motor = wb_robot_get_device("right wheel motor");
   
   // Setting distance sensors
   WbDeviceTag wonsu_ps0 = wb_robot_get_device("ps0");
   WbDeviceTag wonsu_ps7 = wb_robot_get_device("ps7");

   // Enable distance sensors
   wb_distance_sensor_enable(wonsu_ps0, TIME_STEP);
   wb_distance_sensor_enable(wonsu_ps7, TIME_STEP);

   // Setting the properties of each motor
   wb_motor_set_position(left_motor, INFINITY);
   wb_motor_set_position(right_motor, INFINITY);
   wb_motor_set_velocity(left_motor, 0);
   wb_motor_set_velocity(right_motor, 0);
   
   // Enable keyboard input
   wb_keyboard_enable(TIME_STEP);
   
   
   WbUserInputEvent event_type;

   int key_value = 0;
   double ps0_data = 0;
   double ps7_data = 0;
   double left_speed= 0;
   double right_speed = 0;

   double initial_position[3] = {0.1, 0.0, 0.1 };
   double initial_angle[4] = {0.0, 1.0, 0.0, 0.0};

   // Setup intitial potiions
   wb_supervisor_field_set_sf_vec3f(translationField, initial_position);

   // Setup intitial heading angle
   //wb_supervisor_field_set_sf_rotation(rotationField, initial_angle);

   while (wb_robot_step(TIME_STEP) != -1) {
      // For reading robot's current positions and head angle
      const double *translation = wb_supervisor_field_get_sf_vec3f(translationField);
      const double *rotation = wb_supervisor_field_get_sf_rotation(rotationField);

      double robot_current_position[3] = {translation[0], translation[2], rotation[3]};
	  
      // Read each data of the distance sensors
      ps0_data = wb_distance_sensor_get_value(wonsu_ps0);
      ps7_data = wb_distance_sensor_get_value(wonsu_ps7);
    
      //printf("ps0 = %f, ps7 = %f \n", ps0_data, ps7_data);  
      
	  
	  /* Obstacle avoidance */
      if (ps7_data > 60) {
         // turn right
         left_speed += 0.8 * MAX_SPEED;
         right_speed -= 0.8 * MAX_SPEED;
      } 
      else if (ps0_data > 60) {
         // turn left
         left_speed -= 0.8 * MAX_SPEED;
         right_speed += 0.8 * MAX_SPEED;
      }
      
	  else if( ps7_data > 60 || ps0_data > 60){
         left_speed = -MAX_SPEED;
         right_speed = -MAX_SPEED;
      }
      else{
	     //Random walk part!!!
		 
         left_speed = (int)rand()%((int)MAX_SPEED + 1 - 0) + 0;
         right_speed = (int)rand()%((int)MAX_SPEED + 1 - 0) + 0;
      }
          
      if (left_speed > MAX_SPEED){
         left_speed = MAX_SPEED;
      }
      if (left_speed < -MAX_SPEED){
         left_speed = -MAX_SPEED;
      }
      if (right_speed > MAX_SPEED){
         right_speed = MAX_SPEED;
      }
      if (right_speed < -MAX_SPEED){
         right_speed =  -MAX_SPEED;
      }         
      // Control a velocity of the left motor   
      wb_motor_set_velocity(left_motor, left_speed);
	  
	  // Control a velocity of the right motor   
      wb_motor_set_velocity(right_motor, right_speed);
  };

  wb_robot_cleanup();

  return 0;
}
