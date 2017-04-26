//-----------------------------------
//FILE : shader_tools.cpp
//-----------------------------------

#include "shader_tools.h"

Shader::Shader()
:	m_vertexShader(0),
	m_pixelShader(0),
	m_geometryShader(0),
	m_layout(0),
	m_matrixBuffer(0),
	m_sampleState(0),
	m_renderTargetView(0),
	m_depthStencilView(0)
{
}

Shader::~Shader()
{
}

bool Shader::Compile(ID3D11Device* device, HWND hwnd, WCHAR* vs, WCHAR* ps, WCHAR* gs)
{
	bool result;

	// Initialize the vertex and pixel shaders.
	result = InitShader(device, hwnd, vs, ps, gs);
	if (!result)
	{
		return false;
	}

	return true;
}

void Shader::Render(ID3D11DeviceContext* context, int indexcount)
{
	RenderShader(context, indexcount);
}

void Shader::Shutdown()
{
	ShutdownShader();
}

ID3D11ShaderResourceView* Shader::GetSRV()
{
	return m_SRV;
}

//-------------------------------------
//Setup vertex shader, pixel shader, geometry shader
//Create input layout to vertex shader
//-------------------------------------
bool Shader::InitShader(ID3D11Device* device, HWND hwnd, WCHAR* vs, WCHAR* ps, WCHAR* gs)
{
	HRESULT result;

	ID3DBlob* errorMessage=0;
	ID3DBlob* vertexShaderBuffer=0;
	ID3DBlob* pixelShaderBuffer=0;
	ID3DBlob* geometryShaderBuffer = 0;

	//set shaderflag
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined _DEBUG || defined DEBUG
	shaderFlags = D3DCOMPILE_DEBUG;
#endif

	 //Compile the vertex shader code.
	if (vs) {
		result = D3DCompileFromFile(vs, NULL, NULL, "vs_main", "vs_5_0", shaderFlags, 0,
			&vertexShaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				MessageBox(hwnd, vs, (LPCWSTR)errorMessage->GetBufferPointer(), MB_OK);
			}
			else
			{
				MessageBox(hwnd, vs, L"Missing Vertex Shader File", MB_OK);
			}

			return false;
		}

			// Create the vertex shader from the buffer.
			result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
			if (FAILED(result))
			{
				return false;
			}
	}

	// Compile the pixel shader code.
	if (ps)
	{
		result = D3DCompileFromFile(ps, NULL, NULL, "ps_main", "ps_5_0", shaderFlags, 0,
			&pixelShaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				MessageBox(hwnd, ps, (LPCWSTR)errorMessage->GetBufferPointer(), MB_OK);
			}
			else
			{
				MessageBox(hwnd, ps, L"Missing Pixel Shader File", MB_OK);
			}

			return false;
		}
			// Create the pixel shader from the buffer.
			result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
			if (FAILED(result))
			{
				return false;
			}
	}

	// Compile the geometry shader code.
	if (gs)
	{
		result = D3DCompileFromFile(gs, NULL, NULL, "gs_main", "gs_5_0", shaderFlags, 0,
			&geometryShaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				MessageBox(hwnd, gs, (LPCWSTR)errorMessage->GetBufferPointer(), MB_OK);
			}
			else
			{
				MessageBox(hwnd, gs, L"Missing Geometry Shader File", MB_OK);
			}

			return false;
		}
		// Create the pixel shader from the buffer.
		result = device->CreateGeometryShader(geometryShaderBuffer->GetBufferPointer(), geometryShaderBuffer->GetBufferSize(), NULL, &m_geometryShader);
		if (FAILED(result))
		{
			return false;
		}
	}

	//---
	//Create the vertex input layout description.
	//---
	D3D11_INPUT_ELEMENT_DESC inputVertexDesc[3] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	// Get a count of the elements in the layout.
	int numElements = sizeof(inputVertexDesc) / sizeof(inputVertexDesc[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(inputVertexDesc, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer.
	if(vs)
	{
		vertexShaderBuffer->Release();
		vertexShaderBuffer = 0;
	}

	if (ps)
	{
		pixelShaderBuffer->Release();
		pixelShaderBuffer = 0;
	}

	if (gs)
	{
		geometryShaderBuffer->Release();
		geometryShaderBuffer = 0;
	}
}

void Shader::RenderShader(ID3D11DeviceContext* context, int indexcount)
{
	// Set the vertex input layout, include pos, normal, texture.
	context->IASetInputLayout(m_layout);

	// Set the vertex pixel geometry shaders.
	context->VSSetShader(m_vertexShader, NULL, 0);
	context->PSSetShader(m_pixelShader, NULL, 0);
	context->GSSetShader(m_geometryShader, NULL, 0);

	// Render the model with this shader.
	context->DrawIndexed(indexcount, 0, 0);
}

//----------------------
//Setup attributes
//----------------------
bool Shader::CreateSampler(ID3D11Device* device)
{
	bool result;

	//Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool Shader::CreateConstantBuffer(ID3D11Device* device)
{
	bool result;

	//Setup the constant buffer in the vertex shader.
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	bufferDesc.ByteWidth = sizeof(MatrixBufferType);

	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer to access in the vertex shader.
	result = device->CreateBuffer(&bufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

}
//---
//Bind texture 3d to UAV(read/load) and SRV(read).
//---
bool Shader::CreateTex3dUAV(ID3D11Device* device, float width)
{
	bool result;

	//---
	//Setup the texture 3d description.
	//---
	D3D11_TEXTURE3D_DESC textureDesc;
	textureDesc.Width = width;
	textureDesc.Height = textureDesc.Width;
	textureDesc.Depth = textureDesc.Width;
	textureDesc.MipLevels = log2(textureDesc.Width);
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	//Create texture 3d.
	result = device->CreateTexture3D(&textureDesc, NULL, &m_Tex3D);
	if (FAILED(result))
	{
		return false;
	}

	//---
	//Setup the unordered access view description.
	//---
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	UAVDesc.Format = textureDesc.Format;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
	UAVDesc.Texture3D.FirstWSlice = 0;
	UAVDesc.Texture3D.MipSlice = 0;
	UAVDesc.Texture3D.WSize = textureDesc.Width;

	//Bind Tex3D to Unordered Access View
	result = device->CreateUnorderedAccessView(m_Tex3D, &UAVDesc, &m_UAV);

	//---
	//Setup the shader resource view description.
	//---
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	SRVDesc.Format = textureDesc.Format;
	SRVDesc.Texture3D.MipLevels = textureDesc.MipLevels;
	SRVDesc.Texture3D.MostDetailedMip = 0;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;

	//Bind Tex3D to shader resource view
	result = device->CreateShaderResourceView(m_Tex3D, &SRVDesc, &m_SRV);

	if (FAILED(result))
	{
		return false;
	}
}

bool Shader::CreateTex2dSRV(ID3D11Device* device)
{
	bool result;

	if (FAILED(result))
	{
		return false;
	}
}

void Shader::CreateDepthBuffer(ID3D11Device* device)
{

}

void Shader::UpdateRenderTarget(ID3D11DeviceContext* context)
{
	context->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
}

void Shader::ClearRenderTarget(ID3D11DeviceContext* deviceContext, float r, float g, float b, float a)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;

	// Clear the back buffer.
	deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

//-----------------------------------
//Update Matrix constant buffer to vertex shader
//-----------------------------------
void Shader::UpdateMatrixParameters(ID3D11DeviceContext* context, const DirectX::XMMATRIX *world, const DirectX::XMMATRIX *view, const DirectX::XMMATRIX *proj)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// Lock the constant buffer so it can be written to.
	context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = *world;
	dataPtr->view = *view;
	dataPtr->projection = *proj;

	// Unlock the constant buffer.
	context->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	//set the constant buffer in the vertex shader.
	context->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
}

//-----------------------------------
//If use UAV(read/load), Update UAV to pixel shader
//-----------------------------------
void Shader::UpdateUAVParameters(ID3D11DeviceContext* context)
{

}

//-----------------------------------
//If use texture(load), Update srv to pixel shader
//-----------------------------------
void Shader::UpdateSRVParameters(ID3D11DeviceContext* context)
{
	context->PSSetShaderResources(0, 1, &m_SRV);
}


//-----------------------------------
//If use sampler, Update sampler to pixel shader
//-----------------------------------
void Shader::UpdateSamplerParameters(ID3D11DeviceContext* context)
{
	context->PSSetSamplers(0, 1, &m_sampleState);
}

//----------------------------------
//Shut down
//----------------------------------
void Shader::ShutdownShader()
{
	// Release the sampler state.
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if (m_geometryShader)
	{
		m_geometryShader->Release();
		m_geometryShader = 0;
	}

	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}
