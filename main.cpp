//main lol
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include "data-csv-io/CsvToData.h"


int main(int argc, char** argv) {
    std::vector<std::list<std::string>> comments;
    if (load_column_to_vector_of_lists("test.csv", "comment_body", comments)) {
        std::cout << "work";
    } else {
        std::cout << "no work";
    }
    return 0;
}