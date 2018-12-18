
//Using SDL, SDL_haptic, standard IO, math, and strings
#include <SDL.h>
#include <SDL_haptic.h>
#include <stdio.h>
#include <string>
#include <cmath>

static SDL_Haptic *haptic;

SDL_HapticEffect effect;
int effect_id;

void ffb (int, int);

//Game Controller handler
SDL_Joystick* gGameController = NULL;

//Analog joystick dead zone
const int X_JOYSTICK_DEAD_ZONE = 2000, Y_JOYSTICK_DEAD_ZONE= 2000;

//variables initialization
long int xval = 0, yval = 0, x_p, x_d, y_p, y_d;
int tim_dur = 20, fmag = 0, fdir = 0, ang = 0, xDir = 0, yDir = 0;
float fx = 0, fy = 0, x, y, x_s=0, y_s=0;

//Spring constant of the joystick model
float kx, ky, kx_d, ky_d, kx_i, ky_i; // 1 to 23000

//Starts up SDL
bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER | SDL_INIT_HAPTIC) < 0 )
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
			//Create haptic device
			haptic = SDL_HapticOpenFromJoystick(gGameController);
			if (haptic == NULL)
			{
				printf("Unable to create the haptic device: %s\n", SDL_GetError() );
				success = false;
			}
			else if ((SDL_HapticQuery(haptic) & SDL_HAPTIC_CONSTANT)==0)
            {
                printf( "Warning: Unable to initialize constant! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
		}
	}
	return success;
}

void close()
{
	//Close game controller
	SDL_JoystickClose( gGameController );
	SDL_HapticClose(haptic);
	gGameController = NULL;
	haptic = NULL;
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
    	//creating the effect
    	SDL_memset( &effect, 0, sizeof(SDL_HapticEffect) );
    	effect.type = SDL_HAPTIC_CONSTANT;
        effect.constant.direction.type = SDL_HAPTIC_POLAR;

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
				if( e.type == SDL_QUIT )
				{
					quit = true;
				}
				else if( e.type == SDL_JOYAXISMOTION )
				{
					xval = SDL_JoystickGetAxis(gGameController, 0);
                    yval = SDL_JoystickGetAxis(gGameController, 1);

                    //Motion along X-axis
                    if(xval < (-X_JOYSTICK_DEAD_ZONE))
                        xDir= -1;
                    else if(xval > X_JOYSTICK_DEAD_ZONE)
                        xDir= 1;
                    else
                        xDir=0;

                    //Motion along Y-axis
                    if(yval < (-Y_JOYSTICK_DEAD_ZONE))
                        yDir= 1;
                    else if(yval > Y_JOYSTICK_DEAD_ZONE)
                        yDir= -1;
                    else
                        yDir=0;

                    ffb(xval,yval); //force feedback function
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
    //spring constants
    kx = 20000; //20000
    //if (xDir ==0)
    //    kx = 50000;
    if(yDir == 1)
        ky = 22000; //20000
    else if (yDir == -1)
        ky = 20000;
    //if (yDir ==0)
    //    ky = 50000;
    kx_d = 30000;   //30000
    ky_d = 500000;  //500000
    kx_i = 150; //150
    ky_i = 100;  //50

    //time duration
    tim_dur = 20;   //20

    //TO map -32786 to +32767 to -1 to 1
    x = xv/32768.0;
    y = -1*yv/32768.0;

    x_d = (x - x_p);
    y_d = (y - y_p);
    x_s = x_s + x;
    y_s = y_s + y;
    if (xDir == 0)
        x_s = 0;
    if (yDir == 0)
        y_s = 0;
    //if (yDir == 1)
    //..666666666666666666666666666666666666666666666666666666666666666666    ky+= 5000;
    //Desired spring force along X and Y directions
    fx = -kx * x - kx_d*x_d - kx_i*x_s;
    fy = -ky * y - ky_d*y_d - ky_i*y_s;

    if (xDir == 0)
        fx = 0;
    if (yDir==0)
        fy = -10000; //-10550

    //Magnitude of resultant
    fmag = int(sqrt(fx*fx + fy*fy));
    if (fmag>32767)
        fmag=32767;

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
    else if ((xDir == 0) && (yDir == 0))
        fdir = 0;
    if (fdir>=360)
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
    if (fdir>=360)
        fdir -= 360;
    fdir *= 100;

    //Prints the resultant magnitude and direction
    printf("%d",fmag);
    printf("\t");
    printf("%d",fdir);
    printf("\t");
    printf("%d",xv);
    printf("\t");
    printf("%d",yv);
    printf("\n");

   	effect.constant.direction.dir[0] = fdir;    // Direction from which the force comes
    effect.constant.length = tim_dur;
    effect.constant.level = fmag;
    effect.periodic.attack_length = (tim_dur/4); // Takes 1 second to get max strength
    effect.periodic.fade_length = (tim_dur/4); // Takes 1 second to fade away
    effect_id = SDL_HapticNewEffect( haptic, &effect ); //uploading effect
    SDL_HapticRunEffect( haptic, effect_id, 1 );    //run the effect
    SDL_Delay(tim_dur); // Wait for the effect to finish
    SDL_HapticDestroyEffect(haptic, effect_id);

    x_p = x;
    y_p = y;
}
