#pragma once
#pragma warning(disable: 4996)

#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <iostream>
#include <functional>
#include <algorithm>
#include <set>
#include <chrono>
#include <ctime>

#include "Node.h"
#include "Operation.h"
#include "filesystem.h"
#include "InputHandler.h"

class Flow : private NodeVisitor {
public: 
    Flow():m_flowName("Default Flow") {
        // Get the current time point
        auto currentTime = std::chrono::system_clock::now();

        // Convert the time point to a time_t object
        std::time_t currentTime_t = std::chrono::system_clock::to_time_t(currentTime);

        // Convert time_t to a local time string
        char timeString[100];
        std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime_t));

        m_timeStamp = timeString;
      }
    const char* getName() const noexcept {
        return m_flowName.c_str();
    }
    const char* getTimeOfCreation() const noexcept{
        return m_timeStamp.c_str();
    }
    void setName(std::string&& name) {
        this->m_flowName = name;
    }
    void executeFlow() {
        while (!executionOrder.empty()) {
            NodeUid uid = executionOrder.front();
            nodes.at(uid)->acceptVisitor(*this);
            executionOrder.pop();
        }
      }
      void addToFlow(Node* node) {
          if (nodes.find(node->getUid()) == nodes.end()) {
              nodes[node->getUid()] = node;
              executionOrder.push(node->getUid());
          }
      }
      void reset() {
          nodes.clear();
          while (!executionOrder.empty()) {
              executionOrder.pop();
          }
      }
      std::vector<Node*> filterNodesByType(std::function<bool(const Node*)> predicate) {
          std::vector<Node* > result;
          for (const auto entry : nodes) {
              if (predicate(entry.second)) {
                  result.push_back(entry.second);
              }
          }
          return result;
      }
      void printFlow(){
          std::queue tmp_q = executionOrder; //copy the original queue to the temporary queue
          std::cout << "*********** Flow So Far ***********\n";
          while (!tmp_q.empty())
          {
              NodeUid q_element = tmp_q.front();
              auto nodeEntry = nodes.at(q_element);
              std::cout << "(" << nodeTypeToString(nodeEntry->getType()) << "," << nodeEntry->getUid() << ") -> ";
              tmp_q.pop();
          }
          std::cout << "\n**************************\n";
      }
