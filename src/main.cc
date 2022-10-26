#include <SDL2/SDL.h>
#include "chip8.hh"
#include <iostream>
#include <chrono>
#include <thread>

int main(int argc, char** argv)
{

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("Chip8-Emu", 0, 0, 640, 320, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

	std::chrono::milliseconds delay(5);
	Chip8 chip8;
	chip8.read_rom(argv[1]);

	int video_pitch = 64 * 4;

	bool quit = false;

	while(!quit)
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
			{
				quit = true;
			}
		}
		
		chip8.cycle();
		

		SDL_UpdateTexture(texture, nullptr, chip8.get_frame(), video_pitch);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);

		std::this_thread::sleep_for(delay);

	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;

}
