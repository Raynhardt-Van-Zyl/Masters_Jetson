/*
 * rosserial Subscriber Example
 * Blinks an LED on callback
 */

#include <ros.h>
#include <std_msgs/Empty.h>
#include <std_msgs/Int32.h>

ros::NodeHandle nh;

int setPoint = 0;

const int left =  2;
const int right =  3;

void LEDSet( const std_msgs::Empty& toggle_msg){
  digitalWrite(13, HIGH-digitalRead(13));   // blink the led
}

void getSetPoint( const std_msgs::Int32& setP){
  setPoint = setP.data;
}

ros::Subscriber<std_msgs::Empty> subLED("toggle_led", &LEDSet );

ros::Subscriber<std_msgs::Int32> subSetPoint("setPoint", &getSetPoint );

void setup()
{
  pinMode(13, OUTPUT);
  pinMode(left, OUTPUT);
  pinMode(right, OUTPUT);
  nh.initNode();
  nh.subscribe(subLED);
  nh.subscribe(subSetPoint);
}

void loop()
{
  nh.spinOnce();

  analogWrite(left, setPoint);
  analogWrite(right, setPoint);
  
}