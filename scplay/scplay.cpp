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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>
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

void usage(char *a0) {
  std::cerr << "Usage: " << a0 << " [-d duration] [-n] <filename>" << std::endl;
  std::cerr << "  -d  Stop playback after 'duration' seconds" << std::endl;
  std::cerr << "  -n  No looping (eTracker only)" << std::endl;
  std::cerr << "  -o  Output raw S16 stereo audio to stdout" << std::endl;
  exit(EXIT_FAILURE);
}

void signalHandler(int s)
{
  exit(0);
}

int main(int argc, char *argv[])
{
  SCPlayer player;
  SDL_AudioDeviceID audioDevice;

  int opt, duration;
  bool noLoop = false;
  bool toStdOut = false;
  char *filename;
  while ((opt = getopt(argc, argv, "ond:")) != -1) {
    switch (opt) {
    case 'n':
      noLoop = true;
      break;
    case 'd':
      duration = atoi(optarg);
      break;
    case 'o':
      toStdOut = true;
      break;
    default:
      usage(argv[0]);
    }
  }
  if (optind >= argc) {
    usage(argv[0]);
  }

  filename = argv[optind];

  if (!player.load(filename)) {
    std::cerr << "Cannot open file " << filename << std::endl;
    exit(EXIT_FAILURE);
  }

  if(toStdOut) {
    player.init(mixerFreq);
    duration *= mixerFreq;
    int tickSamples = 4*mixerFreq/50;
    Uint8 buffer[tickSamples];
    while(!player.hasLooped()) {
      player.generate(buffer, tickSamples);
      write(1, buffer, tickSamples);
      if(duration && player.getSamplesPlayed() >= duration)
        break;
    }
    exit(0);
  }

  audioDevice = initSdlAudio(&player);
  if(audioDevice == 0) {
    std::cerr << "Failed to initialise audio:" << SDL_GetError() << std::endl;
    SDL_Quit();
    exit(1);
  }
  player.init(mixerFreq);
  duration *= mixerFreq; // seconds -> samples
  std::cout << "Playing: " << filename << std::endl;
  std::cout << "CTRL-C to exit." << std::endl;
  SDL_PauseAudioDevice(audioDevice, 0);
  signal(SIGINT, signalHandler);
  while(true) {
    sleep(1);
    if (noLoop && player.hasLooped())
      break;
    if (duration && player.getSamplesPlayed() >= duration)
      break;
  }
  SDL_PauseAudioDevice(audioDevice, 1);
  SDL_Quit();
  return 0;
}
