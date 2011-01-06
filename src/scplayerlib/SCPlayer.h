#ifndef SCPLAYER_H
#define SCPLAYER_H

class SCPlayer
{
 public:
  SCPlayer();
  ~SCPlayer();

  bool load(const char* filename);
  bool init(const int mixerFreq);
  void generate(unsigned char *buffer, const int length);

  class SCPlayer_impl;
 private:
  SCPlayer_impl* _impl;
};

#endif
