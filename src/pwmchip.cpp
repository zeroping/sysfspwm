#include "sysfspwm.hpp"
#include <string>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <iostream>
#include <fstream>

//only used to see if pwm path exists
#include <sys/stat.h>

namespace sysfspwm
{
  
  PWMChip::PWMChip(std::string syspath)
  :udevice_(Udev::Udev().device_from_syspath(syspath))
  {
    
  }
  
  PWMChip::PWMChip(Udev::UdevDevice dev)
  :udevice_(dev)
  {
    
  }
  
  PWMChip::PWMChip(const PWMChip& other)
  :udevice_(other.udevice_)
  {
    
  }
  
  PWMChip& PWMChip::operator=(const PWMChip& other)
  {
    udevice_ = other.udevice_;
    
    return *this;
  }
  
  PWMChip& PWMChip::operator=(PWMChip&& other)
  {
    udevice_ = other.udevice_;
    
    return *this;
  }
  
  
  std::string PWMChip::get_name(void)
  {
    return udevice_.get_sysname();
  }
 
  int PWMChip::get_npwm(void)
  {
    int npwm = 0;
    if (udevice_.has_sysattr("npwm"))
    {
      npwm = std::stoi(udevice_.get_sysattr("npwm"));
    }
    return npwm;
  }
  
  PWM PWMChip::export_pwm(int number)
  {
    std::string pwmpath = udevice_.get_syspath() + "/pwm" + std::to_string(number);
    
    struct stat info;
    if(stat( pwmpath.c_str(), &info ) != 0 )
    {
      
      if (udevice_.has_sysattr("export"))
      {
        std::cout << "sysfs device has export attribute! The bug is fixed?" << std::endl;
        // This can't work currently, as there's to attribute visible due to the way libsystemd/libudev/libudevpp works
        //udevice_.set_sysattr("export", std::to_string(number));
      }
      else
      {
        std::ofstream attributefile;
        attributefile.open ((udevice_.get_syspath() + "/export"), std::ios::out);
        attributefile << std::to_string(number);
        attributefile.close();
      }

    }
    
    return PWM(pwmpath);
  }
  
  std::vector<PWMChip> PWMChip::getSystemPWMChips(void)
  {
    Udev::Udev udev;
    std::vector<PWMChip> retlist;
    
    Udev::UdevEnumerate enumerator = udev.enumerate_new();
    enumerator.add_match_sysattr("subsystem","pwm");
    
    enumerator.scan_devices();
    
    
    std::vector<Udev::UdevDevice> devices = enumerator.enumerate_devices();
    
    //std::for_each(devices.begin(), devices.end(), [&retlist](Udev::UdevDevice d){ retlist.push_back(PWMChip(d)); });
    
    std::for_each(devices.begin(), devices.end(), [&retlist](Udev::UdevDevice d){ retlist.push_back(PWMChip(d)); });
    
    return retlist;
  }
}
