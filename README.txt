
Kernel driver for the adafruit i2c port extender "seesaw"

currently only 3 pwm's are enabled and 3 ADC's

Its not a mfd driver it is all in hwmon class will hopefully change someday

The i2c-add/del need to be edited for your bus number cange the i2c-17 to your bus number

i2cdetect -l will list all available i2c busses.



