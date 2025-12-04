#include "chip8.hpp"

// TODO
// check how to evaluate opcodes (leftmost nibble, rightmost nibble maybe)
// add implementation to all function
// add parameters to functions (maybe an array of 4 nibbles?)
//

// the idea is to add an entry for each function based on the opcode, recieving
// the opcode with 0 instead of arguments (X,Y,NNN...)
Ops::Ops() {
  // 0x00E0
  map[0x00E0] = disp_clear;

  // 0x00EE
  map[0x00EE] = subroutineReturn;

  // 0x1NNN
  map[0x1000] = gotoNNN;

  // 0x2NNN
  map[0x2000] = callSubroutineAtNNN;

  // 0x3XNN
  map[0x3000] = ifEqualToNN;

  // 0x4XNN
  map[0x4000] = ifNotEqualToNN;

  // 0x5XY0
  map[0x5000] = ifEqualToY;

  // 0x6XNN
  map[0x6000] = setXtoNN;

  // 0x7XNN
  map[0x7000] = addNNtoX;

  // 0x8XY0
  map[0x8000] = setXtoY;

  // 0x8XY1
  map[0x8001] = setXtoXOrY;

  // 0x8XY2
  map[0x8002] = setXtoXAndY;

  // 0x8XY3
  map[0x8003] = setXtoXXorY;

  // 0x8XY4
  map[0x8004] = addYtoX;

  // 0x8XY5
  map[0x8005] = subtractYFromX;

  // 0x8XY6
  map[0x8006] = shiftXRight;

  // 0x8XY7
  map[0x8007] = setXtoYMinusX;

  // 0x8XYE
  map[0x800E] = shiftXLeft;

  // 0x9XY0
  map[0x9000] = ifNotEqualToY;

  // 0xANNN
  map[0xA000] = setIToNNN;

  // 0xBNNN
  map[0xB000] = jumpTo0PlusNNN;

  // 0xCXNN
  map[0xC000] = setXtoRand;

  // 0xDXYN
  map[0xD000] = draw;

  // 0xEX9E
  map[0xE00E] = ifKeyIsEqualToX;

  // 0xEXA1
  map[0xE001] = ifKeyIsNotEqualToX;

  // 0xFX07
  map[0xF007] = setXtoTimer;

  // 0xFX0A
  map[0xF00A] = setXtoKey;

  // 0xFX15
  map[0xF015] = setTimerToX;

  // 0xFX18
  map[0xF018] = setSoundToX;

  // 0xFX1E
  map[0xF01E] = addXToI;

  // 0xFX29
  map[0xF029] = setIToSprite;

  // 0xFX33
  map[0xF033] = setToBCD;

  // 0xFX55
  map[0xF055] = regDump;

  // 0xFX65
  map[0xF065] = regLoad;
}

void Ops::setIToNNN(Chip8 &chip, std::vector<int> nibble) {}

void Ops::subroutineReturn(Chip8 &chip, std::vector<int> nibble) {}
void Ops::disp_clear(Chip8 &chip, std::vector<int> nibble) {}
void Ops::gotoNNN(Chip8 &chip, std::vector<int> nibble) {}
void Ops::callSubroutineAtNNN(Chip8 &chip, std::vector<int> nibble) {}
void Ops::ifEqualToNN(Chip8 &chip, std::vector<int> nibble) {
  int X = (nibble[1]) >> 8;
}
void Ops::ifNotEqualToNN(Chip8 &chip, std::vector<int> nibble) {
  int X = (nibble[1]) >> 8;
}
void Ops::ifEqualToY(Chip8 &chip, std::vector<int> nibble) {
  int X = (nibble[1]) >> 8;
  int Y = (nibble[2]) >> 4;
}
void Ops::setXtoNN(Chip8 &chip, std::vector<int> nibble) {
  int X = (nibble[1]) >> 8;
}
void Ops::addNNtoX(Chip8 &chip, std::vector<int> nibble) {
  int X = (nibble[1]) >> 8;
}
void Ops::setXtoY(Chip8 &chip, std::vector<int> nibble) {
  int X = (nibble[1]) >> 8;
  int Y = (nibble[2]) >> 4;
}
void Ops::setXtoXOrY(Chip8 &chip, std::vector<int> nibble) {
  int X = (nibble[1]) >> 8;
  int Y = (nibble[2]) >> 4;
}
void Ops::setXtoXAndY(Chip8 &chip, std::vector<int> nibble) {
  int X = (nibble[1]) >> 8;
  int Y = (nibble[2]) >> 4;
}
void Ops::setXtoXXorY(Chip8 &chip, std::vector<int> nibble) {

  int X = (nibble[1]) >> 8;
  int Y = (nibble[2]) >> 4;
}
void Ops::addYtoX(Chip8 &chip, std::vector<int> nibble) {

  int X = (nibble[1]) >> 8;
  int Y = (nibble[2]) >> 4;
}
void Ops::subtractYFromX(Chip8 &chip, std::vector<int> nibble) {

  int X = (nibble[1]) >> 8;
  int Y = (nibble[2]) >> 4;
}
void Ops::shiftXRight(Chip8 &chip, std::vector<int> nibble) {
  int X = (nibble[1]) >> 8;
}
void Ops::setXtoYMinusX(Chip8 &chip, std::vector<int> nibble) {

  int X = (nibble[1]) >> 8;
  int Y = (nibble[2]) >> 4;
}
void Ops::shiftXLeft(Chip8 &chip, std::vector<int> nibble) {
  int X = (nibble[1]) >> 8;
}
void Ops::ifNotEqualToY(Chip8 &chip, std::vector<int> nibble) {

  int X = (nibble[1]) >> 8;
  int Y = (nibble[2]) >> 4;
}
void Ops::jumpTo0PlusNNN(Chip8 &chip, std::vector<int> nibble) {}
void Ops::setXtoRand(Chip8 &chip, std::vector<int> nibble) {
  int X = (nibble[1]) >> 8;
}
void Ops::draw(Chip8 &chip, std::vector<int> nibble) {}
void Ops::ifKeyIsEqualToX(Chip8 &chip, std::vector<int> nibble) {
  int X = (nibble[1]) >> 8;
}
void Ops::ifKeyIsNotEqualToX(Chip8 &chip, std::vector<int> nibble) {
  int X = (nibble[1]) >> 8;
}
void Ops::setXtoTimer(Chip8 &chip, std::vector<int> nibble) {
  int X = (nibble[1]) >> 8;
}
void Ops::setXtoKey(Chip8 &chip, std::vector<int> nibble) {}
void Ops::setTimerToX(Chip8 &chip, std::vector<int> nibble) {}
void Ops::setSoundToX(Chip8 &chip, std::vector<int> nibble) {}
void Ops::addXToI(Chip8 &chip, std::vector<int> nibble) {}
void Ops::setIToSprite(Chip8 &chip, std::vector<int> nibble) {}
void Ops::setToBCD(Chip8 &chip, std::vector<int> nibble) {}
void Ops::regDump(Chip8 &chip, std::vector<int> nibble) {}
void Ops::regLoad(Chip8 &chip, std::vector<int> nibble) {}

Ops::~Ops() {}
