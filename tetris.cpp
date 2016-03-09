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


#include <sys\timeb.h>
#include <stdio.h>
#include <windows.h>

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "tetris_utils.cpp"

/* app vars */
#define MATRIX_WIDTH	10
#define MATRIX_HEIGHT	22


/* game vars */
enum game_states {PAUSE, NEW, RUNNING, NEXT_LEVEL, GAME_OVER};

struct TData {
	int shape_x;
	int shape_y;
	int shape_height;
	int shape_width;
	TShape* shape;
	bool** shape_matrix;
	int previous_shape, current_shape, next_shape;
	long timer, ticker;
	int level;
	int score;
	int lines_count;
	int level_lines_count;
	int state;
	long time_init;
	int matrix[MATRIX_HEIGHT][MATRIX_WIDTH];
} game_data;

void update_shape_matrix() {
	int k;

	if(game_data.shape_matrix != NULL) {
		for(k = 0; k < game_data.shape_height; ++k) delete[] game_data.shape_matrix[k];
		delete[] game_data.shape_matrix;
		game_data.shape_matrix = NULL;
	}

	game_data.shape_height = game_data.shape->getHeight();
	game_data.shape_width = game_data.shape->getWidth();
	
	game_data.shape_matrix = new bool*[game_data.shape_height];	
	for(k = 0; k < game_data.shape_height; ++k) game_data.shape_matrix[k] = new bool[game_data.shape_width];
	
	game_data.shape->getResultMatrix(game_data.shape_matrix);
}

void next_shape() {
	randomize();
    game_data.shape_x = 4;
	game_data.shape_y = 0;
	game_data.previous_shape = game_data.current_shape;	
	game_data.current_shape = game_data.next_shape;
	while (true) {
		game_data.next_shape = rand() % 7;
		if(game_data.next_shape != game_data.current_shape || game_data.next_shape != game_data.previous_shape) break;
	}
	if(game_data.shape) delete game_data.shape;
	game_data.shape = new TShape(tetrominos[game_data.current_shape]);
	for(int i = 0; i < rand() % 3; ++i) game_data.shape->rotate(TShape::LEFT);
	update_shape_matrix();
}

void next_level() {
	game_data.level ++;
	game_data.ticker = 0;	
	game_data.level_lines_count = 0;
	for(int x = 0; x < MATRIX_WIDTH; ++x) {
		for(int y = 0; y < MATRIX_HEIGHT; ++y) {
			game_data.matrix[y][x] = -1;
		}
	}
	next_shape();	
}

void update_time_init() {
	struct timeb t_init;	
	ftime(&t_init);
	game_data.time_init = t_init.time*1000 + t_init.millitm;	
}

void init_game() {
	game_data.state = NEW;
	game_data.timer = 0;
	game_data.ticker = 0;	
	game_data.level = 0;
	game_data.score = 0;
	game_data.lines_count = 0;
	game_data.next_shape = rand() % 7;
	game_data.shape = NULL;
	game_data.shape_matrix = NULL;
	
	next_level();
	update_time_init();
}


