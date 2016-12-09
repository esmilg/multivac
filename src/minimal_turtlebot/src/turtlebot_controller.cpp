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
	//ROS_INFO("hi 1 %i", turtlebot_inputs.depthImage.width);
	// if(turtlebot_inputs.depthImage.width != 0)
	// {

	// 	ROS_INFO("hi 2");

	// 	ofstream file;
	// 	file.open("test.txt");
	// 	int s = 0;
	// 	for(int i = 0; i < 614400 / 2; i++)
	// 	{
	// 		file << (turtlebot_inputs.depthImage.data[s]);
	// 		file << (turtlebot_inputs.depthImage.data[s + 1]) << "\n";
	// 		s = s + 2;
	// 	}

	// 	ROS_INFO("size: [%u]", turtlebot_inputs.depthImage.data[s]);

	// 	file.close();
	// }
	
	int depthArraySize = turtlebot_inputs.depthImage.height * turtlebot_inputs.depthImage.width;
	uint16_t depthData[depthArraySize];
	uint16_t depthLF[depthArraySize / 4];
	uint16_t depthLC[depthArraySize / 4];
	uint16_t depthRC[depthArraySize / 4];
	uint16_t depthRF[depthArraySize / 4];

	if(turtlebot_inputs.depthImage.width != 0)
	{
		for(int indx = 0; indx < depthArraySize; indx++)
		{
			//printf("%4.4X ", depthData[indx]);
			depthData[indx] = turtlebot_inputs.depthImage.data[indx*2]|(turtlebot_inputs.depthImage.data[indx*2+1]<<8);
			if(indx%640 == 0)
			{
				//printf("\n");
			}
		}
		printf("\n");

		//DepthImage ranges from roughly .5m to 5.2m

		int arrayPtr = 0; //switches b/w arrays every 160 elements of DepthData
		int LFPtr = 0; //tracks index in LeftFar array
		int LCPtr = 0; //tracks index in LeftCenter array
		int RCPtr = 0; //tracks index in RightCenter array
		int RFPtr = 0; //tracks index in RightFar array
		//Splits depthData into 4 separate arrays representing 
		//LeftFar, LeftCenter, RightCenter, and RightFar.
		for(int indx = 0; indx < depthArraySize; indx++)
		{
			if(arrayPtr % 4 == 0) //LeftFar
			{
				depthLF[LFPtr] = depthData[indx];
				LFPtr++;	
			}
			if(arrayPtr % 4 == 1) //LeftCenter
			{
				depthLC[LCPtr] = depthData[indx];
				LCPtr++;
			}
			if(arrayPtr % 4 == 2) //RightCenter
			{
				depthRC[RCPtr] = depthData[indx];
				RCPtr++;
			}
			if(arrayPtr % 4 == 3) //RightFar
			{
				depthRF[RFPtr] = depthData[indx];
				RFPtr++;
			}
			if(indx % 160 == 0) arrayPtr++;
		}
	}

}

