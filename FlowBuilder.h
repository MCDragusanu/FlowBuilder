#pragma once

#include "Flow.h"
#include <queue>

struct FlowState;

struct FlowController {
	void setState(FlowState& state) {
		state.onEnter(*this);
		state.doWork(*this);
		state.onExit(*this);
	 }
	void addNewFlow(Flow&& flow) {
		m_flows.emplace_back(flow);
	}
private:
	std::vector<Flow> m_flows;
};

struct FlowState {
	virtual void onEnter(FlowController& controller) = 0;
	virtual void onExit(FlowController& controller) = 0;
	virtual void doWork(FlowController& controller) = 0;
};

class CreateNewFlowState : public FlowState {

public:
	void onEnter(FlowController& controller) override {

	}
	void onExit(FlowController& controller) {
		auto newState = StartState();
		controller.setState(newState);
	}
	void doWork(FlowController& controller) {
		bool isRunning = true;
		while (isRunning) {
			auto picked = handler.pickOption("What Type of Node do you want to add?",
				{
					Option("Title Node" , "0" , '0') ,
					Option("Text Node" , "1" , '1') ,
					Option("TextInput Node" , "2" , '2'),
					Option("NumberInput Node" , "3" , '3'),
					Option("Float  Calculus Node" , "4" , '4'),
					Option("String Calculus Node" , "5" , '5'),
					Option("Display Node" , "6" , '6'),
					Option("File Input Node" , "7" , '7'),
					Option("Output Node" , "8" , '8'),
					Option("End Node", "4", '5')
				}
			);
			if (!picked.has_value()) {
				goto restartDecision;
			}

			switch (picked->m_key)
			{
			case '0': addTitleNode(); break;
			case '1':addTextNode(); break;
			case '2':addTextInputNode(); break;
			case '3':addNumberInputNode(); break;
			case '4':addFloatCalculusNode(); break;
			case '5':addStringCalculusNode(); break;
			case '6':addDisplayNode(); break;
			case '7':addFileInputNode(); break;
			case '8':addOutputNode(); break;
			case '9':addEndNode();
				isRunning = false;
				; break;
			default:goto restartDecision;
				break;
			}



		restartDecision:
			auto picked = handler.pickOption("Wrong Input Provided!\nWhat Do you want to do next?", { Option("Restart Flow Creation" , "Enter" , ENTER_KEY) , Option("Exit" , "Esc" , ESC_KEY) });
			if (picked.has_value()) {
				if (picked->m_key == ENTER_KEY) {
					flow.reset();
					doWork(controller);
				}
				else if (picked->m_key == ESC_KEY) {
					onExit(controller);
				}
			}
		}
	}
private:
	void addTitleNode() {
		auto title = handler.readString("Enter title : ");
		auto description = handler.readString("Enter description : ");

		if (title.has_value() && description.has_value()) {
			auto titleNode = new TitleNode(++counter, { *title, *description });
			flow.addToFlow(titleNode);
		}
		else {
			auto picked = handler.pickOption("Wrong Input Provided!\nWhat Do you want to do next?", { Option("Restart Node Creation" , "Enter" , ENTER_KEY) , Option("Exit" , "Esc" , ESC_KEY) });
			if (picked.has_value()) {
				if (picked->m_key == ENTER_KEY) {
					flow.reset();
					addTitleNode();
				}
				else if (picked->m_key == ESC_KEY) {
					return;
				}
			}
			return;
		}

	}
	void addTextNode() {
		auto title = handler.readString("Enter title : ");
		auto description = handler.readString("Enter body : ");

		if (title.has_value() && description.has_value()) {
			auto textNode = new TextNode(++counter, { *title, *description });
			flow.addToFlow(textNode);
		}
		else {
			auto picked = handler.pickOption("Wrong Input Provided!\nWhat Do you want to do next?", { Option("Restart Node Creation" , "Enter" , ENTER_KEY) , Option("Exit" , "Esc" , ESC_KEY) });
			if (picked.has_value()) {
				if (picked->m_key == ENTER_KEY) {
					flow.reset();
					addTextNode();
				}
				else if (picked->m_key == ESC_KEY) {
					return;
				}
			}
			return;
		}
	}
	void addTextInputNode() {
		auto title = handler.readString("Enter Input Description : ");
		

		if (title.has_value() ) {
			auto titleNode = new TextInputNode(++counter , std::move(*title));
			flow.addToFlow(titleNode);
		}
		else {
			auto picked = handler.pickOption("Wrong Input Provided!\nWhat Do you want to do next?", { Option("Restart Node Creation" , "Enter" , ENTER_KEY) , Option("Exit" , "Esc" , ESC_KEY) });
			if (picked.has_value()) {
				if (picked->m_key == ENTER_KEY) {
					flow.reset();
					addTextInputNode();
				}
				else if (picked->m_key == ESC_KEY) {
					return;
				}
			}
			return;
		}
	}
	void addNumberInputNode() {
		auto title = handler.readString("Enter Input Description : ");


		if (title.has_value()) {
			auto numberNode = new NumberInputNode(++counter, std::move(*title));
			flow.addToFlow(numberNode);
		}
		else {
			auto picked = handler.pickOption("Wrong Input Provided!\nWhat Do you want to do next?", { Option("Restart Node Creation" , "Enter" , ENTER_KEY) , Option("Exit" , "Esc" , ESC_KEY) });
			if (picked.has_value()) {
				if (picked->m_key == ENTER_KEY) {
					flow.reset();
					addNumberInputNode();
				}
				else if (picked->m_key == ESC_KEY) {
					return;
				}
			}
			return;
		}
	}

