// Part of SAASound copyright 1998-2004 Dave Hooper <dave@rebuzz.org>
//
// This is the internal implementation (header file) of the SAASound object.
// This is done so that the external interface to the object always stays the same
// (SAASound.h) even though the internal object can change
// .. Meaning future releases don't require relinking everyone elses code against
//    the updated saasound stuff
//
// Version 3.1.3 (8th March 2004)
// (c) 1998-2004 dave @ spc       <dave@rebuzz.org>
//
//////////////////////////////////////////////////////////////////////

#ifndef SAAIMPL_H_INCLUDED
#define SAAIMPL_H_INCLUDED

class CSAASoundInternal : public CSAASound
{
private:
	int m_nCurrentSaaReg;
	bool m_bOutputEnabled;
	bool m_bSync;
	int m_uParam, m_uParamRate;

	CSAAFreq * Osc[6];
	CSAANoise * Noise[2];
	CSAAAmp * Amp[6];
	CSAAEnv * Env[2];

public:
	CSAASoundInternal();
	~CSAASoundInternal();

	void SetSoundParameters(SAAPARAM uParam);
	void WriteAddress(BYTE nReg);
	void WriteData(BYTE nData);
	void WriteAddressData(BYTE nReg, BYTE nData);
	void Clear(void);
	BYTE ReadAddress(void);

	SAAPARAM GetCurrentSoundParameters(void);
	unsigned long GetCurrentSampleRate(void);
	static unsigned long GetSampleRate(SAAPARAM uParam);
	unsigned short GetCurrentBytesPerSample(void);
	static unsigned short GetBytesPerSample(SAAPARAM uParam);

	void GenerateMany(BYTE * pBuffer, unsigned long nSamples);

	int SendCommand(SAACMD nCommandID, long nData);

};

#endif // SAAIMPL_H_INCLUDED
