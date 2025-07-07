#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../Metadata/metadata.h"
#include "../Utilities/utilities.h"

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

    BookId = NewTableColumn(0, "book_id", true, INTEGER, false, true, NULL);
    BookTitle = NewTableColumn(1, "title", false, STRING, false, false, NULL);
    BookISBN = NewTableColumn(2, "isdn", false, STRING, false, false, NULL);
    BookAuthor = NewTableColumn(3, "book_author", false, STRING, false, false, NULL);
    BookISBN = NewTableColumn(4, "isdn", false, STRING, true, false, NULL);
    BookGenre = NewTableColumn(5, "genre", false, STRING, false, false, NULL);
    BookPublicationYear = NewTableColumn(6, "publication_year", false, DATE, false, false, NULL);
    BookPrice = NewTableColumn(7, "price", false, INTEGER, false, false, NULL);
    BookStockQuality = NewTableColumn(8, "stock_quality", false, INTEGER, false, false, "0");
    BookCreatedAt = NewTableColumn(9, "stock_quality", false, DATE, false, false, NowDefaultValue());
    BookRentalPricePerDay = NewTableColumn(10, "rental_price_per_day", false, INTEGER, false, false, NULL);

    if (!BookId ||
        !BookTitle ||
        !BookAuthor ||
        !BookISBN ||
        !BookGenre ||
        !BookPublicationYear ||
        !BookPrice ||
        !BookStockQuality ||
        !BookCreatedAt ||
        !BookRentalPricePerDay)
    {
        FreeTableDefinition(BookId);
        FreeTableDefinition(BookTitle);
        FreeTableDefinition(BookAuthor);
        FreeTableDefinition(BookISBN);
        FreeTableDefinition(BookGenre);
        FreeTableDefinition(BookPublicationYear);
        FreeTableDefinition(BookPrice);
        FreeTableDefinition(BookStockQuality);
        FreeTableDefinition(BookCreatedAt);
        FreeTableDefinition(BookRentalPricePerDay);
        fprintf(stderr, "(create-books-table-defintion-info) table column  creation failed\n");
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

    if (!table)
    {
        FreeTableDefinition(BookId);
        FreeTableDefinition(BookTitle);
        FreeTableDefinition(BookAuthor);
        FreeTableDefinition(BookISBN);
        FreeTableDefinition(BookGenre);
        FreeTableDefinition(BookPublicationYear);
        FreeTableDefinition(BookPrice);
        FreeTableDefinition(BookStockQuality);
        FreeTableDefinition(BookCreatedAt);
        FreeTableDefinition(BookRentalPricePerDay);
        fprintf(stderr, "(create-books-table-defintion-info) table creation failed\n");
    }

    return table;
}

/**
 * CREATE TABLE customers (
*     customer_id INT PRIMARY KEY AUTO_INCREMENT,
*     full_name VARCHAR(100) NOT NULL,
*     email VARCHAR(255) UNIQUE NOT NULL,
*     phone VARCHAR(20),
*     address TEXT,
*     city VARCHAR(100),
*     registration_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
 */
TableDefinition *CreateCustomersTableDefinition()
{
    TableDefinition *table;
    TableColDefinition
        *CustomerId,
        *CustomerFullName,
        *CustomerEmail,
        *CustomerPhone,
        *CustomerAddress,
        *CustomerCity,
        *CustomerRegistrationDate;

    CustomerId = NewTableColumn(0, "customer_id", true, INTEGER, false, true, NULL);
    CustomerFullName = NewTableColumn(1, "full_name", false, STRING, false, false, NULL);
    CustomerEmail = NewTableColumn(2, "email", false, STRING, true, true, NULL);
    CustomerPhone = NewTableColumn(3, "phone", false, STRING, false, true, NULL);
    CustomerAddress = NewTableColumn(4, "address", false, STRING, true, false, NULL);
    CustomerCity = NewTableColumn(5, "city", false, STRING, true, false, NULL);
    CustomerRegistrationDate = NewTableColumn(6, "registration_date", false, DATE, false, false, NowDefaultValue());

    if (!CustomerId ||
        !CustomerFullName ||
        !CustomerEmail ||
        !CustomerPhone ||
        !CustomerAddress ||
        !CustomerCity ||
        !CustomerRegistrationDate)
    {
        FreeTableDefinition(CustomerId);
        FreeTableDefinition(CustomerFullName);
        FreeTableDefinition(CustomerEmail);
        FreeTableDefinition(CustomerPhone);
        FreeTableDefinition(CustomerAddress);
        FreeTableDefinition(CustomerCity);
        FreeTableDefinition(CustomerRegistrationDate);
        fprintf(stderr, "(create-customers-table-defintion-info) table column creation failed\n");
        return NULL;
    }

    table = NewTableDefinition("customers",
                               1,
                               CustomerId->Id,
                               CustomerId,
                               CustomerFullName,
                               CustomerEmail,
                               CustomerPhone,
                               CustomerAddress,
                               CustomerCity,
                               CustomerRegistrationDate);

    if (!table)
    {
        FreeTableDefinition(CustomerId);
        FreeTableDefinition(CustomerFullName);
        FreeTableDefinition(CustomerEmail);
        FreeTableDefinition(CustomerPhone);
        FreeTableDefinition(CustomerAddress);
        FreeTableDefinition(CustomerCity);
        FreeTableDefinition(CustomerRegistrationDate);
        fprintf(stderr, "(create-customers-table-defintion-info) table creation failed\n");
        return NULL;
    }

    return table;
}

SchemaDefinition *CreateBookStoreSchemaDefinition()
{
    SchemaDefinition *def;
    TableDefinition *booksTable = CreateBooksTableDefintion(),
                    *customersTable = CreateCustomersTableDefinition();

    if (!booksTable || !customersTable)
    {
        fprintf(stderr, "(create-bookstore-schema-defintion-info) could not create tables\n");
        return NULL;
    }

    def = NewSchemaDefinition("bookstore-schema", booksTable, customersTable);
    if (!def)
    {
        FreeTableDefinition(booksTable);
        FreeTableDefinition(customersTable);
        fprintf(stderr, "(create-bookstore-schema-defintion-info) could not create schema\n");
        return NULL;
    }

    return def;
}