void reshape(int w, int h){
	if(h == 0) h = 1;
	if(w == 0) w = 1;
	
	/* transformation de cadrage */
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(0.0, 640.0, 0.0, 640.0*h/w);	

	/* chargement de la matrice de modelisation */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void display(void) {
	char txt[1000];
	
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);

	glLoadIdentity(); 

	// score
	sprintf(txt, "Score: %d\n", game_data.score);
	glColor3fv(color_white->getVector());
	drawString(50, 225, txt, GLUT_BITMAP_HELVETICA_18);
	
	// time
	sprintf(txt, "Time: %04.3f\n", ((double) game_data.timer)/1000);
	glColor3fv(color_white->getVector());
	drawString(50, 210, txt, GLUT_BITMAP_HELVETICA_18);

	// total lines 
	sprintf(txt, "Lines: %d\n", game_data.lines_count);
	glColor3fv(color_white->getVector());
	drawString(50, 195, txt, GLUT_BITMAP_HELVETICA_18);

	// level
	sprintf(txt, "Level: %d\n", game_data.level);
	glColor3fv(color_white->getVector());
	drawString(50, 180, txt, GLUT_BITMAP_HELVETICA_18);

	// Copy notice
	glColor3fv(color_white->getVector());
	drawString(50, 10, "7etris by Ced - september 2011 (CopyLeft)", GLUT_BITMAP_HELVETICA_10);

	// instructions
	glColor3fv(color_white->getVector());
	drawString(50, 140, "'A' key : rotate left\n'Q' key : rotate right\nUse direction arrows to move left, right, and down\nClick mouse right button for menu", GLUT_BITMAP_HELVETICA_12, 10);
	
	// matrix border
	glColor3fv(color_blue->getVector());
	glRecti(244, 4, 246+(15*MATRIX_WIDTH), 6+(15*MATRIX_HEIGHT));
	glColor3fv(color_black->getVector());
	glRecti(245, 5, 245+(15*MATRIX_WIDTH), 5+(15*MATRIX_HEIGHT));
	
	// matrix content
	for(int y = MATRIX_HEIGHT-1; y >= 0; --y) {
		for(int x = 0; x < MATRIX_WIDTH; ++x) {
			if(game_data.matrix[y][x] >= 0) {
				float red = colors[game_data.matrix[y][x]]->getRed();
				float green = colors[game_data.matrix[y][x]]->getGreen();
				float blue = colors[game_data.matrix[y][x]]->getBlue();
				if(game_data.state == NEXT_LEVEL || game_data.state == GAME_OVER) {
					red = (red-0.5 > 0)?red-0.5:0.0f;
					green = (green-0.5 > 0)?green-0.5:0.0f;
					blue = (blue-0.5 > 0)?blue-0.5:0.0f;				
				}
				glColor3f(red, green, blue);
				glRecti(245+(x*15), 335-(y*15), 245+(x+1)*15, 335-(y+1)*15);
			}
		}
	}

	if(game_data.state == RUNNING) {	
		// current shape
		int pos_x, pos_y;
		pos_x = 245+(game_data.shape_x*15);
		pos_y = 335-(game_data.shape_y*15);
		drawShape(pos_x, pos_y, game_data.shape_width, game_data.shape_height, game_data.shape->getColor(), game_data.shape_matrix);
		
		// next shape preview
		drawShape(50, 300 , tetrominos[game_data.next_shape]->getWidth(), tetrominos[game_data.next_shape]->getHeight(), colors[game_data.next_shape], tetrominos[game_data.next_shape]->getMatrix());
	} 
	
	if(game_data.state == PAUSE) {
		glColor3fv(color_black->getVector());
		glRecti(245, 335, 395, 5);
		glColor3fv(color_white->getVector());
		drawString(300, 180, "P A U S E", GLUT_BITMAP_HELVETICA_18);
	}

	if(game_data.state == NEXT_LEVEL) {
		glColor3fv(color_white->getVector());
		drawString(295, 180, "NEXT LEVEL", GLUT_BITMAP_HELVETICA_18);
		drawString(283, 170, "press space bar when ready", GLUT_BITMAP_HELVETICA_12);		
	}
	
	if(game_data.state == NEW) {
		glColor3fv(color_white->getVector());
		drawString(295, 180, "NEW GAME", GLUT_BITMAP_HELVETICA_18);
		drawString(283, 170, "press space bar when ready", GLUT_BITMAP_HELVETICA_12);		
	}

	if(game_data.state == GAME_OVER) {
		glColor3fv(color_white->getVector());
		drawString(290, 180, "G A M E  O V E R", GLUT_BITMAP_HELVETICA_18);
	}
	
	glutSwapBuffers();
}

bool move_shape_down() {
	bool collide = false;
	if((game_data.shape_y + game_data.shape_height)< 22) {
		for(int y = 0; y < game_data.shape_height; ++y) {
			for(int x = 0; x < game_data.shape_width; ++x) {
				if(game_data.shape_matrix[y][x] == 1 && game_data.matrix[game_data.shape_y+y+1][game_data.shape_x+x] >= 0) {
					collide = true;
					break;
				}
			}
		}
		if(!collide) game_data.shape_y++;
	}
	else collide = true;
	return collide;
}

void interact(int key, int, int){
	bool collide = false;
  	if (key == ' ') { 
		if(game_data.state == RUNNING) game_data.state = PAUSE; 
		else {
			if(game_data.state == NEXT_LEVEL) next_level();
			game_data.state = RUNNING;
			update_time_init();
		}
	}
	
	if(game_data.state == RUNNING) {

		switch (key) {
			case GLUT_KEY_UP  : 
			case 'a'	:
			case 'A'	:
					// test de distance du bord droit
					if(game_data.shape_x + game_data.shape_height <= 10) {
						// test de collision
						game_data.shape->rotate(TShape::LEFT);
						update_shape_matrix();
						for(int y = 0; y < game_data.shape_height; ++y) {
							for(int x = 0; x < game_data.shape_width; ++x) {
								if(game_data.shape_matrix[y][x] == 1 && game_data.matrix[game_data.shape_y+y][game_data.shape_x+x] >= 0) {
									collide = true;
									break;
								}
							}
						}
						if(collide) {
							// rollback					
							game_data.shape->rotate(TShape::RIGHT);
							update_shape_matrix();
						}
					}
					break;
			case 'q'	:
			case 'Q'	:
					// test de distance du bord droit
					if(game_data.shape_x + game_data.shape_height <= 10) {				
						// test de collision
						game_data.shape->rotate(TShape::RIGHT);
						update_shape_matrix();
						for(int y = 0; y < game_data.shape_height; ++y) {
							for(int x = 0; x < game_data.shape_width; ++x) {
								if(game_data.shape_matrix[y][x] == 1 && game_data.matrix[game_data.shape_y+y][game_data.shape_x+x] >= 0) {
									collide = true;
									break;
								}
							}
						}
						if(collide) {					
							// rollback
							game_data.shape->rotate(TShape::LEFT);
							update_shape_matrix();
						}			
					}
					break;
			case GLUT_KEY_DOWN      : 
// todo : bonuses
					move_shape_down();
					break;
			case GLUT_KEY_LEFT      : 
					if(game_data.shape_x > 0) {
						// test de collision
						for(int y = 0; y < game_data.shape_height; ++y) {
							for(int x = 0; x < game_data.shape_width; ++x) {
								if(game_data.shape_matrix[y][x] == 1 && game_data.matrix[game_data.shape_y+y][game_data.shape_x+x-1] >= 0) {
									collide = true;
									break;
								}
							}
						}
						if(!collide) game_data.shape_x --;
					}
					break;
			case GLUT_KEY_RIGHT     :
					if(game_data.shape_x + game_data.shape_width < 10) {
						// test de collision				
						for(int y = 0; y < game_data.shape_height; ++y) {
							for(int x = 0; x < game_data.shape_width; ++x) {
								if(game_data.shape_matrix[y][x] == 1 && game_data.matrix[game_data.shape_y+y][game_data.shape_x+x+1] >= 0) {
									collide = true;
									break;
								}
							}
						}
						if(!collide) game_data.shape_x ++;
					}
					break;
			case GLUT_KEY_PAGE_DOWN :
					break;
			case GLUT_KEY_PAGE_UP   :
					break;
		}
	}
	
	glutPostRedisplay();
}

