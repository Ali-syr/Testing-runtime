#include <iostream>
#include "core/engine.h"
#include "bindings/native.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using JS::RootedObject;

int main(int argc, char* argv[]) {
    // Validate required shell arguments
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename.js>" << endl;
        return 1;
    }

    string scriptPath = argv[1];
    string jsCode;
    // Map the external source text file stream into main memory
    if (!cpp_readFile(scriptPath, jsCode)) {
        cerr << "Error: Could not read file or file is empty: " << scriptPath << endl;
        return 1;
    }

    cout << "Starting SpiderMonkey initialization" << endl;
    // Instantiate the lifecycle host via RAII
    SpiderMonkeyScope engineScope;
    if (!engineScope.isValid()) {
        cerr << "Error: Failed to initialize SpiderMonkey runtime!" << endl;
        return 1;
    }

    // Configure designated initializers for standard C++20 standard compliance
    static const JSClass globalClass = {
        .name = "global",
        .flags = JSCLASS_GLOBAL_FLAGS,
        .cOps = &JS::DefaultGlobalClassOps
    };

    // Construct the execution sandbox layout using the core core abstractions
    RootedObject global(engineScope.getContext(), engineScope.createGlobal(&globalClass, global_functions));
    if (!global) {
        cerr << "Error: Failed to bootstrap Global Object environment Context!" << endl;
        return 1;
    }

    cout << "Executing " << scriptPath << "..." << endl;
    
    // Submit the script payload structure for evaluation
    if (!engineScope.evaluateScript(global, scriptPath, jsCode)) {
        cerr << "Runtime Error: JavaScript execution failed!" << endl;
        return 1;
    }

    cout << "JavaScript execution finished successfully!" << endl;
    return 0;
}
