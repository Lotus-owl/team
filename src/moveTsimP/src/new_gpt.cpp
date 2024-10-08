#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

#include "sensor_msgs/msg/image.hpp"

#include <chrono>
#include <iostream>
#include <filesystem>
// #include <serial/serial.h>

using namespace std;
using namespace std::chrono_literals;
namespace fs = std::filesystem;

class LaserSensor : public rclcpp::Node
{
public:
    LaserSensor()
        : Node("laser_sensorr")
    {
        auto qos_profile = rclcpp::SensorDataQoS();
        auto qos_profile2 = rclcpp::QoS(rclcpp::KeepLast(10));
        _sub = create_subscription<sensor_msgs::msg::LaserScan>("/scan", qos_profile, std::bind(&LaserSensor::sub_laser, this, std::placeholders::_1));
        _twist_pub = create_publisher<geometry_msgs::msg::Twist>("cmd_vel", qos_profile2);
        _pub_timer = create_wall_timer(30ms, std::bind(&LaserSensor::pub_twist, this));
        _update_timer = create_wall_timer(10ms, std::bind(&LaserSensor::update, this));

    }

private:
    bool _is_wall;
    std::time_t last_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::time_t now_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    float _laser_data[360];
    sensor_msgs::msg::LaserScan _laser_data2;
    geometry_msgs::msg::Twist _twist;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr _twist_pub;
    rclcpp::TimerBase::SharedPtr _pub_timer;
    rclcpp::TimerBase::SharedPtr _update_timer;
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr _sub;

    void pub_twist()
    {
        _twist_pub->publish(_twist);
    }

    void sub_laser(const sensor_msgs::msg::LaserScan msg)
    {
        last_time = now_time;
        now_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        _laser_data2 = msg;
        // 내부변수 업데이트.
        
        _laser_data2.ranges.size();

        for (long unsigned int i=0; i < _laser_data2.ranges.size();i++)
        {
            if(_laser_data2.ranges[i] == 0.0f){
                continue;
            }
            if(std::isnan(_laser_data2.ranges[i])){
                _laser_data2.ranges[i] = 3.5; 
            }
            auto degree = (_laser_data2.angle_min+ _laser_data2.angle_increment * i)*180/3.1415926535;
            auto int_degree = (int)degree;

            //RCLCPP_INFO(this->get_logger(), "degree:: %d\n", int_degree);
            _laser_data[int_degree] = _laser_data2.ranges[i];

        }
    }

    void update()
    {

        if (_laser_data2.ranges.size() == 0)
        {
            RCLCPP_INFO(this->get_logger(), "NO SIZE");
            return;
        }

        if (_is_wall == false){
            _twist.linear.x = 0.1;
            _twist.angular.z = 0.0;
            //RCLCPP_INFO(this->get_logger(), "false로 돌아옴");
            for (int i = 355; i <= 364; ++i) {
                int value = (i > 359) ? (i - 359) : i;
                if (_laser_data[value] < 0.25 && _laser_data[value] > 0.0f)
                {
                    _is_wall = true;
                    RCLCPP_INFO(this->get_logger(), "wa");
                    RCLCPP_INFO(this->get_logger(), "value : %f", _laser_data[value]);
                    RCLCPP_INFO(this->get_logger(), "value : %d", i);
                    break;
                }
            }
        }else{
            _twist.linear.x = 0.0;
            _twist.angular.z = 0.0;
                    // || _laser_data[315] < 0.3

            for (int i = 85; i <= 94; ++i) {
                                    std::cout << "지난 시간: " << std::ctime(&last_time);
                    std::cout << "현재 시간: " << std::ctime(&now_time);
                if(now_time == last_time){
                    continue;
                }

                if (_laser_data[i] > 0.25 && _laser_data[i] > 0.0f){
                    RCLCPP_INFO(this->get_logger(), "value : %f", _laser_data[i]);
                    RCLCPP_INFO(this->get_logger(), "오른쪽 회전:: ");
                    rotate_90_degrees(1);
                    break;
                }else if (_laser_data[i] < 0.25 && _laser_data[i] > 0.0f){
                    RCLCPP_INFO(this->get_logger(), "왼쪽 회전:: ");
                    RCLCPP_INFO(this->get_logger(), "value : %f", _laser_data[i]);
                    RCLCPP_INFO(this->get_logger(), "왼쪽 회전:: ");
                    rotate_90_degrees(-1);
                    break;
                }
            }

        }
    }

    void rotate_90_degrees(int direction)
    {
        RCLCPP_INFO(this->get_logger(), "90도 로직");
        // 90도 회전 각속도 설정
        double angular_speed = -0.2 * direction; // 각속도 (rad/s)
        double target_angle = (-M_PI*direction) / 2; // 목표 회전 각도 (90도 = π/2 rad)
        double current_angle = 0.0;

        // 시간 간격 설정
        rclcpp::Rate loop_rate(100); // 100Hz 주기
        auto start_time = this->now();
        // 회전 시작
        if(direction == 1){
            while (rclcpp::ok() && current_angle > target_angle)
                {
                    _twist.linear.x = 0.0;
                    _twist.angular.z = angular_speed;

                    _twist_pub->publish(_twist);

                    // 시간 경과 계산
                    auto current_time = this->now();
                    auto elapsed_time = (current_time - start_time).seconds();
                    
                    // 현재 회전한 각도 계산
                    current_angle = angular_speed * elapsed_time;
                    //RCLCPP_INFO(this->get_logger(), "회전 : %f", current_angle);

                    loop_rate.sleep();
                }
        }else{
            while (rclcpp::ok() && current_angle < target_angle)
                {
                    _twist.linear.x = 0.0;
                    _twist.angular.z = angular_speed;

                    _twist_pub->publish(_twist);

                    // 시간 경과 계산
                    auto current_time = this->now();
                    auto elapsed_time = (current_time - start_time).seconds();
                    
                    // 현재 회전한 각도 계산
                    current_angle = angular_speed * elapsed_time;
                    //RCLCPP_INFO(this->get_logger(), "회전 : %f", current_angle);

                    loop_rate.sleep();
                }
        }

        _is_wall = false;

        _twist.linear.x = 0.0;
        _twist.angular.z = 0.0;
        
        _twist_pub->publish(_twist);

        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        
        RCLCPP_INFO(this->get_logger(), "회전완료");
    }
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<LaserSensor>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
