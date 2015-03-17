#define _USE_MATH_DEFINES
#include "includes.h"

//namespaces
using namespace cv;
using namespace std;

VideoCapture cap;

//-----------FRAMES--------//

//Frame from camera
Mat currentFrame;
Mat currentFrameGray;
//Frames to do background subtraction
Mat difference;
Mat background;
Mat backgroundgray;
//The frame we draw
Mat drawingFrame;
//images from file
Mat michaelImg = imread("src/michael.jpg");
Mat floorImg = imread("src/floor.png");


// ------ Boolean values for the game structure ------ //
bool gameStarted = false;
bool powerChoosen = false;
bool directionChoosen = false;
bool choosingPower=false;
bool choosingDirection = false;
bool ballThrown = false;
bool ballLanded=false;
bool showScore=false;
bool showHighscore=false;
bool enterName = false;

// Variables for throwing the ball
double tempPower = 0;
double power = 0;
double tempDirection = 0;
double direction;
double step = M_PI/20;
double powerX=0;
double powerY=0;
int hitBoard = 1;
bool hit = false;
double heightOfBall=0;

//Score variables
Vector<string> highscore;
string nameEntered="";
int numberOfHits=0;


//clock variables
clock_t start;
clock_t showScoreStart;
clock_t highscoreClock;


void addLight(){

    //enable and adding light
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { 0.0, 0.0, -50.0, 0.0 };
	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	//Enable depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

}

//Function to draw some parts of the basketball hoop
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

    //Draw micheal jordan image to show players score
    if(showScore==true){
        glDrawPixels(michaelImg.size().width, michaelImg.size().height, GL_BGR_EXT, GL_UNSIGNED_BYTE, michaelImg.ptr());
    }

    //draw basketball, hoop and floor
	else if (gameStarted==true){

        //Draw floor
		glDrawPixels(floorImg.size().width, floorImg.size().height, GL_BGR_EXT, GL_UNSIGNED_BYTE, floorImg.ptr());

		glClear(GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
		glColorMaterial(GL_FRONT, GL_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);

		//draw basketball
		glColor3f(1, 0.45f, 0);
		glTranslated(0.6 - powerX / 800, 0 + heightOfBall / 1000, 0);
		glutSolidSphere(0.05, 20, 20);

        //draw board behind hoop
        glLoadIdentity();
        glColor3f(0, 0, 0);
        glTranslated(-0.90, 0, 0);
        glTranslated(0, 0.1, 0);
        glRotated(87,0,1,0);
        glRotated(-20,0,0,1);
        DrawParallelepiped(0.6,0.5,0.01,GL_POLYGON);

        //Draw cube between hoop and board
        glLoadIdentity();
		glColor3f(1, 0.45f, 0);
		glTranslated(-0.86, 0, 0);
        glRotated(-20,1,0,0);
		glutSolidCube(0.05);
		glTranslated(-0.075, 0, 0);
        glutSolidCube(0.05);

        //Draw stand to basketball hoop
		glLoadIdentity();
		glColor3f(1, 0.45f, 0);
		glTranslated(-0.97, -0.42, 0);
        glRotated(-20,1,0,0);
        DrawParallelepiped(0.05,1,0.05,GL_POLYGON);

        //Draw hoop
		glLoadIdentity();
		glColor3f(1, 0.45f, 0);
		glTranslated(-0.7, 0, 0);
		glRotated(70, 1, 0, 0);
		glutSolidTorus(0.01, 0.15, 20, 20);
		glPopMatrix();

	}

	glutSwapBuffers();
	glutPostRedisplay();

}

//function resets all values after game ends
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
	nameEntered="";
}

//Update the highscore list and write it to file
void writeToHighscore(){

	//Updates the highscore list
	vector<string> temp;

    int i=1;
    int value=0;
    bool newScoreWritten=false;
    while (temp.size()<highscore.size()+1 && temp.size()<20){
        if(i<highscore.size()){
            value = atoi(highscore[i].c_str());
        }
        else{
            value=0;
        }

        if(numberOfHits>=value && !newScoreWritten){
            temp.push_back(nameEntered);
			stringstream convert;
			convert<<numberOfHits;
			string val = convert.str();
			temp.push_back(val);
			newScoreWritten=true;
        }
        else{

            temp.push_back(highscore[i - 1]);
			temp.push_back(highscore[i]);
			i=i+2;
        }
    }

	highscore.clear();

	for (int i = 0; i < temp.size(); i++){
		highscore.push_back(temp[i]);
	}


    //Writes the new highscore list to file
	ofstream writer("src/highscore.txt");
	for (int i = 0; i < highscore.size(); i++){
		writer << highscore[i];
		if (i<highscore.size()-1){
            writer<<endl;
		}
	}
	writer.close();
	numberOfHits = 0;
}

void keyboard(unsigned char key, int x, int y){
    //Keyboard listener when player is entering his/hers name
	if (enterName){
		if (key == 13){
			enterName = false;
			writeToHighscore();
			nameEntered="";
		}
		nameEntered += key;
		cout << nameEntered << endl;
	}
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


void score(){

    if((3 - (clock() - showScoreStart) / (double)CLOCKS_PER_SEC)<0){
        gameStarted=false;
        showScore=false;
		int value = atoi(highscore[highscore.size() - 1].c_str());
		if (numberOfHits >= value || highscore.size()<10 ){
			enterName = true;
			reset();
		}
    }

    char str[200];
    sprintf(str,"You got %i hits!",numberOfHits);

    putText(michaelImg, str, Point(180, 90), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 0), 1, 8, false);

}

