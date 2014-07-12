#include "WheelOfFortune.h"

void drawPowerMeter(float power) {
	static const float positions[] = {
		-0.2f, -1.0f,
		0.2f, -1.0f,
		-0.2f, 0.0f,
		0.2f, 0.0f,
		-0.2f, 1.0f,
		0.2f, 1.0f
	};
	static const unsigned char colors[] = {
		0x00, 0xFF, 0x00,
		0x00, 0xFF, 0x00,
		0xFF, 0xFF, 0x00,
		0xFF, 0xFF, 0x00,
		0xFF, 0x00, 0x00,
		0xFF, 0x00, 0x00
	};
	static const unsigned int edges[] = {
		0, 1, 5, 4
	};

	glPushMatrix();
	glScalef(0.8f, 0.8f, 1.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, positions);
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_UNSIGNED_BYTE, 0, colors);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
	glDisableClientState(GL_COLOR_ARRAY);
	glColor3f(0.0f, 0.0f, 0.0f);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, edges);
	glDrawElements(GL_POINTS, 4, GL_UNSIGNED_INT, edges);
	glDisableClientState(GL_VERTEX_ARRAY);


	glRectf(-0.3f, 2.0f * power - 1.0f - 0.02f, 0.3f, 2.0f * power - 1.0f + 0.02f);

	glBegin(GL_LINES);
	glVertex2f(-0.3f, 2.0f * power - 1.0f);
	glVertex2f(0.3f, 2.0f * power - 1.0f);
	glEnd();

	glPopMatrix();
}

enum State {
	StateInitial,
	StatePower,
	StateSpin,
	StateResult,

	NumStates
};

int main(int argc, char *argv[]) {
	Graphics::glVersionMajor = 2;
	Graphics::glVersionMinor = 1;
	Graphics::glUseCompatibilityProfile = true;
	
	Graphics::highDPI = 1;
	Graphics::videoWidth = 1024;
	Graphics::videoHeight = 768;
	Graphics::fullscreen = 0;
	Graphics::aspectRatio = (float)Graphics::videoWidth / (float)Graphics::videoHeight;

	Graphics::initSDL();
	SDL_SetWindowTitle(Graphics::sdlWindow, "Wheel of Fortune");
	Graphics::init();
	glDisable(GL_DEPTH_TEST);

	SDL_ShowCursor(SDL_FALSE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-Graphics::aspectRatio, Graphics::aspectRatio, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	WheelOfFortune *wof = new WheelOfFortune();
	wof->read("wof.txt");

	glLineWidth(3.0f * Graphics::pixelScale);
	glPointSize(3.0f * Graphics::pixelScale);

	int state = StateInitial;
	float power = 0.0f;
	float powerAlpha = 0.0f;
	float resultAlpha = 0.0f;

	bool running = true;
	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				running = event.key.keysym.sym != SDLK_ESCAPE;
				break;
			case SDL_MOUSEBUTTONUP:
			//case SDL_KEYUP:
				if (event.button.button == SDL_BUTTON_LEFT) {
				//if (event.key.keysym.sym == SDLK_SPACE) {
					if (state == StatePower) {
						wof->_speed = 4.0f + 20.0f * power;
					}
					if (state != StateSpin) {
						state = (state + 1) % NumStates; // next state
					}
				}
				break;
			case SDL_QUIT:
 				running = false;
				break;
			}
		}

		if (state == StatePower) {
			unsigned int ms = SDL_GetTicks();
			power = (ms % 500) * 0.004f;
			if (power > 1.0f) {
				power = 2.0f - power;
			}
			if (powerAlpha < 1.0f) {
				powerAlpha += 0.03f;
			}
		} else {
			if (powerAlpha > 0.0f) {
				powerAlpha -= 0.03f;
			}
		}

		if (state == StateSpin && !wof->isSpinning()) {
			state = StateResult;
			printf("result: %s\n", wof->getSelectedSliceName());
		}

		if (state == StateResult) {
			if (resultAlpha < 1.0f) {
				resultAlpha += 0.03f;
			}
		} else {
			if (resultAlpha > 0.0f) {
				resultAlpha -= 0.03f;
			}
		}

		wof->tick();

		glClearColor(0.4, 0.5, 0.6, 1.0);
		//glClearColor(1.0, 1.0, 1.0, 1.0);
		//glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLoadIdentity();

		float ease = 3.0f * powerAlpha * powerAlpha - 2.0f * powerAlpha * powerAlpha * powerAlpha;
		glTranslatef(-0.4f * ease, 0.0f, 0.0f);
		wof->draw();
		wof->drawResult(resultAlpha);
		glTranslatef(1.2f + (1.0f - ease), 0.0f, 0.0f);
		drawPowerMeter(power);

		SDL_GL_SwapWindow(Graphics::sdlWindow);
	}

	delete wof;

	Graphics::destroy();
}
