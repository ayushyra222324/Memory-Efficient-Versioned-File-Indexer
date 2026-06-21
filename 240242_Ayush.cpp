#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <chrono>
#include <stdexcept>
#include <cctype>

using namespace std;

// TEMPLATE FUNCTION

template <typename T>
void printVector(const vector<T> &vec)
{
    for (const auto &x : vec)
        cout << x.first << " : " << x.second << endl;
}

// BUFFERED FILE READER CLASS

class BufferedFileReader
{
private:
    ifstream file;
    size_t bufferSize;
    vector<char> buffer;

public:
    BufferedFileReader(const string &path, size_t bufSize)
    {
        if (bufSize < 256 * 1024 || bufSize > 1024 * 1024)
            throw runtime_error("Buffer size must be between 256KB and 1024KB");

        bufferSize = bufSize;
        buffer.resize(bufferSize);

        file.open(path);

        if (!file)
            throw runtime_error("Cannot open file: " + path);
    }

    bool readChunk(string &chunk)
    {
        if (file.eof())
            return false;

        file.read(buffer.data(), bufferSize);
        size_t bytesRead = file.gcount();

        if (bytesRead == 0)
            return false;

        chunk.assign(buffer.begin(), buffer.begin() + bytesRead);

        return true;
    }
};

// TOKENIZER CLASS

class Tokenizer
{
private:
    string leftover;

    static bool isWordChar(char c)
    {
        return isalnum(static_cast<unsigned char>(c));
    }

    static string toLower(const string &s)
    {
        string result = s;
        transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

public:
    vector<string> tokenize(const string &chunk)
    {
        vector<string> words;

        string data = leftover + chunk;
        leftover.clear();

        string current;

        for (char c : data)
        {
            if (isWordChar(c))
            {
                current += tolower(c);
            }
            else
            {
                if (!current.empty())
                {
                    words.push_back(current);
                    current.clear();
                }
            }
        }

        leftover = current;

        return words;
    }

    void flush(vector<string> &words)
    {
        if (!leftover.empty())
        {
            words.push_back(leftover);
            leftover.clear();
        }
    }
};


// VERSION INDEX CLASS

class VersionIndex
{
private:
    unordered_map<string, int> freq;
    string versionName;

public:
    VersionIndex() {}

    VersionIndex(string name)
    {
        versionName = name;
    }

    void addWord(const string &word)
    {
        freq[word]++;
    }

    // FUNCTION OVERLOADING

    int getCount(const string &word)
    {
        string lower = word;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

        auto it = freq.find(lower);
        if (it == freq.end())
            return 0;

        return it->second;
    }

    int getCount(const string &word, bool caseSensitive)
    {
        if (caseSensitive)
            return getCount(word);

        string lower = word;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

        return getCount(lower);
    }

    vector<pair<string, int>> getTopK(int k)
    {
        priority_queue<pair<int, string>> pq;

        for (auto &p : freq)
        {
            pq.push({p.second, p.first});
        }

        vector<pair<string, int>> result;

        while (k-- && !pq.empty())
        {
            auto top = pq.top();
            pq.pop();

            result.push_back({top.second, top.first});
        }

        return result;
    }

    string getVersionName()
    {
        return versionName;
    }
};

// INDEX MANAGER CLASS

class IndexManager
{
private:
    unordered_map<string, VersionIndex> versions;

public:
    void createVersion(string version, string filepath, size_t bufferSize)
    {
        BufferedFileReader reader(filepath, bufferSize);
        Tokenizer tokenizer;

        VersionIndex index(version);

        string chunk;

        while (reader.readChunk(chunk))
        {
            vector<string> words = tokenizer.tokenize(chunk);

            for (const string &w : words)
                index.addWord(w);
        }

        vector<string> remaining;
        tokenizer.flush(remaining);

        for (const string &w : remaining)
            index.addWord(w);

        versions[version] = index;
    }

