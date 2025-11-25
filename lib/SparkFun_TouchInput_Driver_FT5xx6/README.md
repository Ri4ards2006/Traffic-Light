SparkFun TouchInput Driver FT5xx6
========================================

This library allows for control of FT5xx6 capacitive touchscreen drivers. It is possible to either read the touch locations directly or use the SparkFun TouchInput compatible wrapper. 

5-point multitouch capability.

Basic readings are performed by polling over I2C, however it is possible to rely on an interrupt to signal new data. Optionally a buffer of touch records can be implemented in order to simplify gesture recognition.


Repository Contents
-------------------

* **/examples** - Example sketches for the library (.ino). Run these from the Arduino IDE.
* **/src** - Source files for the library (.cpp, .h).
* **keywords.txt** - Keywords from this library that will be highlighted in the Arduino IDE.
* **library.properties** - General library properties for the Arduino package manager.

Documentation
--------------

* **[Installing an Arduino Library Guide](https://learn.sparkfun.com/tutorials/installing-an-arduino-library)** - Basic information on how to install an Arduino library.

Products that use this Library 
---------------------------------
* [Qwiic Capacitive Touchscreen - 7in](https://www.sparkfun.com/products/15104)

Version History
---------------
* 1.0.0 - Initial release

License Information
-------------------

This product is _**open source**_!

The **code** is released under the GPL v3 license. See the included LICENSE.md for more information.

Distributed as-is; no warranty is given.

- Your friends at SparkFun.
