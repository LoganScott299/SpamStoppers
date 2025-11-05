#include <string>
#include <vector>
#include <list>
#include <fstream>


// Nicholas Reyes 11/3/25
// Parse previously created csv file and places it into vector of lists

// uses the RFC-4180 standard for csv parsing

//usage: 
//  std::vector<std::list<std::string>> comments;
//  if (load_column_to_vector_of_lists("joined.csv", "comment_body", comments)) {

//       functionality since succeeded

//      }

//remove \r char
static void removeReturn(std::string& str) {
    if (!str.empty() && str.back() == '\r') {
        str.pop_back();
    }
}

//read a single record from the csv, handles any quoted commas/newlines to prevent errors
static bool read_csv_line(std::istream& iFile, std::string& recordO) {
    recordO.clear();
    std::string line;
    bool inQuotes = false;
    bool foundData = false;

//size_t is used here bcs overflow for unsigned still included with <string>
    auto scanQuotes = [&](const std::string& str) {
        for (size_t i = 0; i < str.size(); ++i) {
            if (str[i] == '"') {
                if (inQuotes && i + 1 < str.size() && str [i+1] == '"') {
                    ++i; //pass an escaped one
                } else {
                    inQuotes = !inQuotes;
                }
            }
        }
    };
    while (std::getline(iFile, line)) {
        removeReturn(line);
        if (foundData) {
            recordO.push_back('\n'); //keep newlines
        }
        recordO += line;
        scanQuotes(line);
        foundData = true;
        if (!inQuotes) {
            return true; //one rec done
        }
    }
    return foundData;
}

//split record into cols
static std::vector<std::string> parseCSV(const std::string& rec) {
    std::vector<std::string> out;
    std::string curr;
    bool inQuotes = false;

    for (size_t i = 0; i < rec.size(); ++i) {
        char c = rec[i];
        if (inQuotes) {
            if (c == '"') {
                if (i+1 < rec.size() && rec[i+1] == '"') {
                    curr.push_back('"');
                    ++i;
                } else {
                    inQuotes = false;
                }
            } else {
            curr.push_back(c);
        }
    } else {
        if (c ==  ',') {
            out.emplace_back(std::move(curr)); // adds field to vector
            curr.clear();
        } else if (c == '"') {
            inQuotes = true;
        } else {
            curr.push_back(c);
        }
    }
    }
    out.emplace_back(std::move(curr)); //last field
    return out;
}
//whole return is vector<list<string>>
//each row is one list<string> per csv row for that column
bool load_column_to_vector_of_lists(const std::string& csvPath, const std::string& colName, std::vector<std::list<std::string>>& out) {
    
    out.clear();

    std::ifstream in(csvPath); // load file path
    if (!in.is_open()) {
        return false; //failed to open
    }
    std::string rec;
    if (!read_csv_line(in,rec)) {
        return false; // empty / fail read
    }

    //find target column
    std::vector<std::string> header = parseCSV(rec); // header row with col info
    size_t colIndex = static_cast<size_t>(-1); // max size in case
    for (size_t i = 0; i < header.size(); ++i) {
        if (header[i] == colName) {
            colIndex = i;
            break;
        }
    }
    if (colIndex == static_cast<size_t>(-1)) {
        return false; // col not found
    }
    //now read remaining rows
    while (read_csv_line(in, rec)) {
        std::vector<std::string> cols = parseCSV(rec);
        if (cols.size() <= colIndex) {
            cols.resize(colIndex + 1); // fix missing cols
        }
        std::list<std::string> cellList;
        cellList.push_back(std::move(cols[colIndex])); //move to list
        out.emplace_back(std::move(cellList));
    }
    return true;
}
