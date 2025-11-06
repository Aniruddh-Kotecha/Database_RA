---
marp: true
theme: gaia
class: invert
backgroundColor: black
---

# Database_RA

_Relational Database Management System implementing core database components such as buffer management, query processing, and indexing._

### Important Features

- Relational Algebra Operators
- Integers Only
- No update operations
- No aggregate operations
- No nested queries
- No transaction management
- Single thread programming only

---

### Internals

- Buffer Manager
- Cursors
- Tables
- Executors

---

### Command Execution Flow

![](Fig/flow.png)

---

### Syntactic Parser

- Splits the query into query units

---

### Semantic Parser

- Makes sure your query makes semantic sense

---

### Executors

Every command(COMMAND) has a file in the executors directory, within that directory you'll find 3 functions

```
syntacticParseCOMMAND
semanticParseCOMMAND
executeCOMMAND
```

---

### Buffer Manager

- Load splits and stores the table into blocks. For this we utilise the Buffer Manager

- Buffer Manager follows a FIFO paradigm. Essentially a queue

---

### Cursors

A cursor is an object that acts like a pointer in a table. To read from a table, you need to declare a cursor.

![Cursor](Fig/cursor.png)

Run: `R <- SELECT a == 1 FROM A` with debugger

---

### Logger

Every function call is logged in file names "log"
### Commands

There are 2 kinds of commands in this database.

- Assignment statements
- Non-assignment statements

---

## Non Assignment Statements

Non-assginment statements do not create a new table (except load which just loads an existing table) in the process

- LOAD
- LIST
- PRINT
- RENAME
- EXPORT
- CLEAR
- QUIT

---

## Assignment Statements

All assignment statements lead to the creation of a new table.  
Every statement is of the form ```<new_table_name> <- <assignment_statement>```  
Naturally in all cases, <new_table_name> shouldn't already exist in the system

- CROSS
- PROJECTION
- SELECTION
- JOIN
- SORT

---

### LOAD

Syntax:
```
LOAD <table_name>
```
- To successfully load a table, there should be a csv file names <table_name>.csv consisiting of comma-seperated integers in the data folder
- None of the columns in the data file should have the same name
- every cell in the table should have a value

Run: `LOAD A`

---

### LIST TABLES

Syntax
```
LIST TABLES
```
This command lists all tables that have been loaded or created using assignment statements

Run: `LIST TABLES`
Run: `LOAD B`, `LIST TABLES`

---

### PRINT

Syntax
```
PRINT <table_name>
```

- Displays the first PRINT_COUNT (global variable) rows of the table. 
- Less number of rows can be printed if the table has only a few rows

Run: `PRINT B`

---

### RENAME

Syntax
```
RENAME <toColumnName> TO <fromColumnName> FROM <table_name>
```

- Naturally <table_name> should be a loaded table in the system and <fromColumnName> should be an exsiting column in the table
- <toColumnName> should not be another column in the table

Run: `RENAME b TO c FROM B`

---

### EXPORT

Syntax
```
EXPORT <table_name>
```

- All changes made and new tables created, exist only within the system and will be deleted once execution ends (temp file)
- To keep changes made (RENAME and new tables), you have to export the table (data)

Run: `EXPORT B`

---

### CLEAR

Syntax
```
CLEAR <table_name>
```
- Removes table from system
- The table has to have previously existed in the system to remove it
- If you want to keep any of the changes you've made to an old table or want to keep the new table, make sure to export !

Run: `CLEAR B`

---

### QUIT

Syntax
```
QUIT
```

- Clear all tables present in the system (**_WITHOUT EXPORTING THEM_**)  (temp file - empty)

Run: `QUIT`

---

### CROSS

Syntax
```
<new_table_name> <- CROSS <table_name1> <table_name2>
```

- Both the tables being crossed should exist in the system
- If there are columns with the same names in the two tables, the columns are indexed with the table name. If both tables being crossed are the same, table names are indexed with '1' and '2'

