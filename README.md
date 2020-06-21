# Serializer
Qt Widget app for controlling a bunch of arduinos &amp; visualize incoming data.

Arduino's algoritm hard-coded with VSCode and free Arduino extension. There are custom sensor libraries with lightweight [I2C library](http://dsscircuits.com/articles/arduino-i2c-master-library). Minimalistic communication with `Serial.write()` at 3 million baud instead of classic `Serial.print()` so that refresh rate hits ~950 Hz. Error control executed at receiver's side.
"Server" side is performed with Qt. The app allows user to control COM devices' congiguration from UI instead of opening XML configuration file with editor^
1. Set settings such as Baudrate, Name, Identifier (e.g. Serial number);
2. Load and save configuration;
![Configuration menu](https://github.com/Rukakuka/Serializer/blob/master/img/config_example.PNG)
Raw sensors data and 6-DOF visualization in real-time at high speed (~950 Hz refresh rate). Integrating the gyroscope measurements at such speed allows you to reach very small gyroscope drift. There are individual thread for each sensor separated from UI. All readings stacked into buffer so that it can be parsed lately - and so no data is lost, except if it is damaged (serial intarface errors).

Implemented magnetometer calibration algoritm with saving & loading existing configuration in .xml.
