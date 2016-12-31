#include "ros/ros.h"
#include "std_msgs/String.h"
#include "kobuki_msgs/BumperEvent.h"
#include "nav_msgs/Odometry.h"
#include "geometry_msgs/Twist.h"
#include <sstream>

/**
 * This tutorial demonstrates simple sending of messages over the ROS system.
 */

class GoHitTurnController
{
  public:
    double x;
    double y;
    int left;
    int center;
    int right;
    int bumper_id;
    int bumper_state;
    bool bumper_hit;

    geometry_msgs::Twist twist;


 //   ros::NodeHandle nh;

 //   ros::Publisher cmd_vel_pub;
 //   ros::Subscriber bumper_sub;
 //   ros::Subscriber odom_sub;

    void InitGo(void)
    {

       x = -1;
       y = -1;
       bumper_id = -1;
       bumper_state = -1;

       twist.linear.x = 0.2;
       twist.linear.y = 0;
       twist.linear.z = 0;

       twist.angular.x = 0;
       twist.angular.y = 0;
       twist.angular.z = 0;
    }

    void OdomCallback(const nav_msgs::Odometry::ConstPtr&msg)
    {
      x = msg->pose.pose.position.x;
      y = msg->pose.pose.position.y;
    }

    void BumperCallback(const kobuki_msgs::BumperEvent::ConstPtr&msg)
    {
      left =0;
      center = 0;
      right = 0;
      bumper_id = msg->bumper;
      bumper_state = msg->state;

      ROS_INFO("[x, y, bumper_id, bumper_state] = %f, %f, %d, %d", x, y, bumper_id, bumper_state);

      if(bumper_state == 1)
      {
          if(bumper_id == 0) //left bumper
          {
            twist.linear.x = 0;
            twist.linear.y = 0;
            twist.linear.z = 0;

            twist.angular.x = 0;
            twist.angular.y = 0;
            twist.angular.z = -1; //turn right
          }

          if(bumper_id == 1) //center bumper
          {
            twist.linear.x = 0; 
            twist.linear.y = 0;
            twist.linear.z = 0;

            twist.angular.x = 0;
            twist.angular.y = 0;
            twist.angular.z = -2; //turn right
          }
 
          if(bumper_id == 2) //right bumper
          {
            twist.linear.x = 0;
            twist.linear.y = 0;
            twist.linear.z = 0;

            twist.angular.x = 0;
            twist.angular.y = 0;
            twist.angular.z = 1; //turn left
          }

          bumper_hit = true; //flag of bumper
      }else{ //bumper state = 0
          twist.linear.x = 0.3; //go forward
          twist.linear.y = 0;
          twist.linear.z = 0;

          twist.angular.x = 0;
          twist.angular.y = 0;
          twist.angular.z = 0;     
      }
    
   //   cmd_vel_pub.publish(twist); 
    }
    
    void GoForward(void)
    {
      twist.linear.x = 0.3;
      twist.linear.y = 0;
      twist.linear.z = 0;

      twist.angular.x = 0;
      twist.angular.y = 0;
      twist.angular.z = 0;

//      cmd_vel_pub.publish(twist);
    }

};

int main(int argc, char **argv)
{
  /**
   * The ros::init() function needs to see argc and argv so that it can perform
   * any ROS arguments and name remapping that were provided at the command line.
   * For programmatic remappings you can use a different version of init() which takes
   * remappings directly, but for most command-line programs, passing argc and argv is
   * the easiest way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any other
   * part of the ROS system.
   */
  ros::init(argc, argv, "turtlebot_gohitturn");

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle nh; //nmoved into GHTController

  GoHitTurnController ght_controller;

  /**
   * The advertise() function is how you tell ROS that you want to
   * publish on a given topic name. This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing. After this advertise() call is made, the master
   * node will notify anyone who is trying to subscribe to this topic name,
   * and they will in turn negotiate a peer-to-peer connection with this
   * node.  advertise() returns a Publisher object which allows you to
   * publish messages on that topic through a call to publish().  Once
   * all copies of the returned Publisher object are destroyed, the topic
   * will be automatically unadvertised.
   *
   * The second parameter to advertise() is the size of the message queue
   * used for publishing messages.  If messages are published more quickly
   * than we can send them, the number here specifies how many messages to
   * buffer up before throwing some away.
   */

  ros::Publisher cmd_vel_pub = nh.advertise<geometry_msgs::Twist>("/mobile_base/commands/velocity", 1000);


  ros::Subscriber bumper_sub = nh.subscribe("/mobile_base/events/bumper",
                                             10,
                                             &GoHitTurnController::BumperCallback,
                                             &ght_controller );
  ros::Subscriber odom_sub = nh.subscribe("odom",
                                           10,
                                           &GoHitTurnController::OdomCallback,
                                           &ght_controller );


//  ght_controller.InitGo();

  //ros::spin();

  ros::Rate loop_rate(1);

  int count = 0;

  ght_controller.bumper_hit = false;
  while (ros::ok())
  {

/*    ROS_INFO("twist.linear = [%f, %f, %f], twist.angular = [%f, %f, %f]",
              twist.linear.x,
              twist.linear.y,
              twist.linear.z,
              twist.angular.x,
              twist.angular.y,
              twist.angular.z);
*/
    if(ght_controller.bumper_hit)
    {
      cmd_vel_pub.publish(ght_controller.twist);
      ght_controller.bumper_hit = false;
    }else{
      ght_controller.GoForward();  
      cmd_vel_pub.publish(ght_controller.twist);
    }

    ros::spinOnce();

    loop_rate.sleep();

    ++count;
  }


  return 0;
}

