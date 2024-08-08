#include <stdio.h>
#include <stdlib.h>
//#include <math.h>
#include <stdbool.h>
#include "API.h"

#define width 16
#define height 16



short currentX = 0, currentY = 15;    
char direction = 'u'; //initial Direction
short deltaRow = -1, deltaColumn = 0;

/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*                                                     Structs and custom data datatypes                                             */
/*-----------------------------------------------------------------------------------------------------------------------------------*/

struct Cell {
    bool hasWallNorth;    // Indicates if there is a wall above the cell
    bool hasWallSouth;  // Indicates if there is a wall below the cell
    bool hasWallWest;  // Indicates if there is a wall to the left of the cell
    bool hasWallEast; // Indicates if there is a wall to the right of the cell
};

// Define a struct to hold two int values
/*  int first;     // X coordinate
    int second;   //  Y coordinate*/
typedef struct {
    short first;     // X coordinate
    short second;   //  Y coordinate
} Twoshorts;

// Define a node in the queue
typedef struct Node {
    Twoshorts data;
    struct Node* next;
} Node;

// Define the queue structure
typedef struct Queue {
    Node* front;
    Node* rear;
} Queue;

/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*                                                         Queue Data Structure Functions                                            */
/*-----------------------------------------------------------------------------------------------------------------------------------*/

// Initialize a new queue
Queue* createQueue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    return q;
}

// Add an element to the queue
void enqueue(Queue* q, short x, short y) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data.first = x;
    newNode->data.second = y;
    newNode->next = NULL;
    
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
        return;
    }
    
    q->rear->next = newNode;
    q->rear = newNode;
}

// Remove an element from the queue
Twoshorts dequeue(Queue* q) {
    if (q->front == NULL) {
        return (Twoshorts){-1, -1}; // Queue is empty
    }
    
    Node* temp = q->front;
    q->front = q->front->next;
    
    if (q->front == NULL) {
        q->rear = NULL;
    }
    
    Twoshorts value = temp->data;
    free(temp); // Free the memory of the dequeued node
    return value;
}

// Check if the queue is empty
short isEmpty(Queue* q) {
    return q->front == NULL;
}

// Free the queue to avoid memory leaks
void freeQueue(Queue* q) {
    while (!isEmpty(q)) {
        dequeue(q);
    }
    free(q);
}

/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*                                                    Logic and Solving related Functions                                            */
/*-----------------------------------------------------------------------------------------------------------------------------------*/

void resetManhattanForFloodfill(short array[][width],short rows, short columns, short goalX, short goalY){  //change width to 16 in final code

    for (short i = 0; i < rows; i++){
        for (short j = 0; j < columns; j++)
            array[i][j] = -1;
    }
    array[goalY][goalX] = 0;

}

void resetWallsWithBorder(struct Cell maze[][width]){

    // Initialize the maze cells with a typucal wall maze layout
    for (short i = 0; i < width; i++) {
        for (short j = 0; j < width; j++) {
            maze[i][j].hasWallNorth = false;
            maze[i][j].hasWallSouth = false;
            maze[i][j].hasWallWest = false;
            maze[i][j].hasWallEast = false;
        }
    }
    
    for (short i = 0; i<width; i++){
        maze[0][i].hasWallNorth = true;
    }

    for (short i = 0; i<width; i++){
        maze[width-1][i].hasWallSouth = true;
    }
    
    for (short i = 0; i<width; i++){
        maze[i][0].hasWallWest = true;
    }

    for (short i = 0; i<width; i++){
        maze[i][width-1].hasWallEast = true;
    }
}

