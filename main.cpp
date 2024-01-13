#include "Flow.h"
int main(){
    // Create nodes
    NumberInputNode numberInputNode1(1, "Enter first number:");
    NumberInputNode numberInputNode2(2, "Enter second number:");
    TextInputNode textInputNode(3, "Enter some text:");
    StringCalculusNode calculusNode(4, OperationType::Add, { 1, 2,3});
    auto display = OutputNode(5, ".txt", "MyFile", " Test file", "A simple test", { 1,2,3,4 });
    // Create the flow
    Flow flow;

    // Add nodes to the flow
    flow.addToFlow(&numberInputNode1);
    flow.addToFlow(&numberInputNode2);
    flow.addToFlow(&textInputNode);
    flow.addToFlow(&calculusNode);
    flow.addToFlow(&display);

    // Execute the flow
    flow.executeFlow();

    // Display the result from the CalculusNode
  //  std::cout << "Result of the calculation: " <<calculusNode.getContent() << std::endl;

    return 0;
}