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
        "To Kill a Mockingbird\0Harper Lee\09780061120084\0Fiction\01960\012.99\015\02.50",
        "1984\0George Orwell\09780451524935\0Dystopian Fiction\01949\013.99\020\02.00",
        "Pride and Prejudice\0Jane Austen\09780141439518\0Romance\01813\011.50\012\01.75",
        "The Great Gatsby\0F. Scott Fitzgerald\09780743273565\0Fiction\01925\014.99\018\02.25",
        "Harry Potter and the Sorcerer's Stone\0J.K. Rowling\09780439708180\0Fantasy\01997\016.99\025\03.00",
        "The Catcher in the Rye\0J.D.Salinger\09780316769174\0Fiction\01951\013.50\010\02.00",
        "Lord of the Flies\0William Golding\09780571056866\0Fiction\01954\012.25\014\01.50",
        "The Hobbit\0J.R.R.Tolkien\09780547928227\0Fantasy\01937\015.99\022\02.75",
        "Fahrenheit 451\0Ray Bradbury\09781451673319\0Science Fiction\01953\014.50\016\02.25",
        "Jane Eyre\0Charlotte Brontë\09780141441146\0Romance\01847\013.25\011\01.75",
        "The Art of War\0Sun Tzu\09781599869773\0Philosophy\0-500\09.99\08\01.25",
        "Sapiens\0Yuval Noah Harari\09780062316097\0Non-fiction\02011\018.99\020\02.50",
        "The Alchemist\0Paulo Coelho\09780062315007\0Fiction\01988\014.95\019\02.00",
        "Dune\0Frank Herbert\09780441172719\0Science Fiction\01965\017.99\013\03.00",
        "Gone Girl\0Gillian Flynn\09780307588364\0Thriller\02012\015.50\017\02.50",

        // ?? Add more here
        NULL,
    };
    char *columns = "title\0author\0isbn\0genre\0publication_year\0price\0stock_quantity\0rental_price_per_day";
    char *table = "books";

    // !! Count isn't well tested
    int rowCount = Count(&rows);
    LanguageInsertStatement *statement;

    if (!(statement = CreateLanguageInsertStatement(table, rowCount, columns, rows)))
    {
        return NULL;
    }

    return statement;
}