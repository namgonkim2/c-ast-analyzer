#include <iostream>
#include <clang-c/Index.h>

using namespace std;

void diagnosticHandler(CXDiagnosticSet diagnostics)
{
    unsigned int numDiagnostics = clang_getNumDiagnosticsInSet(diagnostics);

    for (unsigned int i = 0; i < numDiagnostics; ++i)
    {
        CXDiagnostic diagnostic = clang_getDiagnosticInSet(diagnostics, i);
        CXString diagnosticStr = clang_formatDiagnostic(diagnostic, clang_defaultDiagnosticDisplayOptions());
        cout << clang_getCString(diagnosticStr) << endl;
        clang_disposeString(diagnosticStr);
    }
}

int main()
{
    const char *filename = "structs.cpp"; // Change to your C++ source file
    const char *args[] = {
        "-I/usr/include",
        "-I/usr/local/include"};

    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit tu = clang_parseTranslationUnit(index, filename, args, 2, nullptr, 0, CXTranslationUnit_None);

    if (!tu)
    {
        cout << "Failed to parse translation unit" << endl;
        return 1;
    }

    CXDiagnosticSet diagnostics = clang_getDiagnosticSetFromTU(tu);
    diagnosticHandler(diagnostics);

    clang_disposeDiagnosticSet(diagnostics);
    clang_disposeTranslationUnit(tu);
    clang_disposeIndex(index);
    return 0;
}