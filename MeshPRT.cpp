#include "d3dApp.h"
#include "DirectInput.h"
#include "GfxStats.h"
#include "Camera.h"
#include "PRTEngine.h"
#include "CubeMap.h"
#include "Light.h"
#include "PRTHierarchy.h"

class MeshPRT : public D3DApp
{
public:
  MeshPRT(std::string winCaption, ID3DXMesh* mesh, D3DDEVTYPE devType,
          DWORD requestedVP);
  ~MeshPRT();

  bool IsInitialized();

  bool checkDeviceCaps();
  void onLostDevice();
  void onResetDevice();
  void updateScene(float dt);
  void drawScene();

  // Helper methods
  void buildFX();
  void buildViewMtx();
  void buildProjMtx();
  
private:
  HRESULT Init(ID3DXMesh* mesh);
  HRESULT UpdateLighting();
  void SetTechnique();
    
  bool initialized;

  float reflectivity;

  GfxStats* mGfxStats;

  ID3DXEffect* mFX;
  D3DXHANDLE   mhPRTLighting;  
  D3DXHANDLE   mhView;
  D3DXHANDLE   mhProjection;
  D3DXHANDLE   mhEyePosW;
  D3DXHANDLE   mhReflectivity;

  D3DXMATRIX mWorld;
  D3DXMATRIX mProj;

  Camera *mCamera;
  CubeMap* mCubeMap;
  PRTHierarchy* mPRTHierarchy;
};

bool StartDirectX(ID3DXMesh* mesh) {
  // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
  _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

  MeshPRT app( "MeshPRT", mesh, D3DDEVTYPE_HAL,
               D3DCREATE_HARDWARE_VERTEXPROCESSING);
  gd3dApp = &app;

  if(!app.IsInitialized()) return false;

  DirectInput di( DISCL_NONEXCLUSIVE|DISCL_FOREGROUND,
                  DISCL_NONEXCLUSIVE|DISCL_FOREGROUND);
  gDInput = &di;

  return gd3dApp->run();
}

MeshPRT::MeshPRT( std::string winCaption, ID3DXMesh* mesh, D3DDEVTYPE devType, DWORD requestedVP )
  : D3DApp(winCaption, devType, requestedVP)
{
  mPRTHierarchy = 0;
  
  if(!checkDeviceCaps())
  {
    MessageBox(0, L"checkDeviceCaps() Failed", 0, 0);
    PostQuitMessage(0);
  }
  initialized = false;

  HRESULT hr = Init(mesh);
  PD(hr, L"initialize");
  if(FAILED(hr)){
    initialized = false;
  }
  else{
    initialized = true;
  }
}

MeshPRT::~MeshPRT()
{
  delete mGfxStats;
  delete mCamera;
  delete mCubeMap;
  delete mPRTHierarchy;
}

bool MeshPRT::checkDeviceCaps()
{
  D3DCAPS9 caps;
  gd3dDevice->GetDeviceCaps(&caps);

  // Check for vertex shader version 2.0 support.
  if( caps.VertexShaderVersion < D3DVS_VERSION(2, 0) )
    return false;

  // Check for pixel shader version 2.0 support.
  if( caps.PixelShaderVersion < D3DPS_VERSION(2, 0) )
    return false;

  return true;
}

void MeshPRT::onLostDevice()
{
  mGfxStats->onLostDevice();
  mFX->OnLostDevice();
}

void MeshPRT::onResetDevice()
{
  mGfxStats->onResetDevice();
  mFX->OnResetDevice();

  // The aspect ratio depends on the backbuffer dimensions, which can
  // possibly change after a reset.  So rebuild the projection matrix.
  buildProjMtx();
}

bool MeshPRT::IsInitialized() {
  return initialized;
}

