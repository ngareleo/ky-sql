# ky-sql spec

We want to build a minimal database app. To make things as simple and yet functional as possible we will take inspiration from `sqlite`. The database will be file-based. We will also build a simple cli application that can execute basic sql statements.

## Usage

You initialize the shell program using a target file to a `*.kysql` file. If the file does not exist, we setup a new file at the file path created.

```shell
kysql 'shapes.kysql'
```

The server will accept statements that will be executed on the specified `target` file.

```shell
(ky-sql): SELECT * FROM shapes;
```

## Database file structure

The database file will be split into 3 distinct sections that serve different application purposes.

### 1. Metadata section

The metadata will hold critical file specific information like the schema and storage information.

#### Table Definitions

Information about the schema like the column data. i.e. column names, constraints, and data type.

#### Storage Mappings

During runtime, after each write we will store byte offsets for each table here . Since we have a schema, we can search tables in a linear fashion. By implementing indexing and ordering, we can narrow down search to a specific byte location.

Generally, we will load metadata during boot to allow us to do near instant searches in some cases for small tables (we will provide benchmarks at the end).

### 2. Data

Block of data where data is stored. We perform searches against this section.

### 3. Immediate write buffer

Cost of appending to file is very low (searches should be near instant). For writes we will append data here during runtime. This includes deletions as well. _Why now use memory instead?_ We want to avoid memory cost at all costs. Instead, we will append data here and when certain conditions are met. e.g. the process is idle, the size of the block reaches a limit, or we hold too many records of a certain table, we will do synchronization with the `data block`.

To understand the motivation of all this, lets look at a simple insertion: To insert data of size `x`, we will use 2 constant sized buffers of lets say `25MB` _(A user can increase the buffer size using options)

1. We copy `25MB - x` bytes into a buffer `A`.
2. Write the payload `x` bytes.
3. Copy the the next `25MB` after the payload into buffer `B`.
4. Write the contents of buffer `A`.
5. Copy next `25MB` into buffer `A`.
6. Write the payload in buffer `B`.

We loop 3, 4, 5, and 6 until we have no more data to copy.

For extra performs, we can calculate all the memory locations we are going to be writing beforehand, which will allow us to queue all the mini operations ahead of time and perform this costly operation just once.

This means during queries we will need to check the **immediate write buffer** and reconcile data. We can however use in-memory flags to indicate to the algorithm if it needs to do a second search in this block. If say we didn't write any data that belongs in table B, we can skip the reconciliation step by checking flags.
