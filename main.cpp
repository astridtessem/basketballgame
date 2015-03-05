#include "includes.h"

//namespaces
using namespace cv;
using namespace std;

Mat currentFrame;
Mat currentFrameGray;
Mat prevFrame;
Mat prevFrameGray;
Mat flow;
Mat difference;
Mat drawingFrame;
Mat background;
Mat backgroundgray;

VideoCapture cap;

GLfloat ambientColor[] = { 0.2, 0.2, 0.2, 1.0 }; //Color(0.2, 0.2, 0.2)

bool gameStarted = false;

void addLight(){

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { 0.0, 0.0, -50.0, 0.0 };
	glShadeModel(GL_SMOOTH);


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

}

void display(){

	glDrawPixels(drawingFrame.size().width, drawingFrame.size().height, GL_BGR_EXT, GL_UNSIGNED_BYTE, drawingFrame.ptr());

	if (gameStarted){

		glPushMatrix();
		glColorMaterial(GL_FRONT, GL_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);
		glColor3f(1, 0.45f, 0);
		glTranslated(0.6, 0, 0);
		glutSolidSphere(0.1, 20, 20);

		glLoadIdentity();
		glColor3f(1, 0.45f, 0);
		glTranslated(-0.6, 0, 0);
		glRotated(70, 1, 0, 0);
		glutSolidTorus(0.02, 0.3, 20, 20);
		glPopMatrix();

	}


	glutSwapBuffers();
	glutPostRedisplay();

	prevFrame = currentFrame.clone();
	if (!prevFrame.empty())
		cvtColor(prevFrame, prevFrameGray, CV_BGR2GRAY);
}

void keyboard(unsigned char key, int x, int y){
	switch (key){
	case 'q':
		exit(0);
		break;
	case 'b':
		background = currentFrameGray.clone();
		resize(background, background, Size(320, 240));
		break;

	default:
		break;
	}
}

void calcOpticalFlow(){
	calcOpticalFlowFarneback(prevFrameGray, currentFrameGray, flow, 0.5, 1, 3, 1, 5, 1.1, 0);
	Mat xy[2];
	Mat magnitude;
	Mat angle;
	split(flow, xy);

	cartToPolar(xy[0], xy[1], magnitude, angle, true);
	resize(magnitude, flow, Size(640, 480));
	threshold(flow, flow, 10, 255, 0);
	cv::flip(flow, flow, 1);
	imshow("flow", flow);
}

void subtractImages(){
	resize(currentFrameGray, currentFrameGray, Size(320, 240));
	absdiff(background, currentFrameGray, difference);
	threshold(difference, difference, 50, 255, 0);
	cv::flip(difference, difference, 1);
	imshow("Difference", difference);
}

void startGameButton(){
	rectangle(drawingFrame, Point(10, 10), Point(120, 60), Scalar(0, 0, 255), CV_FILLED);
	putText(drawingFrame, "Start game", Point(14, 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1, 8, false);

	if (countNonZero(difference(Rect(Point(10, 10), Point(60, 30))))>500){
		cout << "Game started";
		gameStarted = true;
	}
}

void idle(){

	cap >> currentFrame;
	cvtColor(currentFrame, currentFrameGray, CV_BGR2GRAY);
	drawingFrame = currentFrame.clone();
	cv::flip(drawingFrame, drawingFrame, 1);
	
	if (!background.empty()){
		//calcOpticalFlow();
		subtractImages();
		if (!gameStarted){
			startGameButton();
		}
		if (gameStarted){

		}
	}

	cv::flip(drawingFrame, drawingFrame, 0);
}

int main(int argc, char** argv)
{
	cap = VideoCapture(0);
	// initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Basket Ball Game");
	//glutFullScreen();

	// set up GUI callback functions
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	addLight();
	// start GUI loop
	glutMainLoop();

	return 0;

}