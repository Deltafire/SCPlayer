#include "SCPlayer.h"
#include <string.h>
#include <stdio.h>
#include "etracker_bin.h"
#include "SAASound.h"
extern "C" {
#include "Z80.h"
}

#ifdef SCP_DEBUG
#include <iostream>
#define debug_print(s) std::cerr << s << std::endl;
#else
#define debug_print(s) // debug_print
#endif

const int stub = 0x7000;


// Implementation class
class SCPlayer::SCPlayer_impl
{
  byte _ram[0x8000];
  bool _eTracker;
  int _period;
  LPCSAASOUND _saa;
 public:
  SCPlayer_impl() {}
  bool load(const char* filename);
  bool init(const int mixerFreq);
  void generate(unsigned char *buffer, const int length);
  void setRam(dword addr, byte val);
  byte getRam(dword addr);
  void saaWriteAddress(byte val);
  void saaWriteData(byte val);

};
// Wrapper class functions (Cheshire Cat)
bool SCPlayer::load(const char* filename)
  { _impl->load(filename); }
bool SCPlayer::init(const int mixerFreq)
  { _impl->init (mixerFreq); }
void SCPlayer::generate(unsigned char *buffer, const int length)
  { _impl->generate(buffer, length); }


// Functions called by Z80 emulator (callbacks)

// We use the patch instruction to stop the emulator
void Z80_Patch (Z80_Regs *regs)
{
  regs->PC.D = stub; // Reset PC
  Z80_Running = 0;
}
int Z80_Interrupt() { return 0; }
void Z80_Reti() { return; }
void Z80_Retn() { return; }
unsigned Z80_RDMEM (void *userdata, dword addr)
{
  SCPlayer::SCPlayer_impl *scp = reinterpret_cast<SCPlayer::SCPlayer_impl *> (userdata);
  if(addr < 0x7000) debug_print("readRAM [" << addr << "] = " << (int)scp->getRam(addr));
  //debug_print("readRAM [" << addr << "] = " << (int)ram[addr]);
  return scp->getRam(addr);
}
void Z80_WRMEM (void *userdata, dword addr, byte val)
{
  SCPlayer::SCPlayer_impl *scp = reinterpret_cast<SCPlayer::SCPlayer_impl *> (userdata);
//  if (addr > 0x9000) debug_print("RAM [" << addr << "] = " << (int)val);
  scp->setRam(addr, val);
}
byte Z80_In (void *userdata, word port)
{
  debug_print(std::hex << "IN: [" << port << "]");
  return 0xff;
}
void Z80_Out (void *userdata, word port, byte val)
{
  SCPlayer::SCPlayer_impl *scp = reinterpret_cast<SCPlayer::SCPlayer_impl *> (userdata);
  if(port == 0x01ff)
    scp->saaWriteAddress(val);
  else
    scp->saaWriteData(val);
//  debug_print(std::hex << "OUT" << " [" << port << "] = " << (int)val);
}

// Globals used by the Z80 emu
int Z80_IRQ;    


SCPlayer::SCPlayer() : _impl(new SCPlayer_impl())
{
}


SCPlayer::~SCPlayer()
{
  delete _impl;
}


bool SCPlayer::SCPlayer_impl::load(const char* filename)
{
  FILE *f;
  char buf[8];

  if (!(f = fopen(filename,"rb"))) return false;
  fseek(f, 0x0A, SEEK_SET);
  fread(buf, 1, 8, f);
  if (!strncmp(buf, "ETracker", 8)) {
    debug_print("ETracker data file detected");
    memcpy(_ram, etracker_bin, 1203);
    _eTracker = true;
  }
  else
  {
    _eTracker = false;
  }
  rewind(f);
  fread(_ram+(_eTracker ? 0x04b3 :0x0000), 1, 0x7000, f);
  fclose(f);
  if (!strncmp((char *)(&_ram[0x0013]), "\1\xff\1\x3e\x1c\xed", 6))
  {
    debug_print("Patch #1");
    _ram[0x0001] = 1;
    _ram[0x0002] = 0;
  }
  else if (!strncmp((char *)&_ram[0x0000], "\x43\x72\x3d\xc2\x23\x81", 6))
  {
    _ram[0x0001] = 1;
  }
  else if (!strncmp((char*)&_ram[0x0000], "\x21\xb3\x84\xc3\xef\x83", 6))
  {
    // eTracker compiled song
    _eTracker = true;
  }
  return true;
}


bool SCPlayer::SCPlayer_impl::init(const int mixerFreq)
{
  // Initialise SAASound
  debug_print("Initialising SAA emulator.");
  _saa = CreateCSAASound();
  _saa->SetSoundParameters(SAAP_NOFILTER | SAAP_44100 | SAAP_16BIT | SAAP_STEREO);
  if (mixerFreq != 44100)
    _saa->SendCommand(SAACMD_SetSampleRate, mixerFreq);
  _period = mixerFreq / 50;

  // Initialise CPU
  debug_print("Initialising Z80 CPU.");
  Z80_Reset();
  Z80_Regs r;
  Z80_GetRegs(&r);
  r.userdata = reinterpret_cast<void *> (this);
  r.PC.D = stub;
  Z80_SetRegs(&r);
  _ram[stub] = 0xcd; // CALL
  _ram[stub+1] = 0; _ram[stub+2] = 0x80;
  _ram[stub+3] = 0xed; _ram[stub+4] = 0xfe; // Patch
  if(_eTracker == true)
  {
    debug_print("Initialising eTracker player.");
    Z80();
    _ram[stub+1] = 6;
  }

  debug_print(std::hex);

  return true;
}


void SCPlayer::SCPlayer_impl::generate(unsigned char *buffer, const int length)
{
  // 'length' could be anything, ensure the Z80 code is called at 50Hz
  int samplesToPlay = length / 4; // 16-bit stereo
  static int remainder = 0;

  while (remainder && samplesToPlay)
  {
    int samples = remainder<samplesToPlay ? remainder : samplesToPlay;
    _saa->GenerateMany(buffer, samples);
    buffer += samples * 4;
    remainder -= samples;
    samplesToPlay -= samples;
  }

  while(samplesToPlay >= _period)
  {
    Z80();
    _saa->GenerateMany(buffer, _period);
    buffer += _period * 4;
    samplesToPlay -= _period;
  }

  if (samplesToPlay)
  {
    Z80();
    _saa->GenerateMany(buffer, samplesToPlay);
    remainder = _period - samplesToPlay;
  }
}


void SCPlayer::SCPlayer_impl::setRam(dword addr, byte val)
{
  _ram[addr & 0x7fff] = val;
}


byte SCPlayer::SCPlayer_impl::getRam(dword addr)
{
  return _ram[addr & 0x7fff];
}


void SCPlayer::SCPlayer_impl::saaWriteAddress(byte val)
{
  _saa->WriteAddress(val);
}


void SCPlayer::SCPlayer_impl::saaWriteData(byte val)
{
  _saa->WriteData(val);
}
