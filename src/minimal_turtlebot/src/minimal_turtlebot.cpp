
#include<ros/ros.h> 
#include<std_msgs/Float64.h> 
#include<kobuki_msgs/WheelDropEvent.h>
#include<kobuki_msgs/BumperEvent.h>
#include<kobuki_msgs/Sound.h>
#include<kobuki_msgs/CliffEvent.h>
#include <geometry_msgs/Twist.h>
#include "minimal_turtlebot/turtlebot_controller.h"
#include <sensor_msgs/CompressedImage.h>
#include <sensor_msgs/Image.h>
#include "std_msgs/String.h"
//#include "nav_msgs/Odometry.h"

#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>

//instantiate some special types for our commands  
kobuki_msgs::Sound soundValue; 
geometry_msgs::Twist base_cmd;

//cliff booleans
bool g_cliffDropCenter = false;
bool g_cliffDropRight = false;
bool g_cliffDropLeft = false;

//bumper booleans
bool g_bumperHitCenter_ = false;
bool g_bumperHitRight_ = false;
bool g_bumperHitLeft_ = false;

bool g_firstHit_ = true;
float g_backUpPos_ = 0;
bool g_turn_ = true;
bool g_sideways_ = false;
bool g_turn2_ = true;
float g_turnedForwardPos_ = 0;
bool g_firstTurn_ = true;

float g_originalDirection = 0;;

uint8_t localSoundValue=0; 
float localLinearSpeed=0.0; 
float localAngularSpeed=0.0; 

uint8_t soundValueUpdateCounter = 0; 
  
turtlebotInputs localTurtleBotInputs; 


void colorImageCallback(const sensor_msgs::Image& image_data_holder) 
{ 
	static uint32_t colorImageInfoCounter = 0; 
	
	localTurtleBotInputs.colorImage=image_data_holder; 
	if (colorImageInfoCounter > 30)
	{
		ROS_INFO("color image height: %u",image_data_holder.height);
		ROS_INFO("color image width: %u",image_data_holder.width);
		colorImageInfoCounter=0; 
	}
	else
	{
		colorImageInfoCounter++; 
	}

} 

void depthImageCallback(const sensor_msgs::Image& image_data_holder) 
{ 
	static uint32_t depthImageInfoCounter = 0; 
	
	localTurtleBotInputs.depthImage=image_data_holder; 
	if (depthImageInfoCounter > 1)
	{
		// ROS_INFO("depth image height: %u",image_data_holder.height);
		// ROS_INFO("depth image width: %u",image_data_holder.width);
		//ROS_INFO("depth image encoding: %s",image_data_holder.encoding.c_str());
		depthImageInfoCounter=0; 
	}
	else
	{
		depthImageInfoCounter++; 
	}
} 

void wheelDropCallback(const kobuki_msgs::WheelDropEvent& wheel_data_holder) 
{ 
	
	if (wheel_data_holder.wheel == wheel_data_holder.LEFT)
	{
		localTurtleBotInputs.leftWheelDropped = wheel_data_holder.state; 
		ROS_INFO("left wheel dropped state is: %u",wheel_data_holder.state); 
	}
	
	if (wheel_data_holder.wheel == wheel_data_holder.RIGHT)
	{
		localTurtleBotInputs.rightWheelDropped = wheel_data_holder.state; 
		ROS_INFO("right wheel dropped state is: %u",wheel_data_holder.state); 
	}

} 

void bumperMessageCallback(const kobuki_msgs::BumperEvent& bumper_data_holder) 
{ 
	if (bumper_data_holder.bumper == bumper_data_holder.LEFT)
	{
		localTurtleBotInputs.leftBumperPressed = bumper_data_holder.state; 
		ROS_INFO("left bumper pressed state is: %u",bumper_data_holder.state);
		g_bumperHitLeft_ = true; 
	}
	
	if (bumper_data_holder.bumper == bumper_data_holder.CENTER)
	{
		localTurtleBotInputs.centerBumperPressed = bumper_data_holder.state; 
		ROS_INFO("center bumper pressed state is: %u",bumper_data_holder.state); 
		g_bumperHitCenter_ = true;
	}
	
	if (bumper_data_holder.bumper == bumper_data_holder.RIGHT)
	{
		localTurtleBotInputs.rightBumperPressed = bumper_data_holder.state; 
		ROS_INFO("right bumper pressed state is: %u",bumper_data_holder.state);
		g_bumperHitRight_ = true; 
	}

} 

