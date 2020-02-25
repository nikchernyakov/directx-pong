#include "RectObject.h"
#include <d3dcompiler.h>
#include "Game.h"

struct ConstantBuffer
{
	DirectX::XMMATRIX transform;
};

RectObject::RectObject(Game* game, Vertex* vertex, float width, float height)
	: m_game(game), m_startVertex(vertex), m_width(width), m_height(height)
{
	m_widthDiv2 = width / 2;
	m_heightDiv2 = height / 2;

	HRESULT hr;
	
	Vertex vertices[] =
	{
		{vertex->pos.x + m_widthDiv2, vertex->pos.y + m_heightDiv2, vertex->color.r, vertex->color.g, vertex->color.b, vertex->color.a},
		{vertex->pos.x - m_widthDiv2,	vertex->pos.y - m_heightDiv2, vertex->color.r, vertex->color.g, vertex->color.b, vertex->color.a},
		{vertex->pos.x + m_widthDiv2,	vertex->pos.y - m_heightDiv2, vertex->color.r, vertex->color.g, vertex->color.b, vertex->color.a},
		{vertex->pos.x - m_widthDiv2,	vertex->pos.y + m_heightDiv2, vertex->color.r, vertex->color.g, vertex->color.b, vertex->color.a}
	};

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

	const unsigned short indices[] =
	{
		0,2,1,
		0,1,3
	};


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

void RectObject::draw()
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

	const unsigned short indices[] =
	{
		0,2,1,
		0,1,3
	};

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

Pos RectObject::getPosition()
{
	return { getPositionX(), getPositionY() };
}

float RectObject::getPositionX()
{
	return m_startVertex->pos.x + m_currentPosX;
}

float RectObject::getPositionY()
{
	return m_startVertex->pos.y + m_currentPosY;
}

void RectObject::setPosition(Pos pos)
{
	m_currentPosX = pos.x - m_startVertex->pos.x;
	m_currentPosY = pos.y - m_startVertex->pos.y;
}

void RectObject::setPositionX(float posX)
{
	m_currentPosX = posX - m_startVertex->pos.x;
}

void RectObject::setPositionY(float posY)
{
	m_currentPosY = posY - m_startVertex->pos.y;
}

float RectObject::getWidth()
{
	return m_width;
}

float RectObject::getHeight()
{
	return m_height;
}

Pos RectObject::getLeftTop()
{
	return { getPositionX() - m_widthDiv2, getPositionY() + m_heightDiv2 };
}

Pos RectObject::getRightTop()
{
	return { getPositionX() + m_widthDiv2, getPositionY() + m_heightDiv2 };
}

Pos RectObject::getLeftBottom()
{
	return { getPositionX() - m_widthDiv2, getPositionY() - m_heightDiv2 };
}

Pos RectObject::getRightBottom()
{
	return { getPositionX() + m_widthDiv2, getPositionY() - m_heightDiv2 };
}
