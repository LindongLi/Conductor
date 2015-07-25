// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <algorithm>

// The only file that needs to be included to use the Myo C++ SDK is myo.hpp.
#include <myo/myo.hpp>

// Classes that inherit from myo::DeviceListener can be used to receive events from Myo devices. DeviceListener
// provides several virtual functions for handling different kinds of events. If you do not override an event, the
// default behavior is to do nothing.


#include "AudioPlayer.h"

#include <list>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include "particle.h"
using namespace std;
float gravity = 7.5f;
float flatQuadSize = 15;
float friction = 0.9f;
float cameraPosition[3] = {20, 20, 0};
list<particle> listOfParticles;


#include <thread>


const char* elements[7]  = {
    "/Users/Lindong/Desktop/sdk/samples/elements/do.mp3",
    "/Users/Lindong/Desktop/sdk/samples/elements/re.mp3",
    "/Users/Lindong/Desktop/sdk/samples/elements/mi.mp3",
    "/Users/Lindong/Desktop/sdk/samples/elements/fa.mp3",
    "/Users/Lindong/Desktop/sdk/samples/elements/sol.mp3",
    "/Users/Lindong/Desktop/sdk/samples/elements/la.mp3",
    "/Users/Lindong/Desktop/sdk/samples/elements/si.mp3"
};



int musicbook[63]={
    3,3,4,5,5,4,3,2,1,1,2,3,3,2,2,
    3,3,4,5,5,4,3,2,1,1,2,3,2,1,1,
    2,2,3,1,2,3,4,3,1,2,3,4,3,2,1,2,5,3,
    3,3,4,5,5,4,3,2,1,1,2,3,2,1,1
};

int musicCount = 0;

void bumpMethod(){
    for(list<particle>::iterator i = listOfParticles.begin(); i != listOfParticles.end(); ++i){
        if(i->position.y < 0.1f){
            i->direction.x = int(rand() % 41) - 20.f;
            i->direction.y = int(rand() % 50) + 50;
            i->direction.z = int(rand() % 41) - 20.f;
        }
    }
}

void playtask(const char *id)
{
    playASound(id);
}

class DataCollector : public myo::DeviceListener {
    
       float history_pitch;
    
public:
    DataCollector()
    : onArm(false), isUnlocked(false), roll_w(0), pitch_w(0), yaw_w(0), currentPose()
    {
    }
    
    // onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
    void onUnpair(myo::Myo* myo, uint64_t timestamp)
    {
        // We've lost a Myo.
        // Let's clean up some leftover state.
        roll_w = 0;
        pitch_w = 0;
        yaw_w = 0;
        onArm = false;
        isUnlocked = false;
    }
    
    int count = 0;
    
    
    void onAccelerometerData (myo::Myo* myo, uint64_t timestamp, const myo::Vector3< float > & accel){
        
        
        accelX = accel.x();
        accelY = accel.y();
        accelZ = accel.z();
        
        count+=1;
    }
    
    
    
