# ky-sql spec

We want to build a minimal database app. To make things as simple and yet functional as possible we will take inspiration from sqlite. The database will be file-based. We will also build a simple cli application that can execute basic sql statements.

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

The metadata will hold critical file specific information like the schema and storage information. Let's dig deeper.

#### Table Definitions

This is information about the schema, we will store information like the column information like names and data types. By calculating byte locations before hand, we can seek to a specific byte in the file to acquire data and hence reduce the cost of searches.

#### Storage Mappings

During runtime, after each write we will store byte offsets for each table in the rest of the file. Since we have a schema, we can search tables in a linear fashion. By implementing indexing and ordering, we can narrow down search to a specific byte location.

Generally, we will load metadata during boot, so since mappings are in memory during searches, we can get do almost immediate searches in some cases.

### 2. Data

This is the section that ordered data. We perform searches against this section.

### 3. Immediate write buffer

Cost of appending to file is very low (almost immediate). For writes we will append data here during runtime. This includes deletions as well. _Why now use memory instead?_ We want to avoid the cost all together and we don't lose much from this. (As far as we know). Instead of incurring memory cost, we will append data here and when the process is idle, we will do the synchronization.

To insert data of size `x`, we will use 2 constant sized buffers of lets say `25MB` _(A user can increase the buffer size using options)_ We will copy data in two phases.

During the _first phase_, we copy data into a buffer `A`. Then write data over the byte locations we just copied. In the second phase, we copy the next `25MB` bytes into buffer `B`, and overwrite the bytes we just copied using buffer A. The routine continues until we have nothing else to copy.

For extra performs, we can calculate all the memory locations we are going to be writing beforehand, which will allow us to queue all the mini operations ahead of time and perform this costly operation just once.

This means during queries we will need to check the immediate write buffer and reconcile data. We can however use in-memory flags to indicate to the algorithm if it needs to do a second search in this block. If say we didn't write any data for table B, we can skip the reconciliation step.
