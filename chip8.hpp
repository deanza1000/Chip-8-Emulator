
#ifndef CHIP8
#define CHIP8
#include <string>
#include <unordered_map>
#define MEMORY_SIZE 4096
#define REGISTERS_AMT 16
class Chip8 {
public:
  unsigned char key[16];
  unsigned char gfx[64 * 32];
  bool drawFlag;
  unsigned char delay_timer;
  unsigned char sound_timer;
  void initialize();
  void emulateCycle();
  void loadGame(const char *s);
  void setKeys();
  void updateTimers();
  ~Chip8();

private:
  unsigned short opcode;
  unsigned char memory[MEMORY_SIZE];
  unsigned char V[REGISTERS_AMT];
  unsigned short I;
  unsigned short pc;
  unsigned short stack[16];
  unsigned short sp; // sp points to the current empty index in stack
};
void setupGraphics();
void drawGraphics();
std::string getRomPath();

typedef void (*opcodeFunc)(Chip8 &);
class Ops {
public:
  Ops();
  ~Ops();

private:
  std::pmr::unordered_map<int, opcodeFunc> map;
  // for 0x00E0
  static void disp_clear(Chip8 &chip);

  // for 0x00EE
  static void subroutineReturn(Chip8 &chip);

  // for 0x1NNN
  static void gotoNNN(Chip8 &chip);

  // for 0x2NNN
  static void callSubroutineAtNNN(Chip8 &chip);

  // for 0x3XNN
  static void ifEqualToNN(Chip8 &chip);

  // for 0x4XNN
  static void ifNotEqualToNN(Chip8 &chip);

  // for 0x5XY0
  static void ifEqualToY(Chip8 &chip);

  // for 0x6XNN
  static void setXtoNN(Chip8 &chip);

  // for 0x7XNN
  static void addNNtoX(Chip8 &chip);

  // for 0x8XY0
  static void setXtoY(Chip8 &chip);

  // for 0x8XY1
  static void setXtoXOrY(Chip8 &chip);

  // for 0x8XY2
  static void setXtoXAndY(Chip8 &chip);

  // for 0x8XY3
  static void setXtoXXorY(Chip8 &chip);

  // for 0x8XY4
  static void addYtoX(Chip8 &chip);

  // for 0x8XY5
  static void subtractYFromX(Chip8 &chip);

  // for 0x8XY6
  static void shiftXRight(Chip8 &chip);

  // for 0x8XY7
  static void setXtoYMinusX(Chip8 &chip);

  // for 0x8XYE
  static void shiftXLeft(Chip8 &chip);

  // for 0x9XY0
  static void ifNotEqualToY(Chip8 &chip);

  // for 0xANNN
  static void setIToNNN(Chip8 &chip);

  // for 0xBNNN
  static void jumpTo0PlusNNN(Chip8 &chip);

  // for 0xCXNN
  static void setXtoRand(Chip8 &chip);

  // for 0xDXYN
  static void draw(Chip8 &chip);

  // for 0xEX9E
  static void ifKeyIsEqualToX(Chip8 &chip);

  // for 0xEXA1
  static void ifKeyIsNotEqualToX(Chip8 &chip);

  // for 0xFX07
  static void setXtoTimer(Chip8 &chip);

  // for 0xFX0A
  static void setXtoKey(Chip8 &chip);

  // for 0xFX15
  static void setTimerToX(Chip8 &chip);

  // for 0xFX18
  static void setSoundToX(Chip8 &chip);

  // for 0xFX1E
  static void addXToI(Chip8 &chip);

  // for 0xFX29
  static void setIToSprite(Chip8 &chip);

  // for 0xFX33
  static void setToBCD(Chip8 &chip);

  // for 0xFX55
  static void regDump(Chip8 &chip);

  // for 0xFX65
  static void regLoad(Chip8 &chip);
};
#endif
