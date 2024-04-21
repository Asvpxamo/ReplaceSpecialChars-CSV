#include <iostream>
#include <fstream>
#include <locale>
#include <string>
#include <codecvt> // For codecvt_utf8
#include <filesystem> // For std::filesystem
#include <algorithm> // For std::remove

using namespace std;
namespace fs = std::filesystem;

char DecodeUtf8TwoByte(char c1, char c2) 
{
    // Decode a two-byte UTF-8 character
    unsigned char u1 = static_cast<unsigned char>(c1);
    unsigned char u2 = static_cast<unsigned char>(c2);
    if ((u1 & 0xE0) != 0xC0 || (u2 & 0xC0) != 0x80) 
    {
        // Invalid UTF-8 sequence, return placeholder character
        return '?';
    }
    // Replace specific UTF-8 characters with their ASCII equivalents
    if (u1 == 0xC3) 
    {
        switch (u2) 
        {         
            case 0xA7: return 'c'; // Replace 'ç' with 'c'
            case 0xA8: return 'e'; // Replace 'è' with 'e'
            case 0xA9: return 'e'; // Replace 'é' with 'e'
            case 0xBB: return 'e'; // Replace 'ë' with 'e'
            case 0xB4: return 'o'; // Replace 'ô' with 'o'
            case 0xB6: return 'o'; // Replace 'ö' with 'o'
            
        }
    }
    return '?'; // Placeholder character if no replacement is found
}


char DecodeUtf8ThreeByte(char c1, char c2, char c3) 
{
    // Decode a three-byte UTF-8 character
    unsigned char u1 = static_cast<unsigned char>(c1);
    unsigned char u2 = static_cast<unsigned char>(c2);
    unsigned char u3 = static_cast<unsigned char>(c3);
    if ((u1 & 0xF0) != 0xE0 || (u2 & 0xC0) != 0x80 || (u3 & 0xC0) != 0x80) 
    {
        // Invalid UTF-8 sequence, return placeholder character
        return '?';
    }
    // Replace specific UTF-8 characters with their ASCII equivalents
    
    return '?'; // Placeholder character if no replacement is found
}

/**
 * @brief Converts a string containing potentially non-ASCII characters into a string of ASCII characters.
 *
 * This function iterates over each character in the input string. If a character falls within the ASCII range, 
 * it is added to the result string as is. If a character is a two-byte or three-byte UTF-8 character, 
 * it is decoded and converted to an ASCII equivalent if possible. If a character is not an ASCII character 
 * and not a recognized UTF-8 character, it is appended to the result string as is.
 *
 * @param text The input string to be converted.
 * @return A string where all non-ASCII characters in the input string have been replaced with their ASCII equivalents, 
 * or with a placeholder character if no equivalent exists.
 */
string ConvertToAsciiString(const string text) 
{
    string result;
    char c2, c3; // Declare c2 and c3 outside the loop
    for (size_t i = 0; i < text.size(); ++i) 
    {
        unsigned char c = text[i];
        if (c <= 0x7F) 
        {
            result += c; // Keep ASCII characters as they are
        }
        else 
        {
            // Handle UTF-8 characters correctly
            if ((c & 0xE0) == 0xC0) 
            { // Two-byte UTF-8 character
                c2 = text[++i]; // Reuse c2 without redeclaration and reinitialization
                result += DecodeUtf8TwoByte(c, c2);
            }
            else if ((c & 0xF0) == 0xE0) 
            { // Three-byte UTF-8 character
                c2 = text[++i];
                c3 = text[++i];
                result += DecodeUtf8ThreeByte(c, c2, c3);
            }
            else 
            {
                // If the character is not replaced, append it as it is
                result += c;
            }
        }
    }
    return result;
}

