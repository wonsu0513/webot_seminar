#include <stdio.h>
#include <stdlib.h>

#include <webots/robot.h>
#include <webots/motor.h>
#include <webots/keyboard.h>
#include <webots/distance_sensor.h>
#include <webots/supervisor.h>
#include <webots/mouse.h>


#define TIME_STEP 64
#define MAX_SPEED 6.28

  
int main(int argc, char **argv)
{
  wb_robot_init();
 
  WbNodeRef mybot = wb_supervisor_node_get_from_def("e-puck");
  WbFieldRef translationField = wb_supervisor_node_get_field(mybot, "translation");


   WbDeviceTag left_motor = wb_robot_get_device("left wheel motor");
   WbDeviceTag right_motor = wb_robot_get_device("right wheel motor");
   WbDeviceTag wonsu_ps0 = wb_robot_get_device("ps0");
   WbDeviceTag wonsu_ps7 = wb_robot_get_device("ps7");

   wb_distance_sensor_enable(wonsu_ps0, TIME_STEP);
   wb_distance_sensor_enable(wonsu_ps7, TIME_STEP);

   wb_motor_set_position(left_motor, INFINITY);
   wb_motor_set_position(right_motor, INFINITY);
   wb_motor_set_velocity(left_motor, 0);
   wb_motor_set_velocity(right_motor, 0);
   
   wb_keyboard_enable(TIME_STEP);
   
   wb_mouse_enable(TIME_STEP);
   wb_mouse_enable_3d_position();
   
   
   WbUserInputEvent event_type;


   int key_value = 0;
   double ps0_data = 0;
   double ps7_data = 0;
   double left_speed= 0;
   double right_speed = 0;
   

   while (wb_robot_step(TIME_STEP) != -1) {
    const double *translation = wb_supervisor_field_get_sf_vec3f(translationField);

    const WbMouseState mouse_state = wb_mouse_get_state();
    if (mouse_state.left) {
        wb_motor_set_velocity(left_motor, 0);
        wb_motor_set_velocity(right_motor, 0); 
        printf("Click detected at:\n");
        printf("u: %g\n", mouse_state.u);
        printf("v: %g\n", mouse_state.v);
        printf("x: %g\n", mouse_state.x);
        printf("y: %g\n", mouse_state.y);
        printf("z: %g\n", mouse_state.z);
        printf("%f, %f \n", translation[0], translation[2]);
        
     }
  

    else
    {
      ps0_data = wb_distance_sensor_get_value(wonsu_ps0);
      ps7_data = wb_distance_sensor_get_value(wonsu_ps7);
      //printf("ps0 = %f, ps7 = %f \n", ps0_data, ps7_data);  
  
      key_value =  wb_keyboard_get_key();
       
      
      if(key_value != -1)
      { 
        if (key_value == 315){
          wb_motor_set_velocity(left_motor, 0.3*MAX_SPEED);
          wb_motor_set_velocity(right_motor, 0.3*MAX_SPEED);
    
        }
        else if(key_value == 317){
          wb_motor_set_velocity(left_motor, -0.3*MAX_SPEED);
          wb_motor_set_velocity(right_motor, -0.3*MAX_SPEED);
    
        }
        else if(key_value == 314){
          wb_motor_set_velocity(left_motor, -0.3*MAX_SPEED);
          wb_motor_set_velocity(right_motor, 0.3*MAX_SPEED);
        }
        else if(key_value == 316){
          wb_motor_set_velocity(left_motor, 0.3*MAX_SPEED);
          wb_motor_set_velocity(right_motor, -0.3*MAX_SPEED);
        }
        else{
          wb_motor_set_velocity(left_motor, 0);
          wb_motor_set_velocity(right_motor, 0);
         }
       }
       
       else
       {
         if (ps7_data > 70) {
            // turn right
            left_speed += 0.5 * MAX_SPEED;
            right_speed -= 0.5 * MAX_SPEED;
          } 
          else if (ps0_data > 70) {
            // turn left
            left_speed -= 0.5 * MAX_SPEED;
            right_speed += 0.5 * MAX_SPEED;
          }
          else if( ps7_data > 100 || ps0_data > 100){
             left_speed = -MAX_SPEED;
             right_speed = -MAX_SPEED;
          }
          else
          {
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
          
          wb_motor_set_velocity(left_motor, left_speed);
          wb_motor_set_velocity(right_motor, right_speed);
      }  
     }         
  };

  wb_robot_cleanup();

  return 0;
}
