#include "error.h"
#include <iostream>

using std::cerr;
using std::endl;


void reportPendingException(JSContext* cx) {
    JS::ExceptionStack exnStack(cx);
    if (JS::StealPendingExceptionStack(cx, &exnStack)) {
        JS::ErrorReportBuilder report(cx);
        if (report.init(cx, exnStack, JS::ErrorReportBuilder::WithSideEffects)) {
            cerr << "Detail Error:\n" << report.toStringResult().c_str() << endl;    
        }
    }
}


