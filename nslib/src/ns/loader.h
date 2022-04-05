#pragma once

#include "ns.h"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#ifdef _WIN32
	#include <windows.h>
	#include <libloaderapi.h>
#endif


namespace ns {

	typedef void (*GetGeneratedScriptsFn)(int*, ns::ScriptInfo**);

	typedef Script* (*CreateScriptFn)();
	typedef void (*DestroyScriptFn)(Script*);

	typedef int (*GetIntFn)(Script*);
	typedef char (*GetCharFn)(Script*);
	typedef bool (*GetBoolFn)(Script*);
	typedef float (*GetFloatFn)(Script*);
	typedef double (*GetDoubleFn)(Script*);
	typedef wchar_t (*GetWCharFn)(Script*);

	typedef void (*SetIntFn)(Script*, int);
	typedef void (*SetCharFn)(Script*, char);
	typedef void (*SetBoolFn)(Script*, bool);
	typedef void (*SetFloatFn)(Script*, float);
	typedef void (*SetDoubleFn)(Script*, double);
	typedef void (*SetWCharFn)(Script*, wchar_t);

	struct Parameter {
		void* getFunc = nullptr;
		void* setFunc = nullptr;

		template<typename T>
		T get(Script* script) const {
			//static_assert(false, "Get function unimplemented for type");
			return T{};
		}
		template<>
		int get(Script* script) const {
			return ((GetIntFn)getFunc)(script);
		}
		template<>
		char get(Script* script) const {
			return ((GetCharFn)getFunc)(script);
		}
		template<>
		bool get(Script* script) const {
			return ((GetBoolFn)getFunc)(script);
		}
		template<>
		float get(Script* script) const {
			return ((GetFloatFn)getFunc)(script);
		}
		template<>
		double get(Script* script) const {
			return ((GetDoubleFn)getFunc)(script);
		}
		template<>
		wchar_t get(Script* script) const {
			return ((GetWCharFn)getFunc)(script);
		}

		template<typename T>
		void set(Script* script, T value) const {
			static_assert(false, "Set function unimplemented for type");
		}
		template<>
		void set(Script* script, int value) const {
			((SetIntFn)setFunc)(script, value);
		}
		template<>
		void set(Script* script, char value) const {
			((SetCharFn)setFunc)(script, value);
		}
		template<>
		void set(Script* script, bool value) const {
			((SetBoolFn)setFunc)(script, value);
		}
		template<>
		void set(Script* script, float value) const {
			((SetFloatFn)setFunc)(script, value);
		}
		template<>
		void set(Script* script, double value) const {
			((SetDoubleFn)setFunc)(script, value);
		}
		template<>
		void set(Script* script, wchar_t value) const {
			((SetWCharFn)setFunc)(script, value);
		}
	};

	struct Function {

	};

	struct ScriptInterface {
		std::string name;
		std::vector<Parameter> parameters;
		std::vector<Function> functions;

		CreateScriptFn createScriptFunc = nullptr;
		DestroyScriptFn destroyScriptFunc = nullptr;
	};

	class ScriptCollection {
	public:
		ScriptCollection(void* handle, GetGeneratedScriptsFn scriptInfoFunc, const std::unordered_map<std::string, ScriptInterface>& scripts)
			: m_handle(handle), m_scriptInfoFunc(scriptInfoFunc), m_scripts(scripts) {}
		~ScriptCollection();

		const std::unordered_map<std::string, ScriptInterface>& getScripts() const { return m_scripts; }
		const ScriptInterface& getScriptInterface(const std::string& name) const { return m_scripts.at(name); }

		static std::shared_ptr<ScriptCollection> create(const std::string& path);

	private:
		void* m_handle = nullptr;
		GetGeneratedScriptsFn m_scriptInfoFunc = nullptr;

		std::unordered_map<std::string, ScriptInterface> m_scripts;
	};

}
