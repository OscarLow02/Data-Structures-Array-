#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <cmath>
#include <unordered_set>

using namespace std;

// Structure to store a single news article
struct News {
    string title;
    string content;
    string category;
    string publicationDate;

    News() {} // Default constructor
    News(string t, string c, string cat, string pubDate) 
        : title(t), content(c), category(cat), publicationDate(pubDate) {}
};

// Structure to store word frequency
struct WordFreq {
    string word;
    int count;
};

// Dynamic array class for storing multiple News articles
struct NewsArray {
    News* article;  // Pointer to dynamically allocated array
    int size;        // Number of stored articles
    int capacity;    // Total allocated capacity

    // Constructor
    NewsArray() {
        size = 0;
        capacity = 10;
        article = new News[capacity];
    }

    // Function to resize the dynamic array
    void resize() {
        capacity *= 2;  
        News* newList = new News[capacity]; // Allocate new larger array

        // Copy existing elements to the new array
        for (int i = 0; i < size; i++) {
            newList[i] = article[i];
        }

        delete[] article;  // Free old memory
        article = newList;
    }

    // Destructor to free allocated memory
    ~NewsArray() {
        delete[] article;
        article = nullptr;
        size = 0;
        capacity = 0;
    }

    // Function to add a new article
    void addNews(const string& title, const string& text, const string& subject, const string& date) {
        if (size == capacity) {
            resize();  // Expand the array if full
        }

        article[size++] = News(title, text, subject, date);
    }

    // Function to get number of stored articles
    int getSize() {
        return size;
    }

    // Overload subscript operator to access articles (non-const version)
    News& operator[](int index) {
        if (index < 0 || index >= size) {
            throw out_of_range("Index out of bounds!");
        }
        return article[index];
    }

    // Overload subscript operator for const objects (const version)
    const News& operator[](int index) const {
        if (index < 0 || index >= size) {
            throw out_of_range("Index out of bounds!");
        }
        return article[index];
    }

    // print sorted articles to a file
    void printResult(const string& filename) const {
        // Open file for writing
        ofstream outFile(filename);
        if (!outFile) {
            cout << "Error opening file: " << filename << endl;
            return;
        }   
        
        outFile << "Sorted news by date: " << endl;
        outFile << "\n"; 

        for (int i = 0; i < size; i++) {
            outFile << "Title: " << article[i].title << "\n";
            outFile << "Text: " << article[i].content << "\n";
            outFile << "Subject: " << article[i].category << "\n";
            outFile << "Date: " << article[i].publicationDate << "\n";
            outFile << "-----------------------------------------------------------------\n";
        }

        outFile.close();
        cout << "Sorted articles saved to " << filename << endl;
    }

    int convertDate(const string& date) {
        stringstream ss(date);
        string month, day, year;
        ss >> month >> day >> year;
    
        // Remove commas from day
        if (!day.empty() && day.back() == ',') {
            day.pop_back();
        }
    
        // Month lookup array (supports full & abbreviated names)
        string months[24] = {
            "Jan", "January", "Feb", "February", "Mar", "March", "Apr", "April", "May", "May",
            "Jun", "June", "Jul", "July", "Aug", "August", "Sep", "September", "Oct", "October",
            "Nov", "November", "Dec", "December"
        };
    
        int monthNum = 0;
        
        // Iterate over months array to find a match
        for (int i = 0; i < 24; i++) {
            if (months[i] == month) {
                monthNum = (i / 2) + 1;  // Every full & short name pair shares the same index
                break;
            }
        }
    
        // If month is invalid, return an error
        if (monthNum == 0) {
            cerr << "Error: Unknown month format -> " << month << endl;
            return 0; // Indicate failure
        }
    
        // Convert to integer format YYYYMMDD
        return stoi(year) * 10000 + monthNum * 100 + stoi(day);
    }
    