	void addFloatCalculusNode() {

	}
	void addStringCalculusNode() {

	}
	void addDisplayNode() {

	}
	void addFileInputNode(){
		auto title = handler.readString("Enter Input Description : ");
		auto extension = handler.pickOption("Pick the desired format", { Option(".csv" , "1" , '1') , Option(".txt" , "2" , 1) });

		if (title.has_value()&&extension.has_value()) {

			auto fileInputNode = new FileInputNode(std::move(*title), std::move(extension->m_name), ++counter);
			flow.addToFlow(fileInputNode);
		}
		else {
			auto picked = handler.pickOption("Wrong Input Provided!\nWhat Do you want to do next?", { Option("Restart Node Creation" , "Enter" , ENTER_KEY) , Option("Exit" , "Esc" , ESC_KEY) });
			if (picked.has_value()) {
				if (picked->m_key == ENTER_KEY) {
					flow.reset();
					addFileInputNode();
				}
				else if (picked->m_key == ESC_KEY) {
					return;
				}
			}
			return;
		}
	}
	void addOutputNode() {

	}
	void addEndNode() {

	}
	NodeUid counter = 0;
	Flow flow;
	InputHandler handler;
};
class RunExistingFlowState : public FlowState {
public:
	void onEnter(FlowController& controller) override {

	}
	void onExit(FlowController& controller) {
		
	}
	void doWork(FlowController& controller) {

	}
};
class StartState : public FlowState {
public:
	StartState() :FlowState() {};
	void onEnter(FlowController& controller) override {


	}
	void onExit(FlowController& controller) override {

	}
	void doWork(FlowController& controller) override {
		std::cout << "Welcome to Flow Builder 1.0\n";
		auto picked = handler.pickOption("What do you want to do?", { Option("Run Existing Flow" , "1" , '1') , Option("Create New Flow" , "2" , '2') });
		if (picked.has_value()) {
			if (picked->m_key == '1') {
				auto createFlow = CreateNewFlowState();
				controller.setState(createFlow);
			}
			else if (picked->m_key == '1') {
				auto runExisting = RunExistingFlowState();
				controller.setState(runExisting);
			}
			else goto decision;
		}
		else {
		decision:
			auto picked = handler.pickOption("Wrong Input Provided!\nWhat Do you want to do next?", { Option("Restart" , "Enter" , ENTER_KEY) , Option("Exit" , "Esc" , ESC_KEY) });
			if (picked.has_value()) {
				if (picked->m_key == ENTER_KEY) {
					doWork(controller);
				}
				else if (picked->m_key == ESC_KEY) {
					return;
				}

			}
		}
	}
private:
	InputHandler handler;
};