HRESULT MeshPRT::Init(ID3DXMesh* mesh) {
  HRESULT hr;
    
  reflectivity = 0.3f;
  DWORD order = 6;
    
  buildFX();

  mCubeMap = new CubeMap(gd3dDevice);
  hr = mCubeMap->LoadCubeMap(L"cubemaps/", L"stpeters_cross", L".dds");
  PD(hr, L"load cube map");
  if(FAILED(hr)) return hr;

  mPRTHierarchy = new PRTHierarchy(gd3dDevice);
  mPRTHierarchy->LoadMeshHierarchy(L"bigship1",
                                   L"bigship1",
                                   L"models/",
                                   L".x",
                                   order);
    
  mPRTHierarchy->CalculateSHCoefficients(mCubeMap);
  mPRTHierarchy->CalculateDiffuseColor();
  mPRTHierarchy->LoadEffect(mFX);
  
  mGfxStats = new GfxStats();

  D3DXMatrixIdentity(&mWorld);

  onResetDevice();

  mCamera = new Camera();

  return D3D_OK;
}

void MeshPRT::updateScene(float dt)
{
  mGfxStats->setVertexCount(mPRTHierarchy->GetNumVertices());
  mGfxStats->setTriCount(mPRTHierarchy->GetNumFaces());
  mGfxStats->update(dt);

  mCamera->update(dt);

  gDInput->poll();
  
  if( gDInput->keyDown(DIK_Z) ) {
    reflectivity = reflectivity - 0.0001f;

    if(reflectivity < 0){
      reflectivity = 0;
    }
  }

  if( gDInput->keyDown(DIK_X) ) {
    reflectivity = reflectivity + 0.0001f;

    if(reflectivity > 1){
      reflectivity = 1;
    }
  }
}

HRESULT MeshPRT::UpdateLighting(){
  HRESULT hr;
    
  return D3D_OK;
}


void MeshPRT::drawScene()
{
  D3DXCOLOR color = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.0f);
  gd3dDevice->Clear(0, 0, D3DCLEAR_ZBUFFER, 0xffeeeeee, 1.0f, 0);
  gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET, color, 1.0f, 0);

  gd3dDevice->BeginScene();
    
  D3DXMATRIX mWVP = mCamera->view() * mProj;
  mCubeMap->DrawCubeMap(&mWVP);
  mFX->SetTexture( "EnvMap", mCubeMap->GetTexture() );
  mFX->SetFloat( "gReflectivity", reflectivity );
    
  mFX->SetBool("useTextures", mPRTHierarchy->HasTextures());
  
  mFX->SetTechnique(mhPRTLighting);
    
  mFX->SetValue(mhEyePosW, &(mCamera->pos()), sizeof(D3DXVECTOR3));
  mFX->SetMatrix(mhView, &(mCamera->view()));  
  mFX->SetMatrix(mhProjection, &mProj);

  // Begin passes.
  UINT numPasses = 0;
  mFX->Begin(&numPasses, 0);
  for(UINT i = 0; i < numPasses; ++i)
  {
    mFX->BeginPass(i);
    mPRTHierarchy->DrawMesh();
    mFX->EndPass();
  }
  mFX->End();
  
  mGfxStats->display();

  gd3dDevice->EndScene();
    
  gd3dDevice->Present(0, 0, 0, 0);
}

void MeshPRT::buildFX()
{
  WCHAR* effectName = L"shader/diffuse.fx";
  LoadEffectFile(gd3dDevice, effectName, 0, D3DXSHADER_DEBUG, &mFX);
  
  mhPRTLighting           = mFX->GetTechniqueByName("PRTLighting");
  mhView                  = mFX->GetParameterByName(0, "gView");
  mhProjection            = mFX->GetParameterByName(0, "gProjection");
  mhEyePosW               = mFX->GetParameterByName(0, "gEyePosW");
  mhReflectivity          = mFX->GetParameterByName(0, "gReflectivity");

  PD(D3D_OK, L"done building FX");
}

void MeshPRT::buildProjMtx()
{
  float w = (float)md3dPP.BackBufferWidth;
  float h = (float)md3dPP.BackBufferHeight;
  D3DXMatrixPerspectiveFovLH(&mProj, D3DX_PI * 0.25f, w/h, 1.0f, 5000.0f);
}