private:
    FileSystem* fileSystem = FileSystem::getInstance();
    std::unordered_map<NodeUid, Node*> nodes;
    std::queue<NodeUid> executionOrder;
    std::string m_flowName;
    std::string m_timeStamp;
    InputHandler handler;
    void restartDecision(std::function<void()> onSkip, std::function<void()> onRestart) {
        auto picked = handler.pickOption("Do you want to restart", { Option("Yes" , "Y" , "Y") , Option("No" , "N" , "N")});
        if (picked.has_value()) {
            if (picked->m_key == "Y") {
                onRestart();
            }
            else onSkip();
        }
      
    }

    void visit(NumberInputNode& node) override {
        try{
            auto skip = handler.pickOption("Do you want to skip this step?", { Option("Yes" , "Yes" , "Yes") , Option("No" , "No" , "No") });
            if (skip.has_value()) {
                if (skip->m_key == "Yes") {
                    node.setBuffer(0.0f);
                    return;
                }
            }
            auto result = handler.readFloat(node.getPrompt().c_str());
            if (!result.has_value()) {
                throw InvalidInput("Input provided can't be transformed to float");
            }
            if (result.has_value()) {
                node.setBuffer(std::move(result.value_or(0.0f)));
            }
           
        }
        catch (const InvalidInput& e) {
            std::cerr << e.what() << "\n";
            restartDecision([&node]() {
                node.setBuffer(std::move(0.0f));
                }, [this, &node]() {
                    this->visit(node);
                });
        }
    }
    void visit(TitleNode& node) override {
        //std::cout << node.getContent()<<"\n";
    }
    void visit(FileInputNode& node) override {

        try {
            auto skip = handler.pickOption("Do you want to skip this step", { Option("Yes" , "Yes" , "Yes") , Option("No" , "No" , "No") });
            if (skip.has_value()) {
                if (skip->m_key == "Yes") {
                    node.setBuffer(std::string());
                    return;
                }
            }
            auto fileHandle = fileSystem->getFileHandle(node.getFileName(), translateExtension(node.getExtension()));

            if (fileHandle == nullptr) {
                throw InvalidHandle((std::string("Failed to get a file handle for file ") + std::string(node.getFileName()) + std::string(node.getExtension())).c_str());
            }
            auto fileContent = fileSystem->readFromInputFile(fileHandle.get());
            node.setBuffer(std::move(fileContent));
        }
        catch (const InvalidHandle& e) {
            std::cerr << e.what() << "\n";
            restartDecision([&node]() {

                }, [this, &node]() {
                    this->visit(node);
                });
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
            restartDecision([&node]() {

                }, [this, &node]() {
                    this->visit(node);
                });
        }
    }

    void visit(TextInputNode& node) override {
        try {
            auto skip = handler.pickOption("Do you want to skip this step", { Option("Yes" , "Yes" , "Yes") , Option("No" , "No" , "No") });
            if (skip.has_value()) {
                if (skip->m_key == "Yes") {
                    node.setBuffer("");
                    return;
                }
            }
            auto result = handler.readString(node.getPrompt().c_str());
            if (!result.has_value()) {
                throw InvalidInput("Input provided can't be transformed to string");
            }
            if (result.has_value()) {
                node.setBuffer(std::move(result.value_or(std::string())));
            }

        }
        catch (const InvalidInput& e) {
            std::cerr << e.what() << "\n";
            restartDecision([&node]() {
                node.setBuffer("");
                }, [this, &node]() {
                    this->visit(node);
                });
        }
    }

    void visit(FloatCalculusNode& node) override {
        try {
            auto skip = handler.pickOption("Do you want to skip this step", { Option("Yes" , "Yes" , "Yes") , Option("No" , "No" , "No") });
            if (skip.has_value()) {
                if (skip->m_key == "Yes") {
                    node.setBuffer(0.0f);
                    return;
                }
            }
            auto dependencies = node.getDependencies();
            auto foundNodes = std::vector<float>();
            auto typeSet = std::set<NodeType>();
            if (dependencies.size() == 0) return;


            std::transform(dependencies.begin(), dependencies.end(), std::back_inserter(foundNodes), [this, &typeSet](NodeUid& uid) {
                //check to see if it is found
                auto iterator = nodes.find(uid); 
               
                //if not throw exception
                if (iterator == nodes.end()) {
                    std::stringstream ss;
                    ss << "Leaf Node with uid = " << uid  << " was not found \n";
                    throw InvalidInput(ss.str().c_str());
                } 

                //add the new type to the set
                typeSet.insert(iterator->second->getType());
               
                //check to see if the node implements this interface
                auto storable = dynamic_cast<Storable<float>*>(iterator->second);
                if (storable != nullptr) return float(storable->getBuffer());
                else return 0.0f;
                });


            //check to see if there are more types than supported
            if (typeSet.count(NodeType::NumberInput) + typeSet.count(NodeType::FloatCalculus) != typeSet.size()) {
                std::stringstream ss;
                ss << "Operation cannot be performed! The nodes must be either of type NumberInput and / or FloatCalculus" << "\n";
                ss << "Provided types are : ";
                for (auto& type : typeSet) {
                    ss << nodeTypeToString(type) << ",";
                }
                throw InvalidInput(ss.str().c_str());
            }



            auto result = performNumberOperation(foundNodes, node.getOperationType());
            node.setBuffer(result);


            return;
        }
        catch (const std::exception& e) {
            std::cerr << e.what()<<"\n";
            restartDecision([&node]() {
                node.setBuffer(0.0f);
                }, [this, &node]() {
                    this->visit(node);
                });
        }
    }

    void visit(StringCalculusNode& node) override {
        try {
            auto skip = handler.pickOption("Do you want to skip this step", { Option("Yes" , "Yes" , "Yes") , Option("No" , "No" , "No") });
            if (skip.has_value()) {
                if (skip->m_key == "Yes") {
                    node.setBuffer(std::string());
                    return;
                }
            }
            auto dependencies = node.getDependencies();
            auto foundInput = std::vector<std::string>();
            std::stringstream ss;

            std::transform(dependencies.begin(), dependencies.end(), std::back_inserter(foundInput), [this](NodeUid& uid) {

                //check to see if it is found
                auto iterator = nodes.find(uid);

                //if not throw exception
                if (iterator == nodes.end()) {
                    std::stringstream ss;
                    ss << "Leaf Node with uid = " << uid << " was not found \n";
                    throw InvalidInput(ss.str().c_str());
                }
                //check to see it if implements the interface
                auto displayable = dynamic_cast<Displayable*>(iterator->second);
                if (displayable != nullptr) {
                    return displayable->getContent();
                }

                else return std::string();
                });

            
            auto result = performStringOperation(foundInput, node.getOperationType());

            node.setBuffer(std::move(result));
            return;
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
            restartDecision([&node]() {
                node.setBuffer("");
                }, [this, &node]() {
                    this->visit(node);
                });
        }
    }
    FileExtension translateExtension(const char* extension) {
        if (strcmp(extension, ".csv") == 0) return CSV;
        else if (strcmp(extension, ".txt") == 0) return TXT;
        throw InvalidHandle("The extension was not recognized");
    }
    void visit(OutputNode& node) override {
        try
        {
            auto skip = handler.pickOption("Do you want to skip this step", { Option("Yes" , "Yes" , "Yes") , Option("No" , "No" , "No") });
            if (skip.has_value()) {
                if (skip->m_key == "Yes") return;
            }

            auto handle = fileSystem->getFileHandle(node.getFileName(), translateExtension(node.getExtension()));
            if (handle == nullptr) {
                throw InvalidHandle("Failed to get a valid handle");
            }

            auto dependencies = node.getDependencies();
            auto foundInput = std::vector<std::string>();
            std::stringstream ss;

            std::transform(dependencies.begin(), dependencies.end(), std::back_inserter(foundInput), [this](NodeUid& uid) {

                //check to see if it is found
                auto iterator = nodes.find(uid);

                //if not throw exception
                if (iterator == nodes.end()) {
                    std::stringstream ss;
                    ss << "Leaf Node with uid = " << uid << " was not found \n";
                    throw InvalidInput(ss.str().c_str());
                }
                //check to see it if implements the interface
                auto displayable = dynamic_cast<Displayable*>(iterator->second);
                if (displayable != nullptr) {
                    return displayable->getContent();
                }

                else return std::string();
                });

            char delim = node.getExtension() == ".csv" ? ',' : ' ';
          

            ss << node.getTitle() << "\n";
            ss << node.getDescription() << "\n";
            for (auto iterator = foundInput.cbegin(); iterator < foundInput.cend(); ++iterator) {
                ss << *iterator;
                if (iterator < foundInput.cend() - 1) {
                    ss << delim;
                }
                ss << '\n';
            }

            if (fileSystem->writeToFile(handle.get(), ss.str())) {
                fileSystem->saveFile(handle.get());
            }
        }
        catch (const InvalidHandle& e) {
            std::cerr << e.what() << "\n";
            restartDecision([&node]() {

                }, [this, &node]() {
                    this->visit(node);
                });
        }
        catch (const InvalidInput& e)
        {
            std::cerr << e.what() << "\n";
            restartDecision([&node]() {

                }, [this, &node]() {
                    this->visit(node);
                });
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
            restartDecision([&node]() {

                }, [this, &node]() {
                    this->visit(node);
                });
        }

    }

    void visit(DisplayNode& node) {

        auto skip = handler.pickOption("Do you want to skip this step", { Option("Yes" , "Yes" , "Yes") , Option("No" , "No" , "No") });
        if (skip.has_value()) {
            if (skip->m_key == "Yes") return;
        }
        auto dependencies = node.getDependencies();
        auto foundInput = std::vector<std::string>();
     

        std::transform(dependencies.begin(), dependencies.end(), std::back_inserter(foundInput), [this](NodeUid& uid) {

            //check to see if it is found
            auto iterator = nodes.find(uid);

            //if not throw exception
            if (iterator == nodes.end()) {
                std::stringstream ss;
                ss << "Leaf Node with uid = " << uid << " was not found \n";
                throw InvalidInput(ss.str().c_str());
            }
            //check to see it if implements the interface
            auto displayable = dynamic_cast<Displayable*>(iterator->second);
            if (displayable != nullptr) {
                return displayable->getContent();
            }

            else return std::string();
            });

        char delim = ' ';
     

        std::stringstream ss;
        for (auto iterator = foundInput.cbegin(); iterator < foundInput.cend(); ++iterator) {
            ss << *iterator;
            if (iterator < foundInput.cend() - 1) {
                ss << delim;
            }
        }
        std::cout << ss.str() << "\n";

    }

    float performNumberOperation(const std::vector<float>& operands, OperationType operation) {


        auto operationImpl = getNumberOperation(operation);

        auto result = Calculation<float>().execute(operands, operationImpl.get());

        return *result.release();
    }

   std::string performStringOperation(const std::vector<std::string> operands, OperationType operation) {

        auto operationImpl = getStringOperation(operation);

        auto result = Calculation<std::string>().execute(operands, operationImpl.get());

        return *result.release();
    }



    std::unique_ptr<Operation<float>> getNumberOperation(OperationType operation) {
        switch (operation) {
        case OperationType::Add:
            return OperationFactory<float>::getInstance().createAdditionOperation<float>();
        case OperationType::Sub:
            return OperationFactory<float>::getInstance().createSubtractionOperation<float>();
        case OperationType::Mul:
            return OperationFactory<float>::getInstance().createMultiplicationOperation<float>();
        case OperationType::Div:
            return OperationFactory<float>::getInstance().createDivisionOperation<float>();
        case OperationType::Min:
            return OperationFactory<float>::getInstance().createMinOperation<float>();
        case OperationType::Max:
            return OperationFactory<float>::getInstance().createMaxOperation<float>();

        default:
            throw std::invalid_argument("Unsupported operation type");
        }
    }

    std::unique_ptr<Operation<std::string>> getStringOperation(OperationType operation) {
        switch (operation) {
        case OperationType::Add:
            return OperationFactory<std::string>::getInstance().createAdditionOperation<std::string>();
        case OperationType::Sub:
            return OperationFactory<std::string>::getInstance().createSubtractionOperation<std::string>();
        case OperationType::Mul:
            return OperationFactory<std::string>::getInstance().createMultiplicationOperation<std::string>();
        case OperationType::Div:
            return OperationFactory<std::string>::getInstance().createDivisionOperation<std::string>();
        case OperationType::Min:
            return OperationFactory<std::string>::getInstance().createMinOperation<std::string>();
        case OperationType::Max:
            return OperationFactory<std::string>::getInstance().createMaxOperation<std::string>();

        default:
            throw std::invalid_argument("Unsupported operation type");
        }
    }
    void visit(TextNode& node) {
     //   std::cout << node.getContent();
    }
    void visit(EndNode& node) {
        std::cout << "flow has finished";
    }
    
};