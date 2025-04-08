#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdint>

namespace ConFinat {

	// A Token is just a string, but for simplicity we give a name
	using Token = std::string;

	// All supported value types
	enum ValueType { __STRING, __VECTOR, __NUMBER, __BOOLEAN};
	// A value has an identifier of the type plus an element for each possible type
	struct Value {
		ValueType type;
		std::string string;
		std::vector<float> vector;
		float number;
		bool boolean;
	};

	// A Pair is a key and a value
	struct IniPair {
		std::string key;
		Value value;
	};

	// A line is just a vector of tokens
	using Line = std::vector<Token>;

	// A section has a name and multiple pairs
	struct Section {
		std::string name;
		std::vector<IniPair> values;
	};

	// The return type of the parsing, a vector of sections
	using ConfData = std::vector<Section>;

	// Open the file
	static std::ifstream open(const std::string& path) {
		std::ifstream file(path);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file: " + path);
		}
		return file;
	}

	// Returns if a character is considered a new token
	static bool isTokenDelimiter(char c) {
		std::vector delimiters = {'[', ']', '{', '}', '(', ')', ',', '=', '"', ';', '#', ' ', '\\', '\0', '\n'};
		for (char d : delimiters) 
			if (c == d) return true;

		return false;
	}

	// Get the next token from line starting at "next"
	// Increases next to where the following token should start
	static Token getNextToken(std::string line, uint32_t& next) {
		Token t;
		t = "";
		if (next >= line.size()) return t;
		t += line[next];
		if (isTokenDelimiter(line[next++]))  return t;
		while (!isTokenDelimiter(line[next])) {
			t += line[next++];
		}
		return t;
	}

	// Gets the next line from the input
	// returs true if there is a line, false otherwise
	// Ignores empty lines
	static bool getNextLine(Line& line, std::istream& text) {
		std::string str;
		bool inQuotes = false;
		bool escape = false;
		do {
			if (getline(text, str)) {
				line.clear();
				uint32_t i = 0;
				Token t;
				while (true) {
					t = getNextToken(str, i);
					if (t == "\\") {
						t = getNextToken(str, i);
						line.push_back(t);
					}
					else { // We are not escaping chars
					       // Stop reading tokens if we are at the end
					       // or the start of a comment
						if (t == "" || t == "#" || t == ";") break;
						if (t == "\"") inQuotes = !inQuotes;
						// Ignore space tokens
						if (t != " " || inQuotes) { 
							line.push_back(t);
						}
					}
				}
			}
			else return false;
		} while (line.empty());
		return true;

	}

	// Processes the line and updates data
	// Returns false if there is a syntax error
	static bool parseLine(Line& line, std::vector<Section>& data) {
		// Check if line is a new Section:
		if (line[0] == "[" && line[2] == "]") {
			data.push_back({line[1], {}});
			return true;
		}
		// Is not a section line, so we have a key-pair
		IniPair pair;
		pair.key = line[0]; // First token is the key
		if (line[1] != "=") return false; // Then it has to be an equal
		pair.value.type = __NUMBER; // Initally we assume it's a number
		if (line[2] == "\"") { // It's a string
			if (line.back() != "\"") return false;
			pair.value.type = __STRING;
			pair.value.string = line[3];
			for (int j = 4; j < line.size()-1; ++j) {
				pair.value.string += line[j];
			}
		} else if (line[2] == "True" || line[2] == "TRUE" || line[2] == "true") { // It's true
				pair.value.type = __BOOLEAN;
				pair.value.boolean = true;
		} else if (line[2] == "False" || line[2] == "FALSE" || line[2] == "false") { // It's false
				pair.value.type = __BOOLEAN;
				pair.value.boolean = false;
		} else if (line[2] == "{" || line[2] == "(") { // It's a vector
			pair.value.type = __VECTOR;
			for (int i = 2; i < line.size(); ++i) {
				// Ignore { , }
				if (line[i] == "{" || line[i] == "," || line[i] == "}") continue;
				// Is a replication
				if (line[i] == "(") { // Is a replication (a, b)
					if(line[i+2] != "," || line[i+4] != ")") return false;
					for (int j = 0; j < std::stoi(line[i+1]); ++j) {
						pair.value.vector.push_back(std::stof(line[i+3]));
					}
					i+=4;
					continue;
				}
				// It's a value to add to the vector
				pair.value.vector.push_back(std::stof(line[i]));
			}
		} else {
			pair.value.type = __NUMBER;
			pair.value.number = std::stof(line[2]);
		}
		data.back().values.push_back(pair);
		return true;
	}

	// Does the parsing of text with name and returns the ConfData
	static ConfData doParsing(std::istream& text, const std::string& name) {
		Line line;
		std::vector<Section> s;
		s.push_back({"", {}});
		while (getNextLine(line, text)) {
			if (!parseLine(line, s)) {
				printf("[INI PARSER] - ERROR while parsing %s:\n", name.c_str());
				for (auto k : line) printf("%s", k.c_str());
				printf("\n");
				break;
			}
		}
		return s;
	}

	// Parses input from file
	static ConfData parseFromFile(const std::string& file) {
		std::ifstream text = open(file);
		return doParsing(text, file);
	}

	// Parses input from string
	static ConfData parseFromString(const std::string& input, const std::string& name) {

		std::istringstream text(input);
		return doParsing(text, name);
	}
}