    void merge(int left, int mid, int right, const string& searchType) {
        int number_of_Lelement = mid - left + 1;
        int number_of_Relement = right - mid;
    
        News* leftArray = new News[number_of_Lelement];
        News* rightArray = new News[number_of_Relement];
    
        for (int i = 0; i < number_of_Lelement; i++) {
            leftArray[i] = article[left + i];
        }
        for (int j = 0; j < number_of_Relement; j++) {
            rightArray[j] = article[mid + 1 + j];
        }
    
        int i = 0, j = 0, m = left;
        while (i < number_of_Lelement && j < number_of_Relement) {
            bool condition;
            if (searchType == "year") {
                condition = convertDate(leftArray[i].publicationDate) <= convertDate(rightArray[j].publicationDate);
            } else { // Sort by category
                condition = leftArray[i].category <= rightArray[j].category;
            }
    
            if (condition) {
                article[m++] = leftArray[i++];
            } else {
                article[m++] = rightArray[j++];
            }
        }
    
        while (i < number_of_Lelement) {
            article[m++] = leftArray[i++];
        }
    
        while (j < number_of_Relement) {
            article[m++] = rightArray[j++];
        }
    
        delete[] leftArray;
        delete[] rightArray;
    }

    void mergeSort(int left, int right, const string& searchType) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSort(left, mid, searchType);
            mergeSort(mid + 1, right, searchType);
            merge(left, mid, right, searchType);
        }
    }

    // Public function to sort articles by date
    void sortByDate() {
        mergeSort(0, size - 1, "year");
    }

    void sortByCategory() {
        mergeSort(0, size - 1, "category");
    }

    // Generic Binary Search Function
    // SearchType can be "year" or "category"
    int binarySearch(NewsArray& newsArray, const string& target, int left, int right, const string& searchType) {
        int result = -1; // Store the first found index
        
        while (left <= right) {
            int mid = left + (right - left) / 2;
    
            if (searchType == "year") {
                cout << "\n Debug: Searching by year..." << endl; // debug
                // Convert only if searching by year
                int targetYear;
                try {
                    cout << "Debug: Converting " << target << " to integer..." << endl; // debug
                    targetYear = stoi(target); // Convert target year to int
                    cout << "Debug: Conversion successful! targetYear = " << targetYear << endl; // debug
                } catch (const invalid_argument& e) {
                    cerr << "Error: Invalid year input format!" << endl;
                    return -1;
                }
    
                int key = convertDate(newsArray[mid].publicationDate) / 10000; // Extract year from date
    
                if (key == targetYear) {
                    result = mid;
                    right = mid - 1; // Continue searching left for first occurrence
                } else if (key < targetYear) {
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            } 
            else if (searchType == "category") {
                cout << "\n Debug: Searching by category..." << endl; // debug
                string key = newsArray[mid].category; // Extract category

                cout << "Debug: Comparing '" << key << "' with '" << target << "'" << endl;
    
                if (key == target) {
                    result = mid;
                    right = mid - 1; // Continue searching left for first occurrence
                } else if (key < target) {
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            } 
            else if (searchType == "title") {
                cout << "\n Debug: Searching by title..." << endl; // Debug
                string key = newsArray[mid].title; // Extract title
            
                cout << "Debug: Comparing '" << key << "' with '" << target << "'" << endl;

                if (key.find(target) != string::npos) { // Check substring match
                    result = mid;
                    right = mid - 1; // Continue searching left for first occurrence
                } else if (key < target) {
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }
            else if (searchType == "content") {
                cout << "\n Debug: Searching by content..." << endl; // Debug
                string key = newsArray[mid].content; // Extract content

                cout << "Debug: Comparing '" << key << "' with '" << target << "'" << endl;
            
                if (key.find(target) != string::npos) { // Check substring match
                    result = mid;
                    right = mid - 1; // Continue searching left for first occurrence
                } else if (key < target) {
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }
            else {
                cerr << "Error: Invalid search type!" << endl;
                return -1; // Invalid search type
            }
        }
        return result; // Return first occurrence or -1 if not found
    }

    void searchArticlesByYear(NewsArray& newsArray, const string& targetYearStr) {
        // Open file for writing
        ofstream outFile("search_by_year.txt");
        if (!outFile) {
            cerr << "Error: Could not open file for writing!" << endl;
            return;
        }

        // Ensure articles are sorted by date before searching
        int targetYear = stoi(targetYearStr); // Convert input to integer
        int index = binarySearch(newsArray, targetYearStr, 0, newsArray.getSize() - 1, "year");

        if (index == -1) {
            cout << "No articles found for the year " << targetYear << endl;
            outFile << "No articles found for the year " << targetYear << endl;
            outFile.close();
            return;
        }

        cout << "Debug: Writing search results to search_by_year.txt" << endl;
        outFile << "Article(s) published in " << targetYear << ":" << endl;
        outFile << "---------------------------------------------" << endl;

        // Display all matching articles (move left and right from index)
        int i = index;
        while (i >= 0 && convertDate(newsArray[i].publicationDate) / 10000 == targetYear) {
            outFile << "Article " << i + 1 << " | Date: " << newsArray[i].publicationDate
                        << " | Category: " << newsArray[i].category << endl;
                        outFile << " | Title: " << newsArray[i].title << " |" << endl;
                        outFile << " | Content: " << newsArray[i].content << " |" << endl;
                        outFile << endl;
            i--;
        }
        
        // Expand right
        i = index + 1;
        while (i < newsArray.getSize() && convertDate(newsArray[i].publicationDate) / 10000 == targetYear) {
            outFile << "Article " << i + 1 << " | Date: " << newsArray[i].publicationDate
                        << " | Category: " << newsArray[i].category << endl;
                        outFile << " | Title: " << newsArray[i].title << " |" << endl;
                        outFile << " | Content: " << newsArray[i].content << " |" << endl;
                        outFile << endl;
            i++;
        }
        outFile.close(); // Close the file
        cout << "Debug: Results successfully written to search_by_year.txt" << endl;
    }

    void searchArticlesByCategory(NewsArray& newsArray, const string& targetCategory) {
        // Open file for writing
        ofstream outFile("search_by_category.txt");
        if (!outFile) {
            cerr << "Error: Could not open file for writing!" << endl;
            return;
        }

        newsArray.sortByCategory(); // Ensure articles are sorted by category
        int index = binarySearch(newsArray, targetCategory, 0, newsArray.getSize() - 1, "category");
    
        if (index == -1) {
            cout << "No articles found in category: " << targetCategory << endl;
            outFile << "No articles found in category: " << targetCategory << endl;
            outFile.close();
            return;
        }
    
        cout << "Debug: Writing search results to search_by_category.txt" << endl;
        outFile << "Article(s) category: " << targetCategory << endl;
        outFile << "---------------------------------------------" << endl;
    
        // Expand left to find all matching articles
        int i = index;
        while (i >= 0 && newsArray[i].category == targetCategory) {
            outFile << "Article " << i + 1 << " | Date: " << newsArray[i].publicationDate
                        << " | Category: " << newsArray[i].category << endl;
                        outFile << " | Title: " << newsArray[i].title << " |" << endl;
                        outFile << " | Content: " << newsArray[i].content << " |" << endl;
                        outFile << endl;
            i--;
        }
    
        // Expand right to find all matching articles
        i = index + 1;
        while (i < newsArray.getSize() && newsArray[i].category == targetCategory) {
            outFile << "Article " << i + 1 << " | Date: " << newsArray[i].publicationDate
                        << " | Category: " << newsArray[i].category << endl;
                        outFile << " | Title: " << newsArray[i].title << " |" << endl;
                        outFile << " | Content: " << newsArray[i].content << " |" << endl;
                        outFile << endl;
            i++;
        }

        outFile.close(); // Close the file
        cout << "Debug: Results successfully written to search_by_category.txt" << endl;
    }

    void searchArticlesByTitle(NewsArray& newsArray, const string& searchQuery) {
        // Open file for writing
        ofstream outFile("search_by_title.txt");
        if (!outFile) {
            cerr << "Error: Could not open file for writing!" << endl;
            return;
        }
    
        outFile << "Article(s) title with \"" << searchQuery << "\":" << endl;
        outFile << "---------------------------------------------" << endl;
    
        // Perform binary search to find the first occurrence
        int index = binarySearch(newsArray, searchQuery, 0, newsArray.getSize() - 1, "title");
        if (index == -1) {
            outFile << "No articles found with \"" << searchQuery << "\" in the title." << endl;
            outFile.close();
            cout << "No articles found with \"" << searchQuery << "\" in the title." << endl;
            return;
        }

        bool found = false;
        
        // Expand left to find all matching articles
        int i = index;
        while (i >= 0 && newsArray[i].title.find(searchQuery) != string::npos) {
            found = true;
            outFile << "Article " << i + 1 << " | Date: " << newsArray[i].publicationDate
                    << " | Category: " << newsArray[i].category << endl;
            outFile << " | Title: " << newsArray[i].title << " |" << endl;
            outFile << " | Content: " << newsArray[i].content << " |" << endl;
            outFile << endl;
            i--;
        }

        // Expand right to find all matching articles
        i = index + 1;
        while (i < newsArray.getSize() && newsArray[i].title.find(searchQuery) != string::npos) {
            found = true;
            outFile << "Article " << i + 1 << " | Date: " << newsArray[i].publicationDate
                    << " | Category: " << newsArray[i].category << endl;
            outFile << " | Title: " << newsArray[i].title << " |" << endl;
            outFile << " | Content: " << newsArray[i].content << " |" << endl;
            outFile << endl;
            i++;
        }
    
        outFile.close();
        cout << "Results successfully written to search_by_title.txt" << endl;
    }

    void searchArticlesByContent(NewsArray& newsArray, const string& searchQuery) {
        // Open file for writing
        ofstream outFile("search_by_content.txt");
        if (!outFile) {
            cerr << "Error: Could not open file for writing!" << endl;
            return;
        }
    
        outFile << "Article(s) content with \"" << searchQuery << "\":" << endl;
        outFile << "------------------------------------------------" << endl;

        // Perform binary search to find the first occurrence
        int index = binarySearch(newsArray, searchQuery, 0, newsArray.getSize() - 1, "content");

        if (index == -1) {
            outFile << "No articles found with \"" << searchQuery << "\" in the content." << endl;
            outFile.close();
            cout << "No articles found with \"" << searchQuery << "\" in the content." << endl;
            return;
        }
    
        bool found = false;
        
        // Expand left to find all matching articles
        int i = index;
        while (i >= 0 && newsArray[i].content.find(searchQuery) != string::npos) {
            found = true;
            outFile << "Article " << i + 1 << " | Date: " << newsArray[i].publicationDate
                    << " | Category: " << newsArray[i].category << endl;
            outFile << " | Title: " << newsArray[i].title << " |" << endl;
            outFile << " | Content: " << newsArray[i].content << " |" << endl;
            outFile << endl;
            i--;
        }

        // Expand right to find all matching articles
        i = index + 1;
        while (i < newsArray.getSize() && newsArray[i].content.find(searchQuery) != string::npos) {
            found = true;
            outFile << "Article " << i + 1 << " | Date: " << newsArray[i].publicationDate
                    << " | Category: " << newsArray[i].category << endl;
            outFile << " | Title: " << newsArray[i].title << " |" << endl;
            outFile << " | Content: " << newsArray[i].content << " |" << endl;
            outFile << endl;
            i++;
        }
    
        outFile.close();
        cout << "Results successfully written to search_by_content.txt" << endl;
    }
};

// these words will be ignored
unordered_set<string> stopwords = {"a", "about", "above", "after", "again", "against", "all", "am", "an", "and", "any", "are", "aren't", 
    "as", "at", "be", "because", "been", "before", "being", "below", "between", "both", "but", "by", "can", 
    "can't", "cannot", "could", "couldn't", "did", "didn't", "do", "does", "doesn't", "doing", "don't", "down", 
    "during", "each", "few", "for", "from", "further", "had", "hadn't", "has", "hasn't", "have", "haven't", 
    "having", "he", "he'd", "he'll", "he's", "her", "here", "here's", "hers", "herself", "him", "himself", 
    "his", "how", "how's", "i", "i'd", "i'll", "i'm", "i've", "if", "in", "into", "is", "isn't", "it", "it's", 
    "its", "itself", "let's", "me", "more", "most", "mustn't", "my", "myself", "no", "nor", "not", "of", "off", 
    "on", "once", "only", "or", "other", "ought", "our", "ours", "ourselves", "out", "over", "own", "same", 
    "shan't", "she", "she'd", "she'll", "she's", "should", "shouldn't", "so", "some", "such", "than", "that", 
    "that's", "the", "their", "theirs", "them", "themselves", "then", "there", "there's", "these", "they", 
    "they'd", "they'll", "they're", "they've", "this", "those", "through", "to", "too", "under", "until", "up", 
    "very", "was", "wasn't", "we", "we'd", "we'll", "we're", "we've", "were", "weren't", "what", "what's", 
    "when", "when's", "where", "where's", "which", "while", "who", "who's", "whom", "why", "why's", "with", 
    "won't", "would", "wouldn't", "you", "you'd", "you'll", "you're", "you've", "your", "yours", "yourself", 
    "yourselves", "s", "t"};

    // Dynamic array for word frequencies
struct WordFreqArray {
    WordFreq* words;
    int size;
    int capacity;

    WordFreqArray() {
        size = 0;
        capacity = 50;
        words = new WordFreq[capacity];
    }

    void resize() {
        capacity *= 2;
        WordFreq* newList = new WordFreq[capacity];
        for (int i = 0; i < size; i++) 
        {
            newList[i] = words[i];
        }
        delete[] words;
        words = newList;
    }

    ~WordFreqArray() {
        delete[] words;
        words = nullptr;
    }

    void addWord(const string& word) {
        if (stopwords.find(word) != stopwords.end()) 
        {
            return;
        }
        
        for (int i = 0; i < size; i++)
        {
            if (words[i].word == word) {
                words[i].count++;
                return;
            }
        }
        if (size == capacity) 
        {
            resize();
        }
        words[size++] = {word, 1};
    }

    void merge(int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;
        WordFreq* leftArray = new WordFreq[n1];
        WordFreq* rightArray = new WordFreq[n2];

        for (int i = 0; i < n1; i++) leftArray[i] = words[left + i];
        for (int j = 0; j < n2; j++) rightArray[j] = words[mid + 1 + j];

        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (leftArray[i].count >= rightArray[j].count) {
                words[k++] = leftArray[i++];
            } else {
                words[k++] = rightArray[j++];
            }
        }
        while (i < n1) words[k++] = leftArray[i++];
        while (j < n2) words[k++] = rightArray[j++];

        delete[] leftArray;
        delete[] rightArray;
    }

    void mergeSort(int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSort(left, mid);
            mergeSort(mid + 1, right);
            merge(left, mid, right);
        }
    }

    void sortByFrequency() {
        mergeSort(0, size - 1);
    }

    void printFreqWords() {
        ofstream outFile("most_frequent_words.txt");
        if (!outFile) {
            cout << "Error: Could not open most_frequent_words.txt for writing!" << endl;
            return;
        }

        outFile << "Most frequent words in the fake news articles: " << endl;
        outFile << "\n";

        for (int i = 0; i < size; i++) {
            outFile << words[i].word << " - " << words[i].count << " times" << endl;
        }

        outFile.close();
        cout << "Most frequent words successfully saved to most_frequent_words.txt" << endl;
    }
};

void processGovernmentNews(NewsArray& newsArray, WordFreqArray& wordFreqArray) {
    for (int i = 0; i < newsArray.getSize(); i++) 
    {
        if (newsArray.article[i].category == "Government News") 
        {
            stringstream ss(newsArray.article[i].content);
            string word;
            while (ss >> word) 
            {
                string cleanWord;
                for (char c : word) 
                {
                    if (isalnum(c))
                    {
                        cleanWord += tolower(c);
                    }
                }
                if (!cleanWord.empty()) 
                {
                    wordFreqArray.addWord(cleanWord);
                }
            }
        }
    }
}

// Function to read CSV file and store data in NewsArray
void readCsv(const string& filename, NewsArray& newsArray) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // Skip header line

    while (getline(file, line)) {
        string fields[4];
        string temp;
        bool inQuotes = false;
        int fieldIndex = 0;

        for (size_t i = 0; i < line.length(); i++) 
        {
            char c = line[i];

            if (c == '"') {  
                if (inQuotes && i + 1 < line.length() && line[i + 1] == '"') 
                {  
                    // Handle escaped double quotes ("")
                    temp += '"';  
                    i++; 
                } else {
                    inQuotes = !inQuotes; // Toggle inside/outside quotes
                }
            } else if (c == ',' && !inQuotes) { 
                // Store the field when encountering a comma (if not inside quotes)
                if (fieldIndex < 4) {
                    fields[fieldIndex++] = temp;
                }
                temp.clear();
            } else {
                temp += c;  // Append character to field
            }
        }

        // Add the last field
        if (fieldIndex < 4) 
        {
            fields[fieldIndex++] = temp;
        }

        // Ensure that have exactly 4 fields before adding to the dynamic array
        if (fieldIndex == 4) 
        {
            newsArray.addNews(fields[0], fields[1], fields[2], fields[3]);
        } else {
            cout << "Invalid line: " << line << endl;
        }
    }

    file.close();
}

