# Memory-Efficient Versioned File Indexer

## Setup Instructions

Follow the steps below to compile and run the program.

---

## Step 1: Create a Project Folder

Create a folder and place the following three files inside it:


project_folder/
│
├── 240242_Ayush.cpp
├── test_logs.txt
└── verbose_logs.txt


### File Description

- **240242_Ayush.cpp**  
  Contains the complete C++ source code for the program.

- **test_logs.txt**  
  A smaller dataset used for basic testing.

- **verbose_logs.txt**  
  A larger dataset used to test the program on larger log files.

---

## Step 2: Compile the Program

Open the **Git Bash / terminal** in the project folder and run the following command:


g++ -std=c++17 240242_Ayush.cpp -o analyzer


This command compiles the source code and generates an executable file named:


analyzer


---

## Step 3: Run the Program

After compilation, the program can be executed using the required command-line arguments.

Example:


./analyzer --file test_logs.txt --version v1 --buffer 512 --query word --word error


---

## Supported Queries

### 1. Word Frequency Query

Returns the frequency of a given word in a specific version.


./analyzer --file test_logs.txt --version v1 --buffer 512 --query word --word error


---

### 2. Top-K Frequent Words Query

Displays the top K most frequent words in a version.


./analyzer --file test_logs.txt --version v1 --buffer 512 --query top --top 10


---

### 3. Difference Query

Computes the difference in frequency of a word between two versions.


./analyzer --file1 test_logs.txt --version1 v1
--file2 verbose_logs.txt --version2 v2
--buffer 512 --query diff --word error


---

## Notes

- The buffer size must be between **256 KB and 1024 KB**.
- Word matching is **case-insensitive**.
- The program reads files using a **fixed-size buffer**, ensuring memory usage does not depend on file size.

---
