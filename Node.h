#pragma once

#include <vector>
#include <string>
#include <memory>
#define interface struct

typedef size_t NodeUid;

enum class NodeType
{
	Text,
	Title,
	TextInput,
	NumberInput,
	FileInput,
	Display,
	Calculus,
	Output,
	End
};

enum class PrimitiveType {

	Char,
	Integer,
	Float,
	Double,
	String,
	Unknown
};

enum class OperationType {
	Add,
	Sub,
	Mul,
	Div,
	Min,
	Max
};

interface Displayable {
	virtual  std::string getContent() const noexcept = 0;
};


template <typename T>
interface Storable {

	virtual void setBuffer(T&& newData) {};
	virtual const T& getBuffer() const noexcept = 0;
};


// Forward declarations
class NumberInputNode;
class TextInputNode;
class FloatCalculusNode;
class StringCalculusNode;
class TextNode;
class OutputNode;

// Visitor base class
interface NodeVisitor {

	virtual void visit(NumberInputNode& node) = 0;
	virtual void visit(TextInputNode& node) = 0;
	virtual void visit(FloatCalculusNode& node) = 0;
	virtual void visit(StringCalculusNode& node) = 0;
	virtual void visit(TextNode& node) = 0;
	virtual void visit(OutputNode& node) = 0;
};

class Node {
public:
	Node(NodeUid uid , NodeType type) : m_uid(uid), m_type(type){};
	NodeUid getUid() const noexcept {
		return m_uid;
	}
	NodeType getType() const noexcept {
		return m_type;
	}
	virtual void acceptVisitor(NodeVisitor& visitor) = 0;
private:
	NodeUid m_uid;
	NodeType m_type;
};

class NumberInputNode : public Node, public Storable<float>, public Displayable {
public:
	NumberInputNode(NodeUid uid, std::string&& prompt) : Node(uid, NodeType::NumberInput), m_prompt(prompt) {};
	const std ::string &getPrompt()const noexcept {
		return m_prompt;
	}
	const float& getBuffer() const noexcept override {
		return m_input;
	}
	void setBuffer(float&& newData) noexcept override {
		m_input = newData;
	}
	 std::string getContent() const noexcept {
		return std::to_string(m_input);
	}
	void acceptVisitor(NodeVisitor& visitor) override {
		visitor.visit(*this);
	}
private:
	std::string m_prompt;
	float m_input = 0;
};

class TextInputNode : public Node, public Storable<std::string> , public Displayable {
public:
	TextInputNode(NodeUid uid, std::string&& prompt) : Node(uid , NodeType::TextInput), m_prompt(prompt) {};
	const std::string& getPrompt()const noexcept {
		return m_prompt;
	}
	const std::string& getBuffer() const noexcept override {
		return m_input;
	}
	void setBuffer(std::string&& newData) noexcept override {
		m_input = newData;
	}
	 std::string getContent() const noexcept {
		return m_input;
	}
	void acceptVisitor(NodeVisitor& visitor) override {
		visitor.visit(*this);
	}
private:
	std::string m_prompt;
	std::string m_input;
};


class TextNode : public Node, public Storable<std::pair<std::string, std::string>> , public Displayable{
public:
	TextNode(NodeUid uid, std::pair<std::string, std::string>&& pair) : Node(uid, NodeType::Text) {};
	
	const std::pair<std::string, std::string>& getBuffer() const noexcept override {
		return { title , body };
	}

	 std::string getContent() const noexcept override {
		return title + "\n" + body;
	}

	void setBuffer(std::pair<std::string, std::string>&& pair) noexcept override{
		title = std::move(pair.first);
		body = std::move(pair.second);
	}
	void acceptVisitor(NodeVisitor& visitor) override {
		visitor.visit(*this);
	}
private:
	std::string title, body;
};






class FloatCalculusNode : public Node, public Storable<float>, public Displayable {
public:
	FloatCalculusNode(NodeUid uid,  OperationType operationType, std::vector<NodeUid>&& dependencies)
		: Node(uid, NodeType::Calculus), m_operationType(operationType), m_dependencies(dependencies) {}

	const float& getBuffer() const noexcept override {
		return result;
	}

	void setBuffer(float result) noexcept  {
		this->result = result;
	}

	void acceptVisitor(NodeVisitor& visitor) override {
		visitor.visit(*this);
	}
	const std::vector<NodeUid>& getDependencies() const noexcept {
		return m_dependencies;
	}
	OperationType getOperationType() const noexcept {
		return m_operationType;
	}
	std::string getContent() const noexcept override {
		return std::to_string(result);
	}

private:
	OperationType m_operationType;
	float result;
	std::vector<NodeUid> m_dependencies;
};

class StringCalculusNode : public Node, public Storable < std ::string > , public Displayable {
public:
	StringCalculusNode(NodeUid uid,  OperationType operationType, std::vector<NodeUid>&& dependencies)
		: Node(uid, NodeType::Calculus), m_operationType(operationType), m_dependencies(dependencies) {}

	const std::string& getBuffer() const noexcept override {
		return result;
	}

	void setBuffer(std::string&& result) noexcept override {
		this->result = result;
	}

	void acceptVisitor(NodeVisitor& visitor) override {
		visitor.visit(*this);
	}
	const std::vector<NodeUid>& getDependencies() const noexcept {
		return m_dependencies;
	}
	OperationType getOperationType() const noexcept {
		return m_operationType;
	}
	std::string getContent() const noexcept override {
		return result;
	}

private:
	OperationType m_operationType;
	std::string result;
	std::vector<NodeUid> m_dependencies;
};

class OutputNode : public Node {
public:
	OutputNode(NodeUid uid, std::vector<NodeUid>&& dependencies) : Node(uid, NodeType::Output) {};
	const std::vector<NodeUid>& getDependencies() const noexcept {
		return m_dependencies;
	}
private:
	std::vector<NodeUid>m_dependencies;
};