void analyzeFakePoliticalNews(NewsArray& fakeNewsArray, NewsArray& trueNewsArray) {
    // Months mapping (0 = January, ..., 11 = December)
    int totalNewsPerMonth[12] = {0};  // Count total news (true + fake) per month
    int fakePoliticalCount[12] = {0}; // Count fake political articles per month

    ofstream debugFile("debug.txt"); // Open debug file for writing
    if (!debugFile) {
        cerr << "Error: Could not open debug.txt for writing!" << endl;
        return;
    }

    debugFile << "Debug Log - Fake Political News Analysis\n";
    debugFile << "-----------------------------------------\n";

    // Process all fake news articles
    for (int i = 0; i < fakeNewsArray.getSize(); i++) {
        News& article = fakeNewsArray[i];
        int formattedDate = fakeNewsArray.convertDate(article.publicationDate);
        int year = formattedDate / 10000;  
        int month = (formattedDate / 100) % 100 - 1; 

        // Debug
        debugFile << "Article " << i + 1 << " | Date: " << article.publicationDate
                  << " | Converted: " << formattedDate
                  << " | Year: " << year
                  << " | Month: " << month + 1
                  << " | Category: " << article.category << "\n";

        if (year == 2016 && month >= 0 && month < 12) {
            totalNewsPerMonth[month]++;  // Count total fake news for the month

            if (article.category == "politics") {
                fakePoliticalCount[month]++; // Count fake political news
                debugFile << "  -> Fake political news detected in " << month + 1 << "\n"; // Debug
            }
        }
    }

    // Process all true news articles
    for (int i = 0; i < trueNewsArray.getSize(); i++) {
        News& article = trueNewsArray[i];

        int formattedDate = trueNewsArray.convertDate(article.publicationDate);
        int year = formattedDate / 10000;  
        int month = (formattedDate / 100) % 100 - 1; 

        // Debug
        debugFile << "Article " << i + 1 << " | Date: " << article.publicationDate
                  << " | Converted: " << formattedDate
                  << " | Year: " << year
                  << " | Month: " << month + 1
                  << " | Category: " << article.category << "\n";

        if (year == 2016 && month >= 0 && month < 12) {
            totalNewsPerMonth[month]++;  // Add true news count to total
            debugFile << "  -> Fake political news detected in " << month + 1 << "\n"; // Debug
        }
    }

    // Output results to terminal
    cout << "Percentage of Fake Political News Articles in 2016\n";
    cout << "---------------------------------------------------\n";
    string months[] = {"January", "February", "March", "April", "May", "June",
                       "July", "August", "September", "October", "November", "December"};

    for (int i = 0; i < 12; i++) {
        if (totalNewsPerMonth[i] > 0) {
            double percentage = ceil((fakePoliticalCount[i] * 100.0) / totalNewsPerMonth[i]);
            cout << months[i] << ": " << percentage << "% fake political news (" 
                 << fakePoliticalCount[i] << "/" << totalNewsPerMonth[i] << ")\n";
        } else {
            cout << months[i] << ": No articles available\n";
        }
    }
}

