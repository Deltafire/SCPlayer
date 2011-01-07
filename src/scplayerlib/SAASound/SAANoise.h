// Part of SAASound copyright 1998-2004 Dave Hooper <dave@rebuzz.org>
//
// SAANoise.h: interface for the CSAANoise class.
//
// Version 3.1.3 (8th March 2004)
// (c) 1998-2004 dave @ spc       <dave@rebuzz.org>
//
//////////////////////////////////////////////////////////////////////

#ifndef SAANOISE_H_INCLUDED
#define SAANOISE_H_INCLUDED

class CSAANoise  
{
private:
	unsigned long m_nCounter;
	unsigned long m_nAdd;
	bool m_bSync; // see description of "SYNC" bit of register 28
	BYTE m_nSampleRateMode; // 0=44100, 1=22050; 2=11025
	unsigned long m_nSampleRateTimes4K; // = (44100*4096) when RateMode=0, for example
	int m_nSourceMode;
	static const unsigned long cs_nAddBase; // nAdd for 31.25 kHz noise at 44.1 kHz samplerate

	// pseudo-random number generator
	unsigned long m_nRand;

	void ChangeLevel(void);
	

public:
	CSAANoise();
	CSAANoise(unsigned long seed);
	~CSAANoise();

	void SetSource(int nSource);
	void Trigger(void);
	void SetSampleRateMode(int nSampleRateMode);
	void Seed(unsigned long seed);

	unsigned short Tick(void);
	unsigned short Level(void) const;
	unsigned short LevelTimesTwo(void) const;
	void Sync(bool bSync);

};

#endif	// SAANOISE_H_INCLUDED
