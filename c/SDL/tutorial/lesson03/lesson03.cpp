/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

//The headers
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>

// g++ -g -Wall  lesson03.cpp  `sdl-config --cflags --libs`

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//The surfaces
SDL_Surface *image = NULL;
SDL_Surface *screen = NULL;

SDL_Surface *load_image( std::string filename )
{
	// 加载的图像
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

	// 优化后的图像
    //The optimized image that will be used
    SDL_Surface* optimizedImage = NULL;

	// 使用SDL_image加载图像
    //Load the image using SDL_image
    loadedImage = IMG_Load( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
		// 创建一个优化后的图像
        //Create an optimized image
        optimizedImage = SDL_DisplayFormat( loadedImage );

		// 释放原先加载的图像
        //Free the old image
        SDL_FreeSurface( loadedImage );
    }

	// 返回优化后的图像
    //Return the optimized image
    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination )
{
    //Rectangle to hold the offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit the surface
    SDL_BlitSurface( source, NULL, destination, &offset );
}

bool init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

    //Set the window caption
    SDL_WM_SetCaption( "PNG test", NULL );

    //If everything initialized fine
    return true;
}

void clean_up()
{
    //Free the surface
    SDL_FreeSurface( image );

    //Quit SDL
    SDL_Quit();
}

int main( int argc, char* args[] )
{
    //Initialize
    if( init() == false )
    {
        return 1;
    }

    //Load the image
    image = load_image( "look.png" );

    //If there was a problem in loading the image
    if( image == NULL )
    {
        return 1;
    }

    //Apply the surface to the screen
    apply_surface( 0, 0, image, screen );

    //Update the screen
    if( SDL_Flip( screen ) == -1 )
    {
        return 1;
    }

    //Wait 2 seconds
    SDL_Delay( 2000 );

    //Free the surface and quit SDL
    clean_up();

    return 0;
}