void interact(unsigned char key, int, int){ interact((int) key, 0, 0); }


void gameLoop(int value){
	
	if(game_data.state == RUNNING) {
		bool bottom_reached = false;
		int x, y;
		game_data.ticker ++;

		int throttle = 31-(game_data.level*2);

		if(game_data.ticker%throttle == 0 ) {
			bottom_reached = move_shape_down();
		}
		
		//update timer
		if(game_data.ticker%10 == 0 ) {
			long   time_now;
			struct timeb t_now;
			ftime(&t_now);
			time_now = t_now.time*1000 + t_now.millitm;
			game_data.timer += time_now-game_data.time_init;	
			game_data.time_init = time_now;
		}
		
		if(bottom_reached) {
			if(game_data.shape_y == 0) {
				game_data.state = GAME_OVER;
			}
			else {
				// add current shape to the matrix
				for(y = 0; y < game_data.shape_height; ++y) {
					for(x = 0; x < game_data.shape_width; ++x) {
						if(game_data.shape_matrix[y][x])
							game_data.matrix[game_data.shape_y+y][game_data.shape_x+x] = game_data.current_shape;
					}
				}
				
				// check for lines
				int line_count = 0;
				
				for(y = 0; y < MATRIX_HEIGHT; ++y) {
					bool line_complete = true;
					for(x = 0; x < MATRIX_WIDTH; ++x) {
						if(game_data.matrix[y][x] == -1) {
							line_complete = false;
							break;
						}
					}
					if(line_complete) {
						++line_count;
						// drop lines above
						for(int y1 = y; y1 > 0; --y1) {
							for(int x1 = 0; x1 < MATRIX_WIDTH; ++x1) {
								game_data.matrix[y1][x1] = game_data.matrix[y1-1][x1];
							}
						}
					}
				}
				game_data.lines_count += line_count;
				game_data.level_lines_count += line_count;
				
				// update score;
				switch(line_count) {
					case 1:
						game_data.score += 40*game_data.level;
						break;
					case 2:
						game_data.score += 100*game_data.level;
						break;
					case 3:
						game_data.score += 300*game_data.level;
						break;
					case 4:
						game_data.score += 1200*game_data.level;
						break;
				}
				
				// check for level end
				if(line_count > 0 && game_data.lines_count > 0 && game_data.level_lines_count >= 10) {
					game_data.state = NEXT_LEVEL;
				}
				else {
					// show next shape
					next_shape();
				}
			}
		}
		
		glutPostRedisplay();
	}
	glutTimerFunc(10, gameLoop, 0);
}

static void select(int selection) {
  switch (selection) {
    case 0  :
    		init_game();
            break;
    case 1  :
    		exit(0);
            break;
    case 2  :
            break;			
  }
}

 
int OwlMain(int argc, char* argv[]){
	/* initialisation GLUT */
	glutInit(&argc, argv);

	// RGB + transparent & double buffering
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(0, 0);

	/* creation de la fenetre GLUT */
	glutCreateWindow("Tetris");
	glutSetIconTitle("icone");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(interact);
	glutSpecialFunc(interact);
	
	glutCreateMenu(select);
	glutAddMenuEntry("New game", 0);
	glutAddMenuEntry("Quit", 1);	
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init_game();
	glutTimerFunc(10, gameLoop, 0);

	glutFullScreen();	
	glutMainLoop();	// demmarrage de la boucle principale

	return 0;
}