#pragma once
#include <d3d11.h>
#include "SimpleMath.h"
#include "Vertex.h"
#include <wrl/client.h>

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

class Game;

class RectObject
{
public:
	RectObject(Game* game,
		Vertex* vertex,
		float width,
		float height);
	void draw();
	Pos getPosition();
	float getPositionX();
	float getPositionY();
	void setPosition(Pos pos);
	void setPositionX(float posX);
	void setPositionY(float posY);
	float getWidth();
	float getHeight();
	Pos getLeftTop();
	Pos getRightTop();
	Pos getLeftBottom();
	Pos getRightBottom();
private:
	Game* m_game;
	ID3D11Buffer* pIndexBuffer;
	ID3D11Buffer* pVertexBuffer;
	ComPtr<ID3D11InputLayout> pInputLayout;
	ComPtr<ID3D11VertexShader> pVertexShader;
	ComPtr<ID3D11PixelShader> pPixelShader;
	ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_SUBRESOURCE_DATA* csd;
	
	Vertex* m_startVertex;
	float m_currentPosX = 0;
	float m_currentPosY = 0;
	float m_width = 0;
	float m_widthDiv2 = 0;
	float m_height = 0;
	float m_heightDiv2 = 0;
};

