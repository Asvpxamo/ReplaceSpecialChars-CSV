#include <iostream>
#include <fstream>
#include <locale>
#include <string>
#include <codecvt> // For codecvt_utf8

using namespace std;

char DecodeUtf8TwoByte(char c1, char c2) {
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
            case 0xA0: return 'a'; // Replace 'à' with 'a'
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


char DecodeUtf8ThreeByte(char c1, char c2, char c3) {
    // Decode a three-byte UTF-8 character
    unsigned char u1 = static_cast<unsigned char>(c1);
    unsigned char u2 = static_cast<unsigned char>(c2);
    unsigned char u3 = static_cast<unsigned char>(c3);
    if ((u1 & 0xF0) != 0xE0 || (u2 & 0xC0) != 0x80 || (u3 & 0xC0) != 0x80) {
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
string ConvertToAsciiString(const string& text) {
    string result;
    for (size_t i = 0; i < text.size(); ++i) {
        unsigned char c = text[i];
        if (c <= 0x7F) 
        {
            result += c; // Keep ASCII characters as they are
        }
        else {
            // Handle UTF-8 characters correctly
            if ((c & 0xE0) == 0xC0)         
            { // Two-byte UTF-8 character
                char c2 = text[++i];
                result += DecodeUtf8TwoByte(c, c2);
            }
            else if ((c & 0xF0) == 0xE0)
            { // Three-byte UTF-8 character
                char c2 = text[++i];
                char c3 = text[++i];
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

string InputFilePaths()
{
    string inputPath = "", temp;
    std::cout << "Input the file Path (put in double quotes if the path include spaces): ";
    cin >> temp;
    for (int i = 0; i < temp.size(); i++)
    {
        if (temp[i] != '\"')
        {
            inputPath += temp[i];
        }
    }
    return inputPath;
}

string OutputFilePath(string inputFilePath)
{
    string outputPath = "";
    int flag = 0;
    std::cout << inputFilePath.size() << endl;
    for (int i = inputFilePath.size(); flag != 1; i--)
    {
        if (inputFilePath[i] == '\\')
        {
            for (int k = 0; k < i + 1; k++)
            {
                outputPath += inputFilePath[k];
            }
            flag = 1;
        }
    }
    return outputPath;
}

int main() {
    // Specify the path to the input CSV file
    string inputFilePath = InputFilePaths();
    std::cout << inputFilePath << endl;
    // Specify the path to the output CSV file
    string outputFilePath = OutputFilePath(inputFilePath);
    std::cout << outputFilePath << endl;

    try {
        // Open the input CSV file with specific encoding
        ifstream inputFile(inputFilePath);   // Use binary mode to prevent automatic encoding detection
        if (!inputFile.is_open()) {
            cerr << "Error: Unable to open input file." << endl;
            return 1;
        }

        // Set the locale to UTF-8 to handle UTF-8 encoded characters
        inputFile.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
        //inputFile.imbue(locale("en_US.UTF-8"));

        // Open the output CSV file
        ofstream outputFile(outputFilePath + "NewFile15.csv");
        if (!outputFile.is_open()) {
            throw runtime_error("Failed to open output file.");
        }

        // Process each line of the input CSV file
        string line;
        while (getline(inputFile, line)) {
            // Replace non-ASCII characters in the line
            string modifiedLine = ConvertToAsciiString(line);
            // Write the modified line to the output CSV file
            outputFile << modifiedLine << endl;
        }

        // Close the files
        inputFile.close();
        outputFile.close();

        cout << "CSV file processing completed. Output file created at: " << outputFilePath << endl;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    system("pause");

    return 0;
}
