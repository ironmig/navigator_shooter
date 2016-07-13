#include <Servo.h>

#include <ros.h>
#include <std_msgs/Bool.h>

class Shooter
{
  private:

  public:
    static const int pin = 5;
    static Servo controller;
    static int goal;

    static void init()
    {
      controller.attach(pin);
      goal = 1500;
    }
    static void set(int x)
    {
      goal = constrain(x,1000,2000);
    }
    static void on()
    {
      set(2000);
    }
    static void off()
    {
      set(1500);
    }
    static void run()
    {
      int cur = controller.read();
      if (cur != goal)
      {
        if (cur < goal)
          controller.write(cur+100);
        else controller.write(goal);
      }
    }
};
int Shooter::goal = 0;
Servo Shooter::controller = Servo();

class Comms
{
  private:
    ros::NodeHandle nh;
    ros::Subscriber<std_msgs::Bool> subscriber;
    //std_msgs::String str_msg;
    //ros::Publisher chatter;
    static const char ON_CHAR = 'i';
    static const char OFF_CHAR = 'o';
  public:
    Comms() : 
      nh(),
      subscriber("shooter",&callback)
      //str_msg(),
      //chatter("chatter", &str_msg)
    {
      nh.initNode();
      //nh.advertise(chatter);
      nh.subscribe(subscriber);
    }
    static void callback(const std_msgs::Bool& msg)
    {
      if (msg.data) Shooter::on();
      else if (!msg.data) Shooter::off();
    }
    void run()
    {
      nh.spinOnce();
    }
};

Comms com;
void setup()
{
  com = Comms();
  Shooter::init();
}

void loop()
{
  com.run();
  Shooter::run();
  delay(50);
}

