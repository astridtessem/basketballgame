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

Mat image;
Mat imageUndistorted;
Mat gray_image;
Mat resizedImage;

VideoCapture cap;

//Variables to count frames
int maximum;
int numberOfFrames;

//Variables for motion detection
Mat threshImage;
Mat threshImageResized;
Mat prev;
Mat background;
Mat foreground;
Mat flow;

int angle=0;

//What figure and what type of scene shows
int showFigure=1;
int type = 1;
bool showRectangle=1;

//Variables for face detection
std::vector<Rect> faces;
CascadeClassifier face_cascade;

//Camera matrix with intrinsic variables
Mat intrinsic = Mat::zeros(3, 3, CV_64F);
//distortion variables
Mat distCoeffs;

void addLight(){



    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { 0.0, 0.0, -50.0, 0.0 };
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_SMOOTH);


    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);


    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

}

void computePixelDifference(){

    Mat backgroundGrey;
    Mat resizedImageGrey;

    cvtColor(background, backgroundGrey, CV_BGR2GRAY);
    cvtColor(resizedImage, resizedImageGrey, CV_BGR2GRAY);
    absdiff(backgroundGrey,resizedImageGrey,foreground);
    resize(foreground,foreground,Size(640,480));
    threshold(foreground,threshImage, 10, 255, 0);


}

void findDepth(){

    //is not in use, and is probably wrong....
    int depth;
    int realHeadSize=250;
    int imageHeadSize = faces[0].height;
    depth = (realHeadSize*intrinsic.at<double>(0,0))/(imageHeadSize*(1-intrinsic.at<double>(0,2)));

}

void display()
{
    if(angle<360){
        angle=angle+3;
    }
    else{
        angle=1;
    }
    // clear the window
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    flip(imageUndistorted, imageUndistorted, -1);
    resize(imageUndistorted,resizedImage,Size(320,240));
    cvtColor(resizedImage, gray_image, CV_BGR2GRAY);

    bool leftBoxClicked=0;
    bool rightBoxClicked=0;

    //find faces
    face_cascade.detectMultiScale( gray_image, faces, 1.1, 2, CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT);


    // shows the normal image
    if (type==1){

        //renders rectangles where the faces are found
        if(showRectangle){
            for( int i = 0; i < faces.size(); i++ )
            {
                rectangle(imageUndistorted, Point(faces[i].x*2 , faces[i].y*2-50), Point(faces[i].x*2+faces[i].width*2 , faces[i].y*2+faces[i].height*2), Scalar(0,0,255));
            }
        }

        //Making two "click box"
        rectangle(imageUndistorted, Point(25,405), Point(75,455), Scalar(0,0,255),CV_FILLED);
        rectangle(imageUndistorted, Point(565,405), Point(615,455), Scalar(0,255,0),CV_FILLED);

        glDrawPixels( imageUndistorted.size().width, imageUndistorted.size().height, GL_BGR, GL_UNSIGNED_BYTE, imageUndistorted.ptr() );
    }

    //image shows motion with background and foreground difference
    if (type==2){

        //Computing foreground background difference
        computePixelDifference();

        //checks if box is clicked
        if(countNonZero(threshImage(Rect(Point(25,405), Point(75,455))))>300){
            leftBoxClicked=1;
        }
        if(countNonZero(threshImage(Rect(Point(565,405), Point(615,455))))>300){
            rightBoxClicked=1;
        }

        //convert to bgr to be able to use gldrawpixels
        cvtColor(threshImage, threshImage, CV_GRAY2BGR);

        //if box are clicked, make it yellow, and change figure.
        if (leftBoxClicked){
            rectangle(threshImage, Point(25,405), Point(75,455), Scalar(0,255,255),CV_FILLED);
            if(showFigure<3){
                showFigure++;
            }
            else{
                showFigure=1;
                }
            }
            else{
                rectangle(threshImage, Point(25,405), Point(75,455), Scalar(0,0,255),CV_FILLED);
            }
            if (rightBoxClicked){
                rectangle(threshImage, Point(565,405), Point(615,455), Scalar(0,255,255),CV_FILLED);
                if(showFigure>1){
                    showFigure--;
                }
                else{
                    showFigure=3;
                }
            }
            else{
                rectangle(threshImage, Point(565,405), Point(615,455), Scalar(0,255,0),CV_FILLED);
            }

            glDrawPixels( threshImage.size().width, threshImage.size().height, GL_BGR, GL_UNSIGNED_BYTE, threshImage.ptr() );
        }

    //image that shows optical flow
    if (type==3){

        //Calculates optical flow between previous frame and current frame
        calcOpticalFlowFarneback(prev, gray_image, flow, 0.5, 1, 3, 1, 5, 1.1, 0);
        Mat xy[2];
        Mat magnitude;
        Mat angle;
        split(flow,xy);

        cartToPolar(xy[0], xy[1], magnitude, angle, true);
        resize(magnitude,flow,Size(640,480));
        threshold(flow,flow,10,255,0);

        //checks if box is clicked
        if(countNonZero(flow(Rect(Point(25,405), Point(75,455))))>300){
            leftBoxClicked=1;
        }
        if(countNonZero(flow(Rect(Point(565,405), Point(615,455))))>300){
            rightBoxClicked=1;
        }

        //convert to bgr to be able to use gldrawpixels
        cvtColor(flow, flow, CV_GRAY2BGR);

        //if box are clicked, make it yellow, and change figure.
        if (leftBoxClicked){
            rectangle(flow, Point(25,405), Point(75,455), Scalar(0,255,255),CV_FILLED);
            if(showFigure<3){
                showFigure++;
            }
            else{
                showFigure=1;
            }
        }
        else{
            rectangle(flow, Point(25,405), Point(75,455), Scalar(0,0,255),CV_FILLED);
        }
        if (rightBoxClicked){
            rectangle(flow, Point(565,405), Point(615,455), Scalar(0,255,255),CV_FILLED);
            if(showFigure>1){
                showFigure--;
            }
            else{
                showFigure=3;
            }
        }
        else{
            rectangle(flow, Point(565,405), Point(615,455), Scalar(0,255,0),CV_FILLED);
        }

        glDrawPixels( imageUndistorted.size().width, imageUndistorted.size().height, GL_BGR, GL_FLOAT, flow.ptr() );

    }

    glTranslated(1,0,1);
    glRotated(angle, 0, 1, 0);
    glTranslated(-1,0,0);

    //What figure to show
    if(showFigure==1){
        glutSolidTeapot(0.2);
    }
    if(showFigure==2){

        glutSolidSphere(0.2,5,5);
    }
    if(showFigure==3){
        glRotated(angle, 0, 1, 0);
        glutSolidCone(0.2,0.4,4,4);
    }

    //save the frame to use it as previous frame in next iteration
    prev=gray_image.clone();

    glViewport(0,0,imageUndistorted.size().width, imageUndistorted.size().height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(imageUndistorted.size().height,imageUndistorted.size().width*1.0/imageUndistorted.size().height,1,300);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,0,-1,0,0,0,0,1,0);

    waitKey(45);

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

        case 'r':
        if (showRectangle==1){
         showRectangle=0;
     }
     else{
        showRectangle=1;
    }
    break;
    case 'b':

    type==2;
    background=resizedImage.clone();

    break;
    case 'g':

        if(type==1){
            type=2;
            background=resizedImage.clone();
        }
        else{
            type=1;
        }
        break;

    case 'f':
        if(type==1){
            type=3;
        }
        else{
            type=1;
        }

        break;


    default:
    break;


}
}


