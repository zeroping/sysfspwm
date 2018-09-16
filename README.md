# Sysfsled Library
A C++ library for the Linux kernel's PWM device class.

## What It's For
You might have used the PWM interface on a single-board computer running Linux, such as a Raspberry Pi or other embedded system. If so, you might have seen the [Linux Kernel's sysfs PWM interface](https://www.kernel.org/doc/Documentation/pwm.txt)

    echo -n 0 > /sys/class/pwm/pwmchip0/export
    echo -n 1 > /sys/class/pwm/pwmchip0/pwm0/enable
    echo -n 1000000 > /sys/class/pwm/pwmchip0/pwm0/period 
    echo -n 500000 > /sys/class/pwm/pwmchip0/pwm0/duty_cycle
    
This is a quick little C++ library for that built-in kernel interface, so you don't have to mess with writing strings to magical files. Instead, you can do things like:

    vector<PWMChip> pwmchip_list = PWMChip::getSystemPWMChips();
    PWMChip mypwmchip = pwmchip_list[0];
    PWM mypwm = mypwmchip.export_pwm(0);
or 

    mypwm.set_frequency_and_ratio(1000, 0.5);
    mypwm.set_enabled(true);

## How it works

Under the hood, this is using libudevpp, a C++ wrapper for libudev, which is itself a C library for manipulating entires in /sys.

This uses CMake as the build system, so you'll need it installed, as well as libudev. libudevpp will get downloaded automatically. These dependancies might require you to do something like:

    sudo apt install libudev-dev cmake build-essential

You can try this out by cloning, building, and running the example program:

    git clone https://github.com/zeroping/sysfspwm.git
    cd sysfspwm
    mkdir build
    cd build
    cmake ../
    make

Then, to list available PWM chips on a single board computer:

    ./example/pwm_control

And to set up a 2 KHz square wave on the 0th PWM on the 0th PWM chip:

    ./example/pwm_control /sys/class/pwm/pwmchip0 0 2000 0.5
    
## Work From Here
There are a few rough edges. One known annoyance is that the /sys/class/pwm/pwmchip0/export sysfs attribute can't be written under libudevpp right now, so that's currently accomplished with a file stream.
