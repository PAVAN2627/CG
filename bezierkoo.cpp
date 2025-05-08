#include <iostream>
#include <vector>
#include <cmath> // For sin, cos, sqrt, M_PI
#include <GL/glut.h>

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Structure to represent a 2D point using double for precision
struct Point
{
    double x, y;
};

// Window dimensions
const int window_width = 800;
const int window_height = 600;

// Fractal parameters
enum FractalType
{
    NONE,
    KOCH,
    BEZIER
};
FractalType current_fractal = NONE;
int recursion_level = 0;           // Start with level 0 (basic shape)
const int max_recursion_level = 6; // Limit recursion depth

// --- OpenGL Functions ---

void init()
{
    glClearColor(1.0, 1.0, 1.0, 0.0); // White background
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, window_width, 0, window_height); // Set up 2D orthogonal projection
    glClear(GL_COLOR_BUFFER_BIT);                  // Clear the buffer
}

// Function to draw a line segment
void drawLine(Point p1, Point p2)
{
    glBegin(GL_LINES);
    glVertex2d(p1.x, p1.y);
    glVertex2d(p2.x, p2.y);
    glEnd();
}

// Function to draw a single point
void drawPoint(Point p)
{
    glBegin(GL_POINTS);
    glVertex2d(p.x, p.y);
    glEnd();
}

// --- Koch Snowflake Implementation ---

// Recursive function to draw one segment of the Koch curve
void kochSegment(Point p1, Point p2, int level)
{
    if (level == 0)
    {
        drawLine(p1, p2);
    }
    else
    {
        // Calculate the four points for the subdivision
        Point pA, pB, pC, pD;

        // pA is the first point (same as p1)
        pA = p1;

        // pB is 1/3 of the way from p1 to p2
        pB.x = p1.x + (p2.x - p1.x) / 3.0;
        pB.y = p1.y + (p2.y - p1.y) / 3.0;

        // pD is 2/3 of the way from p1 to p2
        pD.x = p1.x + 2.0 * (p2.x - p1.x) / 3.0;
        pD.y = p1.y + 2.0 * (p2.y - p1.y) / 3.0;

        // pC is the peak of the equilateral triangle
        // Rotate pD - pB by 60 degrees about pB
        double angle_radians = 60.0 * M_PI / 180.0;
        double dx = pD.x - pB.x;
        double dy = pD.y - pB.y;

        pC.x = pB.x + dx * cos(angle_radians) - dy * sin(angle_radians);
        pC.y = pB.y + dx * sin(angle_radians) + dy * cos(angle_radians);

        // Recursively call kochSegment for the four new segments
        kochSegment(pA, pB, level - 1);
        kochSegment(pB, pC, level - 1);
        kochSegment(pC, pD, level - 1);
        kochSegment(pD, p2, level - 1);
    }
}

// Function to draw the Koch Snowflake (starts with an equilateral triangle)
void drawKochSnowflake(int level)
{
    // Define the vertices of the initial equilateral triangle
    Point v1, v2, v3;

    // Center the triangle in the window
    double triangle_side = min(window_width, window_height) * 0.6; // Adjust size as needed
    double center_x = window_width / 2.0;
    double center_y = window_height / 2.0;

    // Calculate vertices of an equilateral triangle centered at (center_x, center_y)
    v1.x = center_x - triangle_side / 2.0;
    v1.y = center_y - triangle_side * sqrt(3.0) / 6.0;

    v2.x = center_x + triangle_side / 2.0;
    v2.y = center_y - triangle_side * sqrt(3.0) / 6.0;

    v3.x = center_x;
    v3.y = center_y + triangle_side * sqrt(3.0) / 3.0;

    // Draw the three segments of the snowflake recursively
    glColor3f(0.0, 0.0, 1.0); // Blue color for the snowflake
    kochSegment(v1, v2, level);
    kochSegment(v2, v3, level);
    kochSegment(v3, v1, level);
}

// --- Bezier Fractal Implementation ---

