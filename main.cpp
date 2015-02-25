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



void display()
{

    glutSwapBuffers();
    glutPostRedisplay();


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


void idle()
{

}




int main(int argc, char** argv)
{


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
