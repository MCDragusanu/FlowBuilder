#pragma once
#include <memory>
#include <vector>
#include <stdexcept>
#include <sstream>

template <typename DataType>
struct Operation {
	virtual DataType execute(const DataType& lhs, const DataType& rhs) const noexcept = 0;
};

template<typename T>
struct AdditionOperation :public Operation<T> {
	T execute(const T& lhs, const T& rhs) const noexcept override {
		return lhs + rhs;
	}
};

template <typename T>
struct SubstractionOperation : public Operation<T> {
	T execute(const T& lhs, const T& rhs)const noexcept override {
		return lhs - rhs;
	}
};

template <typename T>
struct MultiplicationOperation : public Operation<T> {
	T execute(const T& lhs, const T& rhs)const noexcept override {
		return lhs * rhs;
	}
};

template <typename T>
struct DivisionOperation : public Operation<T> {
	T execute(const T& lhs, const T& rhs) const noexcept {
		return lhs / rhs;
	}
};

template <typename T>
struct MaxOperation : public Operation<T> {
	T execute(const T& lhs, const T& rhs)const noexcept override {
		if (lhs > rhs) return lhs;
		else return rhs;
	}
};

template <typename T>
struct MinOperation : public Operation<T> {
	T execute(const T& lhs, const T& rhs)const noexcept override {
		if (lhs > rhs) return rhs;
		else return lhs;
	}
};

std::vector<std::string> splitWords(const std::string& str) {
	std::vector<std::string> words;
	std::istringstream iss(str);
	std::string word;
	while (iss >> word) {
		words.push_back(word);
	}
	return words;
}

std::string operator * (const std::string& lhs, const std::string& rhs) {
	std::string result;

	for (char ch_lhs : lhs) {
		for (char ch_rhs : rhs) {
			result += "(" + std::string(1, ch_lhs) + "," + std::string(1, ch_rhs) + ")";
		}
	}

	return result;
}
std::string operator-(const std::string& lhs, const std::string& rhs) {
	std::string result = lhs;

	// Iterate through each character in rhs and remove it from result
	for (char ch : rhs) {
		size_t pos = result.find(ch);
		if (pos != std::string::npos) {
			result.erase(pos, 1);
		}
	}

	return result;
}
std::string operator / (const std::string& str, const std::string& delimiter) {
	size_t pos = str.find(delimiter);

	// If the delimiter is found, return the substring before the delimiter
	if (pos != std::string::npos) {
		return str.substr(0, pos);
	}

	// If delimiter is not found, return the entire string
	return str;

}


template <typename DataType>
class Calculation {
public:
	/**
	 *
	 *
	 * This function performs a calculation on the provided operands using the specified
	 * operation and returns the result as a unique pointer to the DataType.
	 *
	 * @tparam T The type of data on which the calculation is performed.It is restricted to be of arithmetic type and std::string
	 * @param operands The collection of operands.
	 * @param operation The operation to be applied on the operands.
	 * @return A unique pointer to the result of the calculation.
	 * @throws std::invalid_argument if no operation is provided or if no operands are provided.
	 */
	template <typename T = DataType>
	//std::enable_if_t<std::is_arithmetic_v<T>|| std::is_same_v<T, std::string>, std::unique_ptr<DataType>>
	std::unique_ptr<T> execute(const std::vector<DataType>& operands, Operation<DataType>* operation) const {

		if (operation == nullptr) {
			throw std::invalid_argument("No Operation provided");
		}

		if (operands.empty()) {
			throw std::invalid_argument("No operands provided");
		}


		std::unique_ptr<DataType> result = std::make_unique<DataType>(operands.front());

		for (auto it = std::next(operands.begin()); it != operands.end(); ++it) {
			*result = operation->execute(*result, *it);
		}

		return result;

	}
};

template <typename DataType>
class OperationFactory {

public:
	template <typename T>
	std::unique_ptr<Operation<T>> createAdditionOperation() {
		return std::make_unique<AdditionOperation<T>>();
	}

	template <typename T>
	std::unique_ptr<Operation<T>> createSubtractionOperation() {
		return std::make_unique<SubstractionOperation<T>>();
	}

	template <typename T>
	std::unique_ptr<Operation<T>> createMultiplicationOperation() {
		return std::make_unique<MultiplicationOperation<T>>();
	}

	template <typename T>
	std::unique_ptr<Operation<T>> createDivisionOperation() {
		return std::make_unique<DivisionOperation<T>>();
	}

	template <typename T>
	std::unique_ptr<Operation<T>> createMaxOperation() {
		return std::make_unique<MaxOperation<T>>();
	}

	template <typename T>
	std::unique_ptr<Operation<T>> createMinOperation() {
		return std::make_unique<MinOperation<T>>();
	}

	static OperationFactory& getInstance() {
		static OperationFactory instance;
		return instance;
	}

private:
	// Private constructor for Singleton pattern
	OperationFactory() = default;
};