Run: `cross_AA <- CROSS A A`

`A(A, B) x A(A, B) -> cross_AA(A1_A, A1_B, A2_A, A2_B)`

---

### SELECTION

Syntax
```
<new_table_name> <- SELECT <condition> FROM <table_name>
```

Where <condition> is of either form
```
<first_column_name> <bin_op> <second_column_name>
<first_column_name> <bin_op> <int_literal>
```

Where <bin_op> can be any operator among {>, <, >=, <=, =>, =<, ==, !=}

---

- The selection command only takes one condition at a time

Run: `R <- SELECT a >= 1 FROM A`
`S <- SELECT a > b FROM A`

---

### PROJECTION

Syntax
```
<new_table_name> <- PROJECT <column1>(,<columnN>)* FROM <table_name>
```

- naturally all columns should be present in the original table

Run: `C <- PROJECT c FROM A`

---

## Matrix Operations

### Contraints on Matrix:

1. It's a Square Matrix of size (n x n)
2. N(Number of rows/ Number of columns) <= $10^4$
3. Matrix will be Integer matrix only


### Contraints on blocks:
1. For all in-place operations, you can use 2 extra temporary blocks
2. Max Block size  = 1KB


### Handle Matrices:
- For Handling the matrix we decided do divide main matrix into sqaure submatrix.
- Calculation Size of submatrix 

Given:
- **Max block size** = 1000 B
- **Size of int** = 4 B 

#### Formula:
$\lfloor\sqrt{\frac{\text{max\_block\_size}}{\text{sizeof(int)}}}\rfloor$

**Final result = MaxRowPerBlock**: `15`
**Max Sub Matrix Size** :`15x15`

**Number of pages in one row = blocksInOneRow** = $\frac{\text{matrix\_row\_size}}{\text{MaxRowPerBlock}}$

**Number of pages in one column = blocksInOneCol** = $\frac{\text{matrix\_col\_size}}{\text{MaxRowPerBlock}}$

- Above two aspects are virtually and use for accessing purpose. They are not physically exist.
- All pages are stores sequatially in secondary memory.

- This 15x15 square submatrix will store in one block of second memory (which is `temp` folder inside data folder)
- Naming convention will be `MATRIX_NAME_PAGE_{page_index}`

#### Example:-
- Main matrix is 5x5 and max block size is 16B so max submatrix size is 2x2. Internal Matrix Handeling done as per below:

<br>

![Example of handling matrix](Fig\DS_Blocks.jpg)


<br>

## COMMANDS:

- For every command first query parser parse the quries.
- Then parsed qurey will go through synatatic then semantic parser for syntax and sematic check respectively.

<br>

### 1. SOURCE Command

- The `SOURCE` command is used to execute a series of queries stored in a file. It reads the specified file, processes each query sequentially, and executes them.

#### Syntax:
```sql
    SOURCE <filename>
```

#### Example:

```sql
    SOURCE Queries1
```

- `<filename>`: The name of the file containing the queries (without the `.ra` extension).
- This command will execute all queries stored in the file `Queries1.ra` located in the `/data` directory.


#### Behavior :
- The `.ra` extension is automatically appended to the filename.
- The file should contain valid queries like `LOAD`, `PRINT`, etc.
- Queries are executed in the order they appear in the file.

#### Implementation Notes :
- The system will look for the file inside the `/data` directory.
- The command should read the file line by line and execute each query.
- Error handling should be implemented to ensure invalid queries do not cause failures.


### 2. LOAD MATRIX

- Command to load matrices.
- Given a file ‘A.csv’ stored in the /data directory, this command load the contents of the csv file and store it as blocks in the /data/temp directory.
- Note: ‘.csv’ extension is to be appended to the matrix name in the code base itself and should not be given in the query.

#### Syntax :

```sql
    LOAD MATRIX <matrix name>
```

#### Example:
```sql
    LOAD MATRIX A
```

