#include <ros.h>
#include <std_msgs/Int8.h>
#include <std_msgs/String.h>

#include <Arduino.h>

#include <Servo.h>

class Victor
{
  private:
    Servo controller;
    int goal;
    int cur;
    //Internal set command to write to controller PWM
    void _set(int s)
    {
      controller.writeMicroseconds(x);
      cur = x;
    }
  public:
    Victor(int pin)
    {
      controller = Servo();
      controller.attach(pin);
      goal = 1500;
      _set(goal);
    }
    void set(int speed)
    {
      goal = map(speed,-100,100, 1000,2000);
    }
    int get()
    {
      return map(cur,1000,2000,-100,100);
    }
    void off()
    {
      set(0);
    }
    void on()
    {
      set(100);
    }
    void reverse()
    {
      set(-100);
    }
    //Should be called in each loop so PWM slowly ramps up, doesn't work otherwise
    void run()
    {
      if (cur != goal)
      {
        if (goal == 1500) _set(1500);
        else if (goal < 1500)
        {
          _set(cur - 100);
        }
        else if (goal > 1500)
        {
          _set(cur + 100);
        }
      }
    }
}

Victor Shooter;

class Feeder
{
  private:

  public:
    Victor motor;
    Feeder (int motorPin)
    {
      motor(motorPin);
      set(1500);
      goal = 1500;
    }
    void set(int x)
    {
      controller.writeMicroseconds(x);
      cur = x;
    }
    static void run()
    {
      
    }
};
Feeder feeder;

class Comms
{
  private:
    //ROS
    ros::NodeHandle nh;
    std_msgs::String str_msg;
    //ros::Publisher chatter;
    ros::Subscriber<std_msgs::String> sub;

    static void messageCallback(const std_msgs::String& str_msg)
    {
      String s = str_msg.data;
      if (s == "flyon")
        Shooter.on();
      else if (s == "flyoff")
        Shooter.off();
      else if (s == "feedon")
        feeder.motor.on();
      else if (s == "feedoff")
        feeder.motor.off();
      else if (s == "feedreverse")
        feeder.motor.reverse();
      else if (s == "ledon")
        digitalWrite(13,HIGH);
      else if (s == "ledoff")
        digitalWrite(13,LOW);
    }
  public:
    Comms() :
      str_msg(),
      sub("shooter_control",&messageCallback)
      //chatter("chatter", &str_msg)
    {

    }
    void init()
    {
      nh.initNode();
      nh.subscribe(sub);
      //nh.advertise(chatter);   
    }
    void run()
    {
      nh.spinOnce();
    }
};

Comms com;
void setup()
{
  Shooter = Victor(5);
  feeder = Feeder(6);
  pinMode(13,OUTPUT);
  com.init();
}

void loop()
{
  com.run();
  Shooter.run();
  Feeder::run();
  delay(100);
}
