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
	FloatCalculus,
	StringCalculus,
	Output,
	End
};
std::string nodeTypeToString(NodeType type) {
	switch (type) {
	case NodeType::Text:
		return "Text";
	case NodeType::Title:
		return "Title";
	case NodeType::TextInput:
		return "TextInput";
	case NodeType::NumberInput:
		return "NumberInput";
	case NodeType::FileInput:
		return "FileInput";
	case NodeType::Display:
		return "Display";
	case NodeType::FloatCalculus:
		return "FloatCalculus";
	case NodeType::StringCalculus:
		return "StringCalculus";
	case NodeType::Output:
		return "Output";
	case NodeType::End:
		return "End";
	default:
		return "UnknownNodeType";
	}
}

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
class DisplayNode;
class FileInputNode;
class TitleNode;
// Visitor base class
interface NodeVisitor {

	virtual void visit(NumberInputNode& node) = 0;
	virtual void visit(TextInputNode& node) = 0;
	virtual void visit(FloatCalculusNode& node) = 0;
	virtual void visit(StringCalculusNode& node) = 0;
	virtual void visit(TextNode& node) = 0;
	virtual void visit(OutputNode& node) = 0;
	virtual void visit(DisplayNode& node) = 0;
	virtual void visit(FileInputNode& node) = 0;
	virtual void visit(TitleNode& node) = 0;
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

class TitleNode : public Node, public Storable<std::pair<std::string, std::string>>, public Displayable {
public:
	TitleNode(NodeUid uid, std::pair<std::string, std::string>&& pair) : Node(uid, NodeType::Text) {};

	const std::pair<std::string, std::string>& getBuffer() const noexcept override {
		return { title , body };
	}

	std::string getContent() const noexcept override {
		return title + "\n" + body;
	}

	void setBuffer(std::pair<std::string, std::string>&& pair) noexcept override {
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
		: Node(uid, NodeType::FloatCalculus), m_operationType(operationType), result(0.0f), m_dependencies(dependencies) {}

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
		: Node(uid, NodeType::StringCalculus), m_operationType(operationType), m_dependencies(dependencies) {}

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
	OutputNode(NodeUid uid, 
			   std::string&& extension,
		       std::string&& fileName,
			   std::string&& title, 
			   std::string&& description,
			   std::vector<NodeUid>&& dependencies) : m_fileName(fileName), m_title(title), m_description(description), m_extension(extension), m_dependencies(dependencies), Node(uid, NodeType::Output) {};
	const std::vector<NodeUid>& getDependencies() const noexcept {
		return m_dependencies;
	}
	const char * getFileName() const noexcept {
		return m_fileName.c_str();
	}
	const char *  getTitle() const noexcept {
		return m_title.c_str();
	}
	const char *  getDescription() const noexcept {
		return m_description.c_str();
	}
	const char * getExtension() const noexcept {
		return m_extension.c_str();
	}
	void acceptVisitor(NodeVisitor& visitor) override {
		visitor.visit(*this);
	}
private:
	std::vector<NodeUid>m_dependencies;
	std::string m_fileName,m_title, m_description;
	std::string m_extension;
	std::string removeFileExtension(const std::string& filename) const noexcept {
		size_t lastDotPos = filename.find_last_of('.');
		if (lastDotPos != std::string::npos) {
			return filename.substr(0, lastDotPos);
		}
		else {
			// No file extension found
			return filename;
		}
	}
};

class DisplayNode : public Node {
public:
	DisplayNode(NodeUid uid, std::vector<NodeUid>&& dependencies) :Node(uid, NodeType::Display), m_dependencies(dependencies) {};
	const std::vector<NodeUid>& getDependencies() const noexcept {
		return m_dependencies;
	}
	void acceptVisitor(NodeVisitor& visitor) override {
		visitor.visit(*this);
	}
private:
	std::vector<NodeUid>m_dependencies;
};


class FileInputNode : public Node, public Storable<std::string>, public Displayable {
public:
	FileInputNode(std::string&& fileName, std::string&& extension, NodeUid uid) : m_fileName(fileName) , m_extension(extension), Node(uid, NodeType::FileInput), Storable() {};
	const std::string& getBuffer() const noexcept override {
		return m_buffer;
	}

	void setBuffer(std::string&& result) noexcept override {
		this->m_buffer = result;
	}

	void acceptVisitor(NodeVisitor& visitor) override {
		visitor.visit(*this);
	}
	
	const char* getFileName() const noexcept {
		return m_fileName.c_str();
	}
	const char *  getExtension() const noexcept {
		return m_extension.c_str();
	}
	std::string getContent() const noexcept override {
		return m_buffer;
	}
private:
	std::string m_fileName, m_extension;
	std::string m_buffer;
};