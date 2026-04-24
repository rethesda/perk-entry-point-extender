#include "PerkEntryPointExtenderAPI.h"

#include "EntryPoint.h"



namespace PerkEntryPointExtenderAPI
{
	struct PerkEntryPointExtenderInterface : public CurrentInterface
	{
		Version GetVersion() override { return Version::Current; }

		//Deprecated version. Cannot handle vectors properly.
		RequestResult ApplyPerkEntryPoint_Deprecated(RE::Actor* target, RE::PerkEntryPoint a_entryPoint, std::span<RE::TESForm*> arg_list, void* out, const char* cat, uint8_t channel) override
		{
			std::vector<RE::TESForm*> args{ arg_list.data(), arg_list.data() +  arg_list.size() };
			//RE::BSFixedString category = cat;
			
			return PEPE::EntryPointHandler::ApplyPerkEntryPoint(a_entryPoint, target, args, {}, out, cat, channel, EntryPointFlag::None);
		}

		inline RequestResult ApplyPerkEntryPoint_Deprecated(RE::Actor* target, RE::PerkEntryPoint a_entryPoint, std::span<RE::TESForm*> args, void* out, const char* cat,
			uint8_t channel, EntryPointFlag flags) override
		{
			return ApplyPerkEntryPoint(target, a_entryPoint, args, {}, out, cat, channel, flags);
		}




		RequestResult ApplyPerkEntryPoint(RE::Actor* target, RE::PerkEntryPoint a_entryPoint,
			std::span<RE::TESForm*> args, std::span<RE::ExtraDataList*> extras, void* out,
			const std::string_view& category, uint8_t channel, EntryPointFlag flags) override
		{
			if (flags >= EntryPointFlag::PRIVATE_Total) {
				return RequestResult::UnknownFlags;
			}

			void* in;

			std::vector<RE::SpellItem*> buffer{ 1 };

			if (flags & EntryPointFlag::PRIVATE_UsesCollection) {
				in = kernels_fix ? &buffer : (void*)buffer.data();
			}
			else {

				switch (a_entryPoint)
				{
				case RE::PerkEntryPoint::kApplyBashingSpell:
				case RE::PerkEntryPoint::kApplyCombatHitSpell:
				case RE::PerkEntryPoint::kApplyReanimateSpell:
				case RE::PerkEntryPoint::kApplySneakingSpell:
				case RE::PerkEntryPoint::kApplyWeaponSwingSpell:
					if (kernels_fix) {
						in = &buffer;
						break;
					}
					[[fallthrough]];

				default:
					in = out;
					break;
				}
			}


			auto result = PEPE::EntryPointHandler::ApplyPerkEntryPoint(a_entryPoint, target, args, extras, in, category, channel, flags);

			if (flags & EntryPointFlag::PRIVATE_UsesCollection) {
				IFormCollection* collection = reinterpret_cast<IFormCollection*>(out);

				for (auto entry : buffer)
				{
					if (entry)
						collection->LoadForm(entry);

				}
			}
			else if (in != out) {
				*reinterpret_cast<RE::TESForm**>(out) = buffer.front();
			}

			return result;
		}


		virtual std::string_view GetCurrentCategory() override
		{
			return PEPE::EntryPointHandler::GetCategory();
		}

	};


	[[nodiscard]] CurrentInterface* InferfaceSingleton()
	{
		static PerkEntryPointExtenderInterface intfc{};
		return std::addressof(intfc);
	}


	extern "C" __declspec(dllexport) void* PEPE_RequestInterfaceImpl(Version version)
	{
		CurrentInterface* result = InferfaceSingleton();

		if (result && result->GetVersion() >= version)
			return result;
	
		return nullptr;
	}
}