    void onGyroscopeData	(myo::Myo * myo, uint64_t timestamp, const myo::Vector3< float > & gyro){
    
        
        gyroX = gyro.x();
        gyroY = gyro.y();
        gyroZ = gyro.z();
    }
    
    
    // onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
    // as a unit quaternion.
    void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
    {
        using std::atan2;
        using std::asin;
        using std::sqrt;
        using std::max;
        using std::min;
        
        
        
        // Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
        float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
                           1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
        float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
        float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
                          1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));
        
        // Convert the floating point angles in radians to a scale from 0 to 18.
        roll_w = static_cast<int>((roll + (float)M_PI)/(M_PI * 2.0f) * 10000.f);
        pitch_w = static_cast<int>((pitch + (float)M_PI/2.0f)/M_PI * 10000.f);
        yaw_w = static_cast<int>((yaw + (float)M_PI)/(M_PI * 2.0f) * 10000.f);
    }
    
    // onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
    // making a fist, or not making a fist anymore.
    void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
    {
        currentPose = pose;
        
        if (pose != myo::Pose::unknown && pose != myo::Pose::rest) {
            // Tell the Myo to stay unlocked until told otherwise. We do that here so you can hold the poses without the
            // Myo becoming locked.
            myo->unlock(myo::Myo::unlockHold);
            
            // Notify the Myo that the pose has resulted in an action, in this case changing
            // the text on the screen. The Myo will vibrate.
            myo->notifyUserAction();
        } else {
            // Tell the Myo to stay unlocked only for a short period. This allows the Myo to stay unlocked while poses
            // are being performed, but lock after inactivity.
            myo->unlock(myo::Myo::unlockTimed);
        }
    }
    
    // onArmSync() is called whenever Myo has recognized a Sync Gesture after someone has put it on their
    // arm. This lets Myo know which arm it's on and which way it's facing.
    void onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection, float rotation,
                   myo::WarmupState warmupState)
    {
        onArm = true;
        whichArm = arm;
    }
    
    // onArmUnsync() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
    // it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
    // when Myo is moved around on the arm.
    void onArmUnsync(myo::Myo* myo, uint64_t timestamp)
    {
        onArm = false;
    }
    
    // onUnlock() is called whenever Myo has become unlocked, and will start delivering pose events.
    void onUnlock(myo::Myo* myo, uint64_t timestamp)
    {
        isUnlocked = true;
    }
    
    // onLock() is called whenever Myo has become locked. No pose events will be sent until the Myo is unlocked again.
    void onLock(myo::Myo* myo, uint64_t timestamp)
    {
        isUnlocked = false;
    }
    
    // There are other virtual functions in DeviceListener that we could override here, like onAccelerometerData().
    // For this example, the functions overridden above are sufficient.
    
    // We define this function to print the current values that were updated by the on...() functions above.
    void print()
    {
        
        // Print out the orientation. Orientation data is always available, even if no arm is currently recognized.
        
        //std::cout<< accelX <<" "<<accelY<<" "<<accelZ<<"\n";
        
        
        //std::cout << accelX << " "<< fabs(accelX);
        
        
        float sum = fabs(accelZ) + fabs(accelY) + fabs(accelX);
        
        
        
        std::cout << gyroZ<<"\n";
        
        
        
       
        
        if(onArm && (gyroZ > 250)){
            
            

            
            std::cout << sum << "\n";
            
            thread t1(playtask, elements[musicbook[musicCount] - 1]);
            t1.detach();
            
            musicCount = (musicCount + 1) % 63;
            
            bumpMethod();
            
        }
        
        
        
//        std::cout << accelX << "\n";
//        
//        
//        if (onArm && accelX < -1)  {
//            
//            history_pitch = 0.98f * history_pitch + 0.02f * pitch_w;
//            
//            if(pitch_w > (history_pitch + 50))
//            {
//                
//                thread t1(playtask, elements[musicbook[musicCount] - 1]);
//                t1.detach();
//                
//                musicCount = (musicCount + 1) % 63;
//        
//                bumpMethod();
//            }
//        }
        
        /*
         if(accelZ < -2){
         std::cout <<"Z"<<accelZ<<"\n";
         playASound(elements[0]);
         
         }
         
         if(accelY < -1){
         std::cout <<"Y"<<accelY<<"\n";
         playASound(elements[1]);
         
         }
         
         */
        
        
        
        std::cout << std::flush;
    }
    
    // These values are set by onArmSync() and onArmUnsync() above.
    bool onArm;
    myo::Arm whichArm;
    
    // This is set by onUnlocked() and onLocked() above.
    bool isUnlocked;
    
    // These values are set by onOrientationData() and onPose() above.
    int roll_w, pitch_w, yaw_w;
    float quatY;
    float accelX,accelY,accelZ;
    float gyroX,gyroY,gyroZ;
    float vZ = 0;
    myo::Pose currentPose;
};





void fillListOfParticles(int num){
    for(int i = 0; i<num; ++i){
        particle np(*new vec3(int(rand() % 31) -15.f, int(rand() % 31) -15.f, int(rand() % 31) -15.f),
                    *new vec3(int(rand() % 41) - 20.f, int(rand() % 41) - 20.f, int(rand() % 41) - 20.f),
                    int(rand() % 100 + 20) / 5000.f, 0.5f, 0.f);
        listOfParticles.push_back(np);
    }
}

/*
 renerParticle - renders a given pointer of a particle as a sphere
 */
void renderParticle(particle &p){
    glPushMatrix();
    vec3 Position;
    p.getPosition(Position);
    int RGBColor[3];
    p.getColor(RGBColor);
    float pSize = p.size;
    glTranslatef(Position.x, Position.y, Position.z);
    glColor3b(RGBColor[0], RGBColor[1], RGBColor[2]);
    glutSolidSphere(pSize, 16, 16);
    glPopMatrix();
}

/*
 renderListofParticles - iterates through the list of particles and called the renderParticle method on each of them.
 if the particle trail mode is on it calls the renderParticle method on each of trailing particle of each particle aswell.
 */
void renderListOfParticles(){
    for(list<particle>::iterator i = listOfParticles.begin(); i != listOfParticles.end(); ++i){
        renderParticle(*i);
    }
}



myo::Hub hub("com.example.hello-myo");
DataCollector collector;
/*
 updateMethod  - called every 16 milliseconds,  meant to update the animation
 and add more particles to the list of particles
 */
void updateMethod(int value){
    //iterate through list of particles and call the move function of the particle has no expired else remove it from the list
    for(list<particle>::iterator i = listOfParticles.begin(); i != listOfParticles.end(); ++i){
        i->move(gravity, flatQuadSize, friction);
    }
    glutTimerFunc(16, updateMethod, 0); //call updateMethod after 16 seconds
    glutPostRedisplay(); //call display function
    
    hub.run(1000/20);
    // After processing events, we call the print() member function we defined above to print out the values we've
    // obtained from any events that have occurred.
    collector.print();
}


/*
 drawFlatQuad - meant to be called from the display method, draws the flat plane using GL_QUADS
 */
