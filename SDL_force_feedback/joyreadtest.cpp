//Using SDL, SDL_image, SDL_haptic, standard IO, math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_haptic.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

Sint16 x_move, y_move;

void ffb (int, int);

//Game Controller handler
SDL_Joystick* gGameController = NULL;

//Analog joystick dead zone
const int JOYSTICK_DEAD_ZONE = 8000;

long int xval = 0, yval = 0;
float fx = 0, fy = 0, fmag = 0, fdir = 0, ang = 0;

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
	    SDL_Event e;
    	//Main loop flag
		bool quit = false;

		//While application is running
		while( !quit )
		{
			//Handle events on queue
			while( SDL_PollEvent( &e ) != 0 )
			{
			    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
				//User requests quit
				if( e.type == SDL_QUIT )
				{
					quit = true;
				}
				else if( e.type == SDL_JOYAXISMOTION )
				{
					x_move = SDL_JoystickGetAxis(gGameController, 0);
                    y_move = SDL_JoystickGetAxis(gGameController, 1);

                    if(x_move < (-JOYSTICK_DEAD_ZONE))
                        xDir= -1;
                    else if(x_move > JOYSTICK_DEAD_ZONE)
                        xDir= 1;
                    else
                        xDir=0;

                    if(y_move < (-JOYSTICK_DEAD_ZONE))
                        yDir= 1;
                    else if(y_move > JOYSTICK_DEAD_ZONE)
                        yDir= -1;
                    else
                        yDir=0;
                        //print function
                        if ((xDir!=0)||(yDir!=0))
                        {
                           ffb(x_move,y_move);
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
    fmag = sqrt(fx*fx + fy*fy);

    //To calculate the angle along which the resultant acts
    ang = ( 180 * atan(fy / fx) / 3.1415 ) + 180 ;
    if ((xDir == 1) && (yDir == 1) )
        fdir = ang;
    else if ((xDir == -1) && (yDir == 1) )
        fdir = ang + 180.0;
    else if ((xDir == -1) && (yDir == -1) )
        fdir = ang + 180.0;
    else if ((xDir == 1) && (yDir == -1) )
        fdir = ang + 360.0;
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

    fdir *= 100;
    fmag *= 2300;

    //Prints the resultant magnitude and direction
    printf("%d",xv);
    printf("\t");
    printf("%d",yv);
    printf("\n");

//   	effect.constant.direction.dir[0] = fdir;    // Direction from which the force comes
 //   effect.constant.length = 100;
   // effect.constant.level = fmag;

    //uploading effect
  //  effect_id = SDL_HapticNewEffect( haptic, &effect );

    //run the effect
    //SDL_HapticRunEffect( haptic, effect_id, 1 );
    SDL_Delay(100); // Wait for the effect to finish

//*/
}



