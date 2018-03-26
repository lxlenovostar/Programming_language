/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

//Include SDL functions and datatypes
#include "SDL/SDL.h"

// g++ -g -Wall  lesson01.cpp  `sdl-config --cflags --libs`
int main( int argc, char* args[] )
{
    //The images
    SDL_Surface* hello = NULL;
    SDL_Surface* screen = NULL;

    //Start SDL
    SDL_Init( SDL_INIT_EVERYTHING );

    //Set up screen
	// 用来创建一个640像素宽，480像素高的32位(位/像素)窗口。
	// 其中最后一个参数 SDL_SWSURFACE 指定将表面存放在软件内存中。
	// 返回值为一个指向新建的窗口表面的指针。
    screen = SDL_SetVideoMode( 640, 480, 32, SDL_SWSURFACE );

    //Load image
	//加载图片
	//返回一个指向加载好的表面的指针
    hello = SDL_LoadBMP( "hello.bmp" );

    //Apply image to screen
	//第一个参数为源表面，第三个参数为目的表面。
	//此函数的功能是将源表面粘帖到目的表面上。
    SDL_BlitSurface( hello, NULL, screen, NULL );

    //Update Screen
	//更新窗口
    SDL_Flip( screen );

    //Pause
	//为了让窗口可见
    SDL_Delay( 2000 );

    //Free the loaded image
	//从内存中移除图像
    SDL_FreeSurface( hello );

    //Quit SDL
    SDL_Quit();

    return 0;
}
