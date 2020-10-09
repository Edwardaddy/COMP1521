for(int i = 0; i < n; i++) {
    for (int j = 0; i < n; j++){
        if (a[j] < a[j+1]) {
            int temp = a[j];
            char temp[100];
            strcopy (temp,source);
            a[j] = a[j+1];
            a[j+1]= temp;
        }
    }
}




// COMP1521 Final Exam
// Read points and determine bounding box

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

// Data type definitions

// all values are in the range 0..255
typedef unsigned char Byte;

// an (x,y) coordinate
typedef struct {
	Byte x;
	Byte y;
} Coord;

// a colour, given as 3 bytes (r,g,b)
typedef struct {
	Byte r;
	Byte g;
	Byte b;
} Color;

// a Point has a location and a colour
typedef struct {
	Coord coord;  // (x,y) location of Point
	Color color;  // colour of Point
} Point;

void boundingBox(int, Coord *, Coord *);

int main(int argc, char **argv)
{
	// check command-line arguments
	if (argc < 2) {
		fprintf(stderr, "Usage: %s PointsFile\n", argv[0]);
		exit(1);
	}

	// attempt to open specified file
	int in = open(argv[1],O_RDONLY);
	if (in < 0) {
		fprintf(stderr, "Can't read %s\n", argv[1]);
		exit(1);
	}

	// collect coordinates for bounding box
	Coord topLeft, bottomRight;
	boundingBox(in, &topLeft, &bottomRight);

	printf("TL=(%d,%d)  BR=(%d,%d)\n",
		 topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);

	// clean up
	close(in);
	return 0;
}

void boundingBox(int in, Coord *TL, Coord *BR)
{
    Byte minX, minY, maxX, maxY;
    minY = minX = 0;
    maxX = maxY = 255;
    Point P;
    int psize = sizeof(Point);
    while(read(in, &P, psize) > 0) {
        if (P.coord.x > maxX) maxX = P.coord.x;
        if (P.coord.x < minX) minX = P.coord.x;
        if (P.coord.y > maxY) maxX = P.coord.y;
        if (P.coord.y < minY) minX = P.coord.y;
    }
    TL->x = minX;
    TL->y = maxY;
    BR->x = maxX;
    BR->y = minY;
	
}


sem_t study_room;

// open study room
sem_init (study_room, 10);

//study goto library, get a study room
sem_wait (study_room);

//study for a while
sem_signal(study_room);
