#pragma once


namespace D3D12
{
	class ShaderReflection;

	struct Shader
	{
		ComPtr<ID3DBlob>				m_buffer;
		unique_ptr<ShaderReflection>	m_reflection;
	};
}