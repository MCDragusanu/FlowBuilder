#include "FlowBuilder.h"

#include <conio.h> 

void CreateNewFlowState::doWork(FlowController& controller) {
    bool isRunning = true;
    

    while (isRunning) {
        //method to clear the console screen
        system("CLS");
        flow.printFlow();
        auto picked = handler.pickOption("What Type of Node do you want to add?",
            { Option("Title Node", "0", "0"), Option("Text Node", "1", "1"), Option("TextInput Node", "2", "2"),
                Option("NumberInput Node", "3", "3"), Option("Float Calculus Node", "4", "4"),
                Option("String Calculus Node", "5", "5"), Option("Display Node", "6", "6"),
                Option("File Input Node", "7", "7"), Option("Output Node", "8", "8"),
                Option("End Node", "9", "9") });

        if (!picked.has_value()) {
            restartDecision(controller);
        }

        if (strcmp("0", picked->m_key.c_str()) == 0) {
            std::cout << "\nYou have picked Title Node\n";
            addTitleNode(controller);
        }
        else if (picked->m_key == "1") {
            std::cout << "\nYou have picked Text Node\n";
            addTextNode(controller);
        }
        else if (picked->m_key == "2") {
            std::cout << "\nYou have picked Text Input Node\n";
            addTextInputNode(controller);
        }
        else if (picked->m_key == "3") {
            std::cout << "\nYou have Number Input Node\n";
            addNumberInputNode(controller);
        }
        else if (picked->m_key == "4") {
            std::cout << "\nYou have picked Float Calculus Node\n";
            addFloatCalculusNode(controller);
        }
        else if (picked->m_key == "5") {
            std::cout << "\nYou have picked String Calculus Node\n";
            addStringCalculusNode(controller);
        }
        else if (picked->m_key == "6") {
            std::cout << "\nYou have picked Display Node\n";
            addDisplayNode(controller);
        }
        else if (picked->m_key == "7") {
            std::cout << "\nYou have picked File Input Node\n";
            addFileInputNode(controller);
        }
        else if (picked->m_key == "8") {
            std::cout << "\nYou have picked Output Node\n";
            addOutputNode();
        }
        else if (picked->m_key == "9") {
            std::cout << "\nYou have picked End Node\n";
            addEndNode();
            isRunning = false;
            controller.addNewFlow(std::move(flow));
            onExit(controller);
        }
        else {
            
            restartDecision(controller);
        }

    }

}

void CreateNewFlowState::restartDecision(FlowController& controller ) {
auto picked2 = handler.pickOption("Wrong Input Provided!\nWhat Do you want to do next?",
        { Option("Restart Flow Creation", "Yes", "Yes"), Option("Exit Flow Creation", "Exit","Exit")});
    if (picked2.has_value()) {
        if (picked2->m_key == "Yes") {
            flow.reset();
            doWork(controller);
        }
        else if (picked2->m_key == "Exit") {
            onExit(controller);
        }
    }
}
void CreateNewFlowState::addFloatCalculusNode(FlowController& controller)
{
    
    auto dependencies = buildDependencies([](const auto* node) {
        return node->getType() == NodeType::FloatCalculus || node->getType() == NodeType::NumberInput;
        });

    if (dependencies.size() == 0) {
        return;
    }
    OperationType type = pickOperation().value_or(OperationType::Add);
   
    auto calculusNode =new  FloatCalculusNode(++counter, type, std::move(dependencies));
    flow.addToFlow(calculusNode);
    std::cout << "Float Calculus node Added\n";
}

std::vector<NodeUid> CreateNewFlowState::buildDependencies(std::function<bool(const Node* node)> predicate) {
    auto allPossibleNodes = this->flow.filterNodesByType(predicate);

    if (allPossibleNodes.empty()) {
        std::cout << "No eligible nodes found.\n";
        return {};
    }

    std::vector<Option> options;
    std::transform(allPossibleNodes.begin(), allPossibleNodes.end(), std::back_inserter(options), [](const Node* node) {
        return Option("( " + nodeTypeToString(node->getType()) + " , " + std::to_string(node->getUid()), std::to_string(node->getUid()) + ")", std::to_string(node->getUid()));
        });

    std::string continuation = "Yes";
    std::vector<NodeUid> dependencies;

    while (continuation == "Yes") {
        auto picked = handler.pickOption("Choose the nodes involved in the calculation", options);
        if (picked.has_value()) {
            auto uid = static_cast<NodeUid>(std::atol(picked->m_key.c_str()));
            auto iterator = std::find_if(allPossibleNodes.begin(), allPossibleNodes.end(), [uid](const Node* node) {
                return node->getUid() == uid;
                });

            if (iterator != allPossibleNodes.end()) {
                dependencies.push_back(uid);
                std::cout << "Node with Uid = " << uid << " has been selected.\n";
            }
            else {   
                std::cout << "Invalid Uid entered!\n";
            }
        }
             

        std::cout << "Enter 'Yes' to continue or anything else to stop: ";
        std::getline(std::cin , continuation);
    }

    return dependencies;
}

