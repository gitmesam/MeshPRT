#ifndef PRTHIERARCHY_H
#define PRTHIERARCHY_H

#include "d3dUtil.h"
#include "Mesh.h"
#include "PRTEngine.h"
#include "LightSource.h"

class PRTHierarchy{
public:
  PRTHierarchy(IDirect3DDevice9 *device);
  ~PRTHierarchy();

  HRESULT LoadMeshHierarchy(WCHAR* renderMeshFile, WCHAR* approxMeshFile, 
                            WCHAR* directory, WCHAR* extension, DWORD order);

  HRESULT CalculateSHCoefficients(LightSource *lightSource);

  HRESULT CalculateDiffuseColor();

  HRESULT UpdateLighting(LightSource* lightSource);

  bool HasTextures();
  void DrawMesh();
  void LoadEffect(ID3DXEffect* mEffect);
  int GetNumVertices();
  int GetNumFaces();

private:
  HRESULT FillVertexVectors();   
  HRESULT FillColorVector(std::vector<D3DXCOLOR> &colors, Mesh* mesh);
  HRESULT FillVertexVector(std::vector<Vertex> &vec, Mesh* mesh);
  HRESULT SetRenderMeshVertexColors();

  IDirect3DDevice9 *mDevice;
  Mesh* mRenderMesh;
  Mesh* mApproxMesh;
  PRTEngine* mPRTEngine;

  std::vector<Vertex> mRenderMeshVertices;
  std::vector<Vertex> mApproxMeshVertices;
  std::vector<D3DXCOLOR> mRenderMeshVertexColors;
  std::vector<D3DXCOLOR> mApproxMeshVertexColors;
    
  DWORD mOrder;
};

#endif // PRTHIERARCHY_H