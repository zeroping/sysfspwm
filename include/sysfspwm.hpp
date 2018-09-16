#pragma once

#include <udevpp.hpp>
#include <vector>
#include <chrono>
#include <exception>

namespace sysfspwm
{
  class PWM;
  
  /**
   * @brief Represents a single pwm chip
   * 
   * This mirrors the kernel API: there are PWM Chips (ex: /sys/class/pwm/pwmchip0),
   * which are parents to the actual PWM devices (ex: /sys/class/pwm/pwmchip0/pwm0).
   * They have to be exported before they exist (like GPIOs), so you need to
   * check get_npwm() to see how many there are, and use export_pwm() to export
   * one.
   */
  class PWMChip
  {
  public:
    /**
     * @brief Initializes a PWMChip from a syspath. See libudev's udev_device_new_from_syspath().
     * 
     * @param syspath A syspath string, such as  "/sys/class/pwm/pwmchip0"
     */
    PWMChip(std::string syspath);
    
    /**
     * @brief Initializes from an existing UdevDevice
     * 
     * @param dev UdevDevice pointing at the PWMChip device itself
     */
    PWMChip(Udev::UdevDevice dev);
    
    PWMChip(const PWMChip& other);
    
    PWMChip& operator=(const PWMChip& other);
    PWMChip& operator=(PWMChip&& other);
    
    std::string get_name(void);
    
    /**
     * @brief Checks the number of PWMs availble on this PWMChip
     * 
     * @return int number of PWMs
     */
    int get_npwm(void);
    
    /**
     * @brief Exports a PWM from a PWM Chip
     * 
     * @param number p_number:...the number of the PWM to export
     * @return sysfspwm::PWM The PWM chip exported
     */
    PWM export_pwm(int number);
    
    static std::vector<PWMChip> getSystemPWMChips(void);
    
    /**
     * @brief A reference to the underlying udev device
     * 
     */
    Udev::UdevDevice udevice_;
    
  };
  
  
  /**
   * Class representing an PWM device in udev
   */
  class PWM
  {
  public:
    /**
     * @brief Initializes a PWM from a syspath. See libudev's udev_device_new_from_syspath().
     * 
     * @param syspath A syspath string, such as  "/sys/class/pwm/pwmchip0/pwm0" - must already be exported
     */
    PWM(std::string syspath);
    
    /**
     * @brief Initializes from an existing UdevDevice
     * 
     * @param dev UdevDevice pointing at the PWM device itself
     */
    PWM(Udev::UdevDevice dev);
    
    PWM(const PWM& other);
    
    PWM& operator=(const PWM& other);
    PWM& operator=(PWM&& other);
    
    std::string get_name(void);
    
    std::chrono::nanoseconds get_period(void);
    void set_period(std::chrono::nanoseconds period);
    std::chrono::nanoseconds get_duty_cycle(void);
    void set_duty_cycle(std::chrono::nanoseconds duty);
    bool is_enabled(void);
    void set_enabled(bool enabled);
    bool is_inverted(void);
    void set_inverted(bool inverted);
    
    void set_frequency_and_ratio(long frequency, float ratio);
    
    /**
     * @brief A reference to the underlying udev device
     * 
     */
    Udev::UdevDevice udevice_;
    
    
  private:
    std::chrono::nanoseconds duty_;
    std::chrono::nanoseconds period_;
    
    /**
     * @brief We sometimes need the current duty and period so that we can make sure we don't ask for an invalid setting. We also might not want to assume that no one else is changing our sysfs attrs behind the scenes.
     * 
     */
    void read_current_values(void);
  };
  
  class PWMArgumentException : public std::exception
  {
    PWM& pwm_;
    std::string attr_;
  public:
    PWMArgumentException(PWM& pwm, std::string attr) : pwm_(pwm), attr_(attr)
    { }
    const char * what () const throw ()
    {
      return ("bad PWM argument exception on PWM located at " + pwm_.udevice_.get_syspath() + " while setting " + attr_).c_str() ;
    }
  };
  
  class PWMInterfaceException : public std::exception
  {
  public:
    PWMInterfaceException()
    { }
    const char * what () const throw ()
    {
      return ("A sysfs PWM device didn't conform to the expected interface") ;
    }
  };
  
  
}
