/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

//The headers
#include "SDL/SDL.h"
#include <string>

// g++ -g -Wall  lesson02.cpp  `sdl-config --cflags --libs`

//The attributes of the screen
// 窗口属性
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//The surfaces that will be used
// 写着hello字样的图片
SDL_Surface *message = NULL;
// 背景图片
SDL_Surface *background = NULL;
// 程序窗口
SDL_Surface *screen = NULL;

SDL_Surface *load_image( std::string filename )
{
	// 临时的空间，用于存储刚刚加载好的图像
    //Temporary storage for the image that's loaded
    SDL_Surface* loadedImage = NULL;

	// 优化后的图像，实际使用的是这个图像
    //The optimized image that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = SDL_LoadBMP( filename.c_str() );

    //If nothing went wrong in loading the image
    if( loadedImage != NULL )
    {
		// SDL_DisplayFormat()来创建一个与窗口拥有同样
		// 格式的新版本的图像
        //Create an optimized image
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old image
        SDL_FreeSurface( loadedImage );
    }

    //Return the optimized image
    return optimizedImage;
}

// 传入的参数分别是blit的位置坐标，blit的源表面和blit的目标表面。
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination )
{
	// 新建一个临时的矩形来保存偏移量
    //Make a temporary rectangle to hold the offsets
    SDL_Rect offset;

	// 将传入的偏移量保存到矩形中
    //Give the offsets to the rectangle
    offset.x = x;
    offset.y = y;

    //Blit the surface
    SDL_BlitSurface( source, NULL, destination, &offset );
}

int main( int argc, char* args[] )
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return 1;
    }

	// 设置窗口
	// 对于此函数的返回值:The returned surface is freed by SDL_Quit 
	// and must not be freed by the caller. 
    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return 1;
    }

	// 设置窗口标题
    //Set the window caption
    SDL_WM_SetCaption( "Hello World", NULL );

    //Load the images
    message = load_image( "hello.bmp" );
    background = load_image( "background.bmp" );

	// 执行blit操作
    //Apply the background to the screen
    apply_surface( 0, 0, background, screen );
    apply_surface( 320, 0, background, screen );
    apply_surface( 0, 240, background, screen );
    apply_surface( 320, 240, background, screen );

    //Apply the message to the screen
    apply_surface( 180, 140, message, screen );

	// 更新窗口
    //Update the screen
    if( SDL_Flip( screen ) == -1 )
    {
        return 1;
    }

    //Wait 2 seconds
    SDL_Delay( 2000 );

	// 释放表面
    //Free the surfaces
    SDL_FreeSurface( message );
    SDL_FreeSurface( background );

    //Quit SDL
    SDL_Quit();

    return 0;
}
