# KySQL Dev Spec

I'm trying to build a minimal DBMS that's supafast. Here are the traits we want

1. The database should be portable across systems CPU architectures but to keep things simple its scoped only to linux systems.
2. Reads and writes should be fast (close to instant).
3. The engine should have low resource consumption levels in terms of memory and CPU.Although I'm willing to tradeoff CPU for `io` speed but memory is still considered expensive.
4. Backward compatibility. Each version of the engine should handle operations on a database created on older versions.
5. Single session. This will keep things simple in terms of engine design and effort. If we see this thing come to life, I can remove this restriction.

## 1. Portability across systems

The database can be tracked across different CPU architectures.
It will be a binary that carries all the data and metadata required by engine during runtime.

## 2. Fast reads and writes.

I have a couple of strategies to ensure we achieve high read and write speeds.
For read speeds we will design the database this way:

- A database schema with static typing.
- Fixed sized rows.
- Table offsets and file metadata.
- Indexing.
- Caching (Later)

### a. Schema.

A schema informs the engine of the size of rows based on data types.
This is useful for jumping across the files since we know within a table's boundary by jumping `X` bytes, we are stepping through the table.
We can even step to a specific column since we know the byte allocation for each column in the schema.

### b. Fixed sized rows.

Fixed sized rows means that each entry per table occupies equal space in a file.
Even if a column is not filled-out, we reserve space in the file as this will make inserts quick as well.
If all rows occupy the same number of bytes, search and delete becomes easy to implement.

### c. Table offsets.

The file will have metadata for making operations simpler and one is table offsets.
Table offsets will be a mapping between tables and the file position to the first column of that table.
It allows us to start table operations faster by jumping to specific file position.
Once we find the table, fixed sized rows and the schema come in handy to allow us to locate data instantly during reads.

### d. Indexing.

Since the table offsets and schema will be in memory in the engine.
We can calculate an exact file position to read data.

### e. Caching

Typically, we will maintain an in-memory cache that will keep query-result entires.
So we don't need to do any IO incase multiple queries are made for
With those three strategies I'm confident we can get high read speeds.

For write speeds, we have one strategy

### Append only writes.

We will have an "immediate-write-buffer" which is a section at the end of the file where we will just append data.
This strategy will allow us to perform quick write operations but makes things harder in the engine.
Data in this section will need to be reconciled with data in the "data section".
The "data section" is our long term store for data and will be used to peform searches.
Data in the "immediate-write-buffer" will have metadata to help the engine know where to place data at the end.
This reconciliation will happen on certain conditions like

- When engine is idle.
- If its reaches a high-level mark.
- Scheduled based on time.
- When the engine closes.

We're risking data integrity by taking this course but we should be able to control this.
