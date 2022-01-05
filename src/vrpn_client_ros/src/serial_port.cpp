//serial_port.cpp
#include <ros/ros.h>
#include <serial/serial.h>
#include <geometry_msgs/Twist.h>
#include <iostream>
uint8_t speed_cmd=0x00;
uint8_t steer_cmd=0x07; 

void kbdCallback(const geometry_msgs::Twist::ConstPtr& msg)
{
    //ROS_INFO("cmd: %f,%f",speed_cmd,steer_cmd);
	if(msg->linear.x <= 0.1)
	{
	   speed_cmd=0x00;
	}
	else 
	{
	   speed_cmd=0x07;
	}
	
	
	if(msg->angular.z == 0)
	{
	   steer_cmd=0x07;
	}
	else if(msg->angular.z>0.1)
	{
	   steer_cmd=0x01;
	}
	else
	{
	   steer_cmd=0x0c;
	}
	
    return;
    
}



int main(int argc, char** argv)
{

    ros::init(argc, argv, "serial_port");
    ros::NodeHandle n; //创建句柄（虽然后面没用到这个句柄，但如果不创建，运行时进程会出错）
    ros::Subscriber sub = n.subscribe("/cmd_vel",1000,kbdCallback);
    serial::Serial sp;//创建一个serial类
    serial::Timeout to = serial::Timeout::simpleTimeout(100);//创建timeout
    sp.setPort("/dev/ttyUSB0");//设置要打开的串口名称
    sp.setBaudrate(115200);//设置串口通信的波特率
    sp.setTimeout(to);//串口设置timeout
    try
    {
        sp.open();
    }
    catch(serial::IOException& e)
    {
        ROS_ERROR_STREAM("Unable to open port.");
        return -1;
    }
    if(sp.isOpen())
    {
        ROS_INFO_STREAM("/dev/ttyUSB0 is opened.");
    }
    else
    {
        return -1;
    }
    ros::Rate loop_rate(500);
    while(ros::ok())
    {
        ros::spinOnce();
        //获取缓冲区内的字节数
        size_t n = 4;
        if(n!=0)
        {
            uint8_t buffer[4]={speed_cmd,steer_cmd,0x0d,0x0a};
            for(int i=0; i<4; i++)
            {
                //16进制的方式打印到屏幕
                std::cout << std::hex << (buffer[i] & 0xff) << " ";
            }
            std::cout << std::endl;
            //把数据发送回去
            sp.write(buffer, n);
        }
        loop_rate.sleep();
    }
    
    //关闭串口
    sp.close();
 
    return 0;
}
