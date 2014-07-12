#include "WheelOfFortune.h"

WheelOfFortune::~WheelOfFortune() {
	for (int i = 0; i < _numSlices; i++) {
		delete _sliceStrings[i];
		delete _sliceIcons[i];
	}
	delete _sliceStrings;
	delete _sliceIcons;
}

void WheelOfFortune::read(const char *filePath) {
	FILE *file = fopen(filePath, "r");
	if (file == nullptr) {
		fprintf(stderr, "ERROR: Couldn't open %s.\n", filePath);
		return;
	}

	char line[256];

	// count num slices
	_numSlices = 0;
	while (fgets(line, 256, file)) {
		if (strncmp(line, "slice", 5) == 0) {
			_numSlices++;
		}
	}

	// alloc data
	_sliceIcons = new LineObject*[_numSlices];
	_sliceStrings = new char*[_numSlices];
	int slice = 0;

	fseek(file, 0, SEEK_SET);
	while (fgets(line, 256, file)) {
		if (strncmp(line, "slice", 5) != 0) {
			continue;
		}

		_sliceStrings[slice] = new char[64];
		_sliceIcons[slice] = new LineObject();

		char filePath[256];
		glm::vec3 beginColor;
		glm::vec3 endColor;
		sscanf(line, "slice %s %s %f %f %f %f %f %f", _sliceStrings[slice], filePath, &beginColor.r, &beginColor.g , &beginColor.b, &endColor.r, &endColor.g, &endColor.b);

		_sliceIcons[slice]->read(filePath);
		_sliceIcons[slice]->makeLinearColorGradient(beginColor, endColor, -1.0f, 1.0f, 0.0f);

		slice++;
	}

	fclose(file);

	_arrow.read("arrow.lines");
	_arrow.squiggleAmount = 0.0f;
	_arrow.makeLinearColorGradient(glm::vec3(0.8f, 0.1f, 0.0f), glm::vec3(1.0f, 0.5f, 0.4f), -1.0f, 1.0f, 0.0f);
	_circle.read("circle.lines");
	makeSlice();
}

void WheelOfFortune::makeSlice() {
	const int detail = 12; // increase this number to make those graphics really fancy

	_slice.numPositions = detail + 2;
	_slice.numEdges = detail + 2;
	_slice.numTriangles = detail;

	_slice.positions = new float[2 * _slice.numPositions];
	_slice.edges = new unsigned int[2 * _slice.numEdges];
	_slice.triangles = new unsigned int[3 * _slice.numTriangles];

	const float detailAngle = 2.0f * M_PI / (float)(detail * _numSlices);
	for (int i = 0; i < detail; i++) {
		_slice.positions[2 * i + 0] = sinf(i * detailAngle);
		_slice.positions[2 * i + 1] = cosf(i * detailAngle);
		_slice.edges[2 * i + 0] = i;
		_slice.edges[2 * i + 1] = i + 1;
		_slice.triangles[3 * i + 0] = 0;
		_slice.triangles[3 * i + 1] = i + 2;
		_slice.triangles[3 * i + 2] = i + 1;
	}
	_slice.positions[2 * detail + 0] = sinf(detail * detailAngle);
	_slice.positions[2 * detail + 1] = cosf(detail * detailAngle);
	_slice.edges[2 * detail + 0] = detail;
	_slice.edges[2 * detail + 1] = detail + 1;
	_slice.positions[2 * (detail + 1) + 0] = 0.0f;
	_slice.positions[2 * (detail + 1) + 1] = 0.0f;
	_slice.edges[2 * (detail + 1) + 0] = detail + 1;
	_slice.edges[2 * (detail + 1) + 1] = 0;

	_slice.colors = new unsigned char[3 * _slice.numPositions];
	memset(_slice.colors, 0xFF, 3 * _slice.numPositions * sizeof(unsigned char)); // white
}

void WheelOfFortune::tick() {
	_angle += _speed;
	_speed *= 0.998f; // air drag
	_angle = fmodf(_angle, 360.0f); // wrap

	// snap
	float intervalLength = 360.0f / (4 * _numSlices);
	float f = fmodf(_angle, intervalLength) / intervalLength;
	_speed += 0.2f * powf((0.5f - f), 3.0f);
}

bool WheelOfFortune::isSpinning() {
	return fabsf(_speed) > 0.01f;
}

int WheelOfFortune::getSelectedSliceID() {
	int id = _numSlices * (360.0f - _angle) / 360.0f;
	if (id < 0) id = 0;
	if (id >= _numSlices) id = _numSlices - 1;
	return id;
}