void floodfill(short array[][width], struct Cell walls[][width], short rows, short columns, short goalX, short goalY){   //change width to 16 in final code

    
    Twoshorts Point;
    Queue* queue = createQueue(); 
    enqueue(queue,goalX,goalY);  //adding to the q the goal cell and the 0 has been defined by the other function hopefully
    while( !isEmpty(queue) ){
        Point = dequeue(queue);   // now i have the point out of q decoded for my purposes 🧐🧑🏫 It has come out of line for consideration
        
        //Problem: To avoid out of bounds error for array during the top-most left-most right-most and bottom-most cells while checking the blank and accessibility thing

        //Another idea 1. First check the accessibilty of the neighbors using the the walls array. What this should do is since the edge cells are 
        //                already being faced by a wall they will not be accessed leading to safeguard against out of bounds accesses.
        //                How this is ensured is by using 4 different if statements for each direction.
        //                Its is important that the border wall are defined before hand and is without errors
        //             2. Inside the if statement after ensuring accessibilty in "one" direction only check if the value is blank.
        //             3. However this logic works flawlessly only if the First cell explored ie. (0,15) is recording the wall behind it because all walls are being recorded as the 
        //                mouse explores rightly. 
        //                  
        //                  What it means is :-         If accessibleNorth then{
        //                                                   If blankInNorth then
        //                                                       increment value and add to q
        //                                              }
        //                                              If accessibleSouth then{
        //                                                   If blankInSouth then
        //                                                       increment value and add to q
        //                                              }
        //                                              If accessibleEast then{
        //                                                   If blankInEast then
        //                                                       increment value and add to q
        //                                              }
        //                                              If accessibleWest then{
        //                                                   If blankInWest then
        //                                                       increment value and add to q
        //                                              }                     

        /*
        //Implementation of said idea:
        if (!walls[encodedPoint.y][encodedPoint.x].hasWallNorth){ //if there is no wall then (!0) 
            if(!hasBeenAccessed[encodedPoint.y - 1][encodedPoint.x]){        //array[encodedPoint.y - 1][encodedPoint.x] == -1
                array[encodedPoint.y - 1][encodedPoint.x] = array[encodedPoint.y][encodedPoint.x] + 1;
                point = encode(encodedPoint.x,encodedPoint.y - 1);
                hasBeenAccessed[encodedPoint.y - 1][encodedPoint.x] = true;
                enqueue(queue, point);
            }
        }

        if (!walls[encodedPoint.y][encodedPoint.x].hasWallSouth){ //if there is no wall then (!0) 
            if(!hasBeenAccessed[encodedPoint.y + 1][encodedPoint.x]){
                array[encodedPoint.y + 1][encodedPoint.x] = array[encodedPoint.y][encodedPoint.x] + 1;
                point = encode(encodedPoint.x,encodedPoint.y + 1);
                hasBeenAccessed[encodedPoint.y + 1][encodedPoint.x] = true;
                enqueue(queue, point);
            }
        }

        if (!walls[encodedPoint.y][encodedPoint.x].hasWallEast){ //if there is no wall then (!0) 
            if(!hasBeenAccessed[encodedPoint.y][encodedPoint.x + 1]){
                array[encodedPoint.y][encodedPoint.x + 1] = array[encodedPoint.y][encodedPoint.x] + 1;
                point = encode(encodedPoint.x + 1,encodedPoint.y);
                hasBeenAccessed[encodedPoint.y][encodedPoint.x + 1] = true;
                enqueue(queue, point);
            }
        }

        if (!walls[encodedPoint.y][encodedPoint.x].hasWallWest){ //if there is no wall then (!0) 
            if(!hasBeenAccessed[encodedPoint.y][encodedPoint.x - 1]){
                array[encodedPoint.y][encodedPoint.x - 1] = array[encodedPoint.y][encodedPoint.x] + 1;
                point = encode(encodedPoint.x - 1,encodedPoint.y);
                hasBeenAccessed[encodedPoint.y][encodedPoint.x - 1] = true;
                enqueue(queue, point);
            }
        }
        
        // The above implementation was logically sound but was lost in translation and something during the point encode and decode rounding
        // of the floating-point operation went wrong. What went wrong is explained below.

        // The code above used a queue that stored a single float value in the queue cell which was encoded as "x.(y/100)" or "x.(y*0.01)". 
        //This was causing some issue which the way how floating point numbers are stored in computers. ie say for eg x = y = 2. 
        //Then it should be 2.02 in the queue but computer stores it as 2.01999999999 which causes during decoding
        // the function to extract x = 2 and y = 1.
        // The code below uses the same logic but uses a better queue data structure that is defined such that each queue cell contains 2 ints of x and y. So more memory but 
        // no segfaults and wrong data
        */

       if ( !walls[Point.second][Point.first].hasWallNorth ){
            if ( array[Point.second-1][Point.first] == -1 ){
                array[Point.second-1][Point.first] = array[Point.second][Point.first] + 1;
                enqueue(queue,Point.first,Point.second-1);
            }
       }

       if ( !walls[Point.second][Point.first].hasWallSouth ){
            if ( array[Point.second+1][Point.first] == -1 ){
                array[Point.second+1][Point.first] = array[Point.second][Point.first] + 1;
                enqueue(queue,Point.first,Point.second+1);
            }
       }

       if ( !walls[Point.second][Point.first].hasWallWest ){
            if ( array[Point.second][Point.first-1] == -1 ){
                array[Point.second][Point.first-1] = array[Point.second][Point.first] + 1;
                enqueue(queue,Point.first-1,Point.second);
            }
       }

       if ( !walls[Point.second][Point.first].hasWallEast ){
            if ( array[Point.second][Point.first+1] == -1 ){
                array[Point.second][Point.first+1] = array[Point.second][Point.first] + 1;
                enqueue(queue,Point.first+1,Point.second);
            }
       }
    }
    freeQueue(queue);
}


