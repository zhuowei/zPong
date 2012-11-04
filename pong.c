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

#define LEFT_SIDE 0
#define RIGHT_SIDE 1

#define LOSE_SERVE_TIME 60

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

int loseTimer;

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
	leftPaddle.y = (FIELD_HEIGHT / 2) - (PADDLE_HEIGHT / 2);
	leftPaddle.width = PADDLE_WIDTH;
	leftPaddle.height = PADDLE_HEIGHT;

	rightPaddle.x = FIELD_WIDTH - PADDLE_WIDTH;
	rightPaddle.y = (FIELD_HEIGHT / 2) - (PADDLE_HEIGHT / 2);
	rightPaddle.width = PADDLE_WIDTH;
	rightPaddle.height = PADDLE_HEIGHT;

	ball.x = (FIELD_WIDTH / 2) - (BALL_WIDTH / 2);
	ball.y = (FIELD_HEIGHT / 2) - (BALL_HEIGHT / 2);
	ball.motionX = 2;
	ball.motionY = 2;
	ball.width = BALL_WIDTH;
	ball.height = BALL_HEIGHT;
}

void keyboardHandler(unsigned char keyChar, int mouseX, int mouseY) {
	switch (keyChar) {
		case PADDLE_LEFT_UP_KEY:
			leftPaddle.motionY = 4;
			break;
		case PADDLE_LEFT_DOWN_KEY:
			leftPaddle.motionY = -4;
			break;
		case PADDLE_RIGHT_UP_KEY:
			rightPaddle.motionY = 4;
			break;
		case PADDLE_RIGHT_DOWN_KEY:
			rightPaddle.motionY = -4;
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
void handleLose(int side) {
	loseTimer = LOSE_SERVE_TIME;
	initPaddles();
	if (side == RIGHT_SIDE) {
		ball.motionX *= -1;
	}
}

/* yPaddle = distance from bottom of paddle to ball's centre */

float getVertBallSpeedRebound(float yPaddle) {
	float paddleHalf = PADDLE_HEIGHT / 2;
	float paddleQuarter = paddleHalf / 2;
	if (yPaddle < paddleQuarter) {
		return -4;
	} else if (yPaddle < paddleHalf - 10) {
		return -2;
	} else if (yPaddle < paddleHalf + 10) {
		return 0;
	} else if (yPaddle < paddleHalf + paddleQuarter) {
		return 2;
	}
	return 4;
}

float getHorizBallSpeedRebound(float yPaddle) {
	float paddleHalf = PADDLE_HEIGHT / 2;
	float paddleQuarter = paddleHalf / 2;
	if (yPaddle < paddleQuarter) {
		return 1;
	} else if (yPaddle < paddleHalf - 10) {
		return 2;
	} else if (yPaddle < paddleHalf + 10) {
		return 4.5;
	} else if (yPaddle < paddleHalf + paddleQuarter) {
		return 2;
	}
	return 1;
}


void runPhysics() {
	if (loseTimer > 0) {
		loseTimer --;
		return;
	}
	ball.x += ball.motionX;
	ball.y += ball.motionY;
	if (ball.x + ball.width >= FIELD_WIDTH || ball.x <= 0) {
		ball.motionX *= -1;
	}

	if (ball.y + ball.height >= FIELD_HEIGHT || ball.y <= 0) {
		ball.motionY *= -1;
	}

	if (ball.x <= leftPaddle.x + leftPaddle.width) {
		float ballCentre = ball.y + (ball.width / 2);
		printf("%f %f\n", ballCentre, leftPaddle.y);
		if (ballCentre > leftPaddle.y && ballCentre <= leftPaddle.y + leftPaddle.height) {
			float yOffset = ballCentre - leftPaddle.y;
			ball.motionX = getHorizBallSpeedRebound(yOffset);
			ball.motionY = getVertBallSpeedRebound(yOffset);
			ball.x = leftPaddle.x + leftPaddle.width + 1;
		}
		if (ball.x <= 0) {
			printf("Left loses\n");
			handleLose(LEFT_SIDE);
		}
	}

	if (ball.x + ball.width >= rightPaddle.x) {
		float ballCentre = ball.y + (ball.width / 2);
		printf("%f %f\n", ballCentre, leftPaddle.y);
		if (ballCentre > rightPaddle.y && ballCentre <= rightPaddle.y + rightPaddle.height) {
			float yOffset = ballCentre - rightPaddle.y;
			ball.motionX = -1 * getHorizBallSpeedRebound(yOffset);
			ball.motionY = getVertBallSpeedRebound(yOffset);
			ball.x = rightPaddle.x - 1 - ball.width;
		}
		if (ball.x + ball.width >= FIELD_WIDTH) {
			printf("Right loses\n");
			handleLose(RIGHT_SIDE);
		}
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


int main(int argc, char** argv) {
	initPaddles();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(FIELD_WIDTH, FIELD_HEIGHT);
	glutCreateWindow("Pong");
	glutDisplayFunc(renderFrame);
	glutReshapeFunc(reshapeHandler);
	glutKeyboardFunc(keyboardHandler);
	glutKeyboardUpFunc(keyUpHandler);
	glutTimerFunc(MILLISECONDS_PER_FRAME, frameTimerCallback, 0);
	glutMainLoop();
	return 1; //should never reach here
}