const char *WheelOfFortune::getSelectedSliceName() {
	return _sliceStrings[getSelectedSliceID()];
}

void WheelOfFortune::draw() {
	unsigned int ms = SDL_GetTicks();
	float t = (ms % 10000) * 0.0001f; // 0-1 mapped to 10s

	glPushMatrix();
	glScalef(0.8f, 0.8f, 0.8f);

	glPushMatrix();
	glRotatef(-_angle, 0.0f, 0.0f, 1.0f);

	//const float sliceAngle = 2.0f * M_PI / _numSlices;
	for (int i = 0; i < _numSlices; i++) {
		float angle = _angle + 360.0f * (float)i / _numSlices;
		angle *= M_PI / 180.0f;
		float hue = fmodf((float)i / _numSlices + t, 1.0f);
		float saturation = 0.7f + 0.2f * sinf(2.0f * M_PI * fmodf((float)i / _numSlices + 8.0f * t, 1.0f));
		_slice.makeLinearColorGradient(colorFromHSL(hue, saturation, 0.2f), colorFromHSL(hue, saturation, 0.7f), -1.2f, 1.2f, angle);
		_slice.draw();

		glPushMatrix();
		glRotatef(-180.0f / _numSlices, 0.0f, 0.0f, 1.0f);
		glTranslatef(0.0f, 0.55f, 0.0f);
		glScalef(0.22f, 0.22f, 0.22f);
		_sliceIcons[i]->draw();
		glPopMatrix();

		glRotatef(-360.0f / _numSlices, 0.0f, 0.0f, 1.0f);
	}

	// light bulbs
	int numBulbs = 4 * _numSlices;

	if (t > 0.8f) {
		if (fmodf(t, 0.04f) < 0.02f) {
			_circle.makeLinearColorGradient(glm::vec3(0.9f), glm::vec3(1.0f), -1.0f, 1.0f, 0.0f);
		} else {
			_circle.makeLinearColorGradient(glm::vec3(0.4f), glm::vec3(0.6f), -1.0f, 1.0f, 0.0f);
		}
	}

	glRotatef(-180.0f / numBulbs, 0.0f, 0.0f, 1.0f);
	for (int i = 0; i < numBulbs; i++) {

		if (t < 0.8f) {
			int j = (int)(8.0f * t * numBulbs) % numBulbs;
			if (i > j - 2 && i < j + 2) {
				_circle.makeLinearColorGradient(glm::vec3(0.9f), glm::vec3(1.0f), -1.0f, 1.0f, 0.0f);
			} else {
				_circle.makeLinearColorGradient(glm::vec3(0.4f), glm::vec3(0.6f), -1.0f, 1.0f, 0.0f);
			}
		}

		glPushMatrix();
		glTranslatef(0.0f, 0.9f, 0.0f);
		glScalef(0.03f, 0.03f, 0.03f);
		_circle.draw();
		glPopMatrix();
		glRotatef(-360.0f / (numBulbs), 0.0f, 0.0f, 1.0f);
	}

	glPopMatrix();
	
	float s = sinf(4 * _numSlices * (_angle + 270.0f / (4 * _numSlices)) * M_PI / 180.0f);
	glTranslatef(0.0f, 1.0f + 0.02f * sqrtf(1.0f + s), 0.0f);
	glScalef(0.2f, 0.2f, 0.2f);
	_arrow.draw();

	glPopMatrix();
}

void WheelOfFortune::drawResult(float alpha) {
	if (alpha <= 0.0f) return;

	float ease = 3.0f * alpha * alpha - 2.0f * alpha * alpha * alpha;

	glPushMatrix();
	float angle = (SDL_GetTicks() % 36000) * 0.01f;
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	glScalef(ease, ease, 1.0f);
	glBegin(GL_TRIANGLES);
	const float sliceAngle = 2.0f * M_PI / 24.0f;
	for (int i = 0; i < 12; i++) {
		glColor4f(1.0f, 1.0f, 0.0f, ease * 0.5f);
		glVertex2f(0.0f, 0.0f);
		glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
		glVertex2f(2.0f * cosf((2 * i + 0) * sliceAngle), 2.0f * sinf((2 * i + 0) * sliceAngle));
		glVertex2f(2.0f * cosf((2 * i + 1) * sliceAngle), 2.0f * sinf((2 * i + 1) * sliceAngle));
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glRotatef(ease * 360.0f, 0.0f, 0.0f, 1.0f);
	glScalef(ease * 0.6f, ease * 0.6f, 1.0f);
	_sliceIcons[getSelectedSliceID()]->draw();
	glPopMatrix();
}
