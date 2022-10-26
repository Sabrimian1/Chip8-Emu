#pragma once

#include <iostream>

class Chip8 {

private:

	//registers
	uint8_t registers[16];

	//Memory buffer for storing roms
	uint8_t memory[4096];

	//index register, used for storing mem addrs in operations
	uint16_t idx = 0x0000;

	//16 level stack - capable of 16 layers of recursion
	uint16_t stack[16];

	//stack pointer for knowing whats on top of the stack
	uint8_t sp = 0;

	//program counter for knowing what instruction will be executed next - incremented by 2 each cycle
	uint16_t pc = 0x0000;

	//frame buffer - 64 x 32 pixel - uint32_t because its easier for using w sdl
	uint32_t frame_buffer[64 * 32];

	//opcode, two bytes 
	uint16_t opcode = 0x000F;

	//keypad inputs (0-f in hex)
	uint8_t keypad[16];

	//need an array 8 bit integers to represent the fontset 0 - f
	uint8_t font[80] = 
	{

	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F

	};
public:
	//draw flag
	uint8_t draw_flag = 0;	


	//constructor
	Chip8();

	//Each "cycle" or execution of instructions
	void cycle();

	//read rom files	
	void read_rom(char* filename);

	//opcodes...
	//Jump to an address
	void OP_1NNN();
	
	//clear the screen(frame buffer)
	void OP_00E0();

	//set register vx
	void OP_6XNN();

	//Add a value to a register
	void OP_7XNN();

	//set the index register
	void OP_ANNN();

	//display or draw
	void OP_DXYN();

	//return from a function
	void OP_00EE();

	//Call a function
	void OP_2NNN();

	//skip instruction if value in X register is equal to kk
	void OP_3XKK();

	//skip instruction if value in X register is NOT equal to kk
	void OP_4XKK();

	//skip instruction if value in x reg is equal to value in y reg
	void OP_5XY0();

	//load a byte of data into register x
	void OP_6XKK();

	//return frame buffer
	uint32_t* get_frame();
};
