# c-ast-analyzer
parse C project to AST using libclang

### 전제조건
* gcc, gcc-c++
* clang
* clang-devel (clang 소스코드)
    - 맥의 경우 추가 조사 필요함

### 실행
* object 파일로 빌드
```bash
mkdir build 
clang++ -o ./build/main ./src/main.cpp -lclang
```
* 실행
```bash
./build/main ./test
```