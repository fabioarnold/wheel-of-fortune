#ifndef WHEELOFFORTUNE_H_INCLUDED
#define WHEELOFFORTUNE_H_INCLUDED

#include "LineObject.h"

struct WheelOfFortune {
	~WheelOfFortune();

	float _angle;
	float _speed;

	LineObject _slice;
	LineObject _arrow;
	LineObject _circle;

	int _numSlices;

	char **_sliceStrings;
	LineObject **_sliceIcons;

	void read(const char *filePath);
	void makeSlice();

	bool isSpinning();
	int getSelectedSliceID();
	const char *getSelectedSliceName();

	void tick();
	void draw();
	void drawResult(float alpha);
};

#endif
