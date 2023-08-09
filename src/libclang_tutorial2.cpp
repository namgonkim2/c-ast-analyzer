#include <iostream>
#include <string>
#include <clang-c/Index.h>

using namespace std;

void printFunctionReferenceInfo(CXCursor cursor)
{
    CXCursor referencedCursor = clang_getCursorReferenced(cursor);

    if (clang_getCursorKind(referencedCursor) == CXCursor_FunctionDecl)
    {
        CXString functionName = clang_getCursorSpelling(referencedCursor);
        CXSourceLocation location = clang_getCursorLocation(referencedCursor);
        CXFile file;
        unsigned int line, column, offset;
        clang_getFileLocation(location, &file, &line, &column, &offset);

        cout << ", Function Name: " << clang_getCString(functionName);
        cout << ", Declared in: " << clang_getCString(clang_getFileName(file)) << " (Line: " << line << ", Column: " << column << ")";

        clang_disposeString(functionName);
    }
}

void printNodeInfo(CXCursor cursor, int depth = 0)
{
    CXCursorKind nodeKind = clang_getCursorKind(cursor);
    CXString kindCXStr = clang_getCursorKindSpelling(nodeKind);
    CXString displayName = clang_getCursorDisplayName(cursor);
    CXSourceRange range = clang_getCursorExtent(cursor);

    for (int i = 0; i < depth; ++i)
    {
        cout << "  ";
    }
    string nodeKindStr = clang_getCString(kindCXStr);
    cout << "Node: " << nodeKindStr;

    string displayNameStr = clang_getCString(displayName);
    if (displayNameStr != "")
    {
        cout << ", Name: " << displayNameStr ;
    }
    
    if (nodeKind == CXCursor_DeclStmt)
    {
        CXString stmtStr = clang_getCursorSpelling(cursor);
        cout << ", Statement: " << clang_getCString(stmtStr);
        clang_disposeString(stmtStr);
    }
    else if (nodeKind == CXCursor_VarDecl)
    {
        CXType varType = clang_getCursorType(cursor);
        CXString typeName = clang_getTypeSpelling(varType);
        cout << ", Type: " << clang_getCString(typeName);
    }
    else if (nodeKind == CXCursor_IntegerLiteral || nodeKindStr.find("Operator") != string::npos)
    {
        CXSourceRange range = clang_getCursorExtent(cursor);
        CXToken *tokens = 0;
        unsigned int nTokens = 0;
        clang_tokenize(clang_Cursor_getTranslationUnit(cursor), range, &tokens, &nTokens);
        for (unsigned int i = 0; i < nTokens; i++)
        {
            CXString spelling = clang_getTokenSpelling(clang_Cursor_getTranslationUnit(cursor), tokens[i]);
            cout << ", Value: " << clang_getCString(spelling);
            clang_disposeString(spelling);
        }
        clang_disposeTokens(clang_Cursor_getTranslationUnit(cursor), tokens, nTokens);
    }
    else if (clang_getCursorKind(cursor) == CXCursor_DeclRefExpr)
    {
        printFunctionReferenceInfo(cursor);
    }

    // 각 노드 별 위치정보 저장
    CXSourceLocation startLocation = clang_getRangeStart(range);
    CXSourceLocation endLocation = clang_getRangeEnd(range);

    CXFile file;
    unsigned int startLine, startColumn, endLine, endColumn;
    clang_getSpellingLocation(startLocation, &file, &startLine, &startColumn, nullptr);
    clang_getSpellingLocation(endLocation, nullptr, &endLine, &endColumn, nullptr);

    cout << ", Location: " << clang_getCString(clang_getFileName(file)) << ":"
              << startLine << ":" << startColumn << " - " << clang_getCString(clang_getFileName(file))
              << ":" << endLine << ":" << endColumn << endl;

    clang_disposeString(kindCXStr);
    clang_disposeString(displayName);
}

void traverseAST(CXCursor cursor, int depth = 0)
{
    printNodeInfo(cursor, depth);

    clang_visitChildren(
        cursor, [](CXCursor c, CXCursor parent, CXClientData client_data)
        {
        traverseAST(c, *reinterpret_cast<int*>(client_data) + 1);
        return CXChildVisit_Continue;
        },
        &depth);
}

int main()
{
    const char *filename = "sample.c"; // Change to your source file
    const char *args[] = {
        "-I/usr/include",
        "-I/usr/local/include"};

    CXIndex index = clang_createIndex(1, 1);
    CXTranslationUnit tu = clang_parseTranslationUnit(index, filename, args, 2, nullptr, 0, CXTranslationUnit_None);

    if (!tu)
    {
        cout << "Failed to parse translation unit" << endl;
        return 1;
    }

    CXCursor cursor = clang_getTranslationUnitCursor(tu);
    traverseAST(cursor);

    clang_disposeTranslationUnit(tu);
    clang_disposeIndex(index);
    return 0;
}