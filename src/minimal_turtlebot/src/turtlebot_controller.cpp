#include "minimal_turtlebot/turtlebot_controller.h"
#include <ros/ros.h> 
#include <iostream>
#include <fstream>
using namespace std;

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void turtlebot_controller(turtlebotInputs turtlebot_inputs, uint8_t *soundValue, float *vel, float *ang_vel)
 {
	//Place your code here! you can access the left / right wheel 
	//dropped variables declared above, as well as information about
	//bumper status. 
	
	//outputs have been set to some default values. Feel free 
	//to change these constants to see how they impact the robot. 

	*vel = 0.0; // Robot forward velocity in m/s
	//0.7 is max and is a lot
	*ang_vel = 0.2;  // Robot angular velocity in rad/s
	//0.7 is max and is a lot 
  
	*soundValue = 0;
  
	//here are the various sound value enumeration options
	//soundValue.OFF
	//soundValue.RECHARGE
	//soundValue.BUTTON
	//soundValue.ERROR
	//soundValue.CLEANINGSTART
	//soundValue.CLEANINGEND 	fstream file;
	ROS_INFO("hi 1 %i", turtlebot_inputs.depthImage.width);
	if(turtlebot_inputs.depthImage.width != 0)
	{

		ROS_INFO("hi 2");

		ofstream file;
		file.open("test.txt");
		int s = 0;
		for(int i = 0; i < 614400 / 2; i++)
		{
			file << (turtlebot_inputs.depthImage.data[s]);
			file << (turtlebot_inputs.depthImage.data[s + 1]) << "\n";
			s = s + 2;
		}

		ROS_INFO("size: [%u]", turtlebot_inputs.depthImage.data[s]);

		file.close();
	}


}

