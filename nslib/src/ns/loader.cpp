#include "loader.h"

#include <filesystem>
#include <assert.h>

namespace ns {

	static FARPROC __stdcall tryLoadFunction(HMODULE module, const char* functionName) {
		auto func = GetProcAddress(module, functionName);
		if (func == NULL) {
			printf("Failed to load function from dll\n");
		}

		return func;
	}

	std::optional<ScriptCollection> ScriptCollection::create(const std::string& path) {
		namespace fs = std::filesystem;

		fs::path filePath = fs::absolute(path);

		// Load DLL
		void* handle = LoadLibraryA(filePath.generic_string().c_str());
		if (!handle) {
			return std::nullopt;
		}

		// Retrieve content info
		GetGeneratedScriptsFn scriptInfoFunc = (GetGeneratedScriptsFn)tryLoadFunction((HMODULE)handle, "getGeneratedScripts");

		assert(scriptInfoFunc);

		int count;
		ScriptInfo* scripts;
		scriptInfoFunc(&count, &scripts);

		std::unordered_map<std::string, ScriptInterface> interfaces;

		// Create script interfaces
		for (int i = 0; i < count; ++i) {
			ScriptInfo& info = scripts[i];

			std::vector<Parameter> parameters;
			std::vector<Function> functions;

			for (const auto& param : info.parameters) {
				void* getFunc = tryLoadFunction((HMODULE)handle, param.nameGetFn.c_str());
				void* setFunc = tryLoadFunction((HMODULE)handle, param.nameSetFn.c_str());

				assert(getFunc);
				assert(setFunc);

				parameters.emplace_back(Parameter{ getFunc, setFunc });
			}

			// TODO(Neathan): Implement functions

			CreateScriptFn createFunc = (CreateScriptFn)tryLoadFunction((HMODULE)handle, info.nameCreateFn.c_str());
			DestroyScriptFn destroyFunc = (DestroyScriptFn)tryLoadFunction((HMODULE)handle, info.nameDestroyFn.c_str());

			assert(createFunc);
			assert(destroyFunc);

			interfaces.emplace(info.name, ScriptInterface{ info.name, parameters, functions, createFunc, destroyFunc });
		}

		return std::make_optional<ScriptCollection>(handle, scriptInfoFunc, interfaces);
	}

	ScriptCollection::~ScriptCollection() {
		if (m_handle && !FreeLibrary((HMODULE)m_handle)) {
			DWORD errorCode = GetLastError();
			printf("Failed to free dll. Error code: %ld\n", errorCode);
		}
	}

}