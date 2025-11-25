#ifndef Communication_H
#define Communication_H

// Define the orders that can be sent and received
enum Communication {
  NONE = 0,
  FAST = 1,
  SECURE = 2,
  WIFI = 3,
};

typedef enum Communication Communication;

#endif