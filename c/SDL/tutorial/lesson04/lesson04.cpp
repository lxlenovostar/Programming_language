/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

//The headers
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>

// g++ -g -Wall  lesson04.cpp  `sdl-config --cflags --libs` -lSDL_image

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

// 表面
//The surfaces
SDL_Surface *image = NULL;
SDL_Surface *screen = NULL;

// 将要用到的事件结构体
//The event structure that will be used
SDL_Event event;

SDL_Surface *load_image( std::string filename )
{
	// 存储加载好的图像
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

	// 存储优化后的图像
    //The optimized image that will be used
    SDL_Surface* optimizedImage = NULL;

	// 加载图像
    //Load the image
    loadedImage = IMG_Load( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
		// 创建优化的图像
        //Create an optimized image
        optimizedImage = SDL_DisplayFormat( loadedImage );

		// 释放原先加载的图像
        //Free the old image
        SDL_FreeSurface( loadedImage );
    }

	// 返回优化的图像
    //Return the optimized image
    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination )
{
	// 存储偏移量的临时矩阵
    //Temporary rectangle to hold the offsets
    SDL_Rect offset;

	// 存入偏移量
    //Get the offsets
    offset.x = x;
    offset.y = y;

	// 执行blit操作
    //Blit the surface
    SDL_BlitSurface( source, NULL, destination, &offset );
}

bool init()
{
	// 初始化SDL的所有子系统
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

	// 设置窗口
    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

	// 设置窗口标题
    //Set the window caption
    SDL_WM_SetCaption( "Event test", NULL );

    //If everything initialized fine
    return true;
}

bool load_files()
{
	// 加载图像
    //Load the image
    image = load_image( "x.png" );

    //If there was an error in loading the image
    if( image == NULL )
    {
        return false;
    }

    //If everything loaded fine
    return true;
}

void clean_up()
{
	// 释放图像
    //Free the surface
    SDL_FreeSurface( image );

    //Quit SDL
    SDL_Quit();
}

int main( int argc, char* args[] )
{
    //Make sure the program waits for a quit
    bool quit = false;

    //Initialize
    if( init() == false )
    {
        return 1;
    }

    //Load the files
    if( load_files() == false )
    {
        return 1;
    }

	// 将image表面应用到窗口上
    //Apply the surface to the screen
    apply_surface( 0, 0, image, screen );

	// 更新窗口
    //Update the screen
    if( SDL_Flip( screen ) == -1 )
    {
        return 1;
    }

    //While the user hasn't quit
    while( quit == false )
    {
		// SDL_PollEvent()的作用是从事件队列底部取出一个
		// 事件并将其事件数据粘帖到一个 SDL_Event 类型的结构体中。
        //While there's an event to handle
        while( SDL_PollEvent( &event ) )
        {
			// 如果用户点击了窗口右上角的关闭按钮
            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }
        }
    }

    //Free the surface and quit SDL
    clean_up();

    return 0;
}
