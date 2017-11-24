/*
 * Simple glut demo that can be used as a template for
 * other projects by Garrett Aldrich
 */

/*
 * Manual (for command lines):
 *  To translate:
 *    Use:
 *      tra <id> <dx> <dy> <dz>
 *    Where:
 *      <id> is object id starting from 1
 *      <dx>, <dy>, <dz> are x, y, z values of translation vector
 *  To scale:
 *    Use:
 *      sca <id> <fac>
 *    Where:
 *      <id> is object id starting from 1
 *      <fac> is the scaling factor for all 3 dimentions
 *  To rotate:
 *    Use:
 *      rot <id> <x0> <y0> <z0> <x1> <y1> <z1> <angle>
 *    Where:
 *      <id> is object id starting from 1
 *      <x0>, <y0>, <z0> are coordinate of first point on rotating axis
 *      <x1>, <y1>, <z1> are coordinate of second point on rotating axis
 *      <angle> rotating angle in degree
 *  To exit:
 *    Use:
 *      exit
 * 
 * Known problems: In rare cases, only parts of the window will render. 
 * Exit and run the program again seem to fix the problem.
 */

#ifdef WIN32
#include <windows.h>
#endif

#if defined (__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#else //linux
#include <GL/gl.h>
#include <GL/glut.h>
#endif

//other includes
#include <stdio.h>
#include <stdlib.h>

//////////////////////
                    //
#include <vector>   ////
#include <fstream>    //
#include <iostream>   //
#include <cmath>      // 
#include <string>   ////
                    //
//////////////////////


/****set in main()****/
//the number of pixels in the grid
int grid_width;
int grid_height;

//the size of pixels sets the inital window height and width
//don't make the pixels too large or the screen size will be larger than
//your display size
float pixel_size;

/*Window information*/
int win_height;
int win_width;


