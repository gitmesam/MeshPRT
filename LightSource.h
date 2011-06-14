#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

class LightSource {
public:
 float* GetSHCoeffsRed(){ return mSHCoeffsRed; }
 float* GetSHCoeffsGreen(){ return mSHCoeffsGreen; }
 float* GetSHCoeffsBlue() { return mSHCoeffsBlue; }

 virtual HRESULT CalculateSHCoefficients(DWORD order) = 0;

protected:
  float mSHCoeffsRed[D3DXSH_MAXORDER*D3DXSH_MAXORDER];
  float mSHCoeffsGreen[D3DXSH_MAXORDER*D3DXSH_MAXORDER];
  float mSHCoeffsBlue[D3DXSH_MAXORDER*D3DXSH_MAXORDER];
};

#endif // LIGHTSOURCE_H
