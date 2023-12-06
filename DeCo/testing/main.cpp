#include <iostream>
#include "../Scanner.h"
#include "../Parser.h"

int main() {
    FILE* fp;
    fp = fopen("scanner-input.txt", "r");
    Scanner scanner(fp);

    while (scanner.hasNext()) {
        std::cout << scanner.next() << std::endl;
    }

    Parser::printFirstSets();
}