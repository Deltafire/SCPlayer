#ifndef SCPLAYER_H
#define SCPLAYER_H

#include "SAASound.h"
extern "C" {
#include "Z80.h"
}

class SCPlayer
{
 public:
  SCPlayer();
  ~SCPlayer();

  bool load(const char* filename);
  bool init(const int mixerFreq);
  void generate(unsigned char *buffer, const int length);

 private:
  byte _ram[0x8000];
  bool _eTracker;
  int _period;
  LPCSAASOUND _saa;
};

#endif