// Recursive function to draw a Bezier curve fractal
void bezierFractal(const vector<Point> &control_points, int level)
{
    if (level == 0)
    {
        // Base case: Draw the Bezier curve defined by the control points
        // For simplicity, we'll draw a series of line segments approximating the curve
        if (control_points.size() < 2)
            return;

        glColor3f(1.0, 0.0, 0.0); // Red color for Bezier fractal
        glBegin(GL_LINE_STRIP);
        // Approximate the Bezier curve by evaluating points along the curve
        for (double t = 0.0; t <= 1.0; t += 0.01)
        { // Adjust step for smoother curve
            Point p = {0.0, 0.0};
            int n = control_points.size() - 1;
            for (int i = 0; i <= n; ++i)
            {
                double bernstein = pow(1.0 - t, n - i) * pow(t, i) *
                                   (t == 0.0 && i == 0 ? 1.0 : // Handle 0^0 case
                                        t == 1.0 && i == n ? 1.0
                                    : t > 0.0 && t < 1.0   ?
                                                         // Calculate binomial coefficient nCi
                                        [&]()
                                        {
                                            double res = 1.0;
                                            for (int j = 0; j < i; ++j)
                                            {
                                                res = res * (n - j) / (j + 1);
                                            }
                                            return res;
                                        }()
                                                         : 0.0);

                p.x += control_points[i].x * bernstein;
                p.y += control_points[i].y * bernstein;
            }
            glVertex2d(p.x, p.y);
        }
        glEnd();
    }
    else
    {
        // Recursive step: Subdivide the curve and call recursively
        if (control_points.size() < 2)
            return;

        // Use de Casteljau's algorithm to get control points for the two sub-curves
        vector<Point> left_control_points;
        vector<Point> right_control_points;
        vector<Point> current_points = control_points;

        left_control_points.push_back(current_points[0]);

        for (int j = 1; j < control_points.size(); ++j)
        {
            vector<Point> next_points;
            for (int i = 0; i < current_points.size() - 1; ++i)
            {
                Point mid;
                mid.x = (1.0 - 0.5) * current_points[i].x + 0.5 * current_points[i + 1].x;
                mid.y = (1.0 - 0.5) * current_points[i].y + 0.5 * current_points[i + 1].y;
                next_points.push_back(mid);
            }
            left_control_points.push_back(next_points[0]);
            right_control_points.push_back(next_points.back());
            current_points = next_points;
        }
        right_control_points.push_back(control_points.back());

        // Recursively call for the left and right sub-curves
        bezierFractal(left_control_points, level - 1);
        bezierFractal(right_control_points, level - 1);
    }
}

// Function to set up initial control points for the Bezier fractal
void drawBezierFractal(int level)
{
    vector<Point> initial_control_points;

    // Define some initial control points for a simple Bezier curve
    // You can change these to create different initial shapes
    initial_control_points.push_back({100.0, 300.0});
    initial_control_points.push_back({200.0, 500.0});
    initial_control_points.push_back({400.0, 100.0});
    initial_control_points.push_back({500.0, 300.0});

    bezierFractal(initial_control_points, level);
}

// --- Callback Functions ---

void display()
{
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

    // Draw the selected fractal
    if (current_fractal == KOCH)
    {
        drawKochSnowflake(recursion_level);
    }
    else if (current_fractal == BEZIER)
    {
        drawBezierFractal(recursion_level);
    }

    glFlush(); // Ensure all drawing commands are executed
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'k': // Select Koch Snowflake
    case 'K':
        current_fractal = KOCH;
        recursion_level = 0; // Reset level for new fractal type
        cout << "Selected: Koch Snowflake. Level: " << recursion_level << endl;
        break;
    case 'b': // Select Bezier Fractal
    case 'B':
        current_fractal = BEZIER;
        recursion_level = 0; // Reset level for new fractal type
        cout << "Selected: Bezier Fractal. Level: " << recursion_level << endl;
        break;
    case '+': // Increase recursion level
        if (recursion_level < max_recursion_level)
        {
            recursion_level++;
            cout << "Recursion Level: " << recursion_level << endl;
        }
        else
        {
            cout << "Max recursion level reached (" << max_recursion_level << ")." << endl;
        }
        break;
    case '-': // Decrease recursion level
        if (recursion_level > 0)
        {
            recursion_level--;
            cout << "Recursion Level: " << recursion_level << endl;
        }
        else
        {
            cout << "Minimum recursion level reached (0)." << endl;
        }
        break;
    case 'r': // Reset
    case 'R':
        current_fractal = NONE;
        recursion_level = 0;
        cout << "Reset. Select a fractal type (K or B)." << endl;
        break;
    case 'q': // Quit
    case 'Q':
        exit(0);
        break;
    }
    glutPostRedisplay(); // Request a redraw to show the changes
}

// --- Main Function ---

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Fractal Patterns");

    init(); // Initialize OpenGL settings

    glutDisplayFunc(display);   // Set the display callback function
    glutKeyboardFunc(keyboard); // Set the keyboard callback function

    cout << "Press 'K' for Koch Snowflake." << endl;
    cout << "Press 'B' for Bezier Fractal." << endl;
    cout << "Use '+' and '-' keys to change recursion level." << endl;
    cout << "Press 'R' to reset." << endl;
    cout << "Press 'Q' to quit." << endl;

    glutMainLoop(); // Enter the GLUT event processing loop

    return 0;
}
