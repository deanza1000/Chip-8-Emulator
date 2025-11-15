
#ifndef CHIP8
#define CHIP8
#include <string>
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
#endif
