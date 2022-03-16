#include "generator.h"

#include "ns.h"
#include "lexer.h"

#include <stdio.h>
#include <fstream>
#include <filesystem>
#include <regex>


namespace ns {

	namespace fs = std::filesystem;

	// NOTE: Make sure to use at least C++17
	template<typename First, typename ... T>
	bool isIn(First&& first, T && ... t) {
		return ((first == t) || ...);
	}


	void outputParameterType(std::ostream& out, ParameterType type) {
		switch (type) {
		case ParameterType::INT: out << "int"; break;
		case ParameterType::CHAR: out << "char"; break;
		case ParameterType::BOOL: out << "bool"; break;
		case ParameterType::FLOAT: out << "float"; break;
		case ParameterType::DOUBLE: out << "double"; break;
		case ParameterType::WCHAR_T: out << "wchar_t"; break;
		case ParameterType::VOID_T: out << "void"; break;
		};
	}

	ParameterType parseParameterType(const std::string& lexeme) {
		if (lexeme == "int") {
			return ParameterType::INT;
		}
		else if (lexeme == "char") {
			return ParameterType::CHAR;
		}
		else if (lexeme == "bool") {
			return ParameterType::BOOL;
		}
		else if (lexeme == "float") {
			return ParameterType::FLOAT;
		}
		else if (lexeme == "double") {
			return ParameterType::DOUBLE;
		}
		else if (lexeme == "wchar_t") {
			return ParameterType::WCHAR_T;
		}
		else if (lexeme == "void") {
			return ParameterType::VOID_T;
		}
		return ParameterType::ERROR_T;
	}

	void outputParameter(std::ostream& out, const ScriptInfo& script, const ParameterInfo& parameter) {
		out << "extern \"C\" NS_EXPORT ";
		outputParameterType(out, parameter.type);
		out << " " << parameter.nameGetFn << "(" << script.name << "* x) {\n";
		out << "	return x->" << parameter.name << ";\n";
		out << "}\n\n";

		out << "extern \"C\" NS_EXPORT void " << parameter.nameSetFn << "(" << script.name << "* x, ";
		outputParameterType(out, parameter.type);
		out << " v) {\n";
		out << "	x->" << parameter.name << " = v;\n";
		out << "}\n";
	}

	void outputFunction(std::ostream& out, const ScriptInfo& script, const FunctionInfo& function) {
		// TODO(Neathan): Implement
	}

	void outputScript(std::ostream& out, const ScriptInfo& script) {
		out << "extern \"C\" NS_EXPORT " << script.name << "* create" << script.name << "() {\n";
		out << "	return new " << script.name << "();\n";
		out << "}\n\n";

		out << "extern \"C\" NS_EXPORT void destroy" << script.name << "(" << script.name << "* x) {\n";
		out << "	delete x;\n";
		out << "}\n";

		for (const ParameterInfo& parameter : script.parameters) {
			out << "\n";
			outputParameter(out, script, parameter);
		}
		for (const FunctionInfo& function : script.functions) {
			out << "\n";
			outputFunction(out, script, function);
		}
	}

	void outputScriptInfoSource(std::ostream& out, const ScriptInfo& script, const std::string& rowStart = "") {
		out << rowStart << "ScriptInfo{\n";

		out << rowStart << "	\"" << script.name << "\",\n";

		out << rowStart << "	{\n";
		for (const auto& param : script.parameters) {
			out << rowStart << "		ParameterInfo{\n";
			out << rowStart << "			(ParameterType)" << (int)param.type << ",\n";
			out << rowStart << "			\"" << param.name << "\",\n";
			out << rowStart << "			\"" << param.nameGetFn << "\",\n";
			out << rowStart << "			\"" << param.nameSetFn << "\"\n";
			out << rowStart << "		},\n";
		}
		out << rowStart << "	},\n";

		out << rowStart << "	{\n";
		for (const auto& func : script.functions) {
			out << rowStart << "		FunctionInfo{\n";
			out << rowStart << "			\"" << func.name << "\",\n";
			out << rowStart << "			{\n";
			for (ParameterType type : func.parameters) {
				out << rowStart << "				(ParameterType)" << (int)type << "\n";
			}
			out << rowStart << "			},\n";
			out << rowStart << "			(ParameterType)" << (int)func.returnParameterType << "\n";
			out << rowStart << "		}\n";
		}
		out << rowStart << "	},\n";

		out << rowStart << "	\"" << script.nameCreateFn << "\",\n";
		out << rowStart << "	\"" << script.nameDestroyFn << "\"\n";
		out << rowStart << "}";
	}


