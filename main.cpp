#define _USE_MATH_DEFINES
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

Mat floorImg = imread("src/src/floor.png");

VideoCapture cap;

GLfloat ambientColor[] = { 0.2, 0.2, 0.2, 1.0 }; //Color(0.2, 0.2, 0.2)

bool gameStarted = false;
bool powerChoosen = false;
bool directionChoosen = false;
double tempPower = 0;
double power = 0;
double tempDirection = 0;
double direction;
double step = M_PI/20;
bool choosingPower=false;
bool choosingDirection = false;
bool ballThrown = false;
bool ballLanded=false;
bool showHighScoreList=false;
clock_t start;
clock_t showHighScoreListStart;

int hitBoard = 1;
bool hit = false;

double heightOfBall=0;

Point2f ballPos=Point2f(0.6,0);

double powerX=0;
double powerY=0;

int numberOfHits=0;

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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

}

static void DrawParallelepiped(GLfloat sizeX, GLfloat sizeY, GLfloat sizeZ, GLenum type){

	static GLfloat n[6][3] = {
		{-1.0, 0.0, 0.0},
		{0.0, 1.0, 0.0},
		{1.0, 0.0, 0.0},
		{0.0, -1.0, 0.0},
		{0.0, 0.0, 1.0},
		{0.0, 0.0, -1.0}
	};

	static GLint faces[6][4] = {
		{0, 1, 2, 3},
		{3, 2, 6, 7},
		{7, 6, 5, 4},
		{4, 5, 1, 0},
		{5, 6, 2, 1},
		{7, 4, 0, 3}
	};

	GLfloat v[8][3];
	GLint i;

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -sizeX / 2;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = sizeX / 2;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -sizeY / 2;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = sizeY / 2;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = -sizeZ / 2;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = sizeZ / 2;

	for (i = 5; i >= 0; i--) {
	glBegin(type);
	glNormal3fv(&n[i][0]);
	glVertex3fv(&v[faces[i][0]][0]);
	glVertex3fv(&v[faces[i][1]][0]);
	glVertex3fv(&v[faces[i][2]][0]);
	glVertex3fv(&v[faces[i][3]][0]);
	glEnd();
	}

}

void display(){

	glDrawPixels(drawingFrame.size().width, drawingFrame.size().height, GL_BGR_EXT, GL_UNSIGNED_BYTE, drawingFrame.ptr());

	if (gameStarted){

		glDrawPixels(floorImg.size().width, floorImg.size().height, GL_BGR_EXT, GL_UNSIGNED_BYTE, floorImg.ptr());
		glClear(GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
		glColorMaterial(GL_FRONT, GL_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);
		glColor3f(1, 0.45f, 0);
		glTranslated(0.6 - powerX / 800, 0 + heightOfBall / 1000, 0);
		glutSolidSphere(0.05, 20, 20);

        glLoadIdentity();
        glColor3f(0, 0, 0);
        glTranslated(-0.90, 0, 0);
        glTranslated(0, 0.1, 0);
        glRotated(87,0,1,0);
        glRotated(-20,0,0,1);

        DrawParallelepiped(0.6,0.5,0.01,GL_POLYGON);

        glLoadIdentity();
		glColor3f(1, 0.45f, 0);
		glTranslated(-0.86, 0, 0);
        glRotated(-20,1,0,0);
		glutSolidCube(0.05);
		glTranslated(-0.075, 0, 0);
        glutSolidCube(0.05);


		glLoadIdentity();
		glColor3f(1, 0.45f, 0);
		glTranslated(-0.97, -0.42, 0);
        glRotated(-20,1,0,0);
        DrawParallelepiped(0.05,1,0.05,GL_POLYGON);

		glLoadIdentity();
		glColor3f(1, 0.45f, 0);
		glTranslated(-0.7, 0, 0);
		glRotated(70, 1, 0, 0);
		glutSolidTorus(0.01, 0.15, 20, 20);
		glPopMatrix();

	}

	glutSwapBuffers();
	glutPostRedisplay();

	prevFrame = currentFrame.clone();
	if (!prevFrame.empty())
		cvtColor(prevFrame, prevFrameGray, CV_BGR2GRAY);
}

void reset(){
	power = 0;
	direction = 0;
	powerX = 0;
	powerY = 0;
	tempDirection = 0;
	tempPower = 0;
	hitBoard = 1;
	heightOfBall = 0;
	powerChoosen = false;
	directionChoosen = false;
	choosingDirection = false;
	choosingPower = false;
	ballLanded = false;
	ballThrown = false;
	hit=false;
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
	case 'r':
		reset();
		break;
	default:
		break;
	}
}

void highScore(){

    if((5 - (clock() - showHighScoreListStart) / (double)CLOCKS_PER_SEC)<0){
        gameStarted=false;
        showHighScoreList=false;
    }
    cout<<"highscore";


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
	resize(difference, difference, Size(640, 480));
	threshold(difference, difference, 50, 255, 0);
	cv::flip(difference, difference, 1);
	//imshow("Difference", difference);
}