void init();
void idle();
void display();
void draw_lines();
void reshape(int width, int height);
void key(unsigned char ch, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void check();

int commandEnable = 0;
//////////////////////////////////////////
                                        //
static const float PI = 3.14159265359;  //
                                        //
//////////////////////////////////////////
//
//////////////////////////////////////////////
                                            //
//assume matrix dimentions are compatible   //
class Matrix {                              //
private:                                  ////
    std::vector<std::vector<float>> _m;   //
public:                                   //////////
    Matrix() {}                                   //
    Matrix(const Matrix &m) { this->_m = m._m; }  //
    Matrix(float m[], int r, int c) {             //
        for(int i = 0; i < r; i++) {    //////////////////////////////////
            std::vector<float> v;                                       //
            for (int j = 0; j < c; j++) { v.push_back(m[i * c + j]); }  //
            _m.push_back(v);                                            //
        }                     ////////////////////////////////////////////
    }                         //
    Matrix mult(Matrix m) {   //////////////////
        Matrix result;                        //
        for(int i = 0; i < _m.size(); i++) {  ////////////
            std::vector<float> vec;                     //
            for(int j = 0; j < m._m[i].size(); j++) {   ////
                float value = 0;                          //
                for(int k = 0; k < _m[i].size(); k++) {   //
                    value += (_m[i][k] * m._m[k][j]);     //
                }                                         //
                vec.push_back(value);   ////////////////////
            }                           //
            result._m.push_back(vec);   //
        }                               //
        return result;  //////////////////
    }                         //
    float at(int r, int c) {  //
        return _m[r][c];      //
    }                         //
    //for debugging   //////////////////////////
    void print() {                            //////////
        for(int i = 0; i < _m.size(); i++) {          //
            for(int j = 0; j < _m[i].size(); j++) {   //
                std::cout << _m[i][j] << " ";         //
            }                                   ////////
            std::cout << std::endl;   ////////////
        }                             //
        std::cout << std::endl;   //////
    }                             //
};      ////////////////////////////
    //////
//////
//
////////////////
              //
class Point   //
{             ////////////
public:                 ////////////////////////////////////////////
    float _x, _y, _z;                                             //
    Point(float x, float y, float z) { _x = x, _y = y, _z = z; }  //
    Point(const Point &p) { _x = p._x, _y = p._y, _z = p._z; }    //
};                                                              ////
    //////////////////////////////////////////////////////////////
//////
//
////////////////
              //
class Shape   //
{             ////////
public:             ////////////////
    int _material;                //////////////////
    std::vector<Point> _points;                   //
    std::vector<std::vector<int>> _connections;   //
    std::vector<std::vector<int>> _triangles;     //
public:                                           //////////////
    Shape() {}                                                //
    void addPoint(const Point &p) { _points.push_back(p); }   //
    void addPoint(float x, float y, float z) {                //
        _points.push_back(*(new Point(x, y, z)));   //////////////////////////
    }                                                                       //
    void setConnection(int size) { _connections.resize(_points.size()); }   //
    void addConnection(int from, int to) {                                  //
        _connections[from].push_back(to);   //////////////////////////////////
    }                                         //
    void addTriangle(int a, int b, int c) {   //
        std::vector<int> v;                   //
        v.push_back(a);           //////////////
        v.push_back(b);   //////////
        v.push_back(c);           //
        _triangles.push_back(v);  //
    }                             //////////////
private:                                      //////////////
    void drawXY(float x, float y, float z) {              //
        glVertex3f(x*0.4+0.05, y*0.4+0.55, z*0.4+0.05);   //
    }                                                     //
    void drawYZ(float x, float y, float z) {              //
        glVertex3f(y*0.4+0.05, z*0.4+0.05, x*0.4+0.05);   //
    }                                                     ////
    void drawXZ(float x, float y, float z) {                //
        glVertex3f(x*0.4+0.55, z*0.4+0.55, -(y*0.4+0.05));  //
    }                                                       //
public:             //////////////////////////////////////////
    void draw() {                   //
        //enable clipping           //////////////////////
        glEnable(GL_SCISSOR_TEST);                      //////////  
        for(int i = 0; i < _connections.size(); i++) {          //
            for(int j = 0; j < _connections[i].size(); j++) {   //
                //set color of the shapes                       //
                //glColor3f(0.5, 0.0, 0.9);       ////////////////////////////
                //draw to XY-plane, upper left                              //
                glScissor(0, grid_height/2, grid_width/2, grid_height/2);   //
                glBegin(GL_LINES);                                          //
                drawXY(_points[i]._x, _points[i]._y, _points[i]._z);  ////////
                drawXY(_points[_connections[i][j]]._x,                //
                       _points[_connections[i][j]]._y,    //////////////
                       _points[_connections[i][j]]._z);   //
                glEnd();                                  ////////
                //draw to YZ-plane, lower left                  //
                glScissor(0, 0, grid_width/2, grid_height/2);   ////////
                glBegin(GL_LINES);                                    //
                drawYZ(_points[i]._x, _points[i]._y, _points[i]._z);  //
                drawYZ(_points[_connections[i][j]]._x,                //
                       _points[_connections[i][j]]._y,    //////////////
                       _points[_connections[i][j]]._z);   //
                glEnd();                                  //
                //draw to XZ-plane, upper right         ////
                glScissor(grid_width/2, grid_height/2,    //
                          grid_width/2, grid_height/2);   //////////////
                glBegin(GL_LINES);                                    //
                drawXZ(_points[i]._x, _points[i]._y, _points[i]._z);  //
                drawXZ(_points[_connections[i][j]]._x,                //
                       _points[_connections[i][j]]._y,    //////////////
                       _points[_connections[i][j]]._z);   //
                glEnd();                                  //
            }             //////////////////////////////////
        }                             //
        glDisable(GL_SCISSOR_TEST);   //
    }                                 //////////////////////////////////////////
private:                                                                      //
    //find a rectangular box that contain the shape, then take the center of  //
    //that box to be the center of the shapes                                 //
    Point findCenter() {                        ////////////////////////////////
        float x0, y0, z0, x1, y1, z1;   //////////
        x0 = x1 = _points[0]._x;        //
        y0 = y1 = _points[0]._y;  //////////////////////////////////    
        z0 = z1 = _points[0]._z;                                  //
        for(auto p = _points.begin(); p != _points.end(); p++) {  //
            if(p->_x < x0) { x0 = p->_x; }                        //
            if(p->_y < y0) { y0 = p->_y; }  ////////////////////////
            if(p->_z < z0) { z0 = p->_z; }  //
            if(p->_x > x1) { x1 = p->_x; }  //
            if(p->_y > y1) { y1 = p->_y; }  //
            if(p->_z > z1) { z1 = p->_z; }  //////////////
        }                                               //
        Point point((x0+x1)/2, (y0+y1)/2, (z0+z1)/2);   //
        return point;                                   //
    }                                                   //
    //transform a single point with associated index  //////////////////////////
    void transform(int i, Matrix &matrix) {                                   //
        float point[] = {_points[i]._x, _points[i]._y, _points[i]._z, 1.0};   //
        Matrix pointMatrix(point, 4, 1);                                      //
        Matrix resultMatrix(matrix.mult(pointMatrix));  ////////////////////////
        _points[i]._x = resultMatrix.at(0, 0);          //
        _points[i]._y = resultMatrix.at(1, 0);  //////////
        _points[i]._z = resultMatrix.at(2, 0);          //////
    }                                                       //
    //assume shape is at (0,0), respect to x-axis, radian   ////
    void rotateX(float alpha){                                //
        float matrix[] = {1.0,        0.0,         0.0, 0.0,  //
                          0.0, cos(alpha), -sin(alpha), 0.0,  ////
                          0.0, sin(alpha),  cos(alpha), 0.0,    //
                          0.0,        0.0,         0.0, 1.0};   //
        Matrix rotater(matrix, 4, 4);                           //
        for(int i = 0; i < _points.size(); i++) {   //////////////
            transform(i, rotater);                  //
        }                           //////////////////////////
    }                                                       //
    //assume shape is at (0,0), respect to y-axis, radian   ////
    void rotateY(float alpha){                                //
        float matrix[] = { cos(alpha), 0.0, sin(alpha), 0.0,  //
                                  0.0, 1.0,        0.0, 0.0,  ////
                          -sin(alpha), 0.0, cos(alpha), 0.0,    //
                                  0.0, 0.0,        0.0, 1.0};   //
        Matrix rotater(matrix, 4, 4);                           //
        for(int i = 0; i < _points.size(); i++) {   //////////////
            transform(i, rotater);                  //
        }                           //////////////////////////
    }                                                       //
    //assume shape is at (0,0), respect to z-axis, radian   ////
    void rotateZ(float alpha){                                //
        float matrix[] = {cos(alpha), -sin(alpha), 0.0, 0.0,  //
                          sin(alpha),  cos(alpha), 0.0, 0.0,  ////
                                 0.0,         0.0, 1.0, 0.0,    //
                                 0.0,         0.0, 0.0, 1.0};   //
        Matrix rotater(matrix, 4, 4);                           //
        for(int i = 0; i < _points.size(); i++) {   //////////////
            transform(i, rotater);                  //
        }                           //////////////////
    }                                               //
public:                                             //
    void translate(float dx, float dy, float dz) {  //
        float matrix[] = {1.0, 0.0, 0.0,  dx,       //
                          0.0, 1.0, 0.0,  dy,   //////
                          0.0, 0.0, 1.0,  dz,   //
                          0.0, 0.0, 0.0, 1.0};  //////
        Matrix translator(matrix, 4, 4);            //
        for(int i = 0; i < _points.size(); i++) {   //
            transform(i, translator);               //
        }                               //////////////
    }                         ////////////
    void scale(float fac) {             //////////////////
        Point center(findCenter());                     //
        translate(-center._x, -center._y, -center._z);  //
        float matrix[] = {fac, 0.0, 0.0, 0.0,           //
                          0.0, fac, 0.0, 0.0,   //////////
                          0.0, 0.0, fac, 0.0,   //
                          0.0, 0.0, 0.0, 1.0};  //////
        Matrix scaler(matrix, 4, 4);                //
        for(int i = 0; i < _points.size(); i++) {   //
            transform(i, scaler);                   ////
        }                                             //
        translate(center._x, center._y, center._z);   ////////////////
    }                                                               //
    //points 'a' and 'b' define a rotation axis, 'angle' in degree  //
    void rotate(Point a, Point b, float angle) {                    //
        //set rotation axis properties            ////////////////////
        Shape rotLine;                        //////
        rotLine.addPoint(a._x, a._y, a._z);   //
        rotLine.addPoint(b._x, b._y, b._z);   //
        rotLine.setConnection(1);             //
        rotLine.addConnection(0, 1);          //////////////////////////
        Point center(rotLine.findCenter());                           //
        float dx = b._x - a._x, dy = b._y - a._y, dz = b._z - a._z;   //
        //angle to rotate into XZ-plane                               //
        float alpha = acos((dz == 0? dz : dz/sqrt(dy*dy + dz*dz)));   //
        //angle to roate into z-axis                                  //
        float beta = asin(dx/sqrt(dx*dx + dy*dy + dz*dz));  ////////////
        //translate to (0, 0, 0)                                //
        translate(-center._x, -center._y, -center._z);          //
        rotLine.translate(-center._x, -center._y, -center._z);  //
        rotateX(alpha);                                         //
        rotLine.rotateX(alpha);   ////////////////////////////////
        rotateY(beta);            //
        rotLine.rotateY(beta);    ////////
        rotateZ(angle/180*PI);          //
        rotLine.rotateZ(angle/180*PI);  //
        rotateY(-beta);                 //
        rotLine.rotateY(-beta);   ////////
        rotateX(-alpha);                //////////
        rotLine.rotateX(-alpha);                ////////
        //translate back to original location         //////////
        translate(center._x, center._y, center._z);           //
        rotLine.translate(center._x, center._y, center._z);   //
        glColor3f(1, 0, 0);                                   //
        rotLine.draw();       //////////////////////////////////
    }                     //////
};      ////////////////////
    //////
//////
//
////////////////////
                  //
class FileReader  //
{                 //
private:          //
    int _nShape;  //////////
    int _index;           //
    std::ifstream _file;  ////////
public:                         //
    FileReader(char name[]) {   //
        _file.open(name);       //
        _file >> _nShape;       //
        _index = 0;         ////////////////////////
    }                                             //
    bool hasNext() { return _index < _nShape; }   //
    Shape readNext() {                            //
        Shape shape;    ////////////////////////////
        int n;              //
        //number of points  ////////////
        _file >> n;                   //
        for(int i = 0; i < n; i++) {  //
            float x, y, z;            //
            _file >> x >> y;     //
            shape.addPoint(x, y, z);  //
        }                             //
        //number of connections       //
        _file >> n;               //////
        shape.setConnection(n);       //
        for(int i = 0; i < n; i++) {  //
            int f, t, l;              ////////
            _file >> f >> t >> l;           //
            shape.addConnection(f-1, t-1);  //
        }                                   //
        _index++;       //////////////////////
        return shape;   //
    }                   //
};      //////////////////
    //////
//////
//
////////////////////////////////////
                                  //
//contains all shapes from input  //
std::vector<Shape> shapes;        //
                            ////////
//////////////////////////////


int main(int argc, char **argv)
{
    
    //the number of pixels in the grid
    grid_width = 600;
    grid_height = 600;
    
    //the size of pixels sets the inital window height and width
    //don't make the pixels too large or the screen size will be larger than
    //your display size
    pixel_size = 1;
    
    /*Window information*/
    win_height = grid_height*pixel_size;
    win_width = grid_width*pixel_size;
    
    ////////////////////////////////////
                                      //////////////////////////////////////////
    FileReader filereader(argv[1]);                                           //
    while(filereader.hasNext()) { shapes.push_back(filereader.readNext()); }  //
                                                                              //
    ////////////////////////////////////////////////////////////////////////////



	/*Set up glut functions*/
    /** See https://www.opengl.org/resources/libraries/glut/spec3/spec3.html ***/
    
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    /*initialize variables, allocate memory, create buffers, etc. */
    //create window of size (win_width x win_height
    glutInitWindowSize(win_width,win_height);
    //windown title is "glut demo"
	glutCreateWindow("glut demo");
    
	/*defined glut callback functions*/
	glutDisplayFunc(display); //rendering calls here
	glutReshapeFunc(reshape); //update GL on window size change
	glutMouseFunc(mouse);     //mouse button events
	glutMotionFunc(motion);   //mouse movement events
	glutKeyboardFunc(key);    //Keyboard events
	glutIdleFunc(idle);       //Function called while program is sitting "idle"
    
    //initialize opengl variables
    init();
    //start glut event loop
	glutMainLoop();
	return 0;
}

/*initialize gl stufff*/
void init()
{
    //set clear color (Default background to white)
	glClearColor(1.0,1.0,1.0,1.0);
    //checks for OpenGL errors
	check();
}

//called repeatedly when glut isn't doing anything else
void idle()
{

    //redraw the scene over and over again
	glutPostRedisplay();	
}

//this is where we render the screen
void display()
{
    //clears the screen
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    //clears the opengl Modelview transformation matrix
	glLoadIdentity();
	
    if(commandEnable == 2) {
        std::string command;
        std::cin >> command;
        int shape;
        if(command == "tra") {
            float dx, dy, dz;
            std::cin >> shape >> dx >> dy >> dz;
            shapes[shape-1].translate(dx, dy, dz);
        }
        else if(command == "rot") {
            float x0, y0, z0, x1, y1, z1, ang;
            std::cin >> shape >> x0 >> y0 >> z0 >> x1 >> y1 >> z1 >> ang;
            shapes[shape-1].rotate(*(new Point(x0, y0, z0)), *(new Point(x1, y1, z1)), ang);
        }
        else if(command == "sca") {
            float factor;
            std::cin >> shape >> factor;
            shapes[shape-1].scale(factor);
        }
        else if(command == "exit") {
            exit(0);
        }
        else {
            std::cout << "No such command exist!" << std::endl;
        }
    }
    if(commandEnable == 0) commandEnable = 1;
    if(commandEnable == 1) commandEnable = 2;

    draw_lines();

    
    
    //blits the current opengl framebuffer on the screen
    glutSwapBuffers();
    //checks for opengl errors
	check();
}


void draw_lines(){
    glLineWidth(1.0); //sets the "width" of each line we are rendering

    //tells opengl to interperate every two calls to glVertex as a line
    glBegin(GL_LINES);

    glColor3f(0.0, 0.0, 0.0);
    //draw divider for 4 quadrant view
    glVertex2f(0.5, 0.0);
    glVertex2f(0.5, 1.0);
    glVertex2f(0.0, 0.5);
    glVertex2f(1.0, 0.5);
    //this will be a red line
    //notice we can use 3d points too
    //how will this change if we project to the XZ or YZ plane?
    // glColor3f(1.0,0.0,0.0);
    // glVertex3f(0.1,0.9,0.5);
    // glVertex3f(0.9,0.1,0.3);
    glEnd();

    //////////////////////////////////////////////////////////
                                                            //
    for(auto s = shapes.begin(); s != shapes.end(); s++) {  //
        glColor3f(0.5, 0.0, 0.9);                           //
        s->draw();                  //////////////////////////
    }               //////////////////
        //////////////
    //////
}

/*Gets called when display size changes, including initial craetion of the display*/
/*this needs to be fixed so that the aspect ratio of the screen is consistant with the orthographic projection*/

void reshape(int width, int height)
{
	/*set up projection matrix to define the view port*/
    //update the ne window width and height
	win_width = width;
	win_height = height;
    
    //creates a rendering area across the window
	glViewport(0,0,width,height);

    // uses an orthogonal projection matrix so that
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //you guys should look up this function
	glOrtho(0.0, 1.0, 0.0, 1.0, -10, 10);
    
    //clear the modelview matrix
    //the ModelView Matrix can be used in this project, to change the view on the projection
    //but you can also leave it alone and deal with changing the projection to a different view
    //for project 2, do not use the modelview matrix to transform the actual geometry, as you won't
    //be able to save the results
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //check for opengl errors
    check();
}

//gets called when a key is pressed on the keyboard
void key(unsigned char ch, int x, int y)
{
	switch(ch)
	{
		default:
            //prints out which key the user hit
            printf("User hit the \"%c\" key\n",ch);
			break;
	}
    //redraw the scene after keyboard input
	glutPostRedisplay();
}


//gets called when a mouse button is pressed
void mouse(int button, int state, int x, int y)
{
    //print the pixel location, and the grid location
    printf ("MOUSE AT PIXEL: %d %d, GRID: %d %d\n",x,y,(int)(x/pixel_size),(int)((win_height-y)/pixel_size));
	switch(button)
	{
		case GLUT_LEFT_BUTTON: //left button
            printf("LEFT ");
            break;
		case GLUT_RIGHT_BUTTON: //right button
            printf("RIGHT ");
		default:
            printf("UNKNOWN "); //any other mouse button
			break;
	}
    if(state !=GLUT_DOWN)  //button released
        printf("BUTTON UP\n");
    else
        printf("BUTTON DOWN\n");  //button clicked
    
    //redraw the scene after mouse click
    glutPostRedisplay();
}

//gets called when the curser moves accross the scene
void motion(int x, int y)
{
    //redraw the scene after mouse movement
	glutPostRedisplay();
}

//checks for any opengl errors in the previous calls and
//outputs if they are present
void check()
{
	GLenum err = glGetError();
	if(err != GL_NO_ERROR)
	{
		printf("GLERROR: There was an error %s\n",gluErrorString(err) );
		exit(1);
	}
}
