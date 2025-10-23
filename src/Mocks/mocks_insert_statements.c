#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "mocks.h"
#include "Lang/language.h"

/**
 *```sql
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
 * ```
 */
Linsmt *MockBookTableInsertStatement()
{
    /**
     * The language layer will receive an insert statement, parse it and extract the payload
     * This is format is a placeholder to the one will actually implement
     */
    const char **r_1 = {"To Kill a Mockingbird", "Harper Lee", "9780061120084", "Fiction", "1960", "12.99", "15", "2.50"},
               **r_2 = {"George Orwell", "r9780451524935", "Dystopian Fiction", "1949", "13.99", "20", "2.00"},
               **r_3 = {"Pride and Prejudice", "Jane Austen", "9780141439518", "Romance", "1813", "11.50", "12", "1.75"},
               **r_4 = {"The Great Gatsby", "F. Scott Fitzgerald", "9780743273565", "Fiction", "1925", "14.99", "18", "2.25"},
               **r_5 = {"Harry Potter and the Sorcerer's Stone", "J.K. Rowling", "9780439708180", "Fantasy", "1997", "16.99", "25", "3.00"},
               **r_6 = {"The Catcher in the Rye", "J.D.Salinger", "9780316769174", "Fiction", "1951", "13.50", "10", "2.00"},
               **r_7 = {"Lord of the Flies", "William Golding", "9780571056866", "Fiction", "1954", "12.25", "14", "1.50"},
               **r_8 = {"The Hobbit", "J.R.R.Tolkien", "9780547928227", "Fantasy", "1937", "15.99", "22", "2.75"},
               **r_9 = {"Fahrenheit 451", "Ray Bradbury", "9781451673319", "Science Fiction", "1953", "14.50", "16", "2.25"},
               **r_10 = {"Jane Eyre", "Charlotte Brontë", "9780141441146", "Romance", "1847", "13.25", "11", "1.75"},
               **r_11 = {"The Art of War", "Sun Tzu", "9781599869773", "Philosophy", "-500", "9.99", "8", "1.25"},
               **r_12 = {"Sapiens", "Yuval Noah Harari", "9780062316097", "Non-fiction", "2011", "18.99", "20", "2.50"},
               **r_13 = {"The Alchemist", "Paulo Coelho", "9780062315007", "Fiction", "1988", "14.95", "19", "2.00"},
               **r_14 = {"Dune", " Frank Herbert ", " 9780441172719 ", " Science Fiction ", " 1965 ", " 17.99 ", " 13 ", " 3.00 "},
               **r_15 = {"Gone Girl", "Gillian Flynn", "9780307588364", "Thriller", "2012", "15.50", "17", "2.50"};

    const char ***values = {
        r_1,
        r_2,
        r_3,
        r_4,
        r_5,
        r_6,
        r_7,
        r_8,
        r_9,
        r_10,
        r_11,
        r_12,
        r_13,
        r_14,
        r_15,
        // ?? Add more here
        NULL,
    };
    char **columns = {"title", "author", "isbn", "genre", "publication_year", "price", "stock_quantity", "rental_price_per_day"};
    char *table = "books";

    Linsmt *stm;
    if (!(stm = CreateLinsmt(table, columns, values)))
    {
        fprintf(stderr, "(mock-insert-statements-err) failed to create insert statement \n");
        return NULL;
    }

    return stm;
}