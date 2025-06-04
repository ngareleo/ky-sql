-- Bookstore Database Schema
-- ===========================

-- Create Books table
CREATE TABLE books (
    book_id INT PRIMARY KEY AUTO_INCREMENT,
    title VARCHAR(255) NOT NULL,
    author VARCHAR(255) NOT NULL,
    isbn VARCHAR(13) UNIQUE,
    genre VARCHAR(100),
    publication_year INT,
    price DECIMAL(10, 2) NOT NULL,
    stock_quantity INT DEFAULT 0,
    rental_price_per_day DECIMAL(5, 2),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create Customers table
CREATE TABLE customers (
    customer_id INT PRIMARY KEY AUTO_INCREMENT,
    first_name VARCHAR(100) NOT NULL,
    last_name VARCHAR(100) NOT NULL,
    email VARCHAR(255) UNIQUE NOT NULL,
    phone VARCHAR(20),
    address TEXT,
    city VARCHAR(100),
    registration_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create Transactions table (for book purchases)
CREATE TABLE transactions (
    transaction_id INT PRIMARY KEY AUTO_INCREMENT,
    customer_id INT NOT NULL,
    book_id INT NOT NULL,
    quantity INT NOT NULL DEFAULT 1,
    total_amount DECIMAL(10, 2) NOT NULL,
    transaction_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    payment_method ENUM('cash', 'card', 'online') DEFAULT 'cash',
    FOREIGN KEY (customer_id) REFERENCES customers(customer_id),
    FOREIGN KEY (book_id) REFERENCES books(book_id)
);

-- Create Rentals table (for book rentals)
CREATE TABLE rentals (
    rental_id INT PRIMARY KEY AUTO_INCREMENT,
    customer_id INT NOT NULL,
    book_id INT NOT NULL,
    rental_date DATE NOT NULL,
    due_date DATE NOT NULL,
    return_date DATE NULL,
    daily_rate DECIMAL(5, 2) NOT NULL,
    total_cost DECIMAL(10, 2),
    status ENUM('active', 'returned', 'overdue') DEFAULT 'active',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (customer_id) REFERENCES customers(customer_id),
    FOREIGN KEY (book_id) REFERENCES books(book_id)
);

-- ===========================
-- Sample Data Population
-- ===========================

-- Insert sample books
INSERT INTO books (title, author, isbn, genre, publication_year, price, stock_quantity, rental_price_per_day) VALUES
('To Kill a Mockingbird', 'Harper Lee', '9780061120084', 'Fiction', 1960, 12.99, 15, 2.50),
('1984', 'George Orwell', '9780451524935', 'Dystopian Fiction', 1949, 13.99, 20, 2.00),
('Pride and Prejudice', 'Jane Austen', '9780141439518', 'Romance', 1813, 11.50, 12, 1.75),
('The Great Gatsby', 'F. Scott Fitzgerald', '9780743273565', 'Fiction', 1925, 14.99, 18, 2.25),
("Harry Potter and the Sorcerer\'s Stone", 'J.K. Rowling', '9780439708180', 'Fantasy', 1997, 16.99, 25, 3.00),
('The Catcher in the Rye', 'J.D. Salinger', '9780316769174', 'Fiction', 1951, 13.50, 10, 2.00),
('Lord of the Flies', 'William Golding', '9780571056866', 'Fiction', 1954, 12.25, 14, 1.50),
('The Hobbit', 'J.R.R. Tolkien', '9780547928227', 'Fantasy', 1937, 15.99, 22, 2.75),
('Fahrenheit 451', 'Ray Bradbury', '9781451673319', 'Science Fiction', 1953, 14.50, 16, 2.25),
('Jane Eyre', 'Charlotte Brontë', '9780141441146', 'Romance', 1847, 13.25, 11, 1.75),
('The Art of War', 'Sun Tzu', '9781599869773', 'Philosophy', -500, 9.99, 8, 1.25),
('Sapiens', 'Yuval Noah Harari', '9780062316097', 'Non-fiction', 2011, 18.99, 20, 2.50),
('The Alchemist', 'Paulo Coelho', '9780062315007', 'Fiction', 1988, 14.95, 19, 2.00),
('Dune', 'Frank Herbert', '9780441172719', 'Science Fiction', 1965, 17.99, 13, 3.00),
('Gone Girl', 'Gillian Flynn', '9780307588364', 'Thriller', 2012, 15.50, 17, 2.50);

-- Insert sample customers
INSERT INTO customers (first_name, last_name, email, phone, address, city) VALUES
('John', 'Smith', 'john.smith@email.com', '555-0101', '123 Main Street', 'Springfield'),
('Emily', 'Johnson', 'emily.johnson@email.com', '555-0102', '456 Oak Avenue', 'Springfield'),
('Michael', 'Brown', 'michael.brown@email.com', '555-0103', '789 Pine Road', 'Riverside'),
('Sarah', 'Davis', 'sarah.davis@email.com', '555-0104', '321 Elm Street', 'Springfield'),
('David', 'Wilson', 'david.wilson@email.com', '555-0105', '654 Maple Lane', 'Hillview'),
('Lisa', 'Anderson', 'lisa.anderson@email.com', '555-0106', '987 Cedar Drive', 'Riverside'),
('James', 'Taylor', 'james.taylor@email.com', '555-0107', '147 Birch Avenue', 'Springfield'),
('Jennifer', 'Thomas', 'jennifer.thomas@email.com', '555-0108', '258 Walnut Street', 'Hillview'),
('Robert', 'Jackson', 'robert.jackson@email.com', '555-0109', '369 Cherry Lane', 'Springfield'),
('Amanda', 'White', 'amanda.white@email.com', '555-0110', '741 Spruce Road', 'Riverside');

-- Insert sample transactions (book purchases)
INSERT INTO transactions (customer_id, book_id, quantity, total_amount, transaction_date, payment_method) VALUES
(1, 1, 1, 12.99, '2024-05-15 10:30:00', 'card'),
(2, 5, 2, 33.98, '2024-05-16 14:45:00', 'cash'),
(3, 8, 1, 15.99, '2024-05-17 09:15:00', 'online'),
(1, 12, 1, 18.99, '2024-05-18 16:20:00', 'card'),
(4, 2, 1, 13.99, '2024-05-19 11:30:00', 'cash'),
(5, 4, 1, 14.99, '2024-05-20 13:45:00', 'card'),
(6, 7, 2, 24.50, '2024-05-21 15:10:00', 'online'),
(2, 13, 1, 14.95, '2024-05-22 12:00:00', 'cash'),
(7, 14, 1, 17.99, '2024-05-23 10:45:00', 'card'),
(8, 3, 1, 11.50, '2024-05-24 14:30:00', 'cash'),
(9, 9, 1, 14.50, '2024-05-25 16:15:00', 'online'),
(10, 6, 1, 13.50, '2024-05-26 11:20:00', 'card'),
(3, 15, 1, 15.50, '2024-05-27 13:30:00', 'cash'),
(4, 10, 1, 13.25, '2024-05-28 15:45:00', 'card'),
(5, 11, 2, 19.98, '2024-05-29 09:30:00', 'online');

-- Insert sample rentals
INSERT INTO rentals (customer_id, book_id, rental_date, due_date, return_date, daily_rate, total_cost, status) VALUES
(1, 2, '2024-05-10', '2024-05-17', '2024-05-16', 2.00, 12.00, 'returned'),
(2, 8, '2024-05-12', '2024-05-19', NULL, 2.75, NULL, 'active'),
(3, 5, '2024-05-14', '2024-05-21', '2024-05-20', 3.00, 18.00, 'returned'),
(4, 1, '2024-05-16', '2024-05-23', NULL, 2.50, NULL, 'active'),
(5, 14, '2024-05-18', '2024-05-25', NULL, 3.00, NULL, 'active'),
(6, 4, '2024-05-20', '2024-05-27', '2024-05-26', 2.25, 13.50, 'returned'),
(7, 12, '2024-05-22', '2024-05-29', NULL, 2.50, NULL, 'active'),
(8, 9, '2024-05-24', '2024-05-31', NULL, 2.25, NULL, 'overdue'),
(9, 7, '2024-05-26', '2024-06-02', NULL, 1.50, NULL, 'active'),
(10, 13, '2024-05-28', '2024-06-04', NULL, 2.00, NULL, 'active'),
(1, 15, '2024-05-30', '2024-06-06', NULL, 2.50, NULL, 'active'),
(2, 3, '2024-06-01', '2024-06-08', NULL, 1.75, NULL, 'active');

-- ===========================
-- Useful Sample Queries
-- ===========================

-- View all books with their current stock
SELECT title, author, genre, price, stock_quantity, rental_price_per_day
FROM books
ORDER BY title;

-- View all customers
SELECT customer_id, CONCAT(first_name, ' ', last_name) as full_name, email, city
FROM customers
ORDER BY last_name, first_name;

-- View recent transactions with customer and book details
SELECT 
    t.transaction_id,
    CONCAT(c.first_name, ' ', c.last_name) as customer_name,
    b.title,
    b.author,
    t.quantity,
    t.total_amount,
    t.transaction_date,
    t.payment_method
FROM transactions t
JOIN customers c ON t.customer_id = c.customer_id
JOIN books b ON t.book_id = b.book_id
ORDER BY t.transaction_date DESC;

-- View active rentals with overdue status
SELECT 
    r.rental_id,
    CONCAT(c.first_name, ' ', c.last_name) as customer_name,
    b.title,
    r.rental_date,
    r.due_date,
    r.daily_rate,
    CASE 
        WHEN r.due_date < CURDATE() AND r.status = 'active' THEN 'OVERDUE'
        ELSE r.status
    END as current_status,
    DATEDIFF(CURDATE(), r.due_date) as days_overdue
FROM rentals r
JOIN customers c ON r.customer_id = c.customer_id
JOIN books b ON r.book_id = b.book_id
WHERE r.status IN ('active', 'overdue')
ORDER BY r.due_date;

-- Monthly sales summary
SELECT 
    DATE_FORMAT(transaction_date, '%Y-%m') as month,
    COUNT(*) as total_transactions,
    SUM(total_amount) as total_revenue,
    AVG(total_amount) as average_transaction
FROM transactions
GROUP BY DATE_FORMAT(transaction_date, '%Y-%m')
ORDER BY month DESC;

-- Most popular books (by purchase and rental)
SELECT 
    b.title,
    b.author,
    COUNT(t.transaction_id) as purchases,
    COUNT(r.rental_id) as rentals,
    (COUNT(t.transaction_id) + COUNT(r.rental_id)) as total_activity
FROM books b
LEFT JOIN transactions t ON b.book_id = t.book_id
LEFT JOIN rentals r ON b.book_id = r.book_id
GROUP BY b.book_id, b.title, b.author
ORDER BY total_activity DESC
LIMIT 10;