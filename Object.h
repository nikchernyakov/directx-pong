#pragma once
#include <d3d11.h>
#include "SimpleMath.h"

class Game;

class Object
{
public:
	Object(Game& game, DirectX::SimpleMath::Vector2 position);
	virtual ~Object();
	virtual void draw() = 0;
protected:
	Game& m_game;
	ID3D11Buffer* m_vertexBuffer = nullptr;

	DirectX::SimpleMath::Vector4 m_position;
};

