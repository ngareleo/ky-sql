#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mocks.h"
#include "Lang/language.h"
#include "Utilities/utilities.h"

/**
 *
 * INSERT INTO books (title, author, isbn, genre, publication_year, price, stock_quantity, rental_price_per_day) VALUES
 * ('To Kill a Mockingbird', 'Harper Lee', '9780061120084', 'Fiction', 1960, 12.99, 15, 2.50),
 * ('1984', 'George Orwell', '9780451524935', 'Dystopian Fiction', 1949, 13.99, 20, 2.00),
 * ('Pride and Prejudice', 'Jane Austen', '9780141439518', 'Romance', 1813, 11.50, 12, 1.75),
 * ('The Great Gatsby', 'F. Scott Fitzgerald', '9780743273565', 'Fiction', 1925, 14.99, 18, 2.25),
 * ("Harry Potter and the Sorcerer\'s Stone", 'J.K. Rowling', '9780439708180', 'Fantasy', 1997, 16.99, 25, 3.00),
 * ('The Catcher in the Rye', 'J.D. Salinger', '9780316769174', 'Fiction', 1951, 13.50, 10, 2.00),
 * ('Lord of the Flies', 'William Golding', '9780571056866', 'Fiction', 1954, 12.25, 14, 1.50),
 * ('The Hobbit', 'J.R.R. Tolkien', '9780547928227', 'Fantasy', 1937, 15.99, 22, 2.75),
 * ('Fahrenheit 451', 'Ray Bradbury', '9781451673319', 'Science Fiction', 1953, 14.50, 16, 2.25),
 * ('Jane Eyre', 'Charlotte Brontë', '9780141441146', 'Romance', 1847, 13.25, 11, 1.75),
 * ('The Art of War', 'Sun Tzu', '9781599869773', 'Philosophy', -500, 9.99, 8, 1.25),
 * ('Sapiens', 'Yuval Noah Harari', '9780062316097', 'Non-fiction', 2011, 18.99, 20, 2.50),
 * ('The Alchemist', 'Paulo Coelho', '9780062315007', 'Fiction', 1988, 14.95, 19, 2.00),
 * ('Dune', 'Frank Herbert', '9780441172719', 'Science Fiction', 1965, 17.99, 13, 3.00),
 * ('Gone Girl', 'Gillian Flynn', '9780307588364', 'Thriller', 2012, 15.50, 17, 2.50);
 */
LanguageInsertStatement *MockBookTableInsertStatement()
{
    /**
     * The language layer will receive an insert statement, parse it and extract the payload
     * This is format is a placeholder to the one will actually implement
     */
    char **rows = {
        "To Kill a Mockingbird\rHarper Lee\r9780061120084\rFiction\r1960\r12.99\r15\r2.50",
        "1984\rGeorge Orwell\r9780451524935\rDystopian Fiction\r1949\r13.99\r20\r2.00",
        "Pride and Prejudice\rJane Austen\r9780141439518\rRomance\r1813\r11.50\r12\r1.75",
        "The Great Gatsby\rF. Scott Fitzgerald\r9780743273565\rFiction\r1925\r14.99\r18\r2.25",
        "Harry Potter and the Sorcerer's Stone\rJ.K. Rowling\r9780439708180\rFantasy\r1997\r16.99\r25\r3.00",
        "The Catcher in the Rye\rJ.D.Salinger\r9780316769174\rFiction\r1951\r13.50\r10\r2.00",
        "Lord of the Flies\rWilliam Golding\r9780571056866\rFiction\r1954\r12.25\r14\r1.50",
        "The Hobbit\rJ.R.R.Tolkien\r9780547928227\rFantasy\r1937\r15.99\r22\r2.75",
        "Fahrenheit 451\rRay Bradbury\r9781451673319\rScience Fiction\r1953\r14.50\r16\r2.25",
        "Jane Eyre\rCharlotte Brontë\r9780141441146\rRomance\r1847\r13.25\r11\r1.75",
        "The Art of War\rSun Tzu\r9781599869773\rPhilosophy\r-500\r9.99\r8\r1.25",
        "Sapiens\rYuval Noah Harari\r9780062316097\rNon-fiction\r2011\r18.99\r20\r2.50",
        "The Alchemist\rPaulo Coelho\r9780062315007\rFiction\r1988\r14.95\r19\r2.00",
        "Dune\rFrank Herbert\r9780441172719\rScience Fiction\r1965\r17.99\r13\r3.00",
        "Gone Girl\rGillian Flynn\r9780307588364\rThriller\r2012\r15.50\r17\r2.50",

        // ?? Add more here
        NULL,
    };
    char *columns = "title\rauthor\risbn\rgenre\rpublication_year\rprice\rstock_quantity\rrental_price_per_day";
    char *table = "books";
    int rowCount = Count((void **)&rows);
    LanguageInsertStatement *statement;

    if (!(statement = CreateLanguageInsertStatement(table, rowCount, columns, rows)))
    {
        fprintf(stderr, "(mock-insert-statements-err) failed to create insert statement \n");
        return NULL;
    }

    return statement;
}