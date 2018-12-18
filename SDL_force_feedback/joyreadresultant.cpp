
//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL.h>
#include <stdio.h>
#include <string>
#include <cmath>

void ffb (int, int);

//Game Controller handler
SDL_Joystick* gGameController = NULL;

//Analog joystick dead zone
const int JOYSTICK_DEAD_ZONE = 8000;

long int xval = 0, yval = 0;
int fmag = 0, fdir = 0;
float fx = 0, fy = 0, ang = 0;

//Spring constant of the joystick model
float kx = 10, ky = 10;

//Normalized direction
int xDir = 0;
int yDir = 0;


//Starts up SDL and creates window
bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Check for joysticks
		if( SDL_NumJoysticks() < 1 )
		{
			printf( "Warning: No joysticks connected!\n" );
		}
		else
		{
			//Load joystick
			gGameController = SDL_JoystickOpen( 0 );
			if( gGameController == NULL )
			{
				printf( "Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError() );
			}
		}
	}
	return success;
}

void close()
{
	//Close game controller
	SDL_JoystickClose( gGameController );
	gGameController = NULL;
    SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
        //Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;

				//While application is running
		while( !quit )
		{
			//Handle events on queue
			while( SDL_PollEvent( &e ) != 0 )
			{
				//User requests quit
				if( e.type == SDL_QUIT )
				{
					quit = true;
				}
				else if( e.type == SDL_JOYAXISMOTION )
				{
					//Motion on controller 0
					if( e.jaxis.which == 0 )
					{
						//X axis motion
						if( e.jaxis.axis == 0 )
						{
							if( e.jaxis.value < -JOYSTICK_DEAD_ZONE )
							{
								xDir =  -1;
                                xval = e.jaxis.value;
							}
							//Right of dead zone
							else if( e.jaxis.value > JOYSTICK_DEAD_ZONE )
							{
								xDir =  1;
								xval = e.jaxis.value;
							}
							else
							{
								xDir = 0;
								xval = 0;
							}
						}

						//Y axis motion
                        if( e.jaxis.axis == 1 )
						{
							//Below of dead zone
							if( e.jaxis.value < -JOYSTICK_DEAD_ZONE )
							{
								yDir = 1;
								yval = -1 * e.jaxis.value;
                            }
							//Above of dead zone
							else if( e.jaxis.value > JOYSTICK_DEAD_ZONE )
							{
								yDir = -1;
								yval = -1 * e.jaxis.value;
							}
							else
							{
								yDir = 0;
								yval = 0;
							}
						}

                        //print function
                        if ((xDir!=0)||(yDir!=0))
                        {
                           ffb(xval,yval);
                        }
                    }
                }


            }
        }
	}

	//Free resources and close SDL
	close();
	return 0;
}

void ffb( int xv, int yv)
{
    float x,y;

    //TO map -32786 to +32767 to -1 to 1
    x = xv/32768.0;
    y = yv/32768.0;

    //Desired spring force along X and Y directions
    fx = -1 * kx * x;
    fy = -1 * ky * y;

    //Magnitude of resultant
    fmag = int(sqrt(fx*fx + fy*fy));

    //To calculate the angle along which the resultant acts
    ang = int( 180 * atan(fy / fx) / 3.1415 ) + 180 ;
    if ((xDir == 1) && (yDir == 1) )
        fdir = ang;
    else if ((xDir == -1) && (yDir == 1) )
        fdir = ang + 180;
    else if ((xDir == -1) && (yDir == -1) )
        fdir = ang + 180;
    else if ((xDir == 1) && (yDir == -1) )
        fdir = ang + 360;
    else if ((xDir == 0) && (yDir == 1))
        fdir = 0;
    else if ((xDir == 0) && (yDir == -1))
        fdir = 180;
    else if ((xDir == 1) && (yDir == 0))
        fdir = 90;
    else if ((xDir == -1) && (yDir == 0))
        fdir = 270;

    if (fdir>360)
        fdir -= 360;

    //Converting fdir to SDL polar
    if ((xDir == 1) && (yDir == 1) )
        fdir = 270 - fdir;
    else if ((xDir == -1) && (yDir == 1) )
        fdir = 630 - fdir;
    else if ((xDir == -1) && (yDir == -1) )
        fdir = 270 - fdir;
    else if ((xDir == 1) && (yDir == -1) )
        fdir = 270 - fdir;
    //Prints the resultant magnitude and direction
    printf("%d",fmag);
    printf("\t");
    printf("%d",fdir);
    printf("\n");
}