string InputFilePath()
{
    string inputPath;
    cout << "Input the file Path (put in double quotes if the path include spaces): ";
    getline(cin, inputPath); // Use std::getline to read the entire line including spaces
    inputPath.erase(remove(inputPath.begin(), inputPath.end(), '\"'), inputPath.end()); // Remove double quotes from string
    ifstream file(inputPath);
    while (!file.good())
    {
        cerr << "Error: Invalid file path. Please try again." << endl;
        cout << "Input the file Path (put in double quotes if the path include spaces): ";
        getline(cin, inputPath);

        inputPath.erase(remove(inputPath.begin(), inputPath.end(), '\"'), inputPath.end());
        file.open(inputPath);
    }
    file.close();
    system("cls");  // clear console
    return inputPath;
}
/**
 * @brief Extracts the directory path from a full file path.
 *
 * This function extracts the directory path from a full file path by finding the last occurrence of a slash character
 * ('/' or '\') and returning the substring up to that point.
 * *
 * * @param inputFilePath The full file path from which to extract the directory path.
 * * @return The directory path extracted from the input file path.
 * */
string OutputFilePath(const string& inputFilePath) 
{
    size_t lastSlashIndex = inputFilePath.find_last_of("\\/");
    if (lastSlashIndex == string::npos) 
    {
        return "";
    }
    return inputFilePath.substr(0, lastSlashIndex + 1);
}


string RemoveBOM(const string& content)
{
    // Check if the content starts with the byte order mark (BOM)
    if (content.size() >= 3 && content[0] == '\xEF' && content[1] == '\xBB' && content[2] == '\xBF')
    {
        // Return the content without the BOM
        return content.substr(3);
    }
    
    // Return the content as is if it doesn't start with the BOM
    return content;
}

void ProcessCSVFile(ofstream &outputFile, string cleanedContent)
{
    string line, modifiedLine;
    for (char c : cleanedContent)
    {
        if (c == '\n')
        {
            // Replace non-ASCII characters in the line
            modifiedLine = ConvertToAsciiString(line);
            // Write the modified line to the output CSV file
            outputFile << modifiedLine << endl;
            line.clear();
        }
        else
        {
            line += c;
        }
    }
}

int main() 
{
    // Specify the path to the input CSV file
    string inputFilePath = InputFilePath();
    
    // Specify the path to the output CSV file
    string outputDirectory = OutputFilePath(inputFilePath);
    
    string fileName = "corrected5_" + inputFilePath.substr(inputFilePath.find_last_of("\\/") + 1);
    ifstream inputFile(inputFilePath);   // Use binary mode to prevent automatic encoding detection
    ofstream outputFile(outputDirectory + fileName);

    try {
        // Open the input CSV file with specific encoding
        if (!inputFile.is_open()) 
        {
            throw runtime_error("Failed to open input file: " + inputFilePath);
        }

        // Read the content of the input file
        string content((istreambuf_iterator<char>(inputFile)), (istreambuf_iterator<char>()));
        
        // Remove the BOM from the content
        string cleanedContent = RemoveBOM(content);

        // Set the locale to UTF-8 to handle UTF-8 encoded characters
        inputFile.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
        //inputFile.imbue(locale("en_US.UTF-8"));

        
        if (fs::exists(outputDirectory + fileName)) 
        {
            char response;
            do
            {
                cout << "Output file " << outputDirectory + fileName << " already exists. Do you want to overwrite it ? (y / n)" << endl;
                cin >> response;

            } while (tolower(response) != 'y' && tolower(response) != 'n');

            if (tolower(response) == 'n')
            {
				cout << "Enter a new file name: ";
				getline(cin, fileName);
			}
            else
            {
				fs::remove(outputDirectory + fileName); // Remove the existing file
            }

        }

        // Open the output CSV file
        if (!outputFile.is_open()) 
        {
            throw runtime_error("Failed to create output file at " + outputDirectory + fileName);
        }

        // Process each line of the input CSV
        ProcessCSVFile(outputFile, cleanedContent);

        // Close the files
        inputFile.close();
        outputFile.close();

        cout << "CSV file processing completed. Output file created at: " << outputDirectory + fileName  << endl;
    }
    catch (const exception& e) 
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    system("pause");

    return 0;
}