void startGameButton(){
	rectangle(drawingFrame, Point(10, 10), Point(120, 60), Scalar(0, 0, 255), CV_FILLED);
	putText(drawingFrame, "Start game", Point(14, 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1, 8, false);

	if (countNonZero(difference(Rect(Point(10, 10), Point(60, 30))))>500){
		cout << "Game started";
		start = clock();
		gameStarted = true;
	}
}

void choosePower(){

	rectangle(drawingFrame, Point(580, 10), Point(630, 160), Scalar(0, 0, 0), CV_FILLED);
	rectangle(drawingFrame, Point(580, 160 - (abs(sin(tempPower)) * 45 * 10 / 3)), Point(630, 160), Scalar(0, 0, 255), CV_FILLED);

	if (countNonZero(difference(Rect(Point(480, 220), Point(540, 260))))>500){
        choosingPower=true;
        tempPower+=step;
        //cout << abs(sin(tempPower))*45 << "\n";
    }
    else if(choosingPower==true){
            powerChoosen=true;
            power = abs(sin(tempPower)) * 45;
            cout<<"final power is: " << power;
    }
}

void chooseDirection(){

	line(drawingFrame, Point(510, 240), Point(510 - cos(abs(sin(tempDirection))) * 110, 240 - sin(abs(sin(tempDirection))) * 110), Scalar(0, 0, 255), 5);

	if (countNonZero(difference(Rect(Point(480, 220), Point(540, 260))))>500){
		choosingDirection = true;
		tempDirection+=step;
		//cout << abs(sin(tempDirection))*45 << "\n";
	}
	else if (choosingDirection == true){
		directionChoosen = true;
		direction = abs(sin(tempDirection)) * 45;
		cout << "final direction is: " << direction << endl;
	}
}

void calculateHeight(){

    heightOfBall=heightOfBall+powerY;
    powerY=powerY-1;
    //If ball hits the floor
    if(heightOfBall<-800){
		powerY = -powerY;
    }
    //cout<<"\n powerY: " <<powerY;
}

void idle(){

	if (!(gameStarted && powerChoosen && directionChoosen && ballThrown && !ballLanded)){
		cap >> currentFrame;
		cvtColor(currentFrame, currentFrameGray, CV_BGR2GRAY);
	}
	drawingFrame = currentFrame.clone();
	cv::flip(drawingFrame, drawingFrame, 1);


	if (!background.empty()){
		//calcOpticalFlow();
		subtractImages();
		if (!gameStarted){
			startGameButton();
		}

		else if(showHighScoreList){
            highScore();
		}

		else if(gameStarted){
			rectangle(drawingFrame, Point(10, 10), Point(120, 60), Scalar(0, 0, 255), CV_FILLED);
			ostringstream strs;
			strs << (60 - (clock() - start) / (double)CLOCKS_PER_SEC);
			putText(drawingFrame, strs.str(), Point(10, 50), FONT_HERSHEY_SCRIPT_SIMPLEX, 1, Scalar(0, 0, 0));

			if ((10 - (clock() - start) / (double)CLOCKS_PER_SEC <= 0)){
				//gameStarted = false;
				showHighScoreList=true;
				showHighScoreListStart=clock();
				reset();
				numberOfHits=0;
			}

			if (!powerChoosen){
				choosePower();
			}
			else if (!directionChoosen){
				chooseDirection();
			}
			else if (!ballThrown){
				powerX = power*cos((direction*M_PI) / 180);
				powerY = power*sin((direction*M_PI) / 180);
				ballThrown = true;
			}
			else if (!ballLanded){
				powerX = powerX + power*cos((direction*M_PI) / 180) * hitBoard;
				//If ball is out of screen
				if (powerX > 1.7 * 800){
					ballLanded = true;
				}
				//if ball is out of screen
				if (powerX < -0.5 * 800){
					ballLanded = true;
				}

                //If ball hit board
				if (powerX > 1.45 * 800 && powerX < 1.50 * 800){
					if (heightOfBall > -200 && heightOfBall < 400){
                            if(powerY<0){
                                hitBoard = -1;
                            }
					}
				}

				//if ball hits the loop
				if(powerX<1.35*800 && powerX>1.15*800){
                    if(heightOfBall>-20 && heightOfBall<20){
                        if(powerY>0 && hit==false){
                            //hit is actually not true, but it is not possible to increase numberOfHits in this shot now!
                            hit=true;
                        }
                        if(powerY<0 && hit==false){
                            hit=true;
                            numberOfHits++;
                            cout<<"number of hits: " << numberOfHits << endl;
                        }
                    }
				}

				//if ball hits the floor
				calculateHeight();
			}
			else if (ballLanded){
				reset();
			}

		}
	}

	cv::flip(drawingFrame, drawingFrame, 0);
}

int main(int argc, char** argv)
{
	cap = VideoCapture(0);

	flip(floorImg, floorImg, -1);
	// initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);
	glutCreateWindow("B0sket Ball Game");
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
