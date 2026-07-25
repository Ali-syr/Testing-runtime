#ifndef ENGINE_H
#define ENGINE_H

#include <string>
#include "jsapi.h"  

/* RAII wrapper for managing SpiderMonkey engine lifecycle. Ensures the engine initializes and shuts down automatically,
preventing memory leaks during unexpected execution failures.
*/
class SpiderMonkeyScope {
public:
    SpiderMonkeyScope();
    ~SpiderMonkeyScope();

    JSContext* getContext() const { return cx; }
    bool isValid() const { return initialized && cx != nullptr; }
    
    // Encapsulate global object creation inside core
    JSObject* createGlobal(const JSClass* globalClass, const JSFunctionSpec* functions);

    // Encapsulate script evaluation execution inside core
    bool evaluateScript(JS::RootedObject& global, const std::string& scriptPath, const std::string& jsCode);

private:
    JSContext* cx;
    bool initialized;
};

bool cpp_readFile(const std::string& filePath, std::string& output);

#endif
