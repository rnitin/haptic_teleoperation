// *****   Forward and right force feedback  *****

//Using SDL, SDL_haptic, standard IO, math, strings, standard library, windows
#include <SDL.h>
#include <SDL_haptic.h>
#include <SDL_thread.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <windows.h>

static SDL_Haptic *haptic;
SDL_HapticEffect effect;
int effect_id;

void ffb (int, int);

//Game Controller handler
SDL_Joystick* gGameController = NULL;

//Analog joystick dead zone
const int X_JOYSTICK_DEAD_ZONE = 0, Y_JOYSTICK_DEAD_ZONE= 0;

//variables initialization
long int xval = 0, yval = 0, x_p, x_d, y_p, y_d;
int tim_dur = 20;   //duration of application of force
int fmag = 0, fdir = 0, ang = 0, xDir = 0, yDir = 0, xDirPrev = 0, yDirPrev = 0, ignore = 0;
float fx = 0, fy = 0, fxn = 0, fyn = 0, ffx1 = 0, ffy1 = 0, ffx2 = 0, ffy2 = 0, x, y;

//Spring constant of the joystick model
float kx = 50000, ky=50000, kx_d, ky_d, kx_i, ky_i; // 1 to 23000

//-------------------------------------------------------------------

//-------------------------------------------------------------------

//Serial communication declarations
const int n = 8; //NO OF BYTES TO BE READ
char szBuff [n+1] = {0};
DWORD dwBytesRead = 0;
long read_result = 0;
int value = 0, res = 0, d1 = 0, d2 = 0, d3 = 0, d4 = 0, d_prev[4]={0,0,0,0}, v = 0;  //d1: forward, d2: right, d3: backward, d4: left

//-------------Median Filter-----------------------------
#define swap(a,b) a^=b; b^=a; a^=b;
#define sort(a,b) if(a>b) {swap(a,b); }

int prev_readings[2][5] = {0,0,0,0,0,0,0,0,0,0}, nReadings = 0;

int median (int a, int b, int c, int d, int e)
{
    sort(a,b);
    sort(d,e);
    sort(a,c);
    sort(b,c);
    sort(a,d);
    sort(c,d);
    sort(b,e);
    sort(b,c);

    return c;
}
//---------------------------------------------------------


//Create HANDLE
HANDLE hSerial = CreateFile("COM3",
                            GENERIC_READ | GENERIC_WRITE,
                            0,
                            0,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            0
                            );

//Arduino read function
int serialRead( void* data )
{
    while(1)
    {
        if(!ReadFile(hSerial, szBuff, n, &dwBytesRead, NULL))
        {
            printf("Read error");
            exit(1);
        }
        else
        {
            nReadings++;
            res = atoi(szBuff);
            v = res / 1000000;
            if (v == 4)
            {
                d1 = (res / 1000) % 1000;
                d2 = res % 1000;

                if(nReadings!= 0)
                {
                    if (d1 == 300)  //d1 is an out of range (invalid) value
                        d1 = d_prev[0];
                    else    //d1 is a valid value
                        d_prev[0] = d1;

                    if (d2 == 300)    //d1 is an out of range (invalid) value
                        d2 = d_prev[1];
                    else    //d2 is a valid value
                        d_prev[1] = d2;
                }
/*
                //Median Filter

                prev_readings[0][nReadings%5] = d1;
                prev_readings[1][nReadings%5] = d2;
                if(nReadings>5)
                {
                    d1 = median(prev_readings[0][0], prev_readings[0][1], prev_readings[0][2], prev_readings[0][3], prev_readings[0][4]);
                    d2 = median(prev_readings[1][0], prev_readings[1][1], prev_readings[1][2], prev_readings[1][3], prev_readings[1][4]);
                }
*/
            }
        }
    }
    return 0;
}

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
    //Close serial communication
    CloseHandle(hSerial);

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
	    //Start up Serial Communication
	    //---------------------------------------------------------------------------------------

        if(hSerial == INVALID_HANDLE_VALUE)
        {
            if(GetLastError()==ERROR_FILE_NOT_FOUND)
            {
                printf("Serial port does not exist.");
                exit(1);
            }
            printf("Other error");
            exit(1);
        }

        DCB dcbSerialParams = {0};

        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

        if (!GetCommState(hSerial, &dcbSerialParams))
        {
            printf("Error getting state");
            exit(1);
        }

        dcbSerialParams.BaudRate = CBR_115200;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;

        if (!SetCommState(hSerial, &dcbSerialParams))
        {
            printf("Error setting serial port state");
            exit(1);
        }

        COMMTIMEOUTS timeouts = {0};

        timeouts.ReadIntervalTimeout = 10;
        timeouts.ReadTotalTimeoutConstant = 50;
        timeouts.ReadTotalTimeoutMultiplier = 100;
        timeouts.WriteTotalTimeoutConstant = 50;
        timeouts.WriteTotalTimeoutMultiplier = 10;

        if(!SetCommTimeouts(hSerial, &timeouts))
        {
            printf("Error occurred");
            exit(1);
        }

        //Call serial read function in separate thread
        int data = 1;
        SDL_CreateThread( serialRead, "Serial Read", (void*)data );

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
    //TO map -32786 to +32767 to -1 to 1
    x = xv/32768.0;
    y = -1*yv/32768.0;

    //Desired spring force along X and Y directions
    fx = -kx * x ;
    fy = -ky * y;

    fxn = fx;
    fyn = fy;

    if (fxn >= 20000.0)
    {
        fxn = 20000.0;
    }
    else if (fxn <= -20000.0)
    {
        fxn = -20000.0;
    }

    if (fyn >= 20000.0)
    {
        fyn = 20000.0;
    }
    else if (fyn <= -20000.0)
    {
        fyn = -20000.0;
    }

    if( d1 > 9 && d1 < 100)
    {
        ffy1 = - (100 - d1) * 200;
        fyn = fyn + ffy1;
    }
    else
        ffy1 = 0;

    if( d2 > 9 && d2 < 100)
    {
        ffx1 = - (100 - d2) * 200;
        fxn = fxn + ffx1;
    }
    else
        ffx1 = 0;

/*
    if( d3 > 9 && d3 < 100)
    {
        ffy2 = (100 - d3) * 200;
        //fyn = fyn + ffy2;
    }
    else
        ffy2 = 0;

    if( d4 > 9 && d4 < 100)
    {
        ffx2 = (100 - d4) * 200;
        //fxn = fxn + ffx2;
    }
    else
        ffx2 = 0;
*/

    //Magnitude of resultant
    fmag = int(sqrt(fxn*fxn + fyn*fyn));
    if (fmag>32767)
        fmag=32767;

    //To calculate the angle along which the resultant acts
    ang = int( 180 * atan(fyn / fxn) / 3.1415 ) + 180 ;
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
    printf("%f", fxn);
    printf("\t");
    printf("%f", fyn);
    printf("\t");
    printf("%f", ffy1);
    printf("\t");
    printf("%f", ffx1);
    printf("\t");

    /*printf("%f", ffy);
    printf("\t");
    */
    printf("%d", xv);
    printf("\t");
    printf("%d",yv);
    printf("\t");

    printf("%d", d1);
    printf("\t");
    printf("%d", d2);
    printf("\n");
    /*printf("%d", d3);
    printf("\t");
    printf("%d", d4);
    printf("\n");
*/
    SDL_HapticDestroyEffect(haptic, effect_id);
    effect.constant.direction.dir[0] = fdir;    // Direction from which the force comes
    effect.constant.length = tim_dur;
    effect.constant.level = fmag;   //magnitude of the force
    effect.periodic.attack_length = (tim_dur/4); // time taken to get max strength
    effect.periodic.fade_length = (tim_dur/4); // time taken to fade away
    effect_id = SDL_HapticNewEffect( haptic, &effect ); //uploading effect
    SDL_HapticRunEffect( haptic, effect_id, 1 );    //run the effect
    SDL_Delay(tim_dur); // Wait for the effect to finish
}



