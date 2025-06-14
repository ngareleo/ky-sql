#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../Metadata/metadata_schema.h"
#include "../Metadata/metadata_offsets.h"
#include "../Metadata/metadata_extraction.h"

/**
 * # Table def for books-table
 *
 * ```sql
 *      CREATE TABLE books (
 *      book_id INT PRIMARY KEY AUTO_INCREMENT,
 *      title VARCHAR(255) NOT NULL,
 *      author VARCHAR(255) NOT NULL,
 *      isbn VARCHAR(13) UNIQUE,
 *      genre VARCHAR(100),
 *      publication_year INT,
 *      price DECIMAL(10, 2) NOT NULL,
 *      stock_quantity INT DEFAULT 0,
 *      rental_price_per_day DECIMAL(5, 2),
 *      created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
 *      );
 * ```
 */
TableDefinition *CreateBooksTableDefintion()
{
    time_t now;
    char *nowBuffer;
    TableDefinition *table;
    TableColDefinition
        *BookId,
        *BookTitle,
        *BookAuthor,
        *BookISBN,
        *BookGenre,
        *BookPublicationYear,
        *BookPrice,
        *BookStockQuality,
        *BookRentalPricePerDay,
        *BookCreatedAt;

    time(&now);
    if (now == (time_t)-1)
    {
        fprintf(stderr, "(create-books-table-defintion-info) time failed");
        return NULL;
    }

    nowBuffer = malloc(sizeof(time_t));
    if (!nowBuffer)
    {
        fprintf(stderr, "(create-books-table-defintion-info) malloc failed");
        return NULL;
    }

    sprintf(nowBuffer, "%ld", now);
    BookId = NewTableColumn(0, "book_id", true, INTEGER, false, true, NULL);
    BookTitle = NewTableColumn(1, "title", false, STRING, false, false, NULL);
    BookISBN = NewTableColumn(2, "isdn", false, STRING, false, false, NULL);
    BookAuthor = NewTableColumn(3, "book_author", false, STRING, false, false, NULL);
    BookISBN = NewTableColumn(4, "isdn", false, STRING, true, false, NULL);
    BookGenre = NewTableColumn(5, "genre", false, STRING, false, false, NULL);
    BookPublicationYear = NewTableColumn(6, "publication_year", false, DATE, false, false, NULL);
    BookPrice = NewTableColumn(7, "price", false, INTEGER, false, false, NULL);
    BookStockQuality = NewTableColumn(8, "stock_quality", false, INTEGER, false, false, "0");
    BookCreatedAt = NewTableColumn(9, "stock_quality", false, DATE, false, false, nowBuffer);
    BookRentalPricePerDay = NewTableColumn(10, "rental_price_per_day", false, INTEGER, false, false, NULL);

    if (!BookId |
        !BookTitle |
        !BookAuthor |
        !BookISBN |
        !BookGenre |
        !BookPublicationYear |
        !BookPrice |
        !BookStockQuality |
        !BookCreatedAt |
        !BookRentalPricePerDay)
    {
        fprintf(stderr, "(create-books-table-defintion-info) malloc failed");
        return NULL;
    }

    table = NewTableDefinition("books",
                               0,
                               BookId->Id,
                               BookId,
                               BookTitle,
                               BookAuthor,
                               BookCreatedAt,
                               BookPrice,
                               BookRentalPricePerDay,
                               BookStockQuality,
                               BookISBN);

    return table;
}