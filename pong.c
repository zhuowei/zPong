#include <stdio.h>
#include <time.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define PADDLE_HEIGHT 100
#define PADDLE_WIDTH 20

#define PADDLE_LEFT_UP_KEY 113
#define PADDLE_LEFT_DOWN_KEY 122

#define PADDLE_RIGHT_UP_KEY 112
#define PADDLE_RIGHT_DOWN_KEY 44

#define MILLISECONDS_PER_FRAME 20

#define BALL_WIDTH 20
#define BALL_HEIGHT 20

#define FIELD_WIDTH 640
#define FIELD_HEIGHT 480

typedef struct {
	float x;
	float y;
	int width;
	int height;
	float motionX;
	float motionY;
} Entity;

Entity leftPaddle;
Entity rightPaddle;
Entity ball;

void renderPaddle(Entity *paddle) {

	glRectf(paddle->x, paddle->y, paddle->x + paddle->width, paddle->y + paddle->height);
}

void renderFrame(void) {
	glClearColor(0.5f, 0.5f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 0.0f, 0.0f);
	renderPaddle(&leftPaddle);
	glColor3f(0.0f, 1.0f, 0.0f);
	renderPaddle(&rightPaddle);
	glColor3f(1.0f, 1.0f, 0.0f);
	renderPaddle(&ball);
	glutSwapBuffers();
	//printf("render");
}

void reshapeHandler(int w, int h) {
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
}

void initPaddles() {
	leftPaddle.x = 0;
	leftPaddle.y = 240 - (PADDLE_HEIGHT / 2);
	leftPaddle.width = PADDLE_WIDTH;
	leftPaddle.height = PADDLE_HEIGHT;

	rightPaddle.x = 640 - PADDLE_WIDTH;
	rightPaddle.y = 240 - (PADDLE_HEIGHT / 2);
	rightPaddle.width = PADDLE_WIDTH;
	rightPaddle.height = PADDLE_HEIGHT;

	ball.x = 320 - (BALL_WIDTH / 2);
	ball.y = 240 - (BALL_HEIGHT / 2);
	ball.motionX = 2;
	ball.motionY = 2;
	ball.width = BALL_WIDTH;
	ball.height = BALL_HEIGHT;
}

void keyboardHandler(unsigned char keyChar, int mouseX, int mouseY) {
	printf("%d, %d, %d\n", (int) keyChar, mouseX, mouseY);
	switch (keyChar) {
		case PADDLE_LEFT_UP_KEY:
			leftPaddle.motionY = 2;
			break;
		case PADDLE_LEFT_DOWN_KEY:
			leftPaddle.motionY = -2;
			break;
		case PADDLE_RIGHT_UP_KEY:
			rightPaddle.motionY = 2;
			break;
		case PADDLE_RIGHT_DOWN_KEY:
			rightPaddle.motionY = -2;
			break;
		default:
			break;
	}
}

void keyUpHandler(unsigned char keyChar, int mouseX, int mouseY) {
	switch (keyChar) {
		case PADDLE_LEFT_UP_KEY:
		case PADDLE_LEFT_DOWN_KEY:
			leftPaddle.motionY = 0;
			break;
		case PADDLE_RIGHT_UP_KEY:
		case PADDLE_RIGHT_DOWN_KEY:
			rightPaddle.motionY = 0;
			break;
		default:
			break;
	}
}


void runPhysics() {
	ball.x += ball.motionX;
	ball.y += ball.motionY;
	if (ball.x + ball.width >= FIELD_WIDTH || ball.x <= 0) {
		ball.motionX *= -1;
	}

	if (ball.y + ball.height >= FIELD_HEIGHT || ball.y <= 0) {
		ball.motionY *= -1;
	}

	float newPaddleY = leftPaddle.y + leftPaddle.motionY;
	if (newPaddleY < 0 || newPaddleY + leftPaddle.height >= FIELD_HEIGHT) {
		leftPaddle.motionY = 0;
	} else {
		leftPaddle.y = newPaddleY;
	}

	newPaddleY = rightPaddle.y + rightPaddle.motionY;

	if (newPaddleY < 0 || newPaddleY + rightPaddle.height >= FIELD_HEIGHT) {
		rightPaddle.motionY = 0;
	} else {
		rightPaddle.y = newPaddleY;
	}
}


void frameTimerCallback() {
	runPhysics();
	glutTimerFunc(MILLISECONDS_PER_FRAME, frameTimerCallback, 0);
	glutPostRedisplay();
}

/* from http://gamedev.stackexchange.com/questions/8623/a-good-way-to-build-a-game-loop-in-opengl */

static long queuedMilliseconds; // static or whatever, this must persist outside of your loop
static long lastProcessorTick;


int main(int argc, char** argv) {
	initPaddles();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Pong");
	glutDisplayFunc(renderFrame);
	glutReshapeFunc(reshapeHandler);
	glutKeyboardFunc(keyboardHandler);
	glutKeyboardUpFunc(keyUpHandler);
	glutTimerFunc(MILLISECONDS_PER_FRAME, frameTimerCallback, 0);
	glutMainLoop();
	return 1; //should never reach here
}
