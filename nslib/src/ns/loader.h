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

		int getAsInt(Script* script) const {
			return ((GetIntFn)getFunc)(script);
		}
		char getAsChar(Script* script) const {
			return ((GetCharFn)getFunc)(script);
		}
		bool getAsBool(Script* script) const {
			return ((GetBoolFn)getFunc)(script);
		}
		float getAsFloat(Script* script) const {
			return ((GetFloatFn)getFunc)(script);
		}
		double getAsDouble(Script* script) const {
			return ((GetDoubleFn)getFunc)(script);
		}
		wchar_t getAsWChar(Script* script) const {
			return ((GetWCharFn)getFunc)(script);
		}


		void set(Script* script, int value) const {
			((SetIntFn)setFunc)(script, value);
		}
		void set(Script* script, char value) const {
			((SetCharFn)setFunc)(script, value);
		}
		void set(Script* script, bool value) const {
			((SetBoolFn)setFunc)(script, value);
		}
		void set(Script* script, float value) const {
			((SetFloatFn)setFunc)(script, value);
		}
		void set(Script* script, double value) const {
			((SetDoubleFn)setFunc)(script, value);
		}
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
