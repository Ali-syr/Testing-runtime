#include <iostream>
#include "engine.h"
#include "../utils/error.h"
#include <fstream>
#include "js/Initialization.h"
#include "js/Context.h"
#include "js/CompilationAndEvaluation.h"
#include "js/SourceText.h"

using std::string;

SpiderMonkeyScope::SpiderMonkeyScope() : cx(nullptr), initialized(false) {
    // Initialize the internal structures of the SpiderMonkey library
    if (JS_Init()) {
        initialized = true;
        // Allocate a new JavaScript context with a fixed 32MB heap limit
        cx = JS_NewContext(32 * 1024 * 1024); 
    }
}

SpiderMonkeyScope::~SpiderMonkeyScope() {
    // Safely destroy the context frame and shut down the engine on destruction
    if (cx) JS_DestroyContext(cx);
    if (initialized) JS_ShutDown();
}

JSObject* SpiderMonkeyScope::createGlobal(const JSClass* globalClass, const JSFunctionSpec* functions) {
    // Bootstrap internal self-hosted JS library implementations
    if (!JS::InitSelfHostedCode(cx)) return nullptr;

    JS::RealmOptions options;
    
    // fixed Immediately wrap the raw JSObject* into a GC-managed JS::RootedObject
    JS::RootedObject globalObj(cx, JS_NewGlobalObject(cx, globalClass, nullptr, JS::DontFireOnNewGlobalHook, options));
    if (!globalObj) return nullptr;

    // Enter the execution Realm boundary of the global object
    JSAutoRealm arRealm(cx, globalObj);
    
    // fixed globalObj now cleanly converts to JS::Handle<JSObject*> for the 2nd argument
    if (functions && !JS_DefineFunctions(cx, globalObj, functions)) return nullptr;
    
    // fixed globalObj satisfies the Handle requirement for both target object and property value
    if (!JS_DefineProperty(cx, globalObj, "globalThis", globalObj, JSPROP_READONLY | JSPROP_PERMANENT)) return nullptr;

    // Return the underlying raw pointer back to the caller (cli.cpp)
    return globalObj.get();
}

bool SpiderMonkeyScope::evaluateScript(JS::RootedObject& global, const string& scriptPath, const string& jsCode) {
    // Ensure we enter the target global object's execution realm
    JSAutoRealm arRealm(cx, global);

    // Setup compilation metadata for diagnostic stack traces
    JS::CompileOptions compileOptions(cx);
    compileOptions.setFileAndLine(scriptPath.c_str(), 1);

    // Load the raw string data into the engine's Borrowed SourceText layout
    JS::SourceText<mozilla::Utf8Unit> source;
    if (!source.init(cx, jsCode.c_str(), jsCode.length(), JS::SourceOwnership::Borrowed)) return false;

    JS::RootedValue rval(cx);
    // Compile and run the encapsulated source code frame
    if (!JS::Evaluate(cx, compileOptions, source, &rval)) {
        // Delegate error processing to the dedicated utilities module
        reportPendingException(cx); 
        return false;
    }
    return true;
}

bool cpp_readFile(const std::string& filePath, string& output) {
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) return false;

    // Advance file pointer to calculate total allocation metrics
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    output.resize(size);

    // Stream out data chunks into the pre-allocated container buffer
    file.seekg(0, std::ios::beg);
    file.read(&output[0], size);
    return true;
}
