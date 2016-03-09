/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *  Author  : Cedric Francoys                                              *
 *  Date    : September 2011                                               *
 *                                                                         *
 ***************************************************************************/

class TColor{
		enum {RED, GREEN, BLUE};
		float vector[4];
	public :
		TColor() {}
		TColor(float r, float g, float b) {
			vector[RED] 	= r;
			vector[GREEN]	= g;
			vector[BLUE]	= b;
		}
		float* getVector() { return vector; }
		float getRed() 		{ return vector[RED]; }
		float getGreen() 	{ return vector[GREEN]; }
		float getBlue() 	{ return vector[BLUE]; }				
};

class Tetromino {
	protected:
		bool** matrix;
		TColor* color;
		int width;
		int height;
	public:
		Tetromino(int w, int h, TColor* c, bool* s) {
			width = w;
			height = h;
			color = c;

			matrix = new bool* [height];
			for(int j = 0; j < height; ++j) matrix[j] = new bool[width];

			for(int i = 0; i < 16; ++i) { 
				int x = i%4;
				int y = (int) i/4;
				if(y < height && x < width) matrix[y][x] = s[i];
			}
		}
		~Tetromino();
		int 	getWidth() 	{ return width; }
		int 	getHeight() { return height; }
		TColor* getColor() 	{ return color; }		
		bool** 	getMatrix() { return matrix;}
};

Tetromino::~Tetromino()  {
	for(int y = 0; y < height; ++y) delete[] matrix[y];
	delete[] matrix;
}

class TShape {
		int angle;
		int width, height;
		Tetromino* tetromino;
	public:
		enum {RIGHT, LEFT};
		TShape(Tetromino* t);		
		void rotate(bool dir);
		int getHeight() { return height; }
		int getWidth() 	{ return width; }
		int getAngle() { return angle; }
		TColor* getColor() { return tetromino->getColor(); }
		bool** getMatrix() { return tetromino->getMatrix(); }
		void getResultMatrix(bool**);
};


TShape::TShape(Tetromino* t) {
	tetromino = t;
	width = t->getWidth();
	height = t->getHeight();
	angle = 0;
}

void TShape::rotate(bool dir){
	// swap height and width
	int tmp = width; width = height; height = tmp;

	// update angle 
	if(dir == RIGHT) angle = (angle==0)?3:(angle-1);
	else angle = (angle+1)%4;
}

void TShape::getResultMatrix(bool** m){
	bool** matrix = getMatrix();
	static increments[4][2] = {{1,1}, {-1, 1}, {-1, -1}, {1, -1}};
	
	int inc_h = increments[angle][0];
	int inc_w = increments[angle][1];
	
	for(int j = (inc_h>0)?0:height-1, y = 0; j < height && j >= 0; j += inc_h, ++y) {
		for(int i = (inc_w>0)?0:width-1, x = 0; i < width && i >= 0; i += inc_w, ++x) {
			if( ((angle == 0 || angle == 2) && matrix[j][i]) || ((angle == 1 || angle == 3) && matrix[i][j]) )	m[y][x] = 1;
			else m[y][x] = 0;
		}
	}
}

void drawShape(int x, int y, int w, int h, TColor* c, bool** s_matrix, bool shade=false) {
	for(int j = 0; j < h; ++j) {
		for(int i = 0; i < w; ++i) {
			if (s_matrix[j][i])	{
				float red 	= c->getRed();
				float green = c->getGreen();
				float blue 	= c->getBlue();
				if(shade) {
					red		= (red-0.5 > 0)?red-0.5:0.0f;
					green	= (green-0.5 > 0)?green-0.5:0.0f;
					blue	= (blue-0.5 > 0)?blue-0.5:0.0f;				
				}
				//glColor3fv(c->getVector());
				glColor3f(red, green, blue);
				glRecti(x+(i*15), y-(j*15), x+((i+1)*15), y-((j+1)*15));
			}
		}
	}
}

void drawString(int x, int y, char *s, void* charset = GLUT_BITMAP_8_BY_13, int y_offset = 15){
	glRasterPos2i(x, y);
	for ( int i = 0; i < strlen(s); i++){
		if ( s[i] == '\n' ){
			y -= y_offset;
			glRasterPos2i(x, y);
		}
		else glutBitmapCharacter(charset, s[i]);
   }
}


TColor* color_white		= new TColor(1.0F,1.0F,1.0F);
TColor* color_black		= new TColor(0.0F,0.0F,0.0F);
TColor* color_red		= new TColor(1.0F,0.0F,0.0F);
TColor* color_blue		= new TColor(0.0F,0.0F,1.0F);
TColor* color_green		= new TColor(0.0F,1.0F,0.0F);


// SEGA arcade color scheme
TColor* colors[7]		= 	{	
								new TColor(0.988F,0.008F,0.016F),		// red
								new TColor(0.988F,0.65F,0.016F),		// orange
								new TColor(0.016F,0.008F,0.988F),		// blue								
								new TColor(0.988F,0.996F,0.016F),		// yellow
								new TColor(0.016F,0.996F,0.016F),		// green
								new TColor(0.988F,0.008F,0.988F),		// magenta								
								new TColor(0.016F,0.996F,0.988F)		// cyan								
							};

bool shapes[7][16] = 	{
							{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},	// I
							{0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0}, 	// J
							{1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0}, 	// L
							{1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 	// O
							{0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 	// S							
							{1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	// T 
							{1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}	// Z 							
						};
				

Tetromino* tetrominos[7] = {
								new Tetromino(1, 4, colors[0], shapes[0]),		// I
								new Tetromino(2, 3, colors[1], shapes[1]),		// J
								new Tetromino(2, 3, colors[2], shapes[2]),		// L
								new Tetromino(2, 2, colors[3], shapes[3]),		// O
								new Tetromino(3, 2, colors[4], shapes[4]),		// S
								new Tetromino(3, 2, colors[5], shapes[5]),		// T
								new Tetromino(3, 2, colors[6], shapes[6])		// Z
							};