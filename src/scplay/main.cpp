/* sc_play
 * Player for Sam Coupé music files
 *
 * Christopher O'Neill 30/12/2010
 */

#include <iostream>
#include <SDL.h>

#include "SCPlayer.h"

const int mixerFreq = 44100;

void mixerCallback(void *userdata, Uint8 *stream8, int length)
{
  SCPlayer *player = reinterpret_cast<SCPlayer*> (userdata);
 // std::clog << "Buffer size: " << length << " (requested " << mixerFreq/50 << ")" << std::endl;
 // exit(0);
  player->generate(stream8, length);
}


int sdlInit(SCPlayer *player)
{
  SDL_Init(SDL_INIT_AUDIO);
  SDL_AudioSpec wanted;
  wanted.freq = mixerFreq;
  wanted.format = AUDIO_S16;
  wanted.channels = 2;
  wanted.samples = mixerFreq / 50;   // 50hz ticks
  wanted.callback = &mixerCallback;
  wanted.userdata = reinterpret_cast <void*> (player);
  player->init(mixerFreq);

  return SDL_OpenAudio(&wanted, NULL);
  // Note: SDL docs say that the above call is *guaranteed* to open the audio
  // driver with the specified specs; this is a blatent lie!  During testing I
  // found that OSX gave me a buffer size of 'samples' samples (where 1 sample
  // is 4 bytes, 16 bits X 2).  Ubuntu give me half that amount (I assume they
  // considered the left & right channel to be 2 samples), and on Windows I was
  // given some random large value, which is going to make syncronisation with
  // the GUI akward.
}


int main(int argc, char *argv[])
{
  SCPlayer player;

  if (argc < 2) {
    std::cerr << "Usage: scplay <filename>" << std::endl;
    exit(1);
  }

  if (!player.load(argv[1])) {
    std::cerr << "Cannot open file " << argv[1] << std::endl;
    exit(1);
  }

  if(sdlInit(&player) != 0) {
    std::cerr << "Failed to initialise audio:" << SDL_GetError() << std::endl;
    SDL_Quit();
    exit(1);
  }

  std::cout << "Playing: " << argv[1] << std::endl;
  std::cout << "Hit the return key to exit." << std::endl;
  SDL_PauseAudio(0);
  getchar();
  SDL_PauseAudio(1);
  SDL_Quit();
  return 0;
}
