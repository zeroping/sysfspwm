#include "sysfspwm.hpp"
#include <string>
#include <cmath>
#include <iostream>

#include <algorithm>

namespace sysfspwm
{
  
  PWM::PWM(std::string syspath)
  :udevice_(Udev::Udev().device_from_syspath(syspath))
  {
    
  }
  
  PWM::PWM(Udev::UdevDevice dev)
  :udevice_(dev)
  {
    
  }
  
  PWM::PWM(const PWM& other)
  :udevice_(other.udevice_)
  {
    
  }
  
  PWM& PWM::operator=(const PWM& other)
  {
    udevice_ = other.udevice_;
    
    return *this;
  }
  
  PWM& PWM::operator=(PWM&& other)
  {
    udevice_ = other.udevice_;
    
    return *this;
  }
  
  void PWM::read_current_values(void)
  {
    try
    {
      period_ = std::chrono::nanoseconds(std::stoi(udevice_.get_sysattr("period")));
      duty_ = std::chrono::nanoseconds(std::stoi(udevice_.get_sysattr("duty_cycle")));
    }
    catch (std::exception e)
    {
      throw PWMInterfaceException();
    }
  }
  
  std::string PWM::get_name(void)
  {
    return udevice_.get_sysname();
  }
  
  
  std::chrono::nanoseconds PWM::get_period(void)
  {
    int period = 1;
    try
    {
      period = std::stoi(udevice_.get_sysattr("period"));
    }
    catch (std::exception e)
    {
      throw PWMInterfaceException();
    }
    
    period_ = std::chrono::nanoseconds(period);
    return period_;
  }
  
  void PWM::set_period(std::chrono::nanoseconds period)
  {
    read_current_values();
    //std::cout << " p " << period.count() << std::endl;
    period_ = max(std::chrono::nanoseconds(0), period);
    
    if(duty_ >= period_)
    {
      set_duty_cycle(period_ - std::chrono::nanoseconds(1));
    }
    
    try
    {
      udevice_.set_sysattr("period", std::to_string(period_.count()));
    }
    catch (std::exception e)
    {
      throw PWMArgumentException(*this, "period");
    }
    
  }
  
  std::chrono::nanoseconds PWM::get_duty_cycle(void)
  {
    int duty = 1;
 
    try
    {
      duty = std::stoi(udevice_.get_sysattr("duty_cycle"));
    }
    catch (std::exception e)
    {
      throw PWMInterfaceException();
    }
    
    duty_ = std::chrono::nanoseconds(duty);
    return duty_;
  }
  
  void sysfspwm::PWM::set_duty_cycle(std::chrono::nanoseconds duty)
  {
    //std::cout << " d " << duty.count() << std::endl;
    read_current_values();
    //must be shorter than the period.
    duty_ = std::max(std::chrono::nanoseconds(0),duty);
    
    if(duty_ > period_)
    {
      set_period(duty_+std::chrono::nanoseconds(1));
    }
    try
    {
      udevice_.set_sysattr("duty_cycle", std::to_string(duty_.count()));
    }
    catch (std::exception e)
    {
      throw PWMArgumentException(*this, "duty_cycle");
    }
  }
  
  bool PWM::is_enabled(void)
  {
    bool enabled = false;
    try
    {
      enabled = std::stoi(udevice_.get_sysattr("enable"));
    }
    catch (std::exception e)
    {
      throw PWMInterfaceException();
    }
    return enabled;
  }
  
  void  PWM::set_enabled(bool enabled)
  {
    try
    {
      udevice_.set_sysattr("enable", std::to_string((int) enabled));
    }
    catch (std::exception e)
    {
      throw PWMArgumentException(*this, "enable");
    }
  }
  
  
  bool PWM::is_inverted(void)
  {
    bool inverted = false;
    try
    {
      inverted = udevice_.get_sysattr("polarity") != "normal";
    }
    catch (std::exception e)
    {
      throw PWMInterfaceException();
    }
    return inverted;
  }
  
  void  PWM::set_inverted(bool inverted)
  {
    try
    {
      udevice_.set_sysattr("polarity", inverted ? "inversed" : "normal");
    }
    catch (std::exception e)
    {
      throw PWMArgumentException(*this, "polarity");
    }
    
  }
  
  void PWM::set_frequency_and_ratio(long frequency, float ratio)
  {
    ratio = fmax(0, fmin(1, ratio));
    set_period(std::chrono::nanoseconds(1000000000/frequency));
    set_duty_cycle(std::chrono::nanoseconds((long)(period_.count() * ratio)));
  }
}
