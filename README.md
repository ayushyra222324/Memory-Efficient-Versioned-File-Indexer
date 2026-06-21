# Memory-Efficient Versioned File Indexer Using C++

## 1. Introduction / Problem Overview

The goal of this assignment is to design and implement a **memory-efficient file indexing system** in C++.

The system reads a large text file using a fixed-size buffer and builds a **word-level index** that stores the frequency of each word. The index allows users to perform analytical queries such as:

- Word frequency lookup
- Top-K most frequent words
- Frequency difference of a word between two file versions

The program is designed using **object-oriented programming principles** and demonstrates concepts such as inheritance, polymorphism, templates, and exception handling.

---

## 2. Design and Approach

The overall approach followed in this implementation consists of the following steps:

1. The program reads the input file incrementally using a **fixed-size buffer**.
2. The buffer contents are passed to a **tokenizer** which extracts valid words.
3. Words are converted to **lowercase** to ensure case-insensitive indexing.
4. Each extracted word is inserted into a **hash map** that maintains the frequency count.
5. Separate indexes are maintained for each **version of the file**.
6. Once the index is constructed, the program executes the requested query.

---

## 3. Description of Classes Used

The system is implemented using several classes, each with a well-defined responsibility.

### 3.1 BufferedFileReader

This class is responsible for reading the file using a **fixed-size buffer**.

#### Responsibilities

- Open the file safely
- Read data in chunks using a buffer
- Return chunks of data to the tokenizer

This ensures that the entire file is **never loaded into memory at once**, satisfying the memory efficiency requirement.

---

### 3.2 Tokenizer

The tokenizer processes chunks of text received from the buffered reader.

#### Responsibilities

- Extract words consisting of **alphanumeric characters**
- Convert words to **lowercase**
- Handle words that may be **split across buffer boundaries**

It also stores incomplete tokens temporarily and combines them with the next buffer.

---

### 3.3 VersionIndex

This class stores the **word frequency index for a specific version** of the file.

#### Responsibilities

- Store words and their frequencies using an `unordered_map`
- Provide functions to:
  - Add words to the index
  - Retrieve word counts
  - Compute the top-K frequent words

Function overloading is demonstrated through multiple `getCount()` functions.

---

### 3.4 IndexManager

This class manages **multiple file versions**.

#### Responsibilities

- Create indexes for different versions
- Store and retrieve version-specific indexes

This allows the system to support **difference queries between versions**.

---

### 3.5 QueryProcessor (Abstract Base Class)

`QueryProcessor` is an **abstract base class** used to implement polymorphism.

#### Responsibilities

- Define a virtual function `execute()` for executing queries

#### Derived Classes

- **WordQuery** – retrieves the frequency of a given word
- **DiffQuery** – computes the difference of a word’s frequency between two versions
- **TopKQuery** – displays the top-K most frequent words

Runtime polymorphism is demonstrated through dynamic dispatch of the `execute()` function.

---

## 4. Fixed-Size Buffer File Processing

The file is processed using a **fixed-size buffer**, whose size is specified via the command-line argument.

### Buffer Size Constraints

- Minimum: **256 KB**
- Maximum: **1024 KB**

### Processing Flow

1. The `BufferedFileReader` reads a chunk of data into the buffer.
2. The chunk is passed to the tokenizer.
3. The tokenizer extracts words and sends them to the index.
4. The process repeats until the entire file is processed.

Special care is taken to handle **tokens that span across buffer boundaries**, ensuring that words are not incorrectly split.

This approach guarantees that **memory usage remains independent of the file size**.

---

## 5. Query Execution

After building the index, the system supports the following queries:

### Word Count Query

Returns the frequency of a given word in a specified version.

### Difference Query

Compares two versions and returns the difference in the frequency of a word.

### Top-K Query

Displays the top K most frequent words in a version, sorted by frequency.

Execution time is measured using the C++ `chrono` library.

---

## 6. Use of C++ Features

This implementation demonstrates several important C++ concepts:

| Feature | Implementation |
|----------|---------------|
| Object-Oriented Design | Modular classes with clear responsibilities |
| Inheritance | Query classes derived from `QueryProcessor` |
| Runtime Polymorphism | Virtual `execute()` function |
| Templates | Generic function used for printing results |
| Function Overloading | Multiple versions of `getCount()` |
| Exception Handling | `try`, `catch`, and `throw` used for error handling |
| STL Containers | `unordered_map`, `vector`, `priority_queue` |

---

## 7. Assumptions and Observations

### Assumptions

- Words are defined as continuous sequences of alphanumeric characters.
- Word matching is case-insensitive.
- Memory usage grows only with the number of unique words, not file size.
- The input dataset is assumed to be a valid text file.

### Observations

- The use of `unordered_map` allows efficient word frequency lookup.
- Fixed-size buffer reading ensures scalability for large files.
- Object-oriented design makes the system modular and extensible.
