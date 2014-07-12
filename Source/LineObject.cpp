#include "LineObject.h"

float randf() {
	return (float)rand() / (float)INT_MAX;
}

void squigglyPositions(float *origPositions, float *squigglyPositions, int numPositions, float amount) {
	for (int i = 0; i < numPositions; i++) {
		squigglyPositions[i] = origPositions[i] + amount * randf();
	}
}

LineObject::LineObject() : numPositions(0), numEdges(0), numTriangles(0), positions(nullptr), tmpPositions(nullptr), edges(nullptr), triangles(nullptr), colors(nullptr), squiggleAmount(0.04f), squiggleFrequency(4), squiggleCounter(0) {
}

LineObject::~LineObject() {
	delete positions;
	delete edges;
	delete triangles;

	delete colors;
}

void LineObject::read(const char *filePath) {
	FILE *file = fopen(filePath, "rb");
	if (file == nullptr) {
		fprintf(stderr, "ERROR: Couldn't open %s.\n", filePath);
		return;
	}

	fread(&numPositions, sizeof(unsigned int), 1, file);
	fread(&numEdges, sizeof(unsigned int), 1, file);
	fread(&numTriangles, sizeof(unsigned int), 1, file);

	positions = new float[2 * numPositions];
	edges = new unsigned int[2 * numEdges];
	triangles = new unsigned int[3 * numTriangles];

	colors = new unsigned char[3 * numPositions];

	fread(positions, sizeof(float), 2 * numPositions, file);
	fread(edges, sizeof(unsigned int), 2 * numEdges, file);
	fread(triangles, sizeof(unsigned int), 3 * numTriangles, file);
	memset(colors, 0xFF, 3 * numPositions * sizeof(unsigned char)); // white

	tmpPositions = new float[2 * numPositions];

	fclose(file);
}

glm::vec3 colorFromHSL(float hue, float saturation, float lightness) {
	float c = (1.0f - fabsf(2.0f * lightness - 1.0f)) * saturation;
	float hue6 = 6.0f * hue;
	float x = c * (1.0f - fabsf(fmodf(hue6, 2.0f) - 1.0f));
	float m = lightness - 0.5f * c;
	c += m; x += m;

	if (hue6 < 1.0f) return glm::vec3(c, x, m);
	if (hue6 < 2.0f) return glm::vec3(x, c, m);
	if (hue6 < 3.0f) return glm::vec3(m, c, x);
	if (hue6 < 4.0f) return glm::vec3(m, x, c);
	if (hue6 < 5.0f) return glm::vec3(x, m, c);
	return glm::vec3(c, m, x);
}

void LineObject::makeLinearColorGradient(const glm::vec3 &beginColor, const glm::vec3 &endColor, float minY, float maxY, float angle) {
	const float s = sinf(angle), c = cosf(angle);
	glm::mat2 rotation(c, -s, s, c);

	for (unsigned int i = 0; i < numPositions; i++) {
		glm::vec2 position(positions[2 * i + 0], positions[2 * i + 1]);
		position = rotation * position;
		glm::vec3 color = glm::mix(beginColor, endColor, (position.y - minY) / (maxY - minY));

		colors[3 * i + 0] = (unsigned char)(255.0f * color.r);
		colors[3 * i + 1] = (unsigned char)(255.0f * color.g);
		colors[3 * i + 2] = (unsigned char)(255.0f * color.b);
	}
}

void LineObject::draw() {
	glEnableClientState(GL_VERTEX_ARRAY);
	if (tmpPositions != nullptr) {
		if (squiggleCounter-- <= 0) {
			squigglyPositions(positions, tmpPositions, 2 * numPositions, squiggleAmount);
			squiggleCounter = squiggleFrequency;
		}
		glVertexPointer(2, GL_FLOAT, 0, tmpPositions);
	} else {
		glVertexPointer(2, GL_FLOAT, 0, positions);
	}
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_UNSIGNED_BYTE, 0, colors);
	glDrawElements(GL_TRIANGLES, 3 * numTriangles, GL_UNSIGNED_INT, triangles);
	glDisableClientState(GL_COLOR_ARRAY);

	// outline
	glColor3f(0.0f, 0.0f, 0.0f);
	glDrawElements(GL_LINES, 2 * numEdges, GL_UNSIGNED_INT, edges);
	glDrawElements(GL_POINTS, 2 * numEdges, GL_UNSIGNED_INT, edges);

	glDisableClientState(GL_VERTEX_ARRAY);
}
