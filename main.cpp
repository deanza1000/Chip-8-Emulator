#include "chip8.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <cstdio>
#include <iostream>
#include <string>
Chip8 myChip8;
SDL_Window *gWindow = nullptr;
SDL_Renderer *gRenderer = nullptr;
const int CHIP8_WIDTH = 64;
const int CHIP8_HEIGHT = 32;
const int WINDOW_SCALE = 15;

int main(int argc, char **argv) {

  printf(
      "Welcome to CHIP8 Emulator! Please Choose which game you want to play\n");
  printf("Available Games:\n15PUZZLE BLINKY BLITZ BRIX CONNECT4\nGUESS "
         "HIDDEN INVADERS KALEID MAZE\nMERLIN MISSILE PONG PONG2 PUZZLE\n"
         "SYZYGY TANK TETRIS TICTAC UFO\nVBRIX VERS WIPEOFF\n");

  setupGraphics();
  // Initialize the Chip8 system and load the game into the memory
  myChip8.initialize();
  myChip8.loadGame(getRomPath().c_str());
  bool running = true;
  // to speed up ->increase. to slow dowsn ->decrease
  const int CYCLES_PER_FRAME = 5;
  const int FRAME_DELAY_MS = 1000 / 60; // ~16 ms per frame
  Uint32 lastTimerUpdate = SDL_GetTicks();
  // Emulation loop
  while (running) {
    Uint32 frameStart = SDL_GetTicks();
    // Emulate one cycle
    for (int i = 0; i < CYCLES_PER_FRAME; ++i) {
      myChip8.emulateCycle();
    }

    // If the draw flag is set, update the screen
    if (myChip8.drawFlag)
      drawGraphics();

    // Store key press state (Press and Release)
    myChip8.setKeys();
    Uint32 now = SDL_GetTicks();
    while (now - lastTimerUpdate >= 16) {
      myChip8.updateTimers();
      lastTimerUpdate = now;
    }

    Uint32 frameTime = SDL_GetTicks() - frameStart;
    if (frameTime < FRAME_DELAY_MS) {
      SDL_Delay(FRAME_DELAY_MS - frameTime);
    }
  }

  return 0;
}

void setupGraphics() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
    std::exit(1);
  }
  gWindow = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, CHIP8_WIDTH * WINDOW_SCALE,
                             CHIP8_HEIGHT * WINDOW_SCALE, SDL_WINDOW_SHOWN);
  if (!gWindow) {
    SDL_Log("Window could not be created! SDL_Error: %s", SDL_GetError());
    std::exit(1);
  }
  gRenderer = SDL_CreateRenderer(
      gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!gRenderer) {
    SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
    std::exit(1);
  }
  SDL_RenderSetLogicalSize(gRenderer, CHIP8_WIDTH, CHIP8_HEIGHT);
}

void drawGraphics() {
  // Clear screen (black)
  SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
  SDL_RenderClear(gRenderer);

  // Draw foreground pixels (white)
  SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

  for (int y = 0; y < CHIP8_HEIGHT; ++y) {
    for (int x = 0; x < CHIP8_WIDTH; ++x) {
      int idx = y * CHIP8_WIDTH + x;
      if (myChip8.gfx[idx]) {
        SDL_Rect rect = {x, y, 1, 1};
        SDL_RenderFillRect(gRenderer, &rect);
      }
    }
  }

  SDL_RenderPresent(gRenderer);
  myChip8.drawFlag = false;
}
std::string getRomPath() {
  std::string name;
  std::cout << "Enter ROM name (without extension): ";
  std::cin >> name;

  return "roms/" + name;
}
