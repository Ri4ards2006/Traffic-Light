/*
  GadgetBox.h - Pin definition functions for GadgetBox
  http://www.GadgetFactory.net

  Copyright (c) 2016 Jack Gassett

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA
*/

#ifndef GadgetBox_h
#define GadgetBox_h

  #if defined(CORE_TEENSY)
      //Teensy
      #define GB0 0
      #define GB1 1
      #define GB2 3
      #define GB3 4
      #define GB4 5
      #define GB5 14
      #define GB6 15
      #define GB7 16
      #define GB8 17
      #define GB9 20
      #define GB10 21
      #define GB11 6
      #define GB12 22
      #define GB13 23
      #define GB14 10
      #define GB15 9
      
      #define AC0 GB7
      #define AC1 GB6
      #define AC2 GB5
      #define AC3 GB4
      #define AC4 GB3
      #define AC5 GB2
      #define AC6 GB1
      #define AC7 GB0
      
      #define BC0 GB0
      #define BC1 GB1
      #define BC2 GB2
      #define BC3 GB3
      #define BC4 GB4
      #define BC5 GB5
      #define BC6 GB6
      #define BC7 GB7
      
      #define CC0 GB15
      #define CC1 GB14
      #define CC2 GB13
      #define CC3 GB12
      #define CC4 GB11
      #define CC5 GB10
      #define CC6 GB9
      #define CC7 GB8
      
      #define DC0 GB8
      #define DC1 GB9
      #define DC2 GB10
      #define DC3 GB11
      #define DC4 GB12
      #define DC5 GB13
      #define DC6 GB14
      #define DC7 GB15
      
      #define CSA 2
      #define CSB 7
      #define CSC 8
      #define CSD 17
  
  #elif defined(ARDUINO_AVR_PRO)
      #define GB0 0
      #define GB1 1
      #define GB2 2
      #define GB3 3
      #define GB4 6
      #define GB5 A3
      #define GB6 A2
      #define GB7 A1
      #define GB8 A0
      #define GB9 A6
      #define GB10 A7
      #define GB11 9
      #define GB12 5
      #define GB13 7
      #define GB14 8
      #define GB15 10
      
      #define AC0 GB7
      #define AC1 GB6
      #define AC2 GB5
      #define AC3 GB4
      #define AC4 GB3
      #define AC5 GB2
      #define AC6 GB1
      #define AC7 GB0
      
      #define BC0 GB0
      #define BC1 GB1
      #define BC2 GB2
      #define BC3 GB3
      #define BC4 GB4
      #define BC5 GB5
      #define BC6 GB6
      #define BC7 GB7
      
      #define CC0 GB15
      #define CC1 GB14
      #define CC2 GB13
      #define CC3 GB12
      #define CC4 GB11
      #define CC5 GB10
      #define CC6 GB9
      #define CC7 GB8
      
      #define DC0 GB8
      #define DC1 GB9
      #define DC2 GB10
      #define DC3 GB11
      #define DC4 GB12
      #define DC5 GB13
      #define DC6 GB14
      #define DC7 GB15
      
      #define CSA GB7
      #define CSB 4
      #define CSC GB15
      #define CSD GB8
  
  #elif defined(ARDUINO_RASPI)
      #define GB0 15
      #define GB1 14
      #define GB2 4
      #define GB3 17
      #define GB4 27
      #define GB5 22
      #define GB6 5
      #define GB7 6
      #define GB8 13
      #define GB9 19
      #define GB10 26
      #define GB11 18
      #define GB12 23
      #define GB13 24
      #define GB14 25
      #define GB15 8
      
      #define CSA 7
      #define CSB 12
      #define CSC 16
      #define CSD 20

  #elif defined(ESP8266)
      #define GB0 3
      #define GB1 1
      #define GB2 4
      #define GB3 5

      #define AC4 GB3
      #define AC5 GB2
      #define AC6 GB1
      #define AC7 GB0

      #define BC0 GB0
      #define BC1 GB1
      #define BC2 GB2
      #define BC3 GB3
      
      #define MOSI 13
      #define MISO 12
      #define SCK 14
      
      #define SDA 0
      #define SCL 2
      
      #define CSA 4
      #define CSB 5
      #define CSC 15
      #define CSD 16
  
  #endif

#endif
