#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Loads individual image as texture
SDL_Texture* loadTexture( std::string path );

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Current displayed texture
SDL_Texture* gTexture = NULL;

//Destination rect of circle
SDL_Rect DEST_R;


bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
        std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << '\n';
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
            std::cout << "Warning: Linear texture filtering not enabled!" << '\n';
		}

		//Create window
		gWindow = SDL_CreateWindow(
            "Agar.io",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN
        );
		if( gWindow == NULL )
		{
            std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << '\n';
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer == NULL )
			{
                std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << '\n';
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
                    std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << '\n';
					success = false;
				}


                DEST_R.x = SCREEN_WIDTH/2 - 50;
                DEST_R.y = SCREEN_HEIGHT/2 - 50;
                DEST_R.w = 2 * 50;
                DEST_R.h = 2 * 50;
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load PNG texture
	gTexture = loadTexture( "assets/green_circle.png" );
	if( gTexture == NULL )
	{
        std::cout << "Failed to load texture image!\n";
		success = false;
	}

	return success;
}

void close()
{
	//Free loaded image
	SDL_DestroyTexture( gTexture );
	gTexture = NULL;

	//Destroy window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

SDL_Texture* loadTexture( std::string path )
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
        std::cout << "Unable to load image assets/green_circle.png! SDL_image Error: " << IMG_GetError() << '\n';
	}
	else
	{
		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
            std::cout << "Unable to create texture from assets/green_circle.png! SDL Error: " << SDL_GetError() << '\n';
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return newTexture;
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
        std::cout << "Failed to initialize!\n";
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
            std::cout << "Failed to load media!\n";
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
                    //User presses a key
                    else if( e.type == SDL_KEYDOWN )
                    {
                        //Select surfaces based on key press
                        switch( e.key.keysym.sym )
                        {
                            case SDLK_UP:
                            DEST_R.y -= 5;
                            break;

                            case SDLK_DOWN:
                            DEST_R.y += 5;
                            break;

                            case SDLK_LEFT:
                            DEST_R.x -= 5;
                            break;

                            case SDLK_RIGHT:
                            DEST_R.x += 5;
                            break;

                            default:
                            DEST_R.x = SCREEN_WIDTH/2 - 50;
                            DEST_R.y = SCREEN_HEIGHT/2 - 50;
                            break;
                        }
                    }
				}

				//Clear screen
				SDL_RenderClear( gRenderer );

				//Render texture to screen
				SDL_RenderCopy( gRenderer, gTexture, NULL, &DEST_R );

				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
