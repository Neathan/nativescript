#include "lexer.h"

#include <map>
#include <stdio.h>

namespace ns {
	
	static std::map<std::string, TokenType> s_keywords = {
		{ "UPROPERTY", TokenType::PROPERTY },
		{ "UCLASS", TokenType::CLASS_PROP },
		{ "USTRUCT", TokenType::STRUCT_PROP },
		{ "UFUNCTION", TokenType::FUNCTION_PROP },
		{ "class", TokenType::CLASS_KW },
		{ "struct", TokenType::STRUCT_KW },
		{ "const", TokenType::CONST_KW },
		{ "auto", TokenType::AUTO_KW }
	};


	inline bool isDigit(char c) { return c >= '0' && c <= '9'; }
	inline bool isAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
	inline bool isAlphaNumeric(char c) { return isDigit(c) || isAlpha(c); }
	inline bool eof(int cursor, const std::string& content) { return cursor >= content.size(); }

	inline Token errorToken() { return Token{ TokenType::ERROR_TYPE, -1, -1, "" }; }


	char peekNext(int cursor, const std::string& content) {
		if (eof(cursor + 1, content)) return '\0';
		return content[(size_t)cursor + 1];
	}

	char getNext(int& cursor, int& column, const std::string& content) {
		++column;
		return content[cursor++];
	}

	Token getStringToken(int& cursor, int& line, int& column, const std::string& content, int start) {
		while (!eof(cursor, content) && content[cursor] != '"') {
			if (content[cursor] == '\n') {
				++line;
				column = -1;
			}
			getNext(cursor, column, content);
		}

		if (eof(cursor, content)) {
			printf("Unbound string literal %d col:%d", line, column);
			return errorToken();
		}
		getNext(cursor, column, content);

		std::string value = content.substr(start, (size_t)cursor - start);
		return Token{ TokenType::STRING_LITERAL, line, column, value };
	}

	Token getCommentToken(int& cursor, int& line, int& column, const std::string& content) {
		char next = getNext(cursor, column, content);
		if (next == '/') {
			while (!eof(cursor, content) && content[cursor] != '\n') {
				getNext(cursor, column, content);
			}
		}
		else if (next == '*') {
			while (!eof(cursor, content) && content[cursor] != '*' && peekNext(cursor, content) != '/') {
				char c = getNext(cursor, column, content);
				if (c == '\n') {
					column = 0;
					++line;
				}
			}

			if (!eof(cursor, content)) getNext(cursor, column, content);
			if (!eof(cursor, content)) getNext(cursor, column, content);
		}
		return errorToken();
	}

	Token getIdentifier(int& cursor, int& line, int& column, const std::string& content, int start) {
		while (isAlphaNumeric(content[cursor]) || content[cursor] == '_') getNext(cursor, column, content);
		std::string name = content.substr(start, (size_t)cursor - start);
		TokenType type = TokenType::IDENTIFIER;

		auto it = s_keywords.find(name);
		if (it != s_keywords.end()) {
			type = it->second;
		}

		return Token{ type, line, column, name };
	}

	Token getToken(char c, int& cursor, int& line, int& column, const std::string& content, int start) {
		switch (c) {
		// Single character tokens
		case '(': return Token{TokenType::LEFT_PAREN, line, column, "("};
		case ')': return Token{TokenType::RIGHT_PAREN, line, column, ")"};
		case '{': return Token{TokenType::LEFT_BRACKET, line, column, "{"};
		case '}': return Token{TokenType::RIGHT_BRACKET, line, column, "}"};
		case '#': return Token{TokenType::NUMBER_SIGN, line, column, "#"};
		case ';': return Token{TokenType::SEMICOLON, line, column, ";"};
		case '=': return Token{TokenType::EQUAL, line, column, "="};
		case '*': return Token{TokenType::ASTERIX, line, column, "*"};
		case '&': return Token{TokenType::AMPERSAND, line, column, "&"};
		case '<': return Token{TokenType::LEFT_ANGLE_BRACKET, line, column, "<"};
		case '>': return Token{TokenType::RIGHT_ANGLE_BRACKET, line, column, ">"};
		case ':': return Token{TokenType::COLON, line, column, ":"};

		// String literal
		case '"': return getStringToken(cursor, line, column, content, start);

		// Whitespace
		case '/': return getCommentToken(cursor, line, column, content);
		case ' ':
		case '\r':
		case '\t':
			return errorToken();
		case '\n':
			column = 0;
			++line;
			return errorToken();
		default:
			if (isAlpha(c)) {
				return getIdentifier(cursor, line, column, content, start);
			}
			return errorToken();
		}
	}

	std::vector<Token> lex(const std::string& content) {
		std::vector<Token> tokens;

		int cursor = 0;
		int start = 0;
		
		int line = 0;
		int column = 0;

		while (!eof(cursor, content)) {
			start = cursor;

			char c = getNext(cursor, column, content);
			Token token = getToken(c, cursor, line, column, content, start);
			if (token.type != TokenType::ERROR_TYPE) {
				tokens.push_back(token);
			}
		}

		tokens.emplace_back(Token{ TokenType::END_OF_FILE, line, column, "EOF" });
		return tokens;
	}
}
