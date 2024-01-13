#pragma once
#include <queue>
#include <vector>
#include <iostream>

#include "Flow.h"

// Forward declarations
struct FlowController;
class StartState;
class CreateNewFlowState;
class RunExistingFlowState;

struct FlowState {
    virtual void onEnter(FlowController& controller) = 0;
    virtual void onExit(FlowController& controller) = 0;
    virtual void doWork(FlowController& controller) = 0;
    virtual const char* getType() const noexcept = 0;
};

struct FlowController {
    FlowController(); 

    void setState(FlowState* state) {
        
        state->onEnter(*this);
        state->doWork(*this);
        state->onExit(*this);
        delete state;
    }

    void addNewFlow(Flow&& flow) {
        m_flows.emplace_back(flow);
    }
    void removeFlow(size_t index) {
        m_flows.erase(m_flows.begin() + index);
        std::cout << "\nFlow Removed!\n";
    }
    void start(); 
    std::vector<Flow> getCurrentFlows() const noexcept {
        return m_flows;
    }
private:
    std::vector<Flow> m_flows;
   // FlowState* currentState ;
};

class StartState : public FlowState {
public:
    StartState() : FlowState() {}

    void onEnter(FlowController& controller) override {
        // Initialization code if needed
    }

    void onExit(FlowController& controller) override {
        // Cleanup code if needed
    }
    const char* getType() const noexcept override {
        return "Initial State";
    }
    void doWork(FlowController& controller) override;

private:
    InputHandler handler;
};

class CreateNewFlowState : public FlowState {
public:
public:
    CreateNewFlowState() : flow() {  };
    void onEnter(FlowController& controller) override {
        auto name = handler.readString("\nEnter flow Name : ").value_or("Default flow");
        flow.setName(std::move(name));
    }

    void onExit(FlowController& controller) override {
        StartState* newState = new StartState();
        controller.setState(newState);
    }
    const char* getType() const noexcept override {
        return "Create Flow State";
    }
    void doWork(FlowController& controller) override;

private:
    std::vector<NodeUid> buildDependencies(std::function<bool(const Node* node)> predicate);
    std::optional<OperationType> pickOperation();
    void restartDecision(FlowController& controller);

    void addTitleNode(FlowController& controller);

    void addTextNode(FlowController& controller);

    void addTextInputNode(FlowController& controller);

    void addNumberInputNode(FlowController& controller);

    void addFloatCalculusNode(FlowController& controller);

    void addStringCalculusNode(FlowController& controller);

    void addDisplayNode(FlowController& controller);

    void addFileInputNode(FlowController& controller);

    void addOutputNode();

    void addEndNode();

    void handleInvalidInput(FlowController& controller);

    NodeUid counter = 0;
    Flow flow;
    InputHandler handler;
};

class RunExistingFlowState : public FlowState {
public:
    void onEnter(FlowController& controller) override {
        // Initialization code if needed
    }

    void onExit(FlowController& controller) override {
        controller.setState((new StartState()));
    }

    void doWork(FlowController& controller) override {
        auto options = std::vector<Option>();
        auto flows = controller.getCurrentFlows();
        if (flows.empty()) {
            std::cout << "There are no flows!";
            onExit(controller);
            return;
        }
        for (size_t index = 0; index < flows.size(); index++) {
            std::stringstream ss;
            ss << index + 1 << "." << " " << flows.at(index).getName() << " " << flows.at(index).getTimeOfCreation();
            options.emplace_back(ss.str(), std::to_string(index + 1), std::to_string(index + 1));
        }
        auto result = handler.pickOption("Pick the desired Flow ", options);
        if (result.has_value()) {
            auto index = std::atoi(result->m_key.c_str())-1;
            system("CLS");
            std::cout << "\nExecution has began"<<"\n";
          
            flows.at(index).executeFlow();
            onExit(controller);
        }
    }
    const char* getType() const noexcept override {
        return "Run Existing Flow State";
    }
private:
    InputHandler handler;
};

class DeleteExistingFlow : public FlowState {
public:
    void onEnter(FlowController& controller) {
    };
    void onExit(FlowController& controller);
    void doWork(FlowController& controller);
    const char* getType() const noexcept {
        return "Delete Existing Flow";
    };
private:
    InputHandler handler;
};




