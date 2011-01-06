#include "SCPlayer.h"
#include <string.h>
#include <stdio.h>
#include "etracker_bin.h"

#ifdef SCP_DEBUG
#include <iostream>
#define debug_print(s) std::cerr << s << std::endl;
#else
#define debug_print(s) // debug_print
#endif

static byte *ram;
static LPCSAASOUND saa;
const int stub = 0x7000;

int Z80_IRQ = 0;

void Z80_Patch (Z80_Regs *regs)
{
  // Reset PC
  regs->PC.D = stub;
  Z80_Running = 0;
}


int Z80_Interrupt() { return 0; }
void Z80_Reti() { return; }
void Z80_Retn() { return; }


unsigned Z80_RDMEM (dword addr)                                                                                  
{                                                                                                                      
  if(addr < 0x7000) debug_print("readRAM [" << addr << "] = " << (int)ram[addr]);
  //debug_print("readRAM [" << addr << "] = " << (int)ram[addr]);
  return ram[addr % 0x8000];
}                                                                                                                      
                                                                                                                       
                                                                                                                       
void Z80_WRMEM (dword addr, byte val)                                                                       
{                                                                                                                      
//  if (addr > 0x9000) debug_print("RAM [" << addr << "] = " << (int)val);
  ram[addr % 0x8000] = val;
}


byte Z80_In (word port)
{
  debug_print(std::hex << "IN: [" << port << "]");
  return 0xff;
}


void Z80_Out (word port, byte val)
{
  if(port == 0x01ff)
    saa->WriteAddress(val);
  else
    saa->WriteData(val);
//  debug_print(std::hex << "OUT" << " [" << port << "] = " << (int)val);
}



SCPlayer::SCPlayer()
{
  // Nothing to do
}


SCPlayer::~SCPlayer()
{
  // Nothing to do
}



bool SCPlayer::load(const char* filename)
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


bool SCPlayer::init(const int mixerFreq)
{
  // Initialise SAASound
  debug_print("Initialising SAA emulator.");
  saa = _saa = CreateCSAASound();
  _saa->SetSoundParameters(SAAP_NOFILTER | SAAP_44100 | SAAP_16BIT | SAAP_STEREO);
  if (mixerFreq != 44100)
    _saa->SendCommand(SAACMD_SetSampleRate, mixerFreq);
  _period = mixerFreq / 50;

  // Initialise CPU
  debug_print("Initialising Z80 CPU.");
  ram = _ram;
  Z80_Reset();
  Z80_Regs r;
  Z80_GetRegs(&r);
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

  // Load ROM
  //memcpy(_ram, abSAMROM, 0x8000);
  debug_print(std::hex);

  return true;
}


void SCPlayer::generate(unsigned char *buffer, const int length)
{
  // 'length' could be anything, make sure the Z80 code is called at 50Hz
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