	std::vector<ScriptInfo> parseTokens(const std::vector<Token>& tokens) {
		std::vector<ScriptInfo> scripts;

		auto it = tokens.begin();
		while (it != tokens.end()) {
			if (it->type == TokenType::CLASS_PROP) {
				ScriptInfo scriptInfo;
				// Move iterator to class name
				while (it->type != TokenType::CLASS_KW) ++it;
				scriptInfo.name = (++it)->lexeme;
				scriptInfo.nameCreateFn = "create" + scriptInfo.name;
				scriptInfo.nameDestroyFn = "destroy" + scriptInfo.name;

				// Find all properties
				while (it != tokens.end() && it->type != TokenType::CLASS_PROP) {
					if (it->type == TokenType::PROPERTY) {
						// Move iterator to start of property
						while (it->type != TokenType::RIGHT_PAREN) ++it;

						ParameterInfo param;
						param.type = parseParameterType((++it)->lexeme);
						param.name = (++it)->lexeme;
						param.nameGetFn = scriptInfo.name + "_get_" + param.name;
						param.nameSetFn = scriptInfo.name + "_set_" + param.name;

						scriptInfo.parameters.push_back(param);
					}
					else if (it->type == TokenType::FUNCTION_PROP) {
						// TODO(Neathan): Implement
					}
					++it;
				}
				scripts.push_back(scriptInfo);
			}
			else {
				++it;
			}
		}

		return scripts;
	}

	void processHeaderFile(fs::path filePath, const fs::path& projectRoot, const fs::path& outputRoot, std::vector<ScriptInfo>& projectScripts) {
		std::string content;
		std::getline(std::ifstream(filePath), content, '\0');

		std::vector<Token> tokens = lex(content);
		std::vector<ScriptInfo> scripts = parseTokens(tokens);

		filePath.replace_extension("generated" + filePath.extension().generic_string());
		fs::path newPath = outputRoot / filePath.lexically_relative(projectRoot);
		fs::create_directories(newPath.parent_path());

		std::ofstream out(newPath);
		out << content;

		for (const ScriptInfo& script : scripts) {
			out << "\n";
			outputScript(out, script);
		}

		projectScripts.insert(projectScripts.end(), scripts.begin(), scripts.end());
	}

	void processSourceFile(fs::path filePath, const fs::path& projectRoot, const fs::path& outputRoot) {
		std::string content;
		std::getline(std::ifstream(filePath), content, '\0');

		std::string originalName = filePath.stem().generic_string() + ".h";
		std::string headerName = filePath.stem().generic_string() + ".generated.h";

		filePath.replace_extension("generated" + filePath.extension().generic_string());
		fs::path newPath = outputRoot / filePath.lexically_relative(projectRoot);
		fs::create_directories(newPath.parent_path());

		std::ofstream out(newPath);
		out << std::regex_replace(content, std::regex(originalName), headerName);
	}

	void processDirectory(fs::path path, const fs::path& projectRoot, const fs::path& outputRoot, std::vector<ScriptInfo>& projectScripts) {
		for (const auto& entry : fs::directory_iterator(path)) {
			if (entry.is_regular_file()) {
				if (isIn(entry.path().extension(), ".h", ".hpp")) {
					processHeaderFile(entry.path(), projectRoot, outputRoot, projectScripts);
				}
				else if (isIn(entry.path().extension(), ".cpp", ".cc", ".cxx")) {
					processSourceFile(entry.path(), projectRoot, outputRoot);
				}
			}
			else if (entry.is_directory()) {
				processDirectory(entry.path(), projectRoot, outputRoot, projectScripts);
			}
		}
	}

	void generateScriptsHeader(const fs::path& outputRoot, const std::vector<ScriptInfo>& projectScripts) {
		fs::path headerPath = outputRoot / "scripts.generated.h";
		std::ofstream headerOut(headerPath);

		headerOut << "#pragma once\n\n";
		headerOut << "#include <ns.h>\n\n";
		headerOut << "extern \"C\" NS_EXPORT void getGeneratedScripts(int* count, ns::ScriptInfo** scripts);\n";
		

		fs::path sourcePath = outputRoot / "scripts.generated.cpp";
		std::ofstream sourceOut(sourcePath);

		sourceOut << "#include \"scripts.generated.h\"\n\n";

		sourceOut << "extern \"C\" void getGeneratedScripts(int* count, ns::ScriptInfo** scripts) {\n";
		sourceOut << "	using namespace ns;\n\n";
		sourceOut << "	*count = " << projectScripts.size() << ";\n";
		sourceOut << "	*scripts = new ScriptInfo[" << projectScripts.size() << "]{\n";

		for (const auto& script : projectScripts) {
			outputScriptInfoSource(sourceOut, script, "\t\t");
			sourceOut << ",\n";
		}

		sourceOut << "	};\n";
		sourceOut << "}\n";

	}

	void generateProject(const char* path, const char* outputPath) {
		std::vector<ScriptInfo> projectScripts;

		processDirectory(path, path, outputPath, projectScripts);

		generateScriptsHeader(outputPath, projectScripts);
	}

}

