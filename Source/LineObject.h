#ifndef LINEOBJECT_H_INCLUDED
#define LINEOBJECT_H_INCLUDED

#include "Graphics.h"

struct LineObject {
	unsigned int numPositions;
	unsigned int numEdges;
	unsigned int numTriangles;
	float *positions;
	float *tmpPositions;
	unsigned int *edges;
	unsigned int *triangles;

	unsigned char *colors; // vertex colors

	float squiggleAmount;
	int squiggleFrequency;
	int squiggleCounter;

	LineObject();
	~LineObject();

	void read(const char *filePath);

	void makeLinearColorGradient(const glm::vec3 &beginColor, const glm::vec3 &endColor, float minY, float maxY, float angle);

	void draw();
};

glm::vec3 colorFromHSL(float hue, float saturation, float lightness);

#endif
