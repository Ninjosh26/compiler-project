#include <iostream>
#include "../Scanner.h"
#include "../Parser.h"

int main() {
    FILE* fp;
    fp = fopen("test-files/parse-test.txt", "r");
    Scanner scanner(fp);

    // while (scanner.hasNext()) {
    //     std::cout << scanner.next() << std::endl;
    // }

    // Parser::printFirstSets();

    Parser parser(scanner);
    parser.parse();

    if (parser.hasError()) {
        parser.printErrorReport();
    }
}