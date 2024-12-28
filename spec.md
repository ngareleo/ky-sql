# ky-sql spec

We want to build a minimal database app. To make things as simple and yet functional as possible we will take inspiration from sqlite. The database will be file-based. We will also build a simple cli application that can execute basic sql statements.

## Todos

1. Create a shell application that will accept commands.
2. Implement table creation.
3. Implement sql statement execution.

### ky-sql shell application

This application will accept statements that will be executed on the specified `target` file

```shell
(ky-sql): SELECT * FROM shapes;
```

-   We need to build `target` file initialization commands that will start the session.

    ```
     kysql 'shapes.ksql'
    ```
