#include "BallObject.h"
#include <d3dcompiler.h>
#include "Game.h"
#include "SimpleMath.h"
#include <iostream>

#define M_PI 3.14159265358979323846  /* pi */

using namespace DirectX;

struct ConstantBuffer
{
	DirectX::XMMATRIX transform;
};

BallObject::BallObject(Game* game, Vertex* vertex, float radius)
	: m_game(game), m_startVertex(vertex), m_radius(radius)
{
	m_radius = radius;
	m_radiusDiv2 = radius / 2;

	HRESULT hr;

	Vertex vertices[CIRCLE_RESOLUTION + 1];
	m_radiusX = m_radiusDiv2 / game->screenWidth;
	m_radiusY = m_radiusDiv2 / game->screenHeight;

	std::cout << m_radiusX << " " << m_radiusY << "\n";
	
	vertices[0] = { vertex->pos.x, vertex->pos.y,
			vertex->color.r, vertex->color.g, vertex->color.b, vertex->color.a };
	for (int i = 0; i < CIRCLE_RESOLUTION; ++i)
	{
		vertices[i + 1] = { vertex->pos.x + m_radiusX * static_cast<float>(cos(2 * M_PI * i / CIRCLE_RESOLUTION)),
			vertex->pos.y + m_radiusY * static_cast<float>(sin(2 * M_PI * i / CIRCLE_RESOLUTION)),
			vertex->color.r, vertex->color.g, vertex->color.b, vertex->color.a };
		//std::cout << vertices[i + 1].pos.x << " " << vertices[i + 1].pos.y << "\n";
	}
	/*Vertex vertices[] =
	{
		{vertex->pos.x, vertex->pos.y, vertex->color.r, vertex->color.g, vertex->color.b, vertex->color.a},
		{vertex->pos.x,	vertex->pos.y + m_radiusDiv2, vertex->color.r, vertex->color.g, vertex->color.b, vertex->color.a},
		{vertex->pos.x + m_widthDiv2,	vertex->pos.y - m_heightDiv2, vertex->color.r, vertex->color.g, vertex->color.b, vertex->color.a},
		{vertex->pos.x - m_widthDiv2,	vertex->pos.y + m_heightDiv2, vertex->color.r, vertex->color.g, vertex->color.b, vertex->color.a}
	};*/

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(vertices);
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = vertices;

	m_game->device->CreateBuffer(
		&bd,
		&sd,
		&pVertexBuffer
	);

	for (int i = 0; i < CIRCLE_RESOLUTION - 1; i++)
	{
		indices[i * 3] = 0;
		indices[i * 3 + 1] = i + 1;
		indices[i * 3 + 2] = i + 2;
	}
	indices[CIRCLE_RESOLUTION * 3 - 3] = 0;
	indices[CIRCLE_RESOLUTION * 3 - 2] = CIRCLE_RESOLUTION;
	indices[CIRCLE_RESOLUTION * 3 - 1] = 1;
	
	/*for (int i = 0; i < CIRCLE_RESOLUTION * 3; i += 3)
	{
		std::cout << indices[i] << " " << indices[i + 1] << " " << indices[i + 2] << "\n";
	}*/
	
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	m_game->context->IASetVertexBuffers(
		0u,
		1u,
		&pVertexBuffer,
		&stride,
		&offset
	);

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	m_game->device->CreateBuffer(&ibd, &isd, &pIndexBuffer);

	m_game->context->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0u);

	/*if (!gameStop) {
		// create constant buffer for transformation matrix
		

		value = isLeft ? leftBoxPosition : rightBoxPosition;
		const ConstantBuffer cb =
		{
			{
				DirectX::XMMatrixTranspose(
					DirectX::XMMatrixTranslation(0, value, 0)
				)
			}
		};

	}*/

	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	D3DReadFileToBlob(L"PixelShader.cso", &pBlob);
	m_game->device->CreatePixelShader(
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		nullptr,
		&pPixelShader
	);

	m_game->context->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	D3DReadFileToBlob(L"VertexShader.cso", &pBlob);
	m_game->device->CreateVertexShader(
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		nullptr,
		&pVertexShader
	);

	m_game->context->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 8u, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	m_game->device->CreateInputLayout(
		ied,
		(UINT)std::size(ied),
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&pInputLayout
	);

	CD3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DEFAULT;
	//cbd.Usage = D3D11_USAGE_DYNAMIC;
	//cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.CPUAccessFlags = 0u;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.StructureByteStride = 0u;
	hr = m_game->device->CreateBuffer(&cbd, NULL, &pConstantBuffer);

	//bind constant buffer to vertex shader
	m_game->context->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

}

void BallObject::draw()
{
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	m_game->context->IASetVertexBuffers(
		0u,
		1u,
		&pVertexBuffer,
		&stride,
		&offset
	);
	
	m_game->context->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0u);
	
	m_game->context->PSSetShader(pPixelShader.Get(), nullptr, 0u);
	
	m_game->context->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	
	m_game->context->IASetInputLayout(pInputLayout.Get());

	/*m_game->context->OMSetRenderTargets(
		1u,
		m_game->rtv,
		nullptr
	);*/
	
	m_game->context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*CD3D11_VIEWPORT vp;
	vp.Width = m_wnd->getWidth();
	vp.Height = m_wnd->getWidth();
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_game->context->RSSetViewports(1u, &vp);*/

	/*const unsigned short indices[] =
	{
		0,2,1,
		0,1,3
	};*/

	const ConstantBuffer cb =
	{
		{
			DirectX::XMMatrixTranspose(
				DirectX::XMMatrixTranslation(
					m_startVertex->pos.x + m_currentPosX,
					m_startVertex->pos.y + m_currentPosY,
					0)
			)
		}
	};
	m_game->context->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
	m_game->context->UpdateSubresource(pConstantBuffer.Get(), 0, NULL, &cb, 0, 0);
	
	m_game->context->DrawIndexed((UINT)std::size(indices), 0u, 0u);
}

Pos BallObject::getPosition()
{
	return { m_startVertex->pos.x + m_currentPosX, m_startVertex->pos.y + m_currentPosY };
}

void BallObject::setPosition(Pos pos)
{
	m_currentPosX = pos.x - m_startVertex->pos.x;
	m_currentPosY = pos.y - m_startVertex->pos.y;
}

void BallObject::setPositionX(float posX)
{
	m_currentPosX = posX - m_startVertex->pos.x;
}

void BallObject::setPositionY(float posY)
{
	m_currentPosY = posY - m_startVertex->pos.y;
}

float BallObject::getRadius()
{
	return m_radius;
}

float BallObject::getRadiusX()
{
	return m_radiusX;
}

float BallObject::getRadiusY()
{
	return m_radiusY;
}
