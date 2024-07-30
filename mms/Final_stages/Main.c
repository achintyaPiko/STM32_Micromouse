#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "API.h"

//https://github.com/mackorone/mms
//Issue command: stdout stream
//Read response: stdin stream
//Display to the simulator text box: print to stderr stream 
//If display to simulator is int use fprint, else if char or string use logging fn

//=====================================================================SIMULATOR FUCKERY============================================================================================

    /*LOL TURNS OUT THAT STDERR STREAM NEEDS THE \N CHARACTER TO FULLY FLUSH OUT FROM THE ERROR BUFFER. SO IMMEDIATE FFLUSH AFTER FPRINT AND USE \N IN FPRINTF. NOW THE DEBUGGING IS ON*/
    /*fprintf(stderr, "%d \n",maze[15][0].y);
    fflush(stderr);
    
    
    
    int testValue = 123;
    fprintf(stderr, "%d\n", testValue);
    fflush(stderr);
    */

/*======================================================================DataTypes and structs=============================================================*/

//estimated size of each cell struct => 1+1+1+1 = 4B
//size of this Cell data type has been verified by using sizeof fn

//ALTHOUGH I THINK THE INT X,Y IS NOT NEEDED AS THE NUMBER WITH WHICH WE WILL BE ACCESSING THE 2D ARRAY OF CELL TYPE WILL BE THE X AND Y CO ORDINATES
struct Cell {
    //int x;             // X coordinate of the cell
    //int y;             // Y coordinate of the cell
    bool hasWallNorth;    // Indicates if there is a wall above the cell
    bool hasWallSouth;  // Indicates if there is a wall below the cell
    bool hasWallWest;  // Indicates if there is a wall to the left of the cell
    bool hasWallEast; // Indicates if there is a wall to the right of the cell
};






/*=================================================================Algorithm Function declaration and behavior===================================================================*/


void fillManhattanDistance(int **arr, int m, int n) {
    int center_x = m / 2;
    int center_y = n / 2;

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            arr[i][j] = abs(i - center_x) + abs(j - center_y);
        }
    }
}


//usage of below function:  int **array = createAndFillArray(m, n);
int **createAndFillArray(int m, int n) {

    // Allocate memory for the 2D array
    int **array = (int **)malloc(m * sizeof(int *));
    for (int i = 0; i < m; i++) {
        array[i] = (int *)malloc(n * sizeof(int));
    }

    // Fill the array with Manhattan distances from the center
    fillManhattanDistance(array, m, n); 

    return array;
}

void freeArray(int **array, int m) {
    for (int i = 0; i < m; i++) {
        free(array[i]);
    }
    free(array);
}

//for movement steps
void getDelta(char dir, int *deltaRow, int *deltaColumn) {

    switch (dir) {
        case 'u':
            *deltaRow = -1;  // Moving up decreases the row index
            *deltaColumn = 0;
            break;
        case 'd':
            *deltaRow = 1;   // Moving down increases the row index
            *deltaColumn = 0;
            break;
        case 'l':
            *deltaColumn = -1; // Moving left decreases the column index
            *deltaRow = 0;
            break;
        case 'r':
            *deltaColumn = 1;  // Moving right increases the column index
            *deltaRow = 0;
            break;
    }
}

void modifyCellAndAssignWalls(struct Cell array[][16], char dir, int row, int col, bool front, bool left, bool right){

    bool firstTimeUseonly = true;
    switch (dir) {
        case 'u':
            array[row][col].hasWallNorth = front;
            array[row][col].hasWallSouth = false;
            array[row][col].hasWallWest = left;
            array[row][col].hasWallEast = right;
            break;
        case 'd':
            array[row][col].hasWallNorth = false;
            array[row][col].hasWallSouth = front;
            array[row][col].hasWallWest = right;
            array[row][col].hasWallEast = left;
            break;
        case 'l':
            array[row][col].hasWallNorth = right;
            array[row][col].hasWallSouth = left;
            array[row][col].hasWallWest = front;
            array[row][col].hasWallEast = false;
            break;
        case 'r':
            array[row][col].hasWallNorth = left;
            array[row][col].hasWallSouth = right;
            array[row][col].hasWallWest = false;
            array[row][col].hasWallEast = front;
            break;
    }
    if(firstTimeUseonly){
        array[15][0].hasWallSouth = true; //for this corner case
        firstTimeUseonly = false;
    }


}

void resetWallsWithBorder(struct Cell maze[][16]){

    // Initialize the maze cells with a typucal wall maze layout
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            maze[i][j].hasWallNorth = false;
            maze[i][j].hasWallSouth = false;
            maze[i][j].hasWallWest = false;
            maze[i][j].hasWallEast = false;
        }
    }
    
    for (int i = 0; i<16; i++){
        maze[0][i].hasWallNorth = true;
    }

    for (int i = 0; i<16; i++){
        maze[15][i].hasWallSouth = true;
    }
    
    for (int i = 0; i<16; i++){
        maze[i][0].hasWallWest = true;
    }

    for (int i = 0; i<16; i++){
        maze[i][15].hasWallEast = true;
    }
}


/*============================================================================I/O Functions==========================================================*/


void print2DArray(int **arr, int m, int n) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            fprintf(stderr, "%3d ", arr[i][j]);
            fflush(stderr);
        }
        fprintf(stderr,"\n");
        fflush(stderr);
    }
}


void logging(char* text) {
    fprintf(stderr, "%s\n", text);
    fflush(stderr);
}

void markWalls(int x, int y, bool NorthReading,bool WestReading,bool SouthReading,bool EastReading){

    if (WestReading)
        API_setWall(x, 15-y, 'w');
    
    if (EastReading)
        API_setWall(x, 15-y, 'e');
    
    if (NorthReading)
        API_setWall(x, 15-y, 'n');
    
    if (SouthReading)
        API_setWall(x, 15-y, 's');
    
}

