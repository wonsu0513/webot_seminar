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



double* robot_controller(double* start_pos, double* target_pos) {
    double d = 0.070;
    static double motor_velocity[2];
    
    //printf("robot: x = %f, y = %f, theta = %f \n",start_pos[0],start_pos[1],start_pos[2]);        
    //printf("mouse: x = %f, y = %f, theta = %f \n",target_pos[0],target_pos[1],target_pos[2]);        
	
    double k_rho = 5;        //should be larger than 0, i.e, k_rho > 0
    double k_alpha = 5;      // k_alpha - k_rho > 0
    double k_beta = -0.08;     // should be smaller than 0, i.e, k_beta < 0
	
    double delta_x = (target_pos[0]-start_pos[0]);
    double delta_z = (target_pos[1]-start_pos[1]);
	
    double rho = sqrt(pow(delta_x,2) + pow(delta_z,2));    

    double alpha = -start_pos[2] + atan2(delta_x, delta_z);
    
    //printf("controller: rho = %f, alpha = %f \n", rho, alpha);     
       
    // analge with traget and current position 
   
    if(alpha > M_PI){
        alpha = alpha - (2*M_PI);
    }
    else if (alpha < -M_PI){
        alpha = alpha + (2*M_PI);
    }
    
    
    double beta = -start_pos[2] - alpha;     
    
    ///printf("controller: rho = %f, alpha = %f, beta = %f \n", rho, alpha, beta);     

    double v = k_rho*rho; 
    double w = k_alpha*(alpha) + k_beta*beta;
    //printf("controller: v = %f, w = %f \n", v, w);     

    double vL = v + (d/2*w);    // P controller
    double vR = v - (d/2*w);      // P controller
    
    //printf("controller: vL = %f, vR = %f \n", vL, vR);
    
    //Clamp speed to the maximum speed.
    if(vL > MAX_SPEED){
    	vL = MAX_SPEED;
    }
    else if(vR > MAX_SPEED){
    	vR = MAX_SPEED;
    }
    if(vL < -MAX_SPEED){
	vL = -MAX_SPEED;
    }
    else if(vR < -MAX_SPEED){
	vR = -MAX_SPEED;
    }


    if(rho < 0.03){
      motor_velocity[0] = 0;
      motor_velocity[1] = 0;
    }
      
    else{
       motor_velocity[0] = vL;
       motor_velocity[1] = vR;
    }

    return motor_velocity;
}


int main(int argc, char **argv)
{
  wb_robot_init();
 
  WbNodeRef mybot = wb_supervisor_node_get_from_def("e-puck");
  WbFieldRef translationField = wb_supervisor_node_get_field(mybot, "translation");
  WbFieldRef rotationField = wb_supervisor_node_get_field(mybot, "rotation");


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

   double initial_position[3] = {0.1, 0.0, 0.1 };
   double initial_angle[4] = {0.0, 1.0, 0.0, 0.0};

   wb_supervisor_field_set_sf_vec3f(translationField, initial_position);
   //wb_supervisor_field_set_sf_rotation(rotationField, initial_angle);

   while (wb_robot_step(TIME_STEP) != -1) {
    
    const double *translation = wb_supervisor_field_get_sf_vec3f(translationField);
    const double *rotation = wb_supervisor_field_get_sf_rotation(rotationField);
    const WbMouseState mouse_state = wb_mouse_get_state();

    double mouse_goal_position[3] =  {mouse_state.x, mouse_state.z, -1.5708};
    double robot_current_position[3] = {translation[0], translation[2], rotation[3]};
    ps0_data = wb_distance_sensor_get_value(wonsu_ps0);
    ps7_data = wb_distance_sensor_get_value(wonsu_ps7);


    if (mouse_state.left){
        
       //printf("Wheel Speed: %f, %f\n",ps7_data,ps0_data);
        
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
          else
          {
            const double *wonsu = robot_controller(robot_current_position, mouse_goal_position);
            left_speed=wonsu[0];
            right_speed=wonsu[1];
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
        wb_motor_set_velocity(right_motor,right_speed); 
    }  
        
    else
    {
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
