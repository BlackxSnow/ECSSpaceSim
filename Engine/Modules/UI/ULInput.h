#pragma once

#include <Ultralight/Ultralight.h>
#include "../../utility/CCXBimap.h"
#include "../Input/InputDefs.h"

namespace Thera::UI::Input
{
	namespace {
		using ULMouse = ultralight::MouseEvent::Button;
		using THMouse = Thera::Input::Mouse;
		const CCX::UnorderedBimap<ultralight::MouseEvent::Button, Thera::Input::Mouse> MouseULToThera
		{
			{ ULMouse::kButton_Left, THMouse::ButtonLeft },
			{ ULMouse::kButton_Middle, THMouse::ButtonMiddle },
			{ ULMouse::kButton_Right, THMouse::ButtonRight }
		};
	}
	
	/// <summary>
	/// Converts Ultralight Mouse button values to Thera equivalents.
	/// </summary>
	/// <param name="ulMouse"></param>
	/// <returns></returns>
	inline Thera::Input::Mouse ULToThera(ultralight::MouseEvent::Button ulMouse)
	{
		return MouseULToThera.at(ulMouse);
	}
	
	/// <summary>
	/// Converts Thera mouse button values to Ultralight equivalents.
	/// </summary>
	/// <param name="thButton"></param>
	/// <returns></returns>
	inline ultralight::MouseEvent::Button TheraToUL(Thera::Input::Mouse thButton)
	{
		return MouseULToThera.at(thButton);
	}
}