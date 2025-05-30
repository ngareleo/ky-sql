# KySQL Engine

Look at [spec.md]() for high level detail of what I'm trying to acheieve. Unfortunately I'm not proficient yet in C so I can't build this engine top down.
I'll have to break it down into smaller problems that I can hack at to keep myself motivated through this.
I hope you can tell how excited I am.

## File structure

At the start we have the engine metadata like:

1.  The `Database-Schema`. Schemas for each table.
2.  The `FP-Offsets`. Offsets to file positions for each table.

        a. Data Section (Table-Offsets)
        b. Immediate-Write-Buffer Offset (IM-WEB-Offset)

Below the metadata section is the `Data-Section`.
The data section has tables and is used for read and modify operations.
The tables are ordered according to the `Database-Schema`.

At the end of the file is the `Immediate-Write-Buffer` section.
We use this section for writes and deletions.

## The high level problem

Here are the problems we need to solve.

1. We should be able to store and retrive data from the file.
2. We should be able to execute SQL commands.

Broken down to smaller problems we get:

### Storage and Retreival

1.  Write metadata into a binary file and retrieve the metadata.
2.  Find a way to structure a schema, write it into a binary and read the schema.
3.  Write single table data into a binary file and read data from the binary.
4.  Add multi-table writing and reading.
5.  Implements Offsets in metadata.
6.  Allow search on the `data-section`.
7.  Allow insertions into the `data-section`.
8.  Implement `Immediate-Write-Buffer` and allow write operations.
9.  Allow deletions on the `Immediate-Write-Buffer`.
10. Implement reconciliation between `Immediate-Write-Buffer` and `Data-Section` based on certain factors.

        a. During engine shutdown.
        b. High level water mark. (Later: 1)
        c. Time-based scheduling. (Later: 2)
        d. When engine is idle. (Later: 3)

11. Allow reconciliation for operations between the `Data-Section` and the `Immediate-Write-Buffer`.
12. Implement caching of operations.
13. Implement cursor for pagination.

### SQL

1.  Model KySQL commands and build an interpreter.
2.  Build an interpreter for SQL.

        a. Interpret Create.
        b. Interpret Retrieve.
        c. Interpret Modifications.
        d. Interpret Deletions.
        e. Interpret schema definitions.

3.  Define a model for mapping SQL into actions.
4.  Map actions to engine execution actions.