    VersionIndex &getVersion(string version)
    {
        if (versions.find(version) == versions.end())
            throw runtime_error("Version not found");

        return versions[version];
    }
};

// ABSTRACT QUERY PROCESSOR CLASS

class QueryProcessor
{
public:
    virtual void execute() = 0;

    virtual ~QueryProcessor() {}
};

// WORD QUERY CLASS

class WordQuery : public QueryProcessor
{
private:
    VersionIndex &index;
    string word;

public:
    WordQuery(VersionIndex &idx, string w)
        : index(idx), word(w) {}

    void execute() override
    {
        cout << "Version: " << index.getVersionName() << endl;
        cout << "Word: " << word << endl;
        cout << "Frequency: " << index.getCount(word) << endl;
    }
};

// DIFF QUERY CLASS

class DiffQuery : public QueryProcessor
{
private:
    VersionIndex &v1;
    VersionIndex &v2;
    string word;

public:
    DiffQuery(VersionIndex &a, VersionIndex &b, string w)
        : v1(a), v2(b), word(w) {}

    void execute() override
    {
        int c1 = v1.getCount(word);
        int c2 = v2.getCount(word);

        cout << "Version " << v1.getVersionName() << ": " << c1 << endl;
        cout << "Version " << v2.getVersionName() << ": " << c2 << endl;
        cout << "Difference: " << abs(c1 - c2) << endl;
    }
};

// TOP K QUERY CLASS

class TopKQuery : public QueryProcessor
{
private:
    VersionIndex &index;
    int k;

public:
    TopKQuery(VersionIndex &idx, int k)
        : index(idx), k(k) {}

    void execute() override
    {
        auto result = index.getTopK(k);

        cout << "Version: " << index.getVersionName() << endl;
        cout << "Top " << k << " words:" << endl;

        printVector(result);
    }
};

// COMMAND LINE PARSER


string getArg(int argc, char *argv[], string key)
{
    for (int i = 1; i < argc - 1; i++)
    {
        if (argv[i] == key)
            return argv[i + 1];
    }

    return "";
}

// MAIN FUNCTION

int main(int argc, char *argv[])
{
    try
    {
        auto start = chrono::high_resolution_clock::now();

        string file = getArg(argc, argv, "--file");
        string file1 = getArg(argc, argv, "--file1");
        string file2 = getArg(argc, argv, "--file2");

        string version = getArg(argc, argv, "--version");
        string version1 = getArg(argc, argv, "--version1");
        string version2 = getArg(argc, argv, "--version2");

        string query = getArg(argc, argv, "--query");
        string word = getArg(argc, argv, "--word");

        string bufferStr = getArg(argc, argv, "--buffer");
        size_t bufferSize = stoi(bufferStr) * 1024;

        string topStr = getArg(argc, argv, "--top");
        int topK = topStr.empty() ? 0 : stoi(topStr);

        IndexManager manager;

        QueryProcessor *processor = nullptr;

        if (query == "word")
        {
            manager.createVersion(version, file, bufferSize);

            processor = new WordQuery(
                manager.getVersion(version),
                word);
        }

        else if (query == "top")
        {
            manager.createVersion(version, file, bufferSize);

            processor = new TopKQuery(
                manager.getVersion(version),
                topK);
        }

        else if (query == "diff")
        {
            manager.createVersion(version1, file1, bufferSize);
            manager.createVersion(version2, file2, bufferSize);

            processor = new DiffQuery(
                manager.getVersion(version1),
                manager.getVersion(version2),
                word);
        }

        else
        {
            throw runtime_error("Invalid query type");
        }

        processor->execute();

        auto end = chrono::high_resolution_clock::now();

        chrono::duration<double> duration = end - start;

        cout << "Buffer size: " << bufferSize / 1024 << " KB" << endl;
        cout << "Execution time: " << duration.count() << " seconds" << endl;

        delete processor;
    }

    catch (exception &e)
    {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}