#### Flow of command

1. After syntax and semantic check `Blockify()` function will call for diving matrix into the submatrix and store in the secondary memory.
2. `Blockify()` function will read the csv line by line and fill one temp block (`size :  MaxRowPerBlock*MaxRowPerBlock`).
3. After filling the temp block `bufferManager` will write the temp block into secondary Memory.
4. then it will read next subsquent block and do same.
5. at time we read only 1 row of the csv file and use only 1 block of main memory.
6. for tracking position of last read in csv file, we use `tellg()` function of ofstream library.
7. After succesfull loading it will return true and count of row and columns else return false and error.


![Example of load command flow](Fig\Load.jpg)

<br>

### 3. PRINT Command

- Prints the matrix. If size n > 20, print only the first 20 rows and columns.

#### Syntax:

```sql
    PRINT MATRIX <matrix name>
```

#### Example:

```sql
    PRINT MATRIX A
```

#### Flow of the command:

1. For every row, we are acessing correposnding whole row of blocks by using `blocksInOneRow`.
2. we accessing the matrix blocks by block and store in one block of main memory.
2. we are storing one row, in one vector and limiting to first 20 elements. (which represent 20 columns).
3. same as above we are making first 20 rows by using that one vector and displaying on console.

- `Extra memory usage : 1 matrix block + 1 vector size of origial matrix row`
- `Block Access : Accessing every block by it's number of rows`

<br>

### 4. EXPORT MATRIX

- Exports matrix A in the environment and stores it as ‘A.csv’ in the /data directory.


#### Syntax:

```sql
    EXPORT MATRIX <matrix name>
```

#### Example:

```sql
    EXPORT MATRIX A
```

- It done by `makePermanent()` function.


#### Flow of the command:

1. We are checking whether file exist or not.
2. If it exist then we are deleting that file.
3. For every row, we are acessing correposnding whole row of blocks by using `blocksInOneRow`.
4. we accessing the matrix blocks by block and store in one block of main memory.
5. we are storing one row, in one vector.
6. After collecting whole row we write into one file.
7. Doing same for all rows

- `Extra memory usage : 1 matrix block + 1 vector size of origial matrix row`. 
- `Block Access : Accessing every block by it's number of rows`.


<br>


### 5. ROTATE

- Rotate the matrix clockwise 90 degree and stores inplace

#### Syntax

```sql
    ROTATE <matrix name>
```


#### Example

```sql
    ROTATE A
```


#### Flow of Command:

1. For Rotation we first do the transpose of the matrix
2. then we swap the columns of matrix 

#### Flow of transpose:
1. first we do the transpose of the submtrix internally.
2. Then we swap the blocks along the principle diagonal. while swapping the block we are actually swapping the content of two blocks by one temp block
3. `Count of extra temp blocks : 1`


<br>

![Example of transpose](Fig\Transpose.jpg)


<br>

#### Flow of swapping the columns:
1. for every  `i`<sup>th</sup> columns of the matrix will be `(column_count-i-1)`<sup>th</sup> column of the matrix
2. Totally inplace operation
3. `Count of extra temp blocks : 0`


<br>

![Example of transpose](Fig\Swap_Columns.jpg)

<br>

- `Count of extra temp blocks for whole Rotate Operation: max(0,1) = 1`



<br>

### 6. CROSS TRANSPOSE

-  two matrices, calculates their transpose individually, and swaps them in-place.

#### Syntax

```sql
    CROSSTRANSPOSE <matrix name1> <matrix name2>
```


#### Example

```sql
    CROSSTRANSPOSE A B
```


#### Flow of Command:

1. We take two matrix and calculating the transpose individually.
2. We swapping corresponding blocks of both matrix (A_page_0 will be swapped with B_Page_0).


#### Flow of swapping of blocks:

