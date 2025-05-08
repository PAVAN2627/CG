/*#include <GL/glut.h>
#include <iostream>
using namespace std;

int r;

void E_way(int x, int y)
{

    glBegin(GL_POINTS);
    glVertex2i(x + 320, y + 240);
    glVertex2i(y + 320, x + 240);
    glVertex2i(y + 320, -x + 240);
    glVertex2i(x + 320, -y + 240);
    glVertex2i(-x + 320, -y + 240);
    glVertex2i(-y + 320, -x + 240);
    glVertex2i(-y + 320, x + 240);
    glVertex2i(-x + 320, y + 240);
    glEnd();
    glFlush();
}
void B_circle()
{

    float d;
    d = 3 - 2 * r;

    int x, y;
    x = 0;
    y = r;

    do
    {
        E_way(x, y);

        if (d < 0)
        {

            d = d + 4 * x + 6;
        }
        else
        {
            d = d + 4 * (x - y) + 10;
            y = y - 1;
        }
        x = x + 1;

    } while (x < y);
}
void init()
{

    glClearColor(1, 1, 1, 0);
    glColor3f(1, 0, 0);
    gluOrtho2D(0, 640, 0, 480);
    glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc, char **argv)
{

    cout << "\n Enter Radius \t ";
    cin >> r;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowPosition(100, 100);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Circle");
    init();
    glutDisplayFunc(B_circle);

    glutMainLoop();

    return 0;
}
*/
/*-------------------------------------------------------------------
CONCENTRIC CIRCLES*/

#include <GL/glut.h>
#include <iostream>

using namespace std;

// Global variables for the two radii
int r1; /* r2*/

void E_way(int x, int y)
{
    glBegin(GL_POINTS);
    // The offsets (320, 240) seem to be the center of your window (640/2, 480/2)
    // We draw the 8 points of the circle octants
    glVertex2i(x + 320, y + 240);
    glVertex2i(y + 320, x + 240);
    glVertex2i(y + 320, -x + 240);
    glVertex2i(x + 320, -y + 240);
    glVertex2i(-x + 320, -y + 240);
    glVertex2i(-y + 320, -x + 240);
    glVertex2i(-y + 320, x + 240);
    glVertex2i(-x + 320, y + 240);
    glEnd();
}

// Modified B_circle function to accept radius as a parameter
void B_circle(int radius)
{
    float d;
    d = 3 - 2 * radius;

    int x, y;
    x = 0;
    y = radius; // Use the passed radius

    while (x <= y) // Changed from do-while to while for robustness with small radii
    {
        E_way(x, y);

        if (d < 0)
        {
            d = d + 4 * x + 6;
        }
        else
        {
            d = d + 4 * (x - y) + 10;
            y = y - 1;
        }
        x = x + 1;
    }
    // Need to ensure the points when x > y are also drawn based on symmetry
    // The E_way function handles the 8-way symmetry, so the while(x <= y) loop is sufficient
    // as long as E_way correctly plots all 8 points.
    // The original code had a do-while which might miss the very center pixel for r=0,
    // but for drawing circles > 0 radius, while(x <= y) is standard.

    // Flush is handled in the main display function after drawing all shapes
}

// New display function that draws both circles
void displayConcentricCircles()
{
    glClear(GL_COLOR_BUFFER_BIT); // Clear the buffer at the start

    // Set color for the first circle (e.g., red)
    glColor3f(1, 0, 0);
    B_circle(r1); // Draw the first circle

    // Set color for the second circle (e.g., blue)
    /*glColor3f(0, 0, 1); // You can change this color
    B_circle(r2);*/
    // Draw the second circle

    glFlush(); // Ensure all drawing commands are executed
}

void init()
{
    glClearColor(1, 1, 1, 0); // White background
    // Initial drawing color is set within the display function for each circle
    gluOrtho2D(0, 640, 0, 480); // Set up the coordinate system
    // Clear is now done in the display function
}

int main(int argc, char **argv)
{
    cout << "\n Enter Radius for the first circle\t";
    cin >> r1;

    /*cout << "\n Enter Radius for the second circle\t";
    cin >> r2;

    // Optional: Ensure radii are positive
    if (r1 <= 0 || r2 <= 0)
    {
        cout << "Please enter positive radii." << endl;
        return 1; // Exit with an error code
    }
    // Optional: Draw the smaller circle first to ensure it's on top if needed
    // if (r1 > r2) swap(r1, r2);*/

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowPosition(100, 100);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Concentric Circles");

    init();

    // Set the display function to our new function
    glutDisplayFunc(displayConcentricCircles);

    glutMainLoop();

    return 0;
}