std::optional<OperationType> CreateNewFlowState::pickOperation() {
    std::vector<Option> operationOptions = {
        Option("Plus", "Add", "Add"),
        Option("Minus", "Sub", "Sub"),
        Option("Mul", "Mul", "Mul"),
        Option("Div", "Div", "Div"),
        Option("Min", "Min", "Min"),
        Option("Max", "Max", "Max")
    };

    auto pickedOperation = handler.pickOption("Pick an operation", operationOptions);

    if (pickedOperation.has_value()) {
        const std::string& operationName = pickedOperation->m_key;

        if (operationName == "Add") {
            return OperationType::Add;
        }
        else if (operationName == "Sub") {
            return OperationType::Sub;
        }
        else if (operationName == "Mul") {
            return OperationType::Mul;
        }
        else if (operationName == "Div") {
            return OperationType::Div;
        }
        else if (operationName == "Min") {
            return OperationType::Min;
        }
        else if (operationName == "Max") {
            return OperationType::Max;
        }
        else {
           
            std::cout << "Unknown operation picked. Setting default to Add.\n";
            return OperationType::Add;
        }
    }
    else {
     
        std::cout << "No operation picked. Setting default to Add.\n";
        return OperationType::Add;
    }
}

void CreateNewFlowState::addTitleNode(FlowController& controller) {
    auto title = handler.readString("Enter title : ");
    auto description = handler.readString("Enter description : ");

    if (title.has_value() && description.has_value()) {
        auto titleNode = new TitleNode(++counter, { std::string(*title), std::string(*description) });
        flow.addToFlow(titleNode);
        std::cout << "Title Node Added\n";
    }
    else {
        handleInvalidInput(controller);
    }
}

void CreateNewFlowState::addTextNode(FlowController& controller) {
    auto title = handler.readString("Enter title : ");
    auto body = handler.readString("Enter body : ");

    if (title.has_value() && body.has_value()) {
        auto textNode = new TextNode(++counter, { std::string(*title), std::string(*body) });
        flow.addToFlow(textNode);
        std::cout << "Text Node Added\n";
    }
    else {
        handleInvalidInput(controller);
    }
}

void CreateNewFlowState::addTextInputNode(FlowController& controller) {
    auto inputDescription = handler.readString("Enter Input Description : ");

    if (inputDescription.has_value()) {
        auto textInputNode = new TextInputNode(++counter, std::move(std::string(*inputDescription)));
        flow.addToFlow(textInputNode);
        std::cout << "Text Input Node Added\n";
    }
    else {
        handleInvalidInput(controller);
    }
}

void CreateNewFlowState::addNumberInputNode(FlowController& controller) {
    auto inputDescription = handler.readString("Enter Input Description : ");

    if (inputDescription.has_value()) {
        auto numberInputNode = new NumberInputNode(++counter, std::move(std::string(*inputDescription)));
        flow.addToFlow(numberInputNode);
        std::cout << "Number Input Node Added\n";
    }
    else {
        handleInvalidInput(controller);
    }
}

void CreateNewFlowState::addStringCalculusNode(FlowController& controller)
{
    auto dependencies = buildDependencies([](const auto* node) {
        return node->getType() != NodeType::Display || node->getType() == NodeType::Output || node->getType()!=NodeType::End;
        });
    if (dependencies.size() == 0) {
        std::cout << "\nNo valid Nodes Found\n";
        return;
    }
    OperationType type = pickOperation().value_or(OperationType::Add);

    auto calculusNode = new  StringCalculusNode(++counter, type, std::move(dependencies));
    flow.addToFlow(calculusNode);
    std::cout << "String Calculus node Added\n";
}

