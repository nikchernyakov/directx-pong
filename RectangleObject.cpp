#include "RectangleObject.h"
#include <d3dcompiler.h>
#include <iostream>
#include <directxmath.h>
#include "Game.h"

using namespace DirectX::SimpleMath;

RectangleObject::RectangleObject(Game& game,
	Vector2 position,
	float width,
	float height)
  : Object(game, position)
{
	m_widthDiv2 = width / 2;
	m_heightDiv2 = height / 2;

	DirectX::XMFLOAT4 positions[] = {
		DirectX::XMFLOAT4(position.x + m_widthDiv2, position.y + m_heightDiv2, m_position.z, m_position.w),
		DirectX::XMFLOAT4(position.x - m_widthDiv2, position.y - m_heightDiv2, m_position.z, m_position.w),
		DirectX::XMFLOAT4(position.x + m_widthDiv2, position.y - m_heightDiv2, m_position.z, m_position.w),
		DirectX::XMFLOAT4(position.x - m_widthDiv2, position.y + m_heightDiv2, m_position.z, m_position.w),
	};
	
	DirectX::XMFLOAT4 points[8] = {
		positions[0],
		DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		positions[1],
		DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		positions[2],
		DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		positions[3],
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};

	DirectX::XMFLOAT4 colors[] = {
		DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};

	int indexes[] = { 0,1,2, 0,1,3 };

	D3D11_BUFFER_DESC dataBufDesc = {};
	dataBufDesc.Usage = D3D11_USAGE_DEFAULT;
	dataBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	dataBufDesc.CPUAccessFlags = 0;
	dataBufDesc.MiscFlags = 0;
	dataBufDesc.StructureByteStride = 0;
	dataBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * std::size(positions);

	ID3D11Buffer* pb;
	ID3D11Buffer* cb;

	D3D11_SUBRESOURCE_DATA positionsData = {};
	positionsData.pSysMem = positions;
	positionsData.SysMemPitch = 0;
	positionsData.SysMemSlicePitch = 0;
	D3D11_SUBRESOURCE_DATA colorsData = {};
	colorsData.pSysMem = colors;
	colorsData.SysMemPitch = 0;
	colorsData.SysMemSlicePitch = 0;

	game.device->CreateBuffer(&dataBufDesc, &positionsData, &pb);
	game.device->CreateBuffer(&dataBufDesc, &colorsData, &cb);


	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * std::size(points);

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = points;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	ID3D11Buffer* vb;
	game.device->CreateBuffer(&vertexBufDesc, &vertexData, &vb);


	D3D11_BUFFER_DESC indexBufDesc = {};
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	indexBufDesc.ByteWidth = sizeof(int) * std::size(indexes);

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indexes;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	game.device->CreateBuffer(&indexBufDesc, &indexData, &ib);
	ID3D11Buffer* vBuffers[] = { pb, cb };

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(Matrix);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Create the buffer.
	auto res = game.device->CreateBuffer(&cbDesc, nullptr, &constantBuffer);

	// Set the buffer.
	game.context->VSSetConstantBuffers(0, 1, &constantBuffer);

	m_game.context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
	m_game.context->IASetVertexBuffers(0, 2, vBuffers, strides, offsets);
}

RectangleObject::~RectangleObject()
{
	
}

void RectangleObject::draw()
{
	update();
	float speed = 0.2f;
	Matrix upTranslation = Matrix::CreateTranslation(DirectX::SimpleMath::operator*(Vector3(0, 0, 0), speed));
	m_game.context->UpdateSubresource(constantBuffer, 0, NULL, &upTranslation, 0, 0);
	m_game.context->DrawIndexed(6, 0, 0);
}

void RectangleObject::update()
{
	
}

