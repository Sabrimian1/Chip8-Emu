#include <cstring>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "chip8.hh"
#include <cstring>

Chip8::Chip8()
{
	
	//std::memset(memory, 0, sizeof(memory));

	for(int i = 0; i < 16; i++)
	{
		registers[i] = 0;
	}

	for(int i = 0; i < 4096; i++)
	{
		memory[i] = 0;
	}

	for(int i = 0; i < 2048; i++)
	{
		frame_buffer[i] = 0;
	}	

	pc = 0x200; //entry point of the actual program in memory

	uint8_t font_arr_size = sizeof(font) / sizeof(uint8_t);

	//insert fonts into memory buffer
	for(uint8_t i = 0; i < font_arr_size; i++)
	{

		memory[0x050 + i] = font[i];

	}	

}

void Chip8::read_rom(char* filename)
{

	uint16_t start_addr = 0x200;

	std::ifstream rom(filename, std::ifstream::binary);

	if(rom)
	{

		rom.seekg(0, rom.end);
		uint16_t length = rom.tellg();
		rom.seekg(0, rom.beg);

		char *buff = new char[length];

		std::cout << "Rom length: " << length << std::endl;
		rom.read(buff, length);
		rom.close();

		for(uint16_t i = 0; i < length; i++)
		{
	
			memory[start_addr+i] = buff[i];	
		
		}

		delete [] buff;
	}else{
		std::cout << "Error loading rom: " << filename << std::endl;
		exit(1);
	}




}	

void Chip8::cycle()
{
	/*
	 

		functionality:
			- we will need to fetch the the opcode that the current pc is looking at
			- we will then need to decode it using switch statements


	*/
	
	opcode = memory[pc] << 8 | memory[pc+1];

	pc += 2;
	switch(opcode & 0xF000)
	{
		case 0:
			switch(opcode & 0x00FF)
			{
				case 0x00E0:
					OP_00E0();
					break;
				case 0x00EE:
					OP_00EE();
					break;
			}
			break;
		case 0x1000:
			OP_1NNN();
			break;
		case 0x2000:
			OP_2NNN();
			break;

		case 0x3000:
			OP_3XKK();
			break;

		case 0x4000:
			OP_4XKK();
			break;

		case 0x5000:
			OP_5XY0();
			break;

		case 0x6000:
			OP_6XNN();
			break;

		case 0x7000:
			OP_7XNN();
			break;

		case 0x8000:
			switch(opcode & 0x000F)
			{
				case 0:
					OP_8XY0();
					break;
				case 1:
					OP_8XY1();
					break;
				case 2:
					OP_8XY2();
					break;
				case 3:
					OP_8XY3();
					break;
				case 4:
					OP_8XY4();
					break;
				case 5:
					OP_8XY5();
					break;
				case 6:
					OP_8XY6();
					break;
				case 7:
					OP_8XY7();
					break;
				case 14:
					OP_8XYE();
					break;

			}
			break;

		case 0x9000:
			OP_9XY0();
			break;

		case 0xA000: 
			OP_ANNN();
			break;

		case 0xB000:
			OP_BNNN();
			break;

		case 0xC000:
			OP_CXKK();
			break;

		case 0xD000:
			OP_DXYN();
			break;
	}
		
}

void Chip8::OP_00E0()
{

	std::memset(frame_buffer, 0, sizeof(frame_buffer));

}	


void Chip8::OP_1NNN()
{
	//we're anding with 0x0FFF to extract the address information of where we're jumping to
	pc = opcode & 0x0FFF;	

}

void Chip8::OP_6XNN()
{

	uint8_t reg = opcode & 0x0F00 >> 8;
	registers[reg] = opcode & 0x00FF;

}


void Chip8::OP_7XNN()
{
	uint8_t reg = (opcode & 0x0F00) >> 8;
	registers[reg] += (opcode & 0x00FF);
}


void Chip8::OP_ANNN()
{
	idx = opcode & 0x0FFF;
}


void Chip8::OP_DXYN()
{
	//display
	//Drawn at horizontal X coordinate in VX and Y coordinate in VY
	//Pixel drawing is done using XOR on the screen
	//if any pixels were turned off on the screen, VF will be set to 1, otherwise 0

	draw_flag = 1;

	//step 1: grab the X and Y register
	uint8_t vx = (opcode & 0x0F00) >> 8;
	uint8_t xcord = registers[vx];

	uint8_t vy = (opcode & 0x00F0) >> 4;
	uint8_t ycord = registers[vy];

	//n is essentially the height of whats being drawn to the screen
	uint8_t n = opcode & 0x000F;

	//Since starting position of sprite can change, we will use the modulo operator for x and y
	xcord %= 64;
	ycord %= 32;

	registers[0x000F] = 0;

	//IDX is the starting address and N is the amount of bytes that need to be read
	for(uint8_t i = 0; i < n; i++)
	{
		uint8_t byte = memory[idx + i];
		for(uint8_t j = 0; j < 8; j++)
		{
			//we are now at the pixel level
			uint8_t byte_pixel = byte & (0x80 >> j);

			//check to see if pixel in current frame buffer is turned on
			//if so, VF must be turned on before XOR operation
			//remember, our frame buffer is only a 1 dimensional array
			uint32_t *buffer_pixel = &frame_buffer[(xcord + j) + (64 * (ycord + i))];

			if(byte_pixel)
			{
				if(*buffer_pixel == 0xFFFFFFFF)
				{
					registers[0x000F] = 1;
				}

				*buffer_pixel ^= 0xFFFFFFFF;
			}

		}

	}	
}

void Chip8::OP_00EE()
{
	sp--;
	pc = stack[sp];
}

void Chip8::OP_2NNN()
{
	stack[sp++] = pc;
	pc = opcode & 0x0FFF;
}

void Chip8::OP_3XKK()
{
	if(registers[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
	{
		pc += 2;
	}	
}

void Chip8::OP_4XKK()
{

	if(registers[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
	{
		pc += 2;
	}
}

void Chip8::OP_5XY0() 
{
	if(registers[(opcode & 0x0F00) >> 8] == registers[(opcode & 0x00F0) >> 4])
	{
		pc += 2;
	}	
}

void Chip8::OP_6XKK()
{
	registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
}

uint32_t* Chip8::get_frame()
{
	return frame_buffer;
}




