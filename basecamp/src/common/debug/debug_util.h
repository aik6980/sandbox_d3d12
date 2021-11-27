#pragma once

#include "comdef.h"
#include "debug_output.h"

namespace DBG
{
	inline void throw_hr(HRESULT hr)
	{
		if (FAILED(hr))
		{
			_com_error err(hr);
			LPCTSTR errMsg = err.ErrorMessage();
			OutputString("COM ERROR:: %s", errMsg);
			throw;
		}
	}

	inline void test_hr(HRESULT hr)
	{
		if (FAILED(hr))
		{
			_com_error err(hr);
			LPCTSTR errMsg = err.ErrorMessage();
			OutputString("COM ERROR:: %s", errMsg);
		}
	}
}