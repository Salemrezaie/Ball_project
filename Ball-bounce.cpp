#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <list>
#include <vector>
#include <GL/glut.h>
const GLuint FRAME_RATE = 60;
const float BALL_SPEED = 5.0;
const float g = 9.81;
struct ball
{
    double x;
    double y;
    double angle;
    int time;
};
static std::list<ball> balls;
enum DISPLAY_LISTS {
    DL_CENTRE,
    DL_CROSSHAIR,
    DL_BALL,
    DL_MAX
};
static GLuint DisplayListsBase;
static size_t WindowWidth = 640;
static size_t WindowHeight = 480;
static GLuint CursorX, CursorY;
static GLuint CentreX, CentreY;
void draw_centre()
{
    glBegin(GL_TRIANGLE_FAN);
    double ox = 0.0, oy = 0.0, radius = 0.5;
    int triangles = 32;
    glVertex2f(ox, oy);
    for (int i = 0; i <= triangles; i++)
    {
        double angle = i * 2.0 * M_PI / triangles;
        glVertex2f(ox + radius * cos(angle), oy + radius * sin(angle));
    }
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(0.55, 0.4);
    glVertex2f(0.55, -0.4);
    glVertex2f(0.95, 0.0);
    glEnd();
}
void draw_crosshair()
{
    glBegin(GL_LINE_LOOP);  //box
    glVertex2f(-0.5, -0.5);
    glVertex2f(-0.5, 0.5);
    glVertex2f(0.5, 0.5);
    glVertex2f(0.5, -0.5);
    glEnd();
    glBegin(GL_LINES); //cross
    glVertex2f(-1.5, 0.0);
    glVertex2f(1.5, 0.0);
    glVertex2f(0.0, 1.5);
    glVertex2f(0.0, -1.5);
    glEnd();
}
void draw_ball()
{
    glBegin(GL_TRIANGLE_FAN);
    double ox = 0.0, oy = 0.0, radius = 1.0;
    int triangles = 32;
    glVertex2f(ox, oy);
    for (int i = 0; i <= triangles; i++)
    {
        double angle = i * 2.0 * M_PI / triangles;
        glVertex2f(ox + radius * cos(angle), oy + radius * sin(angle));
    }
    glEnd();
}
double compute_angle(double a_x, double a_y, double b_x, double b_y)
{
    double angle = 0.0;
    if (b_x > a_x && b_y > a_y)
        angle = atan2(b_y - a_y, b_x - a_x) * 180.0 / M_PI;
    else if (b_x < a_x && b_y > a_y)
        angle = 180.0 - (atan2(b_y - a_y, a_x - b_x) * 180.0 / M_PI);
    else if (b_x < a_x && b_y < a_y)
        angle = 180.0 + (atan2(a_y - b_y, a_x - b_x) * 180.0 / M_PI);
    else if (b_x > a_x && b_y < a_y)
        angle = 360.0 - (atan2(a_y - b_y, b_x - a_x) * 180.0 / M_PI);
    return angle;
}
void draw_scene()
{
    double angle = compute_angle(CentreX, CentreY, CursorX, CursorY);
    glColor3f(0.0, 1.0, 0.0);
    glPushMatrix();
    glTranslatef(CentreX, CentreY, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);
    glScalef(20.0, 20.0, 1.0);
    glCallList(DisplayListsBase + DL_CENTRE);
    glPopMatrix();
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glTranslatef(CursorX, CursorY, 0.0);
    glScalef(10.0, 10.0, 1.0);
    glCallList(DisplayListsBase + DL_CROSSHAIR);
    glPopMatrix();
    std::vector<std::list<ball>::iterator> to_erase;

    for (auto i = balls.begin(); i != balls.end(); i++)
    {
        glColor3f(0.0, 0.0, 1.0);
        i->x += BALL_SPEED * cos(i->angle * M_PI / 180.0);
        i->y += BALL_SPEED * sin(i->angle * M_PI / 180.0);
        i->y -= g * 0.01 * i->time;
        i->time++;
        if (i->x < 0 || i->x > WindowWidth || i->y < 0 || i->y > WindowHeight)
            to_erase.push_back(i);
        else
        {
            glPushMatrix();
            glTranslatef(i->x, i->y, 0.0);
            glRotatef(i->angle, 0.0, 0.0, 1.0);
            glScalef(10.0, 10.0, 1.0);
            glCallList(DisplayListsBase + DL_BALL);
            glPopMatrix();
        }
    }
    for (auto i : to_erase)
        balls.erase(i);
}
void init_scene()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glutSetCursor(GLUT_CURSOR_NONE);
    CentreX = WindowWidth / 2;
    CentreY = WindowHeight / 2;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WindowWidth, 0, WindowHeight);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, WindowWidth, WindowHeight);
}
void make_display_lists()
{
    DisplayListsBase = glGenLists(DL_MAX);
    glNewList(DisplayListsBase + DL_CENTRE, GL_COMPILE);
    glPushMatrix();
    draw_centre();
    glPopMatrix();
    glEndList();
    glNewList(DisplayListsBase + DL_CROSSHAIR, GL_COMPILE);
    glPushMatrix();
    draw_crosshair();
    glPopMatrix();
    glEndList();
    glNewList(DisplayListsBase + DL_BALL, GL_COMPILE);
    glPushMatrix();
    draw_ball();
    glPopMatrix();
    glEndList();
}
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    draw_scene();
    glutSwapBuffers();
}
void reshape(GLsizei nw, GLsizei nh)
{
    WindowWidth = nw;
    WindowHeight = nh;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WindowWidth, 0, WindowHeight);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, WindowWidth, WindowHeight);
}
void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        ball m;
        CursorX = x;
        CursorY = WindowHeight - y;

        m.x = CentreX;
        m.y = CentreY;
        m.angle = compute_angle(CentreX, CentreY, CursorX, CursorY);
        m.time = 0;
        balls.push_back(m);
    }
}
void passive_motion(int x, int y)
{
    CursorX = x;
    CursorY = WindowHeight - y;
}
void keyboard(unsigned char key, int x, int y)
{
    if (key == 'q' || key == 'Q')
        exit(EXIT_SUCCESS);
}
void timer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(1000 / FRAME_RATE, timer, value);
}
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WindowWidth, WindowHeight);
    glutInitWindowPosition(300, 300);
    glutCreateWindow("My Window");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(passive_motion);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(1000 / FRAME_RATE, timer, 0);
    init_scene();
    make_display_lists();
    glutMainLoop();
    return (EXIT_SUCCESS);
}