void subtractImages(){
	resize(currentFrameGray, currentFrameGray, Size(320, 240));
	absdiff(background, currentFrameGray, difference);
	resize(difference, difference, Size(640, 480));
	threshold(difference, difference, 50, 255, 0);
	cv::flip(difference, difference, 1);
	imshow("Difference", difference);
}

void startGameButton(){
	rectangle(drawingFrame, Point(10, 10), Point(120, 60), Scalar(0, 0, 255), CV_FILLED);
	putText(drawingFrame, "Start game", Point(14, 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1, 8, false);

	if (countNonZero(difference(Rect(Point(10, 10), Point(120, 60))))>500){
		cout << "Game started";
		start = clock();
		gameStarted = true;
	}
}

void showHighscoreButton(){
    rectangle(drawingFrame, Point(520,10), Point(630, 60), Scalar(0, 0, 255), CV_FILLED);
    putText(drawingFrame, "Highscore", Point(524, 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1, 8, false);

	if (countNonZero(difference(Rect(Point(520, 10), Point(630, 60))))>500){
		cout << "highscore";
		showHighscore=true;
		highscoreClock=clock();

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

void checkCollision(){
	if (powerX > 1.45 * 800 && powerX < 1.50 * 800){ //HITS THE BOARD
		if (heightOfBall > -200 && heightOfBall < 400){
			hitBoard = -1;
		}
	}

	if (powerX > 1.10 * 800 && powerX < 1.15 * 800){//HITS THE RIM
		if (heightOfBall >= -50 && heightOfBall <= 30){
			hitBoard = -1;
		}
	}
}

void showHighscoreFunction(){
	rectangle(drawingFrame, Point(0, 0), Point(640, 480), Scalar(0, 0, 0), CV_FILLED);
	rectangle(drawingFrame, Point(240, 10), Point(350, 60), Scalar(0, 0, 255), CV_FILLED);
	putText(drawingFrame, "back", Point(244, 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1, 8, false);

	if ((countNonZero(difference(Rect(Point(240, 10), Point(350, 60))))>500)){ //&& (2 - (clock() - highscoreClock) / (double)CLOCKS_PER_SEC)<0){
		showHighscore = false;
	}

	int temp = 100;

	for (int i = 0; i < highscore.size(); i += 2){
		ostringstream ost;
		ost << highscore[i] << " : " << highscore[i + 1];
		putText(drawingFrame, ost.str(), Point(200, temp), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1, 8, false);
		temp += 20;
	}
}

void enterNameFunction(){
	rectangle(drawingFrame, Point(0, 0), Point(640, 480), Scalar(255, 255, 255), CV_FILLED);
	putText(drawingFrame, "Enter your name: ", Point(200, 40), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 0), 1, 8, false);

	putText(drawingFrame, nameEntered, Point(200, 200), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1, 8, false);

}

void readHighscore(){
	ifstream reader("src/highscore.txt");
	highscore.clear();
	while (!reader.eof()){
		string name;
		string score;
		reader >> name >> score;
		highscore.push_back(name);
		highscore.push_back(score);
	}
	reader.close();
}

void idle(){


    if(showScore==true){
         michaelImg = imread("src/michael.jpg");
    }

	if (!(gameStarted && powerChoosen && directionChoosen && ballThrown && !ballLanded)){
		cap >> currentFrame;
		cvtColor(currentFrame, currentFrameGray, CV_BGR2GRAY);
	}
	drawingFrame = currentFrame.clone();
	cv::flip(drawingFrame, drawingFrame, 1);
	flip(michaelImg,michaelImg,1);




	if (!background.empty()){
		subtractImages();
        if(showHighscore){
			showHighscoreFunction();
		}

		else if (enterName){
			enterNameFunction();
		}

		else if (!gameStarted){
			startGameButton();
			showHighscoreButton();

		}

		else if(showScore){
            score();
		}


		else if(gameStarted){
			rectangle(drawingFrame, Point(10, 10), Point(120, 60), Scalar(0, 0, 255), CV_FILLED);
			ostringstream strs;
			strs << (60 - (clock() - start) / (double)CLOCKS_PER_SEC);
			putText(drawingFrame, strs.str(), Point(10, 50), FONT_HERSHEY_SCRIPT_SIMPLEX, 1, Scalar(0, 0, 0));

			if ((30 - (clock() - start) / (double)CLOCKS_PER_SEC <= 0)){
				//gameStarted = false;
				showScore=true;
				showScoreStart=clock();
				reset();

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


				calculateHeight();
				checkCollision();
			}
			else if (ballLanded){
				reset();
			}
		}
	}

	cv::flip(drawingFrame, drawingFrame, 0);
	flip(michaelImg,michaelImg,0);
}



int main(int argc, char** argv)
{
	cap = VideoCapture(0);

	readHighscore();

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