void cliffMessageCallback(const kobuki_msgs::CliffEvent& cliff_data_holder) 
{ 

	if (cliff_data_holder.sensor == cliff_data_holder.LEFT)
	{
		localTurtleBotInputs.leftCliffGone = cliff_data_holder.state; 
		ROS_INFO("left cliff drop state is: %u",cliff_data_holder.state); 
	}
	
	if (cliff_data_holder.sensor == cliff_data_holder.CENTER)
	{
		localTurtleBotInputs.centerCliffGone = cliff_data_holder.state; 
		ROS_INFO("center cliff drop state is: %u",cliff_data_holder.state);
	}
	
	if (cliff_data_holder.sensor == cliff_data_holder.RIGHT)
	{
		localTurtleBotInputs.rightCliffGone = cliff_data_holder.state; 
		ROS_INFO("right cliff drop state is: %u",cliff_data_holder.state); 
	}

}

//
void odomCallback(const nav_msgs::Odometry::ConstPtr& msg)
{
	ROS_INFO("Seq: [%d]", msg->header.seq);
    ROS_INFO("Position-> x: [%f], y: [%f], z: [%f]", msg->pose.pose.position.x,msg->pose.pose.position.y, msg->pose.pose.position.z);
    ROS_INFO("Orientation-> x: [%f], y: [%f], z: [%f], w: [%f]", msg->pose.pose.orientation.x, msg->pose.pose.orientation.y, msg->pose.pose.orientation.z, msg->pose.pose.orientation.w);
    ROS_INFO("Vel-> Linear: [%f], Angular: [%f]", msg->twist.twist.linear.x,msg->twist.twist.angular.z);
      
    //normal behavior
    if(g_bumperHitCenter_ == false && g_bumperHitLeft_== false && g_bumperHitRight_== false)
    {
    	g_originalDirection = msg->pose.pose.orientation.z;
    	g_firstHit_ = true;
		g_backUpPos_ = 0;
		g_turn_ = true;
		g_sideways_ = false;
		g_turn2_ = true;
		g_turnedForwardPos_ = 0;
		g_firstTurn_ = true;

    	if (msg->pose.pose.position.x < 3)
    	{ 
    		base_cmd.linear.x = .1;
		}
 		else
 		{
      		base_cmd.linear.x = 0;
 		}
 	}
 	//behavior when the center or right bumper flag is toggled
 	if(g_bumperHitCenter_ || g_bumperHitRight_)
 	{
 		//store the position of the impact
 		if(g_firstHit_)
 		{
 			g_firstHit_ = false;
 			g_backUpPos_ = (msg->pose.pose.position.x) - .2;
 		}
 		//go backwards
 		if(msg->pose.pose.position.x > g_backUpPos_)
 		{
 			base_cmd.linear.x = -.1;
 		}
 		//once you've reached the backUpPos
 		else
 		{
 			base_cmd.linear.x = 0;
 			//start to turn
 			if(g_turn_)
 			{
 				base_cmd.angular.z = .2;
 				//robot turns to ~.83????
 				//.75
 				if(msg->pose.pose.orientation.z > .4)
 				{
 					base_cmd.angular.z = 0;
 					g_turn_ = false;
 					g_sideways_ = true;
 				}
 			}
 			if(g_sideways_)
 			{
 				if(g_firstTurn_)
 				{
 					g_firstTurn_ = false;
 					g_turnedForwardPos_ = msg->pose.pose.position.y + .2;
 					g_backUpPos_ = 20;
 				}
 				if(msg->pose.pose.position.y < g_turnedForwardPos_)
 				{
 					base_cmd.linear.x = .1;
 				}
 				else
 				{
 					base_cmd.linear.x = 0;
 					if(g_turn2_)
 					{
 						base_cmd.angular.z = -.2;
 						if(msg->pose.pose.orientation.z  <= g_originalDirection)
 						{
 							base_cmd.angular.z = 0;
 							g_turn2_ = false;
 							g_bumperHitCenter_ = false;
 							g_bumperHitRight_ = false;
 						}
 					}
 				}
 			}


 		}
 	}
	//behavior when the left bumper flag is toggled
 	if(g_bumperHitLeft_)
 	{
 		//store the position of the impact
 		if(g_firstHit_)
 		{
 			g_firstHit_ = false;
 			g_backUpPos_ = (msg->pose.pose.position.x) - .2;
 		}
 		//go backwards
 		if(msg->pose.pose.position.x > g_backUpPos_)
 		{
 			base_cmd.linear.x = -.1;
 		}
 		//once you've reached the backUpPos
 		else
 		{
 			base_cmd.linear.x = 0;
 			//start to turn
 			if(g_turn_)
 			{
 				base_cmd.angular.z = -.2;
 				//robot turns to ~.83????
 				//.75
 				if(msg->pose.pose.orientation.z < -.4)
 				{
 					base_cmd.angular.z = 0;
 					g_turn_ = false;
 					g_sideways_ = true;
 				}
 			}
 			if(g_sideways_)
 			{
 				if(g_firstTurn_)
 				{
 					g_firstTurn_ = false;
 					g_turnedForwardPos_ = msg->pose.pose.position.y - .2;
 					g_backUpPos_ = 20;
 				}
 				if(msg->pose.pose.position.y > g_turnedForwardPos_)
 				{
 					base_cmd.linear.x = .1;
 				}
 				else
 				{
 					base_cmd.linear.x = 0;
 					if(g_turn2_)
 					{
 						base_cmd.angular.z = .2;
 						if(msg->pose.pose.orientation.z  >= g_originalDirection)
 						{
 							base_cmd.angular.z = 0;
 							g_turn2_ = false;
 							g_bumperHitLeft_ = false;
 						}
 					}
 				}
 			}


 		}
 	}
} 

