# Serializer
Qt Widget app for controlling a bunch of arduinos &amp; visualize incoming data.

Arduino's algoritm hard-coded with VSCode and free Arduino extension. There are custom sensor libraries with lightweight [I2C library](http://dsscircuits.com/articles/arduino-i2c-master-library). Minimalistic communication with `Serial.write()` at 3 million baud instead of classic `Serial.print()` so that refresh rate hits ~950 Hz. Error control executed at receiver's side.
Reciver side is implemented with Qt. The app allows user to control COM devices' congiguration from UI instead of opening XML configuration file with editor
1. Set settings such as Baudrate, Name, Identifier (e.g. Serial number);
2. Load and save configuration;
3. Display current status and port (non-editable fields)

![Configuration menu](https://github.com/Rukakuka/Serializer/blob/master/img/config_example.PNG)

App visualizes raw sensors data and 6-DOF visualization in real-time at ~950 Hz refresh rate. Integrating the gyroscope measurements at such speed allows you to reach very small gyroscope drift. All readings stacked into buffer so that it can be parsed lately - and so no data is lost, except if it is damaged (serial intarface errors). There are individual thread for each sensor separated from UI, so that readings, parsing and UI actions can be done asynchonosly.

![Execution example](https://github.com/Rukakuka/Serializer/blob/master/img/imu_example.gif)

Implemented magnetometer calibration algoritm with saving & loading existing configuration in .xml. Adding each single measurement to a scatter graph allows to perform calinration more accurately.

![Calibration menu](https://github.com/Rukakuka/Serializer/blob/master/img/calib_example.PNG)
