#pragma once

#include "EntryPoint.h"

namespace PEPE::Script
{



	inline float GetEPValue(LEX::StaticTargetTag)
	{
		return EntryPointHandler::GetEPValue(false);
	}

	inline void SetEPValue(LEX::StaticTargetTag, float value)
	{
		return EntryPointHandler::SetEPValue(value);
	}

	//If one only gets the base, it won't clear. If it gets the whole it clears.
	inline float GetEPValueAsAV(std::string_view, std::span<RE::ACTOR_VALUE_MODIFIER> mods, RE::Actor*)
	{
		return EntryPointHandler::GetEPValue(true);
	}


	inline void Register()
	{
		if (auto _interface = AVG::API::RequestInterface())
		{
			auto result = _interface->RegisterAVDelegate("EPValue", GetEPValueAsAV);
			assert(result == AVG::API::DelegateResult::Success);
		}

		if (auto _interface = LEX::ProcedureHandler::RequestSingleton())
		{
			auto result1 = _interface->RegisterFunction(SetEPValue, "Shared::PEPE::SetEPValue");
			auto result2 = _interface->RegisterFunction(GetEPValue, "Shared::PEPE::GetEPValue");
			assert(result1);
			assert(result2);
		}

	}

}