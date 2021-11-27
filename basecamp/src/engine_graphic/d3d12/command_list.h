#pragma once

using Microsoft::WRL::ComPtr;

namespace D3D12
{
	class COMMAND_LIST
	{
	public:
		ID3D12GraphicsCommandList* operator() ()	{ return m_obj.Get();  }
		
		void	init(ComPtr<ID3D12GraphicsCommandList> com_ptr);
		void	reset(ID3D12CommandAllocator* command_allocator, ID3D12PipelineState* pipeline_state);
		void	close();

		bool is_openned() { return m_is_opened; }
	private:
		ComPtr<ID3D12GraphicsCommandList>	m_obj;
		bool								m_is_opened = false;
	};
}