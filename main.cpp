#include <cv.h>
#include <highgui.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <opencv2/video/background_segm.hpp>




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



void display()
{

    glDrawPixels(drawingFrame.size().width, drawingFrame.size().height, GL_BGR, GL_UNSIGNED_BYTE, drawingFrame.ptr() );

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
    imshow("flow",flow);

}


void idle()
{

    cap>>currentFrame;
    flip(currentFrame, currentFrame, -1);
    cvtColor(currentFrame, currentFrameGray, CV_BGR2GRAY);
    if(!prevFrame.empty()){
        calcOpticalFlow();
    }

    drawingFrame=currentFrame.clone();
    rectangle(drawingFrame, Point(10,10), Point(40,40), Scalar(0,0,255), CV_FILLED);

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

    // start GUI loop
    glutMainLoop();

    return 0;

}