void drawFlatQuad(){
    glColor3f(0, 3, 0); //set the color
    glBegin(GL_QUADS); //GL_QUADS begin, draw a flat square plane of flatQuadSize size.
    glVertex3f(-1*flatQuadSize, 0, flatQuadSize);
    glVertex3f(flatQuadSize, 0, flatQuadSize);
    glVertex3f(flatQuadSize, 0, -1*flatQuadSize);
    glVertex3f(-1*flatQuadSize, 0, -1*flatQuadSize);
    glEnd(); //gl end
}


/*
 special - meant to be a call back function to handle the cases of arrows being press to controll the camera
 */
void special(int key, int x, int y){
    /* arrow key presses move the camera */
    switch(key){
        case GLUT_KEY_LEFT:
            cameraPosition[2]-= 1;
            break;
            
        case GLUT_KEY_RIGHT:
            cameraPosition[2]+= 1;
            break;
            
        case GLUT_KEY_UP:
            cameraPosition[1] -= 1;
            break;
            
        case GLUT_KEY_DOWN:
            bumpMethod();
            //cameraPosition[1] += 1;
            break;
    }
    glutPostRedisplay(); //call the display function
}

/*
 display - where all the rendering is performed, frame buffers are swapped
 */
void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); //change matrix mode to model view
    glLoadIdentity(); //load identity matrix
    
    gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2],
              0, 0, 0,
              0, 1, 0); // look at the origin from cameraPosition
    
    //lighting settings
    float m_amb[] = {0.33, 0.33, 0.33, 1.0};
    float m_dif[] = {0.78, 0.57, 0.11, 1.0};
    float m_spec[] = {0.99, 0.91, 0.81, 1.0};
    float shiny = 27;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    
    drawFlatQuad(); //draw the flat plane
    renderListOfParticles(); // draw all the particles in the list of particles
    
    glutSwapBuffers(); //swap the frame buffers
}
/*
 init - procedure sets up the lighting and the projection
 */
void init(){
    glEnable(GL_COLOR_MATERIAL); //enable color material
    glEnable(GL_LIGHTING); //enable lighting
    glEnable(GL_LIGHT0); //enable light
    
    float position[4] = {1.5, 0, 0, 0};
    
    float amb[4] = {1.0, 1, 1, 1};
    float diff[4] = {0,0,0, 1};
    float spec[4] = {0,0,1, 1};
    
    glLightfv(GL_LIGHT1, GL_POSITION, position);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diff);
    glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT1, GL_SPECULAR, spec);
    
    glMatrixMode(GL_PROJECTION); //change matrix mode to gl projection
    glLoadIdentity(); //load an identity matrix
    gluPerspective(80, 1, 1, 100); //set up a perspective projection
}












int main(int argc, char** argv)
{
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    // We catch any exceptions that might occur below -- see the catch statement for more details.
    try {
        
        
        
        srand(time(NULL));
        
        fillListOfParticles(1500);
        
        glutInit(&argc, argv); //starts up GLUT
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //initiate glut display mode
        glutInitWindowSize(800, 800); //set the window size, 800 by 800
        glutInitWindowPosition(100, 100); //position the window at 100, 100
        glutCreateWindow("Particle System"); //creates the window with the given title
        glutDisplayFunc(display); //registers "display" as the display callback function
        glutSpecialFunc(special); //registers "special" as the special callback function
        
        glClearColor(1, 1, 1, 1.0f); //set the clear color to white
        
        glEnable(GL_DEPTH_TEST); //enable gl depth test
        
        init(); //call the init procedure
        
        glutTimerFunc(16, updateMethod, 0);
        
        
        
        
        
        
        
        
        
        
        
        
        // First, we create a Hub with our application identifier. Be sure not to use the com.example namespace when
        // publishing your application. The Hub provides access to one or more Myos.
        
        
        std::cout << "Attempting to find a Myo..." << std::endl;
        
        // Next, we attempt to find a Myo to use. If a Myo is already paired in Myo Connect, this will return that Myo
        // immediately.
        // waitForMyo() takes a timeout value in milliseconds. In this case we will try to find a Myo for 10 seconds, and
        // if that fails, the function will return a null pointer.
        myo::Myo* myo = hub.waitForMyo(10000);
        
        // If waitForMyo() returned a null pointer, we failed to find a Myo, so exit with an error message.
        if (!myo) {
            throw std::runtime_error("Unable to find a Myo!");
        }
        
        // We've found a Myo.
        std::cout << "Connected to a Myo armband!" << std::endl << std::endl;
        
        // Next we construct an instance of our DeviceListener, so that we can register it with the Hub.
        
        
        // Hub::addListener() takes the address of any object whose class inherits from DeviceListener, and will cause
        // Hub::run() to send events to all registered device listeners.
        hub.addListener(&collector);
        
        
        glutMainLoop();	//starts the main loop
        // Finally we enter our main loop.

        
        // If a standard exception occurred, we print out its message and exit.
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Press enter to continue.";
        std::cin.ignore();
        return 1;
    }
}