void markAllKnownWallsInBeginning(struct Cell array[][16]){

    for(int i  = 0; i<16;i++){
        for(int j = 0; j<16;j++){
            if (array[i][j].hasWallWest)
                API_setWall(j, 15-i, 'w');
    
            if (array[i][j].hasWallEast)
                API_setWall(j, 15-i, 'e');
    
            if (array[i][j].hasWallNorth)
                API_setWall(j, 15-i, 'n');
    
            if (array[i][j].hasWallSouth)
                API_setWall(j, 15-i, 's');
        }
    } 
}

/*<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-*/
/*=============================================================================MAIN FN========================================================================*/
/*<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-*/



int main(int argc, char* argv[]) {

/*=====================================================================SETUP============================================*/

/*================================================================STATIC VALUES and INITIALIZATIONS====================================================================*/

    int rows = 16, columns =16;
    
    int xCordMouse = 0, yCordMouse = 15;    
    char direction = 'u'; //initial Direction
    int deltaRow = -1, deltaColumn = 0; //initial facing up step
    
    int **manhattan = createAndFillArray(rows, columns);
    //manhattan[8][8] = 39;
    //create maze walls array
    struct Cell maze[rows][columns];

    /*
    1. There will be two 2d arrays.
        One stores the Manhattan priority of the maze cells
        The other stores the walls at each cell co ordinate. 
        **No consequent inferablilty of wall logic is implemented ie agar left mei wall hai toh left baaju ka toh right hoga hi. Recommend to not implement it too.

    2. There will a robot co ordinate tracker, a which direction is it facing tracker and the movement step modifier to correctly modify the robot (x,y)
        Direction: U,D,L,R
        deltaX = +-1 or 0 depending on orientation
        deltaY = +-1 or 0 depending on orientation
        X is Row
        Y is Column
        !!!!!!It is imperative that the robot is able to correctly keep track of its heading at all times.!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        !!!!!!IT IS VERY IMPORTANT THAT THE FUNCTIONALITY OF REVERSING IS NOT INTRODUCED TO THE ROBOT.!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        -------------------------------------> X direction
        |
        |
        |
        |
        |
        |
        |
        |
        |
        |
        V   Y direction
        
    */

    logging("Running...");
    API_setColor(0, 0, 'G');
    API_setText(0, 0, "abc");
    
    
    print2DArray(manhattan, rows, columns);
    //freeArray(manhattan, rows);

    resetWallsWithBorder(maze);
    markAllKnownWallsInBeginning(maze);

    
    

    
    

/*===============================================================LOOPER==================================================*/
    while (1) {
        bool leftDetect = API_wallLeft();
        bool rightDetect = API_wallRight();
        bool frontDetect = API_wallFront();

        modifyCellAndAssignWalls(maze, direction, yCordMouse, xCordMouse, frontDetect, leftDetect, rightDetect);
        
        

        markWalls(xCordMouse,yCordMouse,maze[yCordMouse][xCordMouse].hasWallNorth,maze[yCordMouse][xCordMouse].hasWallWest,maze[yCordMouse][xCordMouse].hasWallSouth, maze[yCordMouse][xCordMouse].hasWallEast);

        fprintf(stderr,"At (%d  %d) walls in North West South and East are %d %d %d %d \n", xCordMouse, yCordMouse, maze[yCordMouse][xCordMouse].hasWallNorth,
                                                                                                                 maze[yCordMouse][xCordMouse].hasWallWest,
                                                                                                                 maze[yCordMouse][xCordMouse].hasWallSouth,
                                                                                                                 maze[yCordMouse][xCordMouse].hasWallEast); 
        fflush(stderr);
        
        

        /*for queue implementation to store the x and y of the cell into a single storage element of the queue I believe we can use the double/float data types.
          As in the queue cell encoded data could be [ x + (y/100) ]will be stored in the single storage space of the queue. 
          To extract the information or decode simply x = floor(float number) and y = ((float number) - x) * 100 */

        /*THE ASIAN GUYS PSEUDOCODE IS MUCH BETTER AND SHOULD BE FOLLOWED FOR FLOODFILL. 💀*/

        if (!API_wallLeft()) {
            if (direction == 'u')
                direction = 'l';
            else if (direction == 'l')
                direction = 'd';
            else if (direction == 'd')
                direction = 'r';
            else
                direction = 'u';
            API_turnLeft();
            
                      
        }
        while (API_wallFront()) {
            if (direction == 'u')
                direction = 'r';
            else if (direction == 'r')
                direction = 'd';
            else if (direction == 'd')
                direction = 'l';
            else
                direction = 'u';
            API_turnRight();
            
        }
        
        API_moveForward();
        getDelta(direction, &deltaRow, &deltaColumn);
        yCordMouse = yCordMouse + (1*deltaRow);
        xCordMouse = xCordMouse + (1*deltaColumn);
        
    }
}


//move towards decreasing manhattan -> [should also return true and false if possible or not]



//1.//  *find lowest manhattan D4 neighbor [should return manhattan value and direction global reference]
//2.//  is D4 neighbor accessible or not
//3.//  condition checks             -> if accessible and lower manhattan then


////*turn towards given direction from current heading on spot and move

////*sort d4 neighbors increasing


//sort the d4 neighbors in increasing order of their manhattan distances
//if manhattan distance of first element of sorted array greater than current cell manhattan perform floodfill else continue
//check in order of sorted array which are accessible
//as soon as any accessible confirmed turn towards it and move
//if none accessible perform floodfill