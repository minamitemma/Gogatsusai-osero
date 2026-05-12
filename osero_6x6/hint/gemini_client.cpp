#include "hint/gemini_client.hpp"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <utility>

namespace reversi
{

namespace
{

std::string shellQuote(const std::string& value)
{
	std::string quoted = "'";
	for (const char ch : value) {
		if (ch == '\'') {
			quoted += "'\\''";
		} else {
			quoted += ch;
		}
	}
	quoted += "'";
	return quoted;
}

bool isSafeModelName(const std::string& value)
{
	if (value.empty()) {
		return false;
	}
	for (const char ch : value) {
		const bool safe = ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') ||
		                  ('0' <= ch && ch <= '9') || ch == '-' || ch == '_' || ch == '.';
		if (!safe) {
			return false;
		}
	}
	return true;
}

std::string jsonEscape(const std::string& value)
{
	std::ostringstream escaped;
	for (const char raw_ch : value) {
		const unsigned char ch = static_cast<unsigned char>(raw_ch);
		switch (ch) {
		case '\\':
			escaped << "\\\\";
			break;
		case '"':
			escaped << "\\\"";
			break;
		case '\b':
			escaped << "\\b";
			break;
		case '\f':
			escaped << "\\f";
			break;
		case '\n':
			escaped << "\\n";
			break;
		case '\r':
			escaped << "\\r";
			break;
		case '\t':
			escaped << "\\t";
			break;
		default:
			if (ch < 0x20) {
				escaped << "\\u00";
				const char* digits = "0123456789abcdef";
				escaped << digits[(ch >> 4) & 0x0f] << digits[ch & 0x0f];
			} else {
				escaped << ch;
			}
			break;
		}
	}
	return escaped.str();
}

int hexValue(char ch)
{
	if ('0' <= ch && ch <= '9') {
		return ch - '0';
	}
	if ('a' <= ch && ch <= 'f') {
		return 10 + ch - 'a';
	}
	if ('A' <= ch && ch <= 'F') {
		return 10 + ch - 'A';
	}
	return -1;
}

bool parseHex4(const std::string& value, std::size_t offset, unsigned int& codepoint)
{
	if (offset + 4 > value.size()) {
		return false;
	}
	codepoint = 0;
	for (std::size_t i = 0; i < 4; ++i) {
		const int digit = hexValue(value.at(offset + i));
		if (digit < 0) {
			return false;
		}
		codepoint = (codepoint << 4) | static_cast<unsigned int>(digit);
	}
	return true;
}

void appendUtf8(std::string& output, unsigned int codepoint)
{
	if (codepoint <= 0x7f) {
		output.push_back(static_cast<char>(codepoint));
	} else if (codepoint <= 0x7ff) {
		output.push_back(static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
		output.push_back(static_cast<char>(0x80 | (codepoint & 0x3f)));
	} else if (codepoint <= 0xffff) {
		output.push_back(static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
		output.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
		output.push_back(static_cast<char>(0x80 | (codepoint & 0x3f)));
	} else {
		output.push_back(static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
		output.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
		output.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
		output.push_back(static_cast<char>(0x80 | (codepoint & 0x3f)));
	}
}

std::string parseJsonString(const std::string& value, std::size_t quote_pos)
{
	if (quote_pos >= value.size() || value.at(quote_pos) != '"') {
		throw std::runtime_error("Invalid JSON string.");
	}

	std::string output;
	for (std::size_t i = quote_pos + 1; i < value.size(); ++i) {
		const char ch = value.at(i);
		if (ch == '"') {
			return output;
		}
		if (ch != '\\') {
			output.push_back(ch);
			continue;
		}
		if (++i >= value.size()) {
			break;
		}
		const char escaped = value.at(i);
		switch (escaped) {
		case '"':
		case '\\':
		case '/':
			output.push_back(escaped);
			break;
		case 'b':
			output.push_back('\b');
			break;
		case 'f':
			output.push_back('\f');
			break;
		case 'n':
			output.push_back('\n');
			break;
		case 'r':
			output.push_back('\r');
			break;
		case 't':
			output.push_back('\t');
			break;
		case 'u': {
			unsigned int codepoint = 0;
			if (!parseHex4(value, i + 1, codepoint)) {
				throw std::runtime_error("Invalid JSON unicode escape.");
			}
			i += 4;
			appendUtf8(output, codepoint);
			break;
		}
		default:
			output.push_back(escaped);
			break;
		}
	}

	throw std::runtime_error("Unterminated JSON string.");
}

std::string extractFirstTextPart(const std::string& response)
{
	std::size_t pos = 0;
	while (true) {
		pos = response.find("\"text\"", pos);
		if (pos == std::string::npos) {
			break;
		}

		const std::size_t colon = response.find(':', pos + 6);
		if (colon == std::string::npos) {
			break;
		}

		std::size_t quote = colon + 1;
		while (quote < response.size() && (response.at(quote) == ' ' || response.at(quote) == '\n' ||
		                                  response.at(quote) == '\r' || response.at(quote) == '\t')) {
			++quote;
		}
		if (quote < response.size() && response.at(quote) == '"') {
			return parseJsonString(response, quote);
		}
		pos = colon + 1;
	}

	throw std::runtime_error("Gemini response did not include text.");
}

std::string buildRequestBody(const std::string& prompt)
{
	std::ostringstream body;
	body << "{";
	body << "\"contents\":[{\"role\":\"user\",\"parts\":[{\"text\":\"" << jsonEscape(prompt) << "\"}]}],";
	body << "\"generationConfig\":{\"thinkingConfig\":{\"thinkingLevel\":\"low\"}}";
	body << "}";
	return body.str();
}

std::string writeTempRequestFile(const std::string& body)
{
	char path[] = "/tmp/reversi6_gemini_request_XXXXXX";
	const int fd = mkstemp(path);
	if (fd < 0) {
		throw std::runtime_error("Failed to create temporary Gemini request file.");
	}
	close(fd);

	std::ofstream file(path);
	if (!file) {
		std::remove(path);
		throw std::runtime_error("Failed to open temporary Gemini request file.");
	}
	file << body;
	file.close();
	return path;
}

std::string runCommand(const std::string& command)
{
	FILE* pipe = popen(command.c_str(), "r");
	if (pipe == nullptr) {
		throw std::runtime_error("Failed to start curl.");
	}

	std::string output;
	char buffer[4096];
	while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
		output += buffer;
	}

	const int status = pclose(pipe);
	if (status != 0 && output.empty()) {
		throw std::runtime_error("curl failed while calling Gemini API.");
	}
	return output;
}

}  // namespace

GeminiClient::GeminiClient(std::string api_key, std::string model_name)
    : m_api_key(std::move(api_key)), m_model_name(std::move(model_name))
{
	if (!isSafeModelName(m_model_name)) {
		throw std::runtime_error("Invalid Gemini model name.");
	}
}

std::string GeminiClient::generateHint(const std::string& prompt) const
{
	const std::string request_path = writeTempRequestFile(buildRequestBody(prompt));
	const std::string url = "https://generativelanguage.googleapis.com/v1beta/models/" +
	                        m_model_name + ":generateContent";
	const std::string command = "curl -sS " + shellQuote(url) +
	                            " -H " + shellQuote("x-goog-api-key: " + m_api_key) +
	                            " -H " + shellQuote("Content-Type: application/json") +
	                            " -X POST --data-binary @" + shellQuote(request_path) + " 2>&1";

	const std::string response = runCommand(command);
	std::remove(request_path.c_str());

	return extractFirstTextPart(response);
}

}  // namespace reversi
