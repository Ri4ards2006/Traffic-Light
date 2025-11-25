#pragma once
#include <Arduino.h>
#define MAX_FIELDS 5

 class KrokoTS
 {
 public:
 	KrokoTS(byte x1, byte x2, byte y1, byte y2);
 	int getX();
 	int getY();
    void attachClick(byte id, int minX, int maxX, int minY, int maxY, void (*func)());
    void tick();

 private:
   void (*p_function[MAX_FIELDS])(void);   // указатель на p_function
   int functionattrs[MAX_FIELDS][4];
 	byte _x1;
 	byte _x2;
 	byte _y1;
 	byte _y2;

 };
