#include <iostream>
#include <time.h> // time
#include <stdlib.h> // srand
 
using namespace std;
 
int main() {
    srand(time(NULL));
    cout.precision(10);
    
    const int N[] = {1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9};
    for (int j = 0; j < (sizeof(N) / sizeof(N[0])); j ++) {
        double circlePoints = 0, squarePoints = 0, x, y;
        for (int i = 0; i < N[j]; i ++) {
            x = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
            y = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
            if (x * x + y * y <= 1.0) 
                circlePoints ++;
            squarePoints++;
        }
 
        cout << "Inside Circle: " << circlePoints << "\t\tInside Square: " << squarePoints
            << "\t\tEstimated PI: " << (double)circlePoints / N[j] * 4.0 << endl << endl; 
    }
    return 0;
}