/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*                                                         Movement Software functions                                               */
/*-----------------------------------------------------------------------------------------------------------------------------------*/

//for movement steps
void getDelta(char dir) {

    switch (dir) {
        case 'u':
            deltaRow = -1;  // Moving up decreases the row index
            deltaColumn = 0;
            break;
        case 'd':
            deltaRow = 1;   // Moving down increases the row index
            deltaColumn = 0;
            break;
        case 'l':
            deltaColumn = -1; // Moving left decreases the column index
            deltaRow = 0;
            break;
        case 'r':
            deltaColumn = 1;  // Moving right increases the column index
            deltaRow = 0;
            break;
    }
}

void turnToDesiredDirection(char currentDirection, char desiredDirection) {
    // Directions in order: N -> E -> S -> W
    char directions[] = {'u', 'r', 'd', 'l'};
    short currentIndex, desiredIndex;

    // Find the index of the current and desired directions
    for (short i = 0; i < 4; i++) {
        if (directions[i] == currentDirection) {
            currentIndex = i;
        }
        if (directions[i] == desiredDirection) {
            desiredIndex = i;
        }
    }

    // Calculate the difference between the current and desired indices
    short diff = desiredIndex - currentIndex;

    // Normalize the difference to be within the range [-3, 3]
    if (diff < -2) {
        diff += 4;
    } else if (diff > 2) {
        diff -= 4;
    }

    // Turn to the desired direction
    if (diff == -1 || diff == 3) {
        API_turnLeft();
    } else if (diff == 1 || diff == -3) {
        API_turnRight();
    } else if (diff == 2 || diff == -2) {
        API_turnRight();
        API_turnRight();
    }
}



//!!!!!!!!!!!to add the functionality to turn towards lowest accessible manhattan distance






/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*                                                         Maze functions                                                            */
/*-----------------------------------------------------------------------------------------------------------------------------------*/


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!to add the inferred wall functionality!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void asssignSensedWalls(struct Cell array[][width], char dir, short row, short col, bool front, bool left, bool right){

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
        array[width-1][0].hasWallSouth = true; //for this corner case
        firstTimeUseonly = false;
    }

    //inferred wall functionality
    if(row>0)
        array[row-1][col].hasWallSouth = array[row][col].hasWallNorth;
    if(row<height-1)
        array[row+1][col].hasWallNorth = array[row][col].hasWallSouth;
    if(col>0)
        array[row][col-1].hasWallEast = array[row][col].hasWallWest;
    if(col<width-1)
        array[row][col+1].hasWallWest = array[row][col].hasWallEast;
}



/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*                                                         Simulator IO functions                                                    */
/*-----------------------------------------------------------------------------------------------------------------------------------*/

/*
void print2DArray(int arr[][width]) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            fprintf(stderr, "  %d  ", arr[i][j]);
            fflush(stderr);
        }
        fprintf(stderr,"\n");
        fflush(stderr);
    }
}*/


