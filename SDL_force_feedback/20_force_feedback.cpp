/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, SDL_ttf, standard IO, math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>

//Starts up SDL
bool init();

//Frees media and shuts down SDL
void close();

//Game Controller 1 handler with force feedback
SDL_Joystick* gGameController = NULL;
SDL_Haptic* gControllerHaptic = NULL;

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC ) < 0 )
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
			else
			{
				//Get controller haptic device
				gControllerHaptic = SDL_HapticOpenFromJoystick( gGameController );
				if( gControllerHaptic == NULL )
				{
					printf( "Warning: Controller does not support haptics! SDL Error: %s\n", SDL_GetError() );
				}
				else
				{
					//Get initialize sine
					if ((SDL_HapticQuery(gControllerHaptic) & SDL_HAPTIC_CONSTANT)==0)
                    {
                        printf( "Warning: Unable to initialize sine! SDL Error: %s\n", SDL_GetError() );
                    }
				}
			}
		}
    }

    return success;
}

void close()
{
	//Close game controller with haptics
	SDL_HapticClose( gControllerHaptic );
	SDL_JoystickClose( gGameController );
	gGameController = NULL;
	gControllerHaptic = NULL;

	SDL_Quit();
}

int main( int argc, char* args[] )
{
 	//Start up SDL and create window
    SDL_HapticEffect effect;
    int effect_id;
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

		 // Create the effect
        SDL_memset( &effect, 0, sizeof(SDL_HapticEffect) ); // 0 is safe default
        effect.type = SDL_HAPTIC_SINE;
        effect.periodic.direction.type = SDL_HAPTIC_POLAR; // Polar coordinates
        effect.periodic.direction.dir[0] = 18000; // Force comes from south
        effect.periodic.period = 1000; // 1000 ms
        effect.periodic.magnitude = 20000; // 20000/32767 strength
        effect.periodic.length = 5000; // 5 seconds long
        effect.periodic.attack_length = 1000; // Takes 1 second to get max strength
        effect.periodic.fade_length = 1000; // Takes 1 second to fade away

        // Upload the effect
        effect_id = SDL_HapticNewEffect( gControllerHaptic, &effect );

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
                //Joystick button press
                else if( e.type == SDL_JOYBUTTONDOWN )
                {
                    //Play sine
                    SDL_HapticRunEffect( gControllerHaptic, effect_id, 1 );
                    SDL_Delay( 5000); // Wait for the effect to finish
                }
            }
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