int main()
{
    NewsArray fakeNews, trueNews;
    WordFreqArray wordFreqArray;
    
    readCsv("cleaned_fake.csv", fakeNews);
    readCsv("cleaned_true.csv", trueNews);

    //prompt user to enter the option
    int option = 0;
    
    // Loop through the options
    while (true) 
    {
        cout << "Enter your option: " << endl
            << "1. Sort and display the articles of both Fake News and True News by their publication date" << endl
            << "2. Display total number of articles in both datasets" << endl
            << "3. Find most frenquent words in the fake news articles" << endl
            << "4. Search for articles" << endl
            << "5. Search for the percentage of fake political news articles for each month in the year 2016" << endl
            << "6. Exit program" << endl << endl;
        
        cout << "Enter your option: ";
        cin >> option;

        if (option == 1)
        {
            cout << "Saving sorted articles to text file....\n" << endl;
            //sort the fake news by date
            fakeNews.sortByDate();
            fakeNews.printResult("sorted_fakeNews.txt");

            //sort the true news by date
            trueNews.sortByDate();
            trueNews.printResult("sorted_trueNews.txt");
        }
        else if (option == 2)
        {
            cout << "Total numbers of fake news: " << fakeNews.getSize() << endl;
            cout << "Total numbers of true news: " << trueNews.getSize() << endl;
            
        }
        else if (option == 3)
        {
            cout << "Saving most frequent words to text file...." << endl;
            processGovernmentNews(fakeNews, wordFreqArray);
            wordFreqArray.sortByFrequency();;
            wordFreqArray.printFreqWords();
        }
        else if (option == 4)
        {
            string searchType;
            cout << "Enter the search type (year/category/title/content): ";
            cin >> searchType;

            // Ensure sorting before searching
            fakeNews.sortByDate();
            trueNews.sortByDate();
            
            if (searchType == "year") {
                string targetYear;
                cout << "Enter publication year: ";
                cin >> targetYear;
                fakeNews.searchArticlesByYear(trueNews, targetYear);
                trueNews.searchArticlesByYear(fakeNews, targetYear);
            } 
            else if (searchType == "category") {
                string targetCategory;
                cout << "Enter category: ";
                cin >> targetCategory;
                fakeNews.searchArticlesByCategory(trueNews, targetCategory);
                trueNews.searchArticlesByCategory(fakeNews, targetCategory);
            } 
            else if (searchType == "title") {
                string searchQuery;
                cout << "Enter search query: ";
                cin.ignore(); // Ignore newline character
                getline(cin, searchQuery);
                fakeNews.searchArticlesByTitle(trueNews, searchQuery);
                trueNews.searchArticlesByTitle(fakeNews, searchQuery);
            }
            else if (searchType == "content") {
                string searchQuery;
                cout << "Enter search query: ";
                cin.ignore(); // Ignore newline character
                getline(cin, searchQuery);
                fakeNews.searchArticlesByContent(trueNews, searchQuery);
                trueNews.searchArticlesByContent(fakeNews, searchQuery);
            }
            else {
                cout << "Invalid search type!" << endl;
            }
        }
        else if (option == 5)
        {
            analyzeFakePoliticalNews(fakeNews, trueNews);
        }
        else if (option == 6)
        {
            // Exit the program
            cout << "Exiting program... Goodbye!" << endl;
            break; // Exit the while loop
        }
        else
        {
            cout << "Invalid option! Please try again." << endl;
        }

        cout << "\nReturning to the main menu...\n" << endl;
    }
}
