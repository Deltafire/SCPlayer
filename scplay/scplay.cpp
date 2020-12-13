/***
Copyright (C) 2011 by Christopher O'Neill

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
***/

#include <stdlib.h>
#include <iostream>
#include <SDL.h>

#include "SCPlayer.h"

int mixerFreq = 44100;

void mixerCallback(void *userdata, Uint8 *stream8, int length)
{
  SCPlayer *player = reinterpret_cast<SCPlayer*> (userdata);
 // std::clog << "Buffer size: " << length << " (requested " << mixerFreq/50 << ")" << std::endl;
 // exit(0);
  player->generate(stream8, length);
}


SDL_AudioDeviceID initSdlAudio(SCPlayer *player)
{
  SDL_Init(SDL_INIT_AUDIO);
  SDL_AudioSpec wanted, obtained;
  SDL_AudioDeviceID dev;
  SDL_zero(wanted);
  wanted.freq = mixerFreq;
  wanted.format = AUDIO_S16;
  wanted.channels = 2;
  wanted.samples = 1024;
  wanted.callback = &mixerCallback;
  wanted.userdata = reinterpret_cast <void*> (player);

  dev = SDL_OpenAudioDevice(NULL, 0, &wanted, &obtained,
                            SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
  if(dev > 0)
      mixerFreq = obtained.freq;
  std::cout << "mixerFreq = " << mixerFreq << std::endl;
  return dev;
}


int main(int argc, char *argv[])
{
  SCPlayer player;
  SDL_AudioDeviceID audioDevice;


  if (argc < 2) {
    std::cerr << "Usage: scplay <filename>" << std::endl;
    exit(1);
  }

  if (!player.load(argv[1])) {
    std::cerr << "Cannot open file " << argv[1] << std::endl;
    exit(1);
  }

  audioDevice = initSdlAudio(&player);
  if(audioDevice == 0) {
    std::cerr << "Failed to initialise audio:" << SDL_GetError() << std::endl;
    SDL_Quit();
    exit(1);
  }

  player.init(mixerFreq);
  std::cout << "Playing: " << argv[1] << std::endl;
  std::cout << "Hit the return key to exit." << std::endl;
  SDL_PauseAudioDevice(audioDevice, 0);
  getchar();
  SDL_PauseAudioDevice(audioDevice, 1);
  SDL_Quit();
  return 0;
}
