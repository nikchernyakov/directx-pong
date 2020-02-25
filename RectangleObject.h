#pragma once
#include "Object.h"
#include "SimpleMath.h"

class Game;

class RectangleObject : public Object
{
public:
	RectangleObject(Game& game, 
		DirectX::SimpleMath::Vector2 position, 
		float width,
		float height);
	~RectangleObject();
	void draw() override;
	void update();
protected:
	float m_width = 0;
	float m_widthDiv2 = 0;
	float m_height = 0;
	float m_heightDiv2 = 0;
private:
	ID3D11Buffer* vBuffers[2] = {};
	ID3D11Buffer* ib;
	ID3D11Buffer* constantBuffer;
	UINT strides[2] = { 16, 16 };
	UINT offsets[2] = { 0, 0 };
};