1. We consider the `merging logic of merge sort`.
2. Take `min_limit = min(block_count of A, block_count of B)`.
3. For `i = 0 to min_limit`, we directly swapping the blocks directly.
4. after if `i < block_count of A` then we creating the blocks for matrix B and erase the matrix A.
5. after if `i < block_count of B` then we creating the blocks for matrix A and erase the matrix B.
6. We are updating the whole metadata of both matrix.

<br>

- We are using 2 temp blocks for each matrix.
- We are using 1 temp blocks for swapping the matrix blocks in merging logic.
- `Count of extra temp blocks for whole cross transpose Operation: 1`


<br>

### 7. CHECK ANTI-SYMMETRY

- Takes two matrices and reports whether or not they are anti-symmetrical to each other.

- **Returns**: If A = $ -1 * B^T $, then `True`, else `False`.


#### Syntax

```sql
    CHECKANTISYM <matrix name1> <matrix name2>
```


#### Example

```sql
    CHECKANTISYM A B
```


#### Flow of Command:

1. We take two matrix and calculating the transpose of Second matrix and store inplace.
2. We are checking the dimension of both matrix if it's not equal then it return false.
3. If it's match the dimension then we are comparing the value of each blocks from both matrix as per formula
4. If anyone is not matched as per formula then it return false.
5. Else it returns true.
6. We do again transpose of the second matrix to keep it in it's original state.


<br>

- We are using 2 temp blocks for each matrix.
- We are using doing inplace checking.
- `Count of extra temp blocks for whole check Anti-Symmetry Operation: 0`



### Error Handeling:

1. Malformed the CSV whether CSV contain the only integer or not
2. Any row of the CSV has different size of other row of CSV.

<br>

### Summary of temp blocks

| Operation         | Number of temp Block |
|------------------|------------|
| Load Matrix       | 0       |
| Print Matrix      | 0       |
| Export Matrix     | 0       |
| Rotate           | 1       |
| Cross Transpose  | 1       |
| Anti Symmetry    | 0       |

- temp blocks is the extra block use apart of matrix block

---

## Query Operations Implementation

### 1. SORT  

**SYNTAX:**  

```sql
SORT <table-name> BY <col1>, <col2>, <col3> IN <ASC|DESC>, <ASC|DESC>, <ASC|DESC>
```

**Working:**  

- SORT is implementing through **K-way merge sort**.
- K = 9
- Algorithm is inplace.
- Perform syntactic and semantic checks on the query.  Checking if the columns are present in the table.

- 2 Passes :
- Sort Phase:
    - All pages of table is sorted internally first and write back in the original disk space only.
- Merge Phase
    - In merge phase merging degree of merging subfiles at a time. (Here 9).
    - Temporary storage is created by extending same table's pages.
    - At the end deleting these extra pages and store the final result in main table so that sorting will remain in place.

- **Number of Buffer Pages Used:** 10.  

**Assumptions**
- Not printing or exporting the result relation, run explicit command to do so.

---


### 2. ORDER BY  

**SYNTAX:**  

```sql

Result-table <- ORDER BY <attribute-name> ASC|DESC ON <table-name>
```

**Working:**  
- Perform syntactic and semantic checks on the query.  
- Copy the original relation to create the **resultant relation (Result-table)**.  
- While doign this copy also performing sort phase of sorting so that number pf page accesses can be reduced.
- Sort the **Result-table** in-place using a **k-way merge sort** (k=9).  
- Once the result table is created, with individual sorted pages, calling the merge phase method to sort the entire result relation.
- **Number of Buffer Pages Used:** 10.  

**Assumptions:**  
- When running multiple queries, different result table names must be used.  

---

### 3. GROUP BY  

**SYNTAX:**  

```sql
Result-table FROM <table> <- GROUP BY <attribute1> HAVING <Aggregate-Func1(attribute2)> <bin-op> <attribute-value> RETURN <Aggregate-Func2(attribute3)>
```