void idle()
{
    //Reading video frames. When near end of video, start over.
    if (numberOfFrames<maximum-12){

        cap >> image;
        undistort(image, imageUndistorted, intrinsic, distCoeffs);
        numberOfFrames++;

    }
    else{

        cap.open("C:/Users/Astrid/Downloads/ARChatTest1.mov");
        numberOfFrames=0;
    }
}





void readCalibrationParameters(){

    string line;

    double fcx;
    double fcy;
    double ccx;
    double ccy;
    double k1;
    double k2;
    double p1;
    double p2;

    ifstream reader("C:/Users/Astrid/Documents/calibrationvalues.txt");

    reader >> fcx >> fcy >> ccx >> ccy >> k1 >> k2 >> p1 >> p2;


    //Make dist and intrinsic arrays
    distCoeffs.push_back(k1);
    distCoeffs.push_back(k2);
    distCoeffs.push_back(p1);
    distCoeffs.push_back(p2);


    intrinsic.at<double>(0,0)=fcx;
    intrinsic.at<double>(1,1)=fcy;
    intrinsic.at<double>(0,2)=ccx;
    intrinsic.at<double>(1,2)=ccy;
    intrinsic.at<double>(2,2)=1;

}



int main(int argc, char** argv)
{

    //Open video file
    cap.open("C:/Users/Astrid/Downloads/ARChatTest1.mov");
    maximum = cap.get(CV_CAP_PROP_FRAME_COUNT);
    numberOfFrames=0;

    //Read calibration parameters from file
    readCalibrationParameters();

    //Face recognitioner
    face_cascade.load( "C:/OpenCV/sources/data/Haarcascades/haarcascade_frontalface_default.xml" );


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
