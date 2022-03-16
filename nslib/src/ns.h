#pragma once

#include <string>
#include <vector>

#ifdef _WIN32
	#define NS_EXPORT __declspec(dllexport)
#else
	#define NS_EXPORT
#endif

#define UPROPERTY()
#define UCLASS()
#define USTRUCT()
#define UFUNCTION()


class Script {
public:
	virtual void start() {};
	virtual void update() {};
};

namespace ns {

	enum class ParameterType : int {
		ERROR_T	= 0,
		INT		= 1,
		CHAR	= 2,
		BOOL	= 3,
		FLOAT	= 4,
		DOUBLE	= 5,
		WCHAR_T	= 6,
		VOID_T	= 7
	};

	struct ParameterInfo {
		ParameterType type = ParameterType::ERROR_T;
		std::string name;
		std::string nameGetFn;
		std::string nameSetFn;
	};

	struct FunctionInfo {
		std::string name;
		std::vector<ParameterType> parameters;
		ParameterType returnParameterType;
	};

	struct ScriptInfo {
		std::string name;
		std::vector<ParameterInfo> parameters;
		std::vector<FunctionInfo> functions;
		std::string nameCreateFn;
		std::string nameDestroyFn;
	};

}