**Working:**  
- Perform syntactic and semantic checks on the query.  
- Copy the original relation to create an **intermediate relation**.  
- **Sort** the intermediate relation by the **GROUP BY attribute** in ascending order.  
- Create the **resultant relation** with attributes `<attribute1>` and `<attribute3>`.  
- Iterate through each row to compute aggregate functions on `<attribute2>` and `<attribute3>`.  
- Insert the computed aggregate values into the resultant table.  
- **Number of Buffer Pages Used:** 10.  

**Assumptions**
- Not printing or exporting the result relation, run explicit command to do so.


---

### 4. Hash JOIN  
Performs equi-join.

**SYNTAX:**  
```sql
Result-table <- JOIN <table1>, <table2> ON <col1>, <col2>
```
**Working:**  
- Perform syntactic and semantic checks on the query.  
- Use **partition-based hash join**:  
  - Create `(BLOCK_COUNT - 1)` **hash buckets** (Pages/Blocks) to store rows.  
  - Iterate over rows of both tables, compute a **hash** for the join column, and insert them into respective buckets.  
  - Keep **1 bucket reserved** for iterating through **table1** page by page.  
  - These partitons may get full, so storing relevent partition into sencondary storage by creating temporary pages.
  - After this bringing each mod value's bucket into the main memory.
  - To do so first fetch the (Block count - 1) pages of smaller partition table and one by one page of another one.
  - Basically in probing phase doing nested loop join for same hash value, and if it matches the equlity condition then adding it into result table.
  - To make it optimize keeping smaller pages of mod values table outer in the loop so that block accesses can be reduced.
  - Deleted those temporary pages.
  - Sort the resultant relation on joining attribute.

- **Total Buffer Blocks Used:** 10.

---

## Indexing

### Contraints on blocks:

1. Can store a maximum of 10 blocks worth of memory at any given time.
2. Max Block size = 1KB

### Index Structure: B+ Tree

For this phase, we have implemented B+ Tree indexing for the following reasons:

- **Balanced Structure:** Every INSERT/DELETE invokes local splits or merges (`splitLeaf`, `splitInternal`, `handleUnderflow`), keeping all leaf nodes at the same depth. This guarantees strict O(logN) performance.

- **Dense Secondary Index:** Each leaf entry directly associates a key with its `PageRowPair` (pageIndex, rowIndex), pointing to the exact record on disk. Bidirectional `next`/`prev` pointers between leaves allow both forward and backward sequential scans.

- **Efficient Range Queries:** Range operations (<, ≤, >, ≥) locate the starting leaf in O(log N), then traverse the leaf-level linked list, fetching only relevant blocks. This approach achieves O(log N + K) I/O cost for K results and excels at the project’s SEARCH command requirements.

- **Durability & Crash‑Resistance:** We write every index change to disk right away, employing persistent writes, so even if the program crashes or is terminated, the index stays intact.

Our B+ Tree implementation uses integer keys that correspond to the attribute values being indexed. Each leaf node stores:

- **Keys**: Integer values representing the attribute values.
- **Values**: PageRowPair objects where each entry contains:
  - **pageIndex**: Points to the specific page in secondary storage where the record is located.
  - **rowIndex**: Points to the specific row within that page.
- **Bidirectional links**: Pointers to the next and previous leaf nodes to facilitate efficient range queries.
- **Parent pointer:** A reference to the parent node to simplify tree traversal and maintenance operations.

#### The B+ tree implemetation

- The index is created dynamically when one of the commands (`SEARCH`, `INSERT`, `UPDATE`, `DELETE`) is executed.
- The index structure is maintained in secondary memory to ensure persistence across program executions.
- When an `INSERT`, `UPDATE` or `DELETE` command is executed -

  1. The operation is first performed on the data file
  2. The corresponding index modifications are processed in main memory
  3. The updated index nodes are immediately written back to secondary storage
  4. This immediate persistence strategy prevents index corruption in case of program termination

#### The implementation consists of two key structures:

1. `PageRowPair`: Contains page and row indices to directly locate records in storage.
2. `Node`: Represents tree nodes with appropriate fields for leaf and internal nodes

## COMMANDS:

- For every command first query parser parse the quries.
- Then parsed qurey will go through synatatic then semantic parser for syntax and sematic check respectively.
- On first use of any command that involves a column, a B+ Tree index is created dynamically by scanning the data file and inserting `(key, PageRowPair)` entries. Index nodes are written to secondary storage to ensure persistence.
- A `CompOp` enum is created for each of the binary operations

  ```c++
    enum class CompOp {
        EQ,  // ==
        LT,  // <
        GT,  // >
        LE,  // <=
        GE,  // >=
        NE   // !=
    };
  ```

<br>

---

### 1. SEARCH Command

- The `SEARCH` command utilizes our B+ Tree index to efficiently locate records matching the condition.

#### Syntax:

```sql
    <result_table> <- SEARCH FROM table_name WHERE condition
```

#### Example:

```sql
    Result <- SEARCH FROM Marks WHERE physics >= 30
```

#### Flow of Command

1. Check if the B+ Tree index on the specified column exists; if not, construct it.
2. Call `findLeaf(key)` to locate the relevant leaf node.
3. Depending on the binary operator, call the appropriate search variant:
   - `search(key)`, `searchLessThan`, `searchGreaterEqual`, etc.
4. Traverse the leaf level using `next` pointers for range queries.
5. Collect all matching `PageRowPair` values.
6. Use the buffer manager to load the relevant pages and extract matching rows.
7. Store and return the final result set.

---

### 2. INSERT Command

```sql
INSERT INTO table_name (col1 = val1, col2 = val2, …)
```

#### Example:

```sql
INSERT INTO Marks (chemistry = 50)
```

#### Flow of Command

1. Check if the B+ Tree index on the specified column exists; if not, construct it.
2. Locate a free space in existing data pages or allocate a new one.
3. Generate a `PageRowPair` for the inserted row.
4. For each indexed column:
   - Call `insert(key, PageRowPair)` on the corresponding B+ Tree.
   - If node overflows, invoke `splitLeaf()` or `splitInternal()` as needed.
5. All changes are written immediately to disk using `serializeNode()`.

---

### 3. UPDATE Command

```sql
UPDATE table_name WHERE col_cond bin_op value SET col_upd = new_value
```

#### Example:

```sql
UPDATE Marks WHERE maths >= 75 SET grade = 'B'
```

#### Flow of Command

1. Check if the B+ Tree index on the specified column exists; if not, construct it.
2. Use the B+ Tree on the condition column to find matching records.
3. For each matched `PageRowPair`:
   - Load the page via buffer manager, update the row.
   - If the updated column is indexed then we need to update the index file.
   - Remove old key from the B+ Tree.
   - Insert new key using `insert()`.
4. Updated pages and index nodes are immediately serialized to ensure persistence.
5. If no records match the condition, no updates are made.

---

### 4. DELETE Command

```sql
DELETE FROM table_name WHERE col_cond bin_op value
```

#### Example:

```sql
DELETE FROM Marks WHERE chemistry < 40
```

#### Flow of Command

1. Check if the B+ Tree index on the specified column exists; if not, construct it.
2. For each identified PageRowPair:
   - Load page using buffer manager and mark the record as deleted.
   - Write updated page back to disk.
3. For each indexed column:
   - Remove the key from the B+ Tree using `remove()`.
   - If underflow occurs, call `handleUnderflow()` to merge siblings.
4. All updated index nodes are serialized back to disk immediately.

### Conclusion

In summary, our implementation uses a persistent B+ Tree index to efficienty complete `SEARCH`, `INSERT`, `UPDATE`, and `DELETE`. By dynamically creating indexes, using a buffer manager to cap in-memory blocks at 10, and immediately persisting every modification to secondary storage, we ensure both efficiency and crash resistance. Our design provides scalable, reliable indexing for large datasets.
