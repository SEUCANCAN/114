#include "ros/ros.h"
#include "std_msgs/String.h"
#include "geometry_msgs/PoseStamped.h"
#include <sstream>
#include "math.h"
#include "vrpn_client_ros/Position_Yaw.h"
#include "tf/transform_datatypes.h"
#include <cmath>


ros::Publisher information_pub;
	double last_time = 0;
	double last_x_m = 0;
	double last_y_m = 0;
	



void chatterCallback(const geometry_msgs::PoseStamped::ConstPtr& msg)
{
    geometry_msgs::PoseStamped  infor;
    double xn;double yn;double rou;
    double thta = 0.01;
    xn = ((-1) * msg->pose.position.x)+3.014 -1.769-0.025 - 0.5697 + 0.067;
    yn = ((-1) * msg->pose.position.y)+2.592 -0.708+0.013 - 0.2749 - 0.032;
    rou = sqrt(xn * xn + yn * yn);
    int fix = 0;
    if (xn < 0)
    {
        fix = 1;
    }
    else
    {
        fix = 0;
    }
    infor.pose.position.x=1.769 + rou * cos(fix*3.141592+atan(yn/xn)+thta);
    infor.pose.position.y=0.708 + rou * sin(fix*3.141592+atan(yn/xn)+thta);
   //infor.pose.position.x=xn;
   //infor.pose.position.y=yn;
	double now_time = msg->header.stamp.toSec();	
	if(last_time == 0)
	{
	    last_time = now_time;
	    return;
	}
    //ROS_INFO("I heard: %lf", msg->pose.position.x);
    //ROS_INFO("time: %f",msg->header.stamp.sec+1e-9*msg->header.stamp.nsec);
    double delta_time = now_time - last_time;    
    //ROS_INFO("delta_time: %f",delta_time);
    //last_time = now_time;    
    double body_v_ms;    
    if (delta_time > 0.01)
    {
        last_time = now_time;
        body_v_ms= sqrt((msg->pose.position.x - last_x_m)*(msg->pose.position.x - last_x_m) + (msg->pose.position.y - last_y_m)*(msg->pose.position.y - last_y_m)) / delta_time;
        //ROS_INFO("position: %f,%f ",msg->pose.position.x,last_x_m);
        last_x_m=msg->pose.position.x;
        last_y_m=msg->pose.position.y;
        //ROS_INFO("time: %f,%f,%f ",now_time,delta_time,body_v_ms);
    }
    infor.pose.position.z = body_v_ms;
    //infor.pose.orientation.w = asin(2*(msg->pose.orientation.w*msg->pose.orientation.z - msg->pose.orientation.y*msg->pose.orientation.x));
    
    tf::Quaternion quat;
    tf::quaternionMsgToTF(msg->pose.orientation, quat);
    double roll, yaw, pitch;
    tf::Matrix3x3(quat).getRPY(roll, pitch, yaw);
    
    
    
    std::cout << roll*180.0/M_PI << "\t" <<  pitch*180.0/M_PI << "\t" <<  yaw*180.0/M_PI << "\t" <<std::endl;
    infor.pose.orientation.x=roll;
    infor.pose.orientation.y=yaw - 3.1415926 + 3.134 + 0.7019 + 2.4759;
    infor.pose.orientation.z=pitch;
    infor.pose.orientation.w=((yaw - 3.1415926 + 3.134 + 0.7019 + 2.4759) * 180) / 3.1415926535;
    
    
    //infor.pose.orientation.w=tf::getPitch(msg->pose.orientation);
    information_pub.publish(infor);
    return;
}
int main(int argc, char **argv)
{
    ros::init(argc,argv,"listener");
    ros::NodeHandle n;
     information_pub = n.advertise<geometry_msgs::PoseStamped>("information", 1000 );
    ros::Subscriber sub = n.subscribe("/vrpn_client_node/zwc/pose",1000,chatterCallback);
    //ros::Publisher information_pub = n.advertise<std_msgs::String>("information", 1000 );
    //ros::Rate loop_rate(10);
    ros::spin();
    return 0;
}
