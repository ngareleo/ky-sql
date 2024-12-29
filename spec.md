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

Cost of appending to file is very low (searches should be near instant). As for writes, we will append data and return. This includes insertions and deletions. 

_Why not use memory?_ This is to avoid memory build up especially with large block sizes. Instead, we will append data here and when certain conditions are met. e.g. the process is idle, the size of the block reaches a limit, or we hold too many records of a certain table, we will do synchronization with the `data block`.

To understand the motivation of all this, lets look at a simple insertion: Assuming we are inserting data of size `X`, we need 2 constant sized buffers of lets say `25MB` (can be set to much higher size for performance tweaks to a certain extent. Depends of you). We will say the buffer has `Y` bytes

1. We copy `Y - X` bytes into a buffer `A` from where we are inserting the data.
2. Write the payload `X` bytes.
3. Copy the the next `Y` bytes past the payload into `B`.
4. Write the contents of `A`.
5. Copy next `Y` bytes into `A`.
6. Write the contents of `B`
7. We go to num 4 or quit when we have no more data to copy.

For extra performance, we can calculate all the byte locations we are going to be writing or deleting beforehand, which will allow us to queue all the mini operations ahead of time and perform this costly operation just once.

This also means that during queries we will need to check query the **immediate write buffer** and reconcile data we fetched from the `data block`. We even use in-memory flags to indicate if we need to do a search in this block. If say we didn't write any data that belongs in table B, we can skip the reconciliation step by checking flags.
