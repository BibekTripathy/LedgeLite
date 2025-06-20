CREATE TABLE IF NOT EXISTS transactions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    counterparty TEXT,
    amount TEXT,
    date TEXT,
    time TEXT,
    category TEXT,
    notes TEXT
);

INSERT INTO transactions (counterparty, amount, date, time, category, notes) VALUES
('Alice', '120.50', '2025-05-20', '09:15', 'Groceries', 'Weekly shopping at local market'),
('Bob', '66.99', '2025-06-09', '14:30', 'Transport', 'Monthly metro card recharge'),
('Charlie', '200.00', '2025-05-18', '17:45', 'Utilities', 'Electricity bill payment'),
('Diana', '45.75', '2025-05-17', '11:00', 'Entertainment', 'Cinema with friends'),
('Evan', '99.99', '2025-05-16', '08:20', 'Healthcare', 'Pharmacy purchase');
