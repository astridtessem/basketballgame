#include "includes.h"


//namespaces
using namespace cv;
using namespace std;

Mat currentFrame;
Mat currentFrameGray;
Mat prevFrame;
Mat prevFrameGray;
Mat flow;
Mat drawingFrame;

VideoCapture cap;


bool gameStarted=false;
bool choosePower=false;
bool chooseDirection=false;

int powerCount=0;

void addLight(){

  glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);

    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { 0.0, 0.0, -50.0, 0.0 };
    glShadeModel (GL_SMOOTH);


    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

}

void display()
{

    glDrawPixels(drawingFrame.size().width, drawingFrame.size().height, GL_BGR_EXT, GL_UNSIGNED_BYTE, drawingFrame.ptr() );

    if(gameStarted){
        glPushMatrix();
        glColorMaterial(GL_FRONT, GL_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        glColor3f(1,0.45f,0);
        glTranslated(0.6,0,0);
        glutSolidSphere(0.1, 20, 20);

        glLoadIdentity();
        glColor3f(1,0.45f,0);
        glTranslated(-0.6,0,0);
        glRotated(70,1,0,0);
        glutSolidTorus(0.02,0.3,20, 20);
        glPopMatrix();
    }


    glutSwapBuffers();
    glutPostRedisplay();

    prevFrame = currentFrame.clone();
    cvtColor(prevFrame, prevFrameGray, CV_BGR2GRAY);

}


void keyboard( unsigned char key, int x, int y )
{
    switch ( key )
    {
        case 'q':
        exit(0);
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
    split(flow,xy);

    cartToPolar(xy[0], xy[1], magnitude, angle, true);
    resize(magnitude,flow,Size(640,480));
    threshold(flow,flow,10,255,0);
    flip(flow, flow, 1);
    imshow("flow",flow);

}

void startGameButton(){

    rectangle(drawingFrame, Point(10,10), Point(120,60), Scalar(0,0,255), CV_FILLED);
    putText(drawingFrame, "Start game" , Point(14,40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,255,0), 1, 8, false );

    if(countNonZero(flow(Rect(Point(10,10), Point(120,60))))>500){
        cout<<"Game started";
        gameStarted=true;
    }
}

void power(){

        if(countNonZero(flow(Rect(Point(500,230), Point(520,250))))>100){
            powerCount++;
            cout<<"poooower: " << powerCount << "\n";
        //putText(drawingFrame, char(powerCount) , Point(14,40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,255,0), 1, 8, false );

        //choosePower=true;
        }

}

void idle()
{

    cap>>currentFrame;

    cvtColor(currentFrame, currentFrameGray, CV_BGR2GRAY);

    drawingFrame=currentFrame.clone();

    flip(drawingFrame, drawingFrame, 1);

    if(!prevFrame.empty()){
        calcOpticalFlow();

        if(!gameStarted){
            startGameButton();
        }
        else if(!choosePower){

            power();
           rectangle(drawingFrame, Point(490,220), Point(530,260), Scalar(0,0,255), CV_FILLED);



        }

    }

    flip(drawingFrame, drawingFrame, 0);


}






int main(int argc, char** argv)
{

    cap = VideoCapture(0);
    // initialize GLUT
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
    glutInitWindowPosition( 20, 20 );
    glutInitWindowSize(640,480 );
    glutCreateWindow( "OpenGL / OpenCV Example" );

    // set up GUI callback functions
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutIdleFunc( idle );

    addLight();
    // start GUI loop
    glutMainLoop();

    return 0;

}
