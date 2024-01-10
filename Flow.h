#pragma once


#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <iostream>
#include <functional>

#include "Node.h"
#include "Operation.h"
#include "filesystem.h"


class Flow : private NodeVisitor {
public: 
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
   
private:
    FileSystem* fileSystem = FileSystem::getInstance();
    std::unordered_map<NodeUid, Node*> nodes;
    std::queue<NodeUid> executionOrder;

    void restartDecision(std::function<void()> onSkip, std::function<void()> onRestart) {
        std::cout << "Do you want to retry or skip?\n";
        std::cout << "Y or y -> Yes" << "    Any other -> Skip\n";
        char picked;
        std::cin >> picked;

        if (picked == 'y' || picked == 'Y') {
            onRestart();
        }
        else onSkip();
    }

    void visit(NumberInputNode& node) override {
        try{
            std::cout << node.getPrompt();
            std::string input;
            std::cin >> input;
            node.setBuffer(std::move(std::atof(input.c_str())));
        }
        catch (std::bad_alloc& e) {
            std::cerr << e.what() << "\n";
            restartDecision([&node]() {
                node.setBuffer(std::move(0.0f));
                }, [this, &node]() {
                    this->visit(node);
                });
        }
    }

    void visit(TextInputNode& node) override {
        try {
            std::cout << node.getPrompt();
            std::string input;
            std::cin >> input;
            node.setBuffer(std::move((input.c_str())));
        }
        catch (std::bad_alloc& e) {
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
            auto dependencies = node.getDependencies();
            auto foundNodes = std::vector<Node*>();

            if (dependencies.size() == 0) return;

            //used to determine if more than 1 type is present 
            bool operationCanBePerformed = true;

            //retrieve the dependenies from map
            for (const auto& dependency : dependencies) {


                auto where = nodes.find(dependency);

                auto isInside = where != nodes.end();

                if (isInside) {

                    //check to see if there are more than 1 type
                    if (where->second->getType() != NodeType::NumberInput) {
                        operationCanBePerformed = false;
                        break;
                        //exit the loop
                        //calculation can't be performed
                    }

                    //added to retrieved nodes
                    foundNodes.push_back(where->second);
                }
                if (!isInside) {
                    std::stringstream ss;
                    ss<< "Leaf Node with uid = " << node.getUid() << " and type  = " << (int)node.getType() << " was not found \n";
                    throw std::invalid_argument(ss.str());
                }
            }

            if (!operationCanBePerformed) {
                std::stringstream ss;
                ss << "Can't perform operation on instances of different types! Required Types are : NumberInputNode ";
                throw std::invalid_argument(ss.str());
                
            }

            auto members = std::vector<float>();
            std::transform(foundNodes.begin(), foundNodes.end(), std::back_inserter(members), [](Node* node) {
                return dynamic_cast<Storable<float>*>(node)->getBuffer();
                });
            auto result = performNumberOperation(members, node.getOperationType());
            node.setBuffer(*result.release());
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
            auto dependencies = node.getDependencies();
            auto foundInput = std::vector<std::string>();
            std::stringstream ss;
            if (dependencies.size() == 0) return;

            //used to determine if more than 1 type is present 
            bool operationCanBePerformed = true;

            //retrieve the dependenies from map
            for (const auto& dependency : dependencies) {


                auto where = nodes.find(dependency);

                auto isInside = where != nodes.end();

                if (isInside) {
                    auto displayable = dynamic_cast<Displayable*>(&(*where->second));
                    if (displayable != nullptr) {
                        foundInput.push_back(displayable->getContent());
                    }

                }
                if (!isInside) {
                    std::stringstream ss;
                    ss << "Leaf Node with uid = " << node.getUid() << " and type  = " << (int)node.getType() << " was not found \n";
                    throw std::invalid_argument(ss.str());
                    return;
                }


            }

            auto result = performStringOperation(foundInput, node.getOperationType());

            node.setBuffer(std::move(*result.release()));
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
    void visit(OutputNode& node) override {

    }

    std::unique_ptr<float> performNumberOperation(const std::vector<float>& operands, OperationType operation) {


        auto operationImpl = getNumberOperation(operation);

        auto result = Calculation<float>().execute(operands, operationImpl.get());

        return result;
    }

    std::unique_ptr<std::string> performStringOperation(const std::vector<std::string> operands, OperationType operation) {

        auto operationImpl = getStringOperation(operation);

        auto result = Calculation<std::string>().execute(operands, operationImpl.get());

        return result;
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

    }
};