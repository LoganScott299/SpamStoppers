#ifndef CSV_TO_DATA_H
#define CSV_TO_DATA_H

#include <string>
#include <vector>
#include <list>

bool load_column_to_vector_of_lists(const std::string& csvPath, const std::string& colName, std::vector<std::list<std::string>>& out);

#endif