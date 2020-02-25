#include "Object.h"
#include "Game.h"

Object::Object(Game& game, DirectX::SimpleMath::Vector2 position)
: m_game(game), m_position(DirectX::SimpleMath::Vector4(position.x, position.y, 0.5f, 1.0f))
{
	
}

Object::~Object()
{
}
