sudo -u postgres psql << EOF
\c n3250_367228;
CREATE SCHEMA n3250_367228_schema_lab3;
SET search_path = n3250_367228_schema_lab3;
CREATE TABLE n3250_367228_schema_lab3.boosters_info (id INTEGER PRIMARY KEY, name VARCHAR, count_of_orders INTEGER, rating REAL, balance INTEGER);
INSERT INTO boosters_info (id, name, count_of_orders, rating, balance) VALUES
(1, 'alice_smith', 12, 4.7, 0),
(2, 'bob_johnson', 25, 3.9, 30),
(3, 'carol_davis', 8, 4.5, 20),
(4, 'dave_wilson', 30, 4.2, 3),
(5, 'eve_martin', 15, 4.8, 39);
SELECT * FROM boosters_info;
CREATE TABLE n3250_367228_schema_lab3.service_list (id INTEGER PRIMARY KEY, name VARCHAR, cost INTEGER);
INSERT INTO service_list (id, name, cost) VALUES
(1, 'raid', 5),
(2, 'dungeon', 3),
(3, 'coaching', 10),
(4, 'pvp', 4),
(5, 'levelup', 30);
SELECT * FROM service_list;
CREATE TABLE n3250_367228_schema_lab3.orders_info (id INTEGER PRIMARY KEY, service_id INTEGER, booster_id INTEGER, ordered_date DATE, is_done BOOLEAN);
INSERT INTO orders_info (id, service_id, booster_id, ordered_date, is_done) VALUES
(1, 2, 2, '01/02/2000', FALSE),
(2, 1, 1, '02/02/2000', FALSE),
(3, 3, 1, '01/02/2000', TRUE),
(4, 4, 5, '03/02/2000', FALSE),
(5, 5, 1, '02/02/2000', FALSE),
(6, 2, 1, '01/02/2000', TRUE),
(7, 5, 3, '02/02/2000', FALSE),
(8, 5, 5, '04/02/2000', FALSE),
(9, 3, 1, '03/02/2000', FALSE),
(10, 3, 1, '02/02/2000', TRUE);
\echo 1
CREATE OR REPLACE PROCEDURE update_boosters_info()
LANGUAGE plpgsql
AS \$\$
BEGIN
    UPDATE boosters_info b
    SET count_of_orders = (
        SELECT COUNT(*)
        FROM orders_info o
        WHERE o.booster_id = b.id
    );
END;
\$\$;
\echo 2
CREATE OR REPLACE FUNCTION trigger_update_boosters_info()
RETURNS TRIGGER
LANGUAGE plpgsql
AS \$\$
BEGIN
    PERFORM update_boosters_info();
    RETURN NEW;
END;
\$\$;

CREATE TRIGGER update_boosters_info_trigger
AFTER INSERT ON orders_info
FOR EACH ROW
EXECUTE FUNCTION trigger_update_boosters_info();
\echo 3
CREATE OR REPLACE FUNCTION trigger_insert_by_date()
RETURNS TRIGGER
LANGUAGE plpgsql
AS \$\$
BEGIN
    IF NEW.ordered_date BETWEEN DATE '2000-01-01' AND DATE '2000-01-31' THEN
        INSERT INTO orders_info_january SELECT NEW.*;
    ELSIF NEW.ordered_date BETWEEN DATE '2000-02-01' AND DATE '2000-02-29' THEN
        INSERT INTO orders_info_february SELECT NEW.*;
    END IF;
    RETURN NULL; -- Prevent insertion into the original table
END;
\$\$;
CREATE TRIGGER insert_by_date_trigger
BEFORE INSERT ON orders_info
FOR EACH ROW
EXECUTE FUNCTION trigger_insert_by_date();
\echo 4
CREATE TABLE n3250_367228_schema_lab3.dictionary (
    old_value VARCHAR PRIMARY KEY,
    new_value VARCHAR
);

INSERT INTO dictionary (old_value, new_value) VALUES
('alice_smith', 'alice'),
('bob_johnson', 'bob');

CREATE OR REPLACE FUNCTION trigger_substitute_value()
RETURNS TRIGGER
LANGUAGE plpgsql
AS \$\$
BEGIN
    NEW.name := COALESCE((SELECT new_value FROM dictionary WHERE old_value = NEW.name), NEW.name);
    RETURN NEW;
END;
\$\$;

CREATE TRIGGER substitute_value_trigger
BEFORE INSERT OR UPDATE ON boosters_info
FOR EACH ROW
EXECUTE FUNCTION trigger_substitute_value();
\echo 5
CREATE OR REPLACE FUNCTION golden_sum() 
RETURNS VOID
LANGUAGE plpgsql
AS \$\$
DECLARE
    summ INTEGER := 0;
    first_value INTEGER;
    last_value INTEGER;
    fib_curr INTEGER := 1;
    fib_last INTEGER := 0;
    tmp INTEGER;
    total INTEGER;
BEGIN
    total := (SELECT COUNT(*) FROM boosters_info);
    first_value := (SELECT balance FROM boosters_info ORDER BY id ASC LIMIT 1);
    last_value := (SELECT balance FROM boosters_info ORDER BY id DESC LIMIT 1);
    summ := first_value + last_value;
    WHILE fib_curr <= total
    LOOP 
        summ := summ + (SELECT balance FROM boosters_info WHERE id = fib_curr);
        tmp := fib_curr;
        fib_curr := fib_curr + fib_last;
        fib_last := fib_curr;
    END LOOP;
    RAISE NOTICE 'SUM = %', summ;
END;
\$\$;
SELECT golden_sum();
\quit
EOF