#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Lang/language.h"

/**
 * SELECT title, author, genre, price, stock_quantity, rental_price_per_day
 * FROM books
 * ORDER BY title;
 */
Liqsmt *CreateMockBooksLiqsmt()
{
    const tableName = "books";
    const columns = {"title", "author", "genre", "price", "stock_quantity", "rental_price_per_day"};
    const order = {"title"};

    Liqsmt *smt = CreateLqsmt(tableName, columns, order, false);
    if (!smt)
    {
        fprintf(stderr, "(create-mock-books-liqsmt) Error creating language query statement\n");
        return NULL;
    }
    return smt;
}