void logging(char* text) {
    fprintf(stderr, "%s\n", text);
    fflush(stderr);
}

void markWalls(short x, short y, bool NorthReading,bool WestReading,bool SouthReading,bool EastReading){

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

    for(short i  = 0; i<16;i++){
        for(short j = 0; j<16;j++){
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

void markManhattanForEntireMaze(short arr[][width]){

    for(short i  = 0; i<16;i++){
        for(short j = 0; j<16;j++){
            API_setText(j, 15-i,  arr[i][j]);        
        }
    }    
}


/*+++++++++++++++++++++++++++++====================++++++++++++++++++++++++++========================+++++++++++++++++++++++=====================++++++++++++++++++++++++================*/

void moveToPointFromPoint(short goalX, short goalY, struct Cell maze[][width]){
    short manhattan [height][width];
    resetManhattanForFloodfill(manhattan,height,width,goalX,goalY);
    floodfill(manhattan,maze,height,width,goalX,goalY);

    while(!((currentX == goalX) && (currentY == goalY))){           
            
            char desiredDirection = 'x';//x= null
            bool leftDetect = API_wallLeft();
            bool rightDetect = API_wallRight();
            bool frontDetect = API_wallFront();
            asssignSensedWalls(maze,direction,currentY,currentX,frontDetect,leftDetect,rightDetect);
            markWalls(currentX,currentY,maze[currentY][currentX].hasWallNorth,maze[currentY][currentX].hasWallWest,maze[currentY][currentX].hasWallSouth, maze[currentY][currentX].hasWallEast);
          
            if(currentY>0 && manhattan[currentY][currentX] > manhattan[currentY-1][currentX] && !maze[currentY][currentX].hasWallNorth)
                desiredDirection = 'u';
            else if (currentY<height-1 && manhattan[currentY][currentX] > manhattan[currentY+1][currentX] && !maze[currentY][currentX].hasWallSouth)
                desiredDirection = 'd';
            else if(currentX>0 && manhattan[currentY][currentX] > manhattan[currentY][currentX-1] && !maze[currentY][currentX].hasWallWest)
                desiredDirection = 'l';
            else if(currentX<width-1 && manhattan[currentY][currentX] > manhattan[currentY][currentX+1] && !maze[currentY][currentX].hasWallEast)
                desiredDirection = 'r';
    
            if(desiredDirection == 'x'){
                resetManhattanForFloodfill(manhattan,height,width,goalX,goalY);// Then: defining the center as manhattan goal
                floodfill(manhattan,maze,height,width,goalX,goalY);
                markManhattanForEntireMaze(manhattan);
                //logging("is floodfilling");
            }
            else{
                //logging("is turning");
                turnToDesiredDirection(direction,desiredDirection);
                direction = desiredDirection;
                API_moveForward();
                getDelta(direction);
                currentX = currentX + deltaColumn;
                currentY = currentY + deltaRow;
            }

            

            

        }

}



/*<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-*/
/*=============================================================================MAIN FN============================================================================*/
/*<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-*/



int main(int argc, char* argv[]) {

/*=====================================================================SETUP============================================*/

/*================================================================STATIC VALUES and INITIALIZATIONS====================================================================*/
    
 //initial facing up step
    
    //char directions[] = {'u','l','d','r'};
    //int deltaX[] = {0,-1,0,1};
    //int deltaY[] = {-1,0,1,0};
    //bool accessibility[] = {false,false,false,false};
    //short directionSense = 0; //initially facing up and the correct delta gets assigned automatically from same array index  

    //int manhattan [height][width];
    struct Cell maze[height][width];

    logging("Running...");
    API_setColor(0, 0, 'G');
    //API_setText(0, 0, "abc");

    resetWallsWithBorder(maze); //First step of video: Assume maze no walls. Defined borders
    
    

    
    //print2DArray(manhattan);
    markAllKnownWallsInBeginning(maze);

    moveToPointFromPoint(8,8,maze);

    logging("Reached at goal");
    logging("Returning back to starting point is to be implemented easily tho");

    moveToPointFromPoint(0,15,maze);
    logging("Reached at start");

    moveToPointFromPoint(8,8,maze);


}

