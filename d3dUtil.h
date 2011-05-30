#ifndef D3DUTIL_H
#define D3DUTIL_H

#if defined(DEBUG) | defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

#define UNICODE

#include "d3d9.h"
#include "d3dx9.h"

#include <string>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <stdio.h>

#pragma comment(lib, "dxerr.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

//===============================================================
// Globals for convenient access.
class D3DApp;
extern D3DApp* gd3dApp;
extern IDirect3DDevice9* gd3dDevice;

WCHAR* Concat(const WCHAR *a, const WCHAR *b);
WCHAR* AppendToRootDir(const WCHAR *b);

#define MESHLAB
WCHAR* GetRootDir();

//===============================================================
// Clean up

#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

// print debug
void PD(HRESULT, const WCHAR*);
void PD(HRESULT, const char*);
void CharArrayToWCharArray(const char* in, WCHAR* out);
void DbgOutInt(std::string label, int value );

#endif // D3DUTIL_H
