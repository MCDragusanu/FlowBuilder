#pragma once


#include <string>
#include <vector>
#include <optional>
#include <exception>
#include <iostream>


#define ESC_KEY 33
#define ENTER_KEY 13
#define BACKSPACE_KEY 8

class InvalidInput : public std::exception {
public:
	InvalidInput(const char* reason) : exception(reason) {};

};

struct Option {
	std::string m_name, m_keyName;
	std::string m_key;
	Option(std::string&& name, std::string&& keyName ,std::string&& key) :m_name(name), m_keyName(keyName), m_key(key) {};
};
struct InputHandler {

	virtual std::optional<std::string> readString(const char* inputDescription) {
		
		std::cout << inputDescription;
		std::string input;
		if (!(std::getline(std::cin , input))) {
			return {};
		} else return std::optional(input);
	}
	virtual std::optional<char> readCharacter(const char* inputDescription) {
		std::cout << inputDescription;
		char input;
		if (!(std::cin >> input)) {
			return {};
		}
		else return std::optional(input);
	}
	virtual std::optional<float> readFloat(const char* inputDescription) {
		std::cout << inputDescription;
		float input;
		if (!(std::cin >> input)) {
			return {};
		}
		else return std::optional(input);
	
	};
	virtual std::optional<Option> pickOption(const char* decisionMessage, std::vector<Option>&& options) {
		if (options.empty()) {
			std::cerr << "No options available.\n";
			return {};
		}

		std::cout << decisionMessage << "\n";
		for (const auto& option : options) {
			std::cout << option.m_name << " -> " << option.m_keyName << "\n";
		}

		std::optional<std::string> userInput = readString("Your Input : ");
		if (!userInput.has_value()) {
			return {};
		}

		auto iterator = std::find_if(options.begin(), options.end(), [&userInput](const Option& item) {
			return userInput.value_or(std::string()) == item.m_key;
			});

		if (iterator == options.end()) {
			std::cerr << "Invalid option selected.\n";
			return {};
		}
		else {
			return *iterator;
		}
	};
	virtual std::optional<Option> pickOption(const char* decisionMessage, const std::vector<Option>& options) {
		if (options.empty()) {
			std::cerr << "No options available.\n";
			return {};
		}

		std::cout << decisionMessage << "\n";
		for (const auto& option : options) {
			std::cout << option.m_name << " -> " << option.m_keyName << "\n";
		}

		std::optional<std::string> userInput = readString("");
		if (!userInput.has_value()) {
			return {};
		}

		auto iterator = std::find_if(options.begin(), options.end(), [&userInput](const Option& item) {
			return userInput.value_or(std::string()) == item.m_key;
			});

		if (iterator == options.end()) {
			std::cerr << "Invalid option selected.\n";
			return {};
		}
		else {
			return *iterator;
		}
	};
};