int main(int argc, char **argv) 
{ 
  ros::init(argc,argv,"my_minimal_subscriber"); //name this node 
  // when this compiled code is run, ROS will recognize it as a node called "minimal_subscriber" 
  ros::NodeHandle n; // need this to establish communications with our new node 
  //create a Subscriber object and have it subscribe to the topic "topic1" 
  // the function "myCallback" will wake up whenever a new message is published to topic1 
  // the real work is done inside the callback function 
  
  ros::Rate naptime(10); // use to regulate loop rate 
  
  //subscribe to wheel drop and bumper messages
  ros::Subscriber my_wheel_drop_subscription= n.subscribe("mobile_base/events/wheel_drop",1,wheelDropCallback); 
  ros::Subscriber my_bumper_subscription= n.subscribe("mobile_base/events/bumper",1,bumperMessageCallback); 
  ros::Subscriber my_cliff_sensor_object= n.subscribe("mobile_base/events/cliff",1,cliffMessageCallback);
  //ros::Subscriber colorImageSubscription= n.subscribe("camera/rgb/image_rect_color",1,colorImageCallback); 
  ros::Subscriber depthSubscription= n.subscribe("camera/depth/image_raw",1,depthImageCallback);
  ros::Subscriber sub_odom=n.subscribe("odom",60,odomCallback);

  
  //publish sound and command vel messages 
  ros::Publisher my_publisher_object = n.advertise<kobuki_msgs::Sound>("mobile_base/commands/sound", 1);
  ros::Publisher cmd_vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 60);
  

  while (n.ok())
  {
  	ros::spinOnce();
	turtlebot_controller(localTurtleBotInputs, &localSoundValue, &localLinearSpeed, &localAngularSpeed);
 	cmd_vel_pub.publish(base_cmd);
	ROS_INFO("hello");
	naptime.sleep(); 
  }

 //  while (ros::ok()) //while robot is not at the end position
 //  {
 //    ros::spinOnce();
 //    if (x_pos < x_epos)
 //    { 
 //    	base_cmd.linear.x = 0.19;
	// 	base_cmd.angular.z = 0.0;
	// }
 // 	else
 // 	{
 //      	base_cmd.linear.x = -0.19;
	// 	base_cmd.angular.z = 0.0;
 // 	}

 // 	cmd_vel_pub.publish(base_cmd);
 	
 // 	if(x_pos >= x_epos){
 // 		ROS_INFO("PASSED ENDPOINT");
 // 	}
 //  }

  return 0;

/*  while(ros::ok())
  {
	ros::spinOnce();
	turtlebot_controller(localTurtleBotInputs, &localSoundValue, &localLinearSpeed, &localAngularSpeed);
	
	//ROS_INFO("supposed to be cmd_vel angular = %f", localAngularSpeed);
	//ROS_INFO("supposed to be cmd_vel linear = %f", localLinearSpeed);
	soundValue.value=localSoundValue;
	base_cmd.linear.x=localLinearSpeed;
	base_cmd.angular.z=localAngularSpeed;
	
	if (soundValueUpdateCounter > 10)
	{
		my_publisher_object.publish(soundValue); 
		soundValueUpdateCounter = 0; 
	}
	else
	{
		soundValueUpdateCounter++;
	}
	
	cmd_vel_pub_.publish(base_cmd);
	
	naptime.sleep(); 
	
  }
  return 0; // should never get here, unless roscore dies */
}

