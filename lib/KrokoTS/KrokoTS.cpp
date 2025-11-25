#include <KrokoTS.h>

KrokoTS::KrokoTS(byte x1, byte x2, byte y1, byte y2){
	_x1 = x1;
	_x2 = x2;
	_y1 = y1;
	_y2 = y2;

}

int KrokoTS::getX(){
	pinMode(_x1, OUTPUT);
  	pinMode(_x2, OUTPUT);

  	pinMode(_y1, INPUT_PULLUP);
  	pinMode(_y2, INPUT_PULLUP);

  	digitalWrite(_x2, HIGH);
  	digitalWrite(_x1, LOW);
	delay(10);
  	int x_pos = analogRead(_y1);
  	delay(2);
  	return constrain(x_pos, 0, 1023);

}

int KrokoTS::getY(){
	pinMode(_y1, OUTPUT);
  	pinMode(_y2, OUTPUT);

  	pinMode(_x1, INPUT_PULLUP);
  	pinMode(_x2, INPUT_PULLUP);

  	digitalWrite(_y2, HIGH);
  	digitalWrite(_y1, LOW);
  	delay(10);
  	int y_pos = analogRead(_x1);
  	delay(2);
  	return constrain(y_pos, 0, 1023);

}

void KrokoTS::attachClick(byte id, int minX, int maxX, int minY, int maxY, void (*func)()){
	p_function[id] = *func;
	int attrs[4] = {minX, maxX, minY, maxY};
	for(int i = 0; i<5; i++){
	functionattrs[id][i] = attrs[i];
}
}

void KrokoTS::tick(){
	 int x_pos = KrokoTS::getX();
	 int y_pos = KrokoTS::getY();
	 for(int i = 0; i<MAX_FIELDS; i++){

	 	int funcattr[4];
	 	for(int j = 0; j<5; j++) {
	 	funcattr[j]= functionattrs[i][j];
	 }
	 	if (funcattr[0]<x_pos&&funcattr[1]>x_pos&&funcattr[2]<y_pos&&funcattr[3]>y_pos){
	 		p_function[i]();
	 	}
	 }

}

