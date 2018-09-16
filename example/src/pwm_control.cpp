#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <memory>
#include <udevpp.hpp>
#include <thread>
#include <chrono>
#include <math.h>

#include <sysfspwm.hpp>


int main(int argc, char **argv)
{
  std::cout << "sysfspwm Example" << std::endl;
  
  std::string path = "/sys/class/pwm/pwmchip0/pwm0";
  if (argc != 5)
  {
    //we don't have  path on the command line. Print the usage, and then list all the available LEDs.
    std::cout << "Usage:" << std::endl;
    std::cout << "  to list PWMs: " << argv[0] << std::endl;
    std::cout << "  to set a PWM: " << argv[0] << " <sysfs path of PWMChip> <pwm number> <frequency in hz> <duty cycle as float>" << std::endl;
    
    //for each LED in the system
    for (sysfspwm::PWMChip pwmchip : sysfspwm::PWMChip::getSystemPWMChips())
    {
      std::cout << std::endl;
      std::cout << "Available PWMChip " << pwmchip.get_name() << std::endl;
      std::cout << "  at sysfs path: " << pwmchip.udevice_.get_syspath() << std::endl;
      std::cout << "  with " << pwmchip.get_npwm() << " pwms" << std::endl;
    }
    return -1;
  }
  
  path = argv[1];
  // we have a path from the command line, try controlling the PWM
  
  int pwmnum = std::stoi(argv[2]);
  
  int pwmfreq = std::stoi(argv[3]);
  
  float pwmduty = fmax(0, fmin(1, std::stof(argv[4])));
  
  //make the LED from a given path
  sysfspwm::PWMChip mypwmchip(path);
  
  sysfspwm::PWM mypwm = mypwmchip.export_pwm(pwmnum);
  
  std::cout << "configuring pwm at " << mypwm.udevice_.get_syspath() << std::endl;
  
  if(pwmduty > 0)
  {
    std::cout << "frequency is " << pwmfreq << " and duty cycle is " << pwmduty * 100 << "%" <<  std::endl;
    mypwm.set_frequency_and_ratio(pwmfreq, pwmduty);
    mypwm.set_enabled(true);
  }
  else
  {
    std::cout << "requested duty cycle is 0%, turning off PWM" <<  std::endl;
    mypwm.set_enabled(false);
  }
  
  return 0;
}
