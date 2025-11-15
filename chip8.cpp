
#include "chip8.hpp"
#include <SDL2/SDL.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
/*
 * 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
 * 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
 * 0x200-0xFFF - Program ROM and work RAM
 *
 */
// TODO
// opcodes left: 0xDXYN,0xEX9E,0xEXA1,0xFX0A,0xFX29,0xFX65

// Initialize registers and memory once
void Chip8::initialize() {
  pc = 0x200; // pc starts there
  opcode = 0;
  I = 0;
  sp = 0;
  drawFlag = false;
  // Clear display
  for (int i = 0; i < 64 * 32; i++) {
    gfx[i] = 0;
  }
  // Clear stack
  // Clear registers V0-VF
  for (int i = 0; i < 16; i++) {
    stack[i] = 0;
    V[i] = 0;
  }
  // Clear memory
  for (int i = 0; i < MEMORY_SIZE; i++) {
    memory[i] = 0;
  }
  // Load fontset
  unsigned char chip8_fontset[80] = {
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
  for (int i = 0; i < 80; ++i)
    memory[0x50 + i] = chip8_fontset[i];

  // Reset timers
  delay_timer = 0;
  sound_timer = 0;
}

void Chip8::loadGame(const char *s) {
  FILE *f = fopen(s, "rb");
  if (!f) {
    printf("Failed to open ROM: %s\n", s);
    std::exit(1);
  }
  size_t bytes = fread(&memory[0x200], 1, sizeof(memory) - 0x200, f);
  printf("Loaded %zu bytes\n", bytes);
  fclose(f);
}

void Chip8::emulateCycle() {
  // Fetch opcode
  opcode = (memory[pc] << 8) | (memory[pc + 1]);
  int X = (opcode & 0x0F00) >> 8;
  int Y = (opcode & 0x00F0) >> 4;
  // decode opcode
  switch (opcode & 0xF000) {
  // opcode starts with 0xA...
  case 0xA000:
    I = opcode & 0x0FFF;
    pc += 2;
    break;
  // opcode starts with 0x0...
  case 0x0000:
    switch (opcode & 0x000F) {
    case 0x0000: // 0x00E0- clears the screen
      for (size_t i = 0; i < 64 * 32; i++) {
        gfx[i] = 0;
      }
      pc += 2;
      drawFlag = true;
      break;
    case 0x000E: // 0x00EE- Returns from the subroutine
      pc = stack[--sp] + 2;
      break;
    default:
      printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
    }
    break;
  // opcode starts with 0x1...
  case 0x1000:
    // goto 0x0NNN
    pc = (opcode & 0x0FFF);
    break;
  case 0x2000:
    // calls subroutine at opcode & 0x0FFF;
    stack[sp++] = pc;
    pc = opcode & 0x0FFF;
    break;
  case 0x3000:
    // skips the next instruction if register #(opcode & 0x0F00) == (opcode &
    // 0x00FF)
    pc += 2;
    X = (opcode & 0x0F00) >> 8;
    if (V[X] == (opcode & 0x00FF)) {
      pc += 2;
    }
    break;
  case 0x4000:
    // skips the next instruction if register #(opcode & 0x0F00) != (opcode &
    // 0x00FF)
    pc += 2;
    X = (opcode & 0x0F00) >> 8;
    if (V[X] != (opcode & 0x00FF)) {
      pc += 2;
    }
    break;
  case 0x5000:
    // skips the next instruction if register #(opcode & 0x0F00) == register
    // #(opcode & 0x00FF)
    if (V[X] == V[Y]) {
      pc += 2;
    }
    pc += 2;
    break;
  case 0x6000:
    // sets register #(opcode & 0x0F00) to be (opcode & 0x00FF)
    X = (opcode & 0x0F00) >> 8;
    V[X] = (opcode & 0x00FF);
    pc += 2;
    break;
  case 0x7000:
    // Adds (opcode & 0x00FF) to register #(opcode & 0x0F00) (carry flag is not
    // changed)
    V[X] += opcode & 0x00FF;
    pc += 2;
    break;
  // opcode = 0x8XY#
  // X -> opcode & 0x0F00.
  // Y-> opcode & 0x00F0.
  case 0x8000:
    X = (opcode & 0x0F00) >> 8;
    Y = (opcode & 0x00F0) >> 4;
    switch (opcode & 0x000F) {
    case 0x0000:
      // Sets VX to the value of VY.
      V[X] = V[Y];
      pc += 2;
      break;
    case 0x0001: // Sets VX to VX or VY. (bitwise OR operation).
      V[X] |= V[Y];
      pc += 2;
      break;
    case 0x0002: // Sets VX to VX and VY. (bitwise AND operation).
      V[X] &= V[Y];
      pc += 2;
      break;
    case 0x0003: // Sets VX to VX xor VY.
      V[X] ^= V[Y];
      pc += 2;
      break;
    case 0x0004: // Adds VY to VX. VF is set to 1 when there's an overflow, and
      if (V[X] + V[Y] > 255) {
        V[15] = 1;
      } // to 0 when there is not.
      else
        V[15] = 0;
      V[X] += V[Y];
      pc += 2;
      break;
    case 0x0005: // VY is subtracted from VX. VF is set to 0 when there's an
                 // underflow, and 1 when there is not. (i.e. VF set to 1 if VX
                 // >= VY and 0 if not).
      if (V[X] >= V[Y]) {
        V[15] = 1;
      } else
        V[15] = 0;
      V[X] -= V[Y];
      pc += 2;
      break;
    case 0x0006: // Shifts VX to the right by 1, then stores the least
                 // significant bit of VX prior to the shift into VF.
      V[15] = (V[X] & 1);
      V[X] >>= 1;
      pc += 2;
      break;
    case 0x0007: // Sets VX to VY minus VX. VF is set to 0 when there's an
                 // underflow, and 1 when there is not. (i.e. VF set to 1 if VY
                 // >= VX).
      if (V[Y] >= V[X]) {
        V[15] = 0;
      } else
        V[15] = 1;
      V[X] = V[Y] - V[X];
      pc += 2;
      break;
    case 0x000E: // Shifts VX to the left by 1, then sets VF to 1 if the most
                 // significant bit of VX prior to that shift was set, or to 0
                 // if it was unset.
      V[15] = (V[X] & 0x80) ? 1 : 0;
      V[X] <<= 1;
      pc += 2;
      break;
    default:
      printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
    }
    break;
  case 0x9000:
    // Skips the next instruction if VX does not equal VY. (Usually the next
    // instruction is a jump to skip a code block).
    pc += 2;
    if (V[X] != V[Y]) {
      pc += 2;
    }
    break;
  // opcode = 0xBNNN
  case 0xB000:
    // Jumps to the address NNN plus V0.
    pc = V[0] + (opcode & 0x0FFF);
    break;
  // opcode = 0xCXNN
  case 0xC000:
    // Sets VX to the result of a bitwise and operation on a random number
    // (Typically: 0 to 255) and NN.
    V[X] = (opcode & 0x00FF) & (rand() % 256);
    pc += 2;
    break;
  // opcode = 0xDXYN
  case 0xD000:
    // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a
    // height of N pixels. Each row of 8 pixels is read as bit-coded starting
    // from memory location I; I value does not change after the execution of
    // this instruction. As described above, VF is set to 1 if any screen pixels
    // are flipped from set to unset when the sprite is drawn, and to 0 if that
    // does not happen.
    {
      unsigned short x = V[X];
      unsigned short y = V[Y];
      unsigned short height = opcode & 0x000F;
      unsigned short pixel;
      V[0xF] = 0;
      for (int yline = 0; yline < height; yline++) {
        pixel = memory[I + yline];
        for (int xline = 0; xline < 8; xline++) {
          if ((pixel & (0x80 >> xline)) != 0) {
            int px = (x + xline) % 64;
            int py = (y + yline) % 32;
            if (gfx[px + py * 64] == 1)
              V[0xF] = 1;
            gfx[px + py * 64] ^= 1;
          }
        }
      }
      drawFlag = true;
      pc += 2;
    }

    break;
  // opcode = 0xEX##
  case 0xE000:
    switch (opcode & 0x00FF) {
    case 0x009E:
      // Skips the next instruction if the key stored in VX(only consider the
      // lowest nibble) is pressed (usually the next instruction is a jump to
      // skip a code block).
      if (key[V[X] & 0x000F]) {
        pc += 2;
      }
      pc += 2;
      break;
    case 0x00A1:
      // Skips the next instruction if the key stored in VX(only consider the
      // lowest nibble) is not pressed (usually the next instruction is a jump
      // to skip a code block).
      if (!key[V[X] & 0x000F]) {
        pc += 2;
      }
      pc += 2;
      break;
    default:
      printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
    }
    break;
  // opcode = 0xFX##
  case 0xF000:
    switch (opcode & 0x00FF) {
    case 0x0007: // Sets VX to the value of the delay timer.
      V[X] = delay_timer;
      pc += 2;
      break;
    case 0x000A: // A key press is awaited, and then stored in VX (blocking
                 // operation, all instruction halted until next key event,
                 // delay and sound timers should continue processing).
    {
      bool keyPressed = false;

      for (int i = 0; i < 16; i++) {
        if (key[i] != 0) { // key is currently down
          V[X] = i;
          pc += 2; // move to next instruction
          keyPressed = true;
          break;
        }
      }
      if (!keyPressed) {
        // update timers
        updateTimers();
        return;
      }
    } break;
    case 0x0015: // Sets the delay timer to VX.
      delay_timer = V[X];
      pc += 2;
      break;
    case 0x0018: // Sets the sound timer to VX.
      sound_timer = V[X];
      pc += 2;
      break;
    case 0x001E: // Adds VX to I. VF is not affected.
      I += V[X];
      pc += 2;
      break;
    case 0x0029: // Sets I to the location of the sprite for the character in
                 // VX(only consider the lowest nibble). Characters 0-F (in
                 // hexadecimal) are represented by a 4x5 font.
      I = 0x50 + (V[X] * 5);
      pc += 2;
      break;
    case 0x0033: // Stores the binary-coded decimal representation of VX, with
                 // the hundreds digit in memory at location in I, the tens
                 // digit at location I+1, and the ones digit at location I+2.
      memory[I] = V[X] / 100;
      memory[I + 1] = (V[X] / 10) % 10;
      memory[I + 2] = V[X] % 10;
      pc += 2;
      break;
    case 0x0055: // Stores from V0 to VX (including VX) in memory, starting at
                 // address I. The offset from I is increased by 1 for each
                 // value written, but I itself is left unmodified.
      for (int i = 0; i <= X; i++) {
        memory[I + i] = V[i];
      }
      pc += 2;
      break;
    case 0x0065: // Fills from V0 to VX (including VX) with values from memory,
                 // starting at address I. The offset from I is increased by 1
                 // for each value read, but I itself is left unmodified.
      for (int i = 0; i <= X; i++) {
        V[i] = memory[I + i];
      }
      pc += 2;
      break;
    default:
      printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
    }
    break;
  default:
    printf("Unknown opcode: 0x%X\n", opcode);
  }
  updateTimers();
}
Chip8::~Chip8() {}

void Chip8::setKeys() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    // Handle window close
    if (event.type == SDL_QUIT) {
      SDL_Quit();
      std::exit(0);
    }

    // Key down
    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
      case SDLK_1:
        key[0x1] = 1;
        break;
      case SDLK_2:
        key[0x2] = 1;
        break;
      case SDLK_3:
        key[0x3] = 1;
        break;
      case SDLK_4:
        key[0xC] = 1;
        break;

      case SDLK_q:
        key[0x4] = 1;
        break;
      case SDLK_w:
        key[0x5] = 1;
        break;
      case SDLK_e:
        key[0x6] = 1;
        break;
      case SDLK_r:
        key[0xD] = 1;
        break;

      case SDLK_a:
        key[0x7] = 1;
        break;
      case SDLK_s:
        key[0x8] = 1;
        break;
      case SDLK_d:
        key[0x9] = 1;
        break;
      case SDLK_f:
        key[0xE] = 1;
        break;

      case SDLK_z:
        key[0xA] = 1;
        break;
      case SDLK_x:
        key[0x0] = 1;
        break;
      case SDLK_c:
        key[0xB] = 1;
        break;
      case SDLK_v:
        key[0xF] = 1;
        break;

      // Optional: ESC to quit
      case SDLK_ESCAPE:
        SDL_Quit();
        std::exit(0);
      }
    }

    // Key up
    else if (event.type == SDL_KEYUP) {
      switch (event.key.keysym.sym) {
      case SDLK_1:
        key[0x1] = 0;
        break;
      case SDLK_2:
        key[0x2] = 0;
        break;
      case SDLK_3:
        key[0x3] = 0;
        break;
      case SDLK_4:
        key[0xC] = 0;
        break;

      case SDLK_q:
        key[0x4] = 0;
        break;
      case SDLK_w:
        key[0x5] = 0;
        break;
      case SDLK_e:
        key[0x6] = 0;
        break;
      case SDLK_r:
        key[0xD] = 0;
        break;

      case SDLK_a:
        key[0x7] = 0;
        break;
      case SDLK_s:
        key[0x8] = 0;
        break;
      case SDLK_d:
        key[0x9] = 0;
        break;
      case SDLK_f:
        key[0xE] = 0;
        break;

      case SDLK_z:
        key[0xA] = 0;
        break;
      case SDLK_x:
        key[0x0] = 0;
        break;
      case SDLK_c:
        key[0xB] = 0;
        break;
      case SDLK_v:
        key[0xF] = 0;
        break;
      }
    }
  }
}

void Chip8::updateTimers() {
  // update timers
  if (delay_timer > 0)
    --delay_timer;

  if (sound_timer > 0) {
    if (sound_timer == 1)
      printf("BEEP!\n");
    --sound_timer;
  }
}