void CreateNewFlowState::addDisplayNode(FlowController& controller)
{
    auto dependencies = buildDependencies([](const auto* node) {
        return node->getType() != NodeType::Display || node->getType() == NodeType::Output || node->getType() != NodeType::End;
        });

    auto calculusNode = new  DisplayNode(++counter,  std::move(dependencies));
    flow.addToFlow(calculusNode);
    std::cout << "Display node Added\n";
}

void CreateNewFlowState::addFileInputNode(FlowController& controller) {
    auto inputDescription = handler.readString("Enter Input Description : ");
    auto extension = handler.pickOption("Pick the desired format",
        { Option(".csv", "a", "a"), Option(".txt", "b", "b") });

    if (inputDescription.has_value() && extension.has_value()) {
        auto fileInputNode =
            new FileInputNode(std::move(std::string(*inputDescription)), std::move(std::string(extension->m_name)), ++counter);
        flow.addToFlow(fileInputNode);
        std::cout << "File Input Node Added\n";
    }
    else {
        handleInvalidInput(controller);
    }
}

void CreateNewFlowState::addOutputNode()
{

    std::string title, description, extension, fileName;
 
    fileName = handler.readString("Enter File Name : ").value_or("DEFAULT_FILE_NAME");
    title = handler.readString("Enter Title : ").value_or("Default Title");
    description = handler.readString("Enter Description : ").value_or("Default description");
    extension = handler.pickOption("Pick Extension", { Option(".csv" , "a" , "a") , Option(".txt" , "b" , "b") }).value_or(Option(".txt", "b", "b")).m_name;
    
    auto dependencies = buildDependencies([](const auto* node) {
        return node->getType() != NodeType::Display || node->getType() == NodeType::Output || node->getType() != NodeType::End;
        });
    if (dependencies.empty()) {
    
        return;
    }
    auto outputNode = new OutputNode(++counter, std::move(extension), std::move(fileName), std::move(title), std::move(description), std::move(dependencies));
    flow.addToFlow(outputNode);
    std::cout << "Output Node Added\n";
}

void CreateNewFlowState::addEndNode()
{
    EndNode* node = new EndNode(++counter);
    flow.addToFlow(node);
}

void CreateNewFlowState::handleInvalidInput(FlowController& controller) {
    auto picked = handler.pickOption("Wrong Input Provided!\nWhat Do you want to do next?",
        { Option("Restart Node Creation", "Restart", "Restart"), Option("Exit", "Exit", "Exit") });

    if (picked.has_value()) {
        if (picked->m_key == "Restart") {
            flow.reset();
            doWork(controller);
        }
        else if (picked->m_key == "Exit") {
            onExit(controller);
        }
    }
}

void StartState::doWork(FlowController& controller) {
    system("CLS");
    std::cout << "Welcome to Flow Builder 1.1.0\n";
    auto picked = handler.pickOption("What do you want to do?",
        { Option("Run Existing Flow", "a", "a"), Option("Create New Flow", "b", "b")  , Option("Delete Flow", "c", "c") });

    if (picked.has_value()) {
        if (picked->m_key == "a") {

            RunExistingFlowState* runExisting = new RunExistingFlowState();
            controller.setState(runExisting);
           
        }
        else if (picked->m_key == "b") {
            CreateNewFlowState* createFlow = new CreateNewFlowState();
            controller.setState(createFlow);
           
        }
        else if (picked->m_key == "c") {
            DeleteExistingFlow* deleteFlow = new DeleteExistingFlow();
            controller.setState(deleteFlow);

        }
        else {
            goto decision;
        }
    }
    else {
    decision:
        auto picked2 = handler.pickOption("Wrong Input Provided!\nWhat Do you want to do next?",
            { Option("Restart", "Yes", "Yes"), Option("Exit", "No", "No") });
        if (picked2.has_value()) {
            if (picked2->m_key == "Y") {
                doWork(controller);
            }
            else if (picked2->m_key == "N") {
                return;
            }
        }
    }
}

FlowController::FlowController()
{
    
}

void FlowController::start() {
    FlowState* state = new StartState();
    state->doWork(*this);
}

void DeleteExistingFlow::onExit(FlowController& controller)
{
    controller.setState(new StartState());
}

void DeleteExistingFlow::doWork(FlowController& controller)
{
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
    auto result = handler.pickOption("Pick the desired Flow to be deleted ", options);
    if (result.has_value()) {
        auto index = std::atoi(result->m_key.c_str()) - 1;
        system("CLS");
        std::cout << "\nExecution has began" << "\n";
        controller.removeFlow(index);
        onExit(controller);
    }
}
