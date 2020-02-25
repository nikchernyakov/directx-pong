#pragma once
#include <d3d11.h>
#include "SimpleMath.h"
#include "Vertex.h"
#include <wrl/client.h>

#define CIRCLE_RESOLUTION 36

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

class Game;

class BallObject
{
public:
	BallObject(Game* game,
		Vertex* vertex,
		float radius);
	void draw();
	Pos getPosition();
	void setPosition(Pos pos);
	void setPositionX(float posX);
	void setPositionY(float posY);
	float getRadius();
	float getRadiusX();
	float getRadiusY();
private:
	Game* m_game;
	ID3D11Buffer* pIndexBuffer;
	ID3D11Buffer* pVertexBuffer;
	ComPtr<ID3D11InputLayout> pInputLayout;
	ComPtr<ID3D11VertexShader> pVertexShader;
	ComPtr<ID3D11PixelShader> pPixelShader;
	ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_SUBRESOURCE_DATA* csd;
	unsigned short indices[CIRCLE_RESOLUTION * 3];
	
	Vertex* m_startVertex;
	float m_currentPosX = 0;
	float m_currentPosY = 0;
	float m_radius = 0;
	float m_radiusX = 0;
	float m_radiusY = 0;
	float m_radiusDiv2 = 0;
};

