
#include <stdlib.h>
#include <stdio.h>              /* printf */
#include <string.h>             /* strstr */
#include <ctype.h>              /* isdigit */

#include <SDL.h>

#ifndef SDL_HAPTIC_DISABLED

#include <SDL_haptic.h>

static SDL_Haptic *haptic;

/*
 * prototypes
 */
static void abort_execution(void);
static void HapticPrintSupported(SDL_Haptic * haptic);


/**
 * @brief The entry point of this force feedback demo.
 * @param[in] argc Number of arguments.
 * @param[in] argv Array of argc arguments.
 */

int main(int argc, char **argv)
{
    int i;
    char *name;
    int index;
    SDL_HapticEffect efx[5];
    int id[5];
    int nefx;
    unsigned int supported;

    name = NULL;
    index = -1;
    if (argc > 1)
    {
        name = argv[1];
        if ((strcmp(name, "--help") == 0) || (strcmp(name, "-h") == 0))
	{
            printf("USAGE: %s [device]\n"
                   "If device is a two-digit number it'll use it as an index, otherwise\n"
                   "it'll use it as if it were part of the device's name.\n",
                   argv[0]);
            return 0;
        }

        i = strlen(name);
        if ((i < 3) && isdigit(name[0]) && ((i == 1) || isdigit(name[1])))
	{
            index = atoi(name);
            name = NULL;
        }
    }

    /* Initialize the force feedbackness */
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC);
    printf("%d Haptic devices detected.\n", SDL_NumHaptics());
    if (SDL_NumHaptics() > 0)
    {
        /* We'll just use index or the first force feedback device found */
        if (name == NULL)
	{
            i = (index != -1) ? index : 0;
        }
        /* Try to find matching device */
        else
	{
            for (i = 0; i < SDL_NumHaptics(); i++)
	    {
                if (strstr(SDL_HapticName(i), name) != NULL)
                    break;
            }

            if (i >= SDL_NumHaptics())
	    {
                printf("Unable to find device matching '%s', aborting.\n", name);
                return 1;
            }
        }

        haptic = SDL_HapticOpen(i);
        if (haptic == NULL)
	{
            printf("Unable to create the haptic device: %s\n", SDL_GetError());
            return 1;
        }
        printf("Device: %s\n", SDL_HapticName(i));
    }
    else
    {
        printf("No Haptic devices found!\n");
        return 1;
    }

    /* We only want force feedback errors. */
    SDL_ClearError();

    /* Create effects. */
    memset(&efx, 0, sizeof(efx));
    nefx = 0;
    supported = SDL_HapticQuery(haptic);

    printf("\nUploading effects\n");

    /* Now the classical constant effect. */
    if (supported & SDL_HAPTIC_SPRING)
    {
        printf("   effect %d: Constant Force\n", nefx);
        efx[nefx].type = SDL_HAPTIC_SPRING;
        efx[nefx].constant.direction.type = SDL_HAPTIC_POLAR;
        efx[nefx].constant.direction.dir[0] = 0;    /* Force comes from the north. */
        efx[nefx].constant.length = 5000;
        efx[nefx].constant.level = 0x5000;
        //efx[nefx].constant.attack_length = 1000;
        //efx[nefx].constant.fade_length = 1000;
        id[nefx] = SDL_HapticNewEffect(haptic, &efx[nefx]);
        if (id[nefx] < 0)
	{
            printf("UPLOADING EFFECT ERROR: %s\n", SDL_GetError());
            abort_execution();
        }
        nefx++;
    }
    /* Now the classical constant effect. */
    if (supported & SDL_HAPTIC_SPRING)
    {
        printf("   effect %d: Constant Force\n", nefx);
        efx[nefx].type = SDL_HAPTIC_SPRING;
        efx[nefx].constant.direction.type = SDL_HAPTIC_POLAR;
        efx[nefx].constant.direction.dir[0] = 9000;    /* Force comes from the east. */
        efx[nefx].constant.length = 5000;
        efx[nefx].constant.level = 0x5000;
        //efx[nefx].constant.attack_length = 1000;
        //efx[nefx].constant.fade_length = 1000;
        id[nefx] = SDL_HapticNewEffect(haptic, &efx[nefx]);
        if (id[nefx] < 0)
	{
            printf("UPLOADING EFFECT ERROR: %s\n", SDL_GetError());
            abort_execution();
        }
        nefx++;
    }
    /* Now the classical constant effect. */
    if (supported & SDL_HAPTIC_SPRING)
    {
        printf("   effect %d: Constant Force\n", nefx);
        efx[nefx].type = SDL_HAPTIC_SPRING;
        efx[nefx].constant.direction.type = SDL_HAPTIC_POLAR;
        efx[nefx].constant.direction.dir[0] = 18000;    /* Force comes from the south. */
        efx[nefx].constant.length = 5000;
        efx[nefx].constant.level = 0x5000;
        //efx[nefx].constant.attack_length = 1000;
        //efx[nefx].constant.fade_length = 1000;
        id[nefx] = SDL_HapticNewEffect(haptic, &efx[nefx]);
        if (id[nefx] < 0)
	{
            printf("UPLOADING EFFECT ERROR: %s\n", SDL_GetError());
            abort_execution();
        }
        nefx++;
    }
    /* Now the classical constant effect. */
    if (supported & SDL_HAPTIC_SPRING)
    {
        printf("   effect %d: Constant Force\n", nefx);
        efx[nefx].type = SDL_HAPTIC_SPRING;
        efx[nefx].constant.direction.type = SDL_HAPTIC_POLAR;
        efx[nefx].constant.direction.dir[0] = 27000;    /* Force comes from the west. */
        efx[nefx].constant.length = 5000;
        efx[nefx].constant.level = 0x5000;
        //efx[nefx].constant.attack_length = 1000;
        //efx[nefx].constant.fade_length = 1000;
        id[nefx] = SDL_HapticNewEffect(haptic, &efx[nefx]);
        if (id[nefx] < 0)
	{
            printf("UPLOADING EFFECT ERROR: %s\n", SDL_GetError());
            abort_execution();
        }
        nefx++;
    }

    printf("\nNow playing effects for 5 seconds each with 5 second delay between\n");

    for (i = 0; i < nefx; i++)
    {
        printf("   Playing effect %d\n", i);
        SDL_HapticRunEffect(haptic, id[i], 1);
        SDL_Delay(5000);        /* Effects only have length 5000 */
	//SDL_HapticSetAutocenter( haptic, 1);
	//SDL_HapticSetGain(haptic, 100);
	//SDL_Delay(900);
   }

    /* Quit */
    if (haptic != NULL)
        SDL_HapticClose(haptic);
    SDL_Quit();

    return 0;
}


/*
 * Cleans up a bit.
 */
static void abort_execution(void)
{
    printf("\nAborting program execution.\n");

    SDL_HapticClose(haptic);
    SDL_Quit();

    exit(1);
}




#else

int main(int argc, char *argv[])
{
    fprintf(stderr, "SDL compiled without Haptic support.\n");
    exit(1);
}

#endif
