sudo -u postgres psql -e << EOF
\c n3250_367228;
CREATE SCHEMA n3250_367228_schema_lab4;
SET search_path = n3250_367228_schema_lab4;
\echo 1
CREATE TABLE n3250_367228_schema_lab4.employees (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    position VARCHAR(100),
    salary NUMERIC
);

CREATE TABLE n3250_367228_schema_lab4.departments (
    id SERIAL PRIMARY KEY,
    department_name VARCHAR(100) NOT NULL
);
\echo 2
CREATE USER user1 WITH PASSWORD 'password1';
CREATE USER user2 WITH PASSWORD 'password2';
CREATE USER user3 WITH PASSWORD 'password3';
CREATE USER user4 WITH PASSWORD 'password4';

GRANT ALL PRIVILEGES ON TABLE employees TO user1;
GRANT ALL PRIVILEGES ON TABLE departments TO user1;

GRANT INSERT, SELECT, UPDATE ON TABLE employees TO user2;
GRANT INSERT, SELECT, UPDATE ON TABLE departments TO user2;

GRANT DELETE ON TABLE employees TO user3;
GRANT DELETE ON TABLE departments TO user3;

GRANT DELETE ON TABLE employees TO user3 WITH GRANT OPTION;
GRANT DELETE ON TABLE departments TO user3 WITH GRANT OPTION;

\echo 3
GRANT DELETE ON TABLE employees TO user4;
GRANT DELETE ON TABLE departments TO user4;

SELECT grantee, privilege_type
FROM information_schema.role_table_grants
WHERE table_name = 'employees' AND grantee IN ('user3', 'user4');
\echo 4
REVOKE ALL PRIVILEGES ON TABLE employees FROM user1, user2, user3, user4;
REVOKE ALL PRIVILEGES ON TABLE departments FROM user1, user2, user3, user4;
\echo 5
CREATE SCHEMA schema_user1 AUTHORIZATION user1;
CREATE SCHEMA schema_user2 AUTHORIZATION user2;

CREATE TABLE schema_user1.employees (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    position VARCHAR(100),
    salary NUMERIC
);

CREATE TABLE schema_user2.departments (
    id SERIAL PRIMARY KEY,
    department_name VARCHAR(100) NOT NULL
);
\echo 6
CREATE VIEW combined_view AS
SELECT e.id, e.name, d.department_name
FROM schema_user1.employees e
JOIN schema_user2.departments d ON e.position = d.department_name;
\echo 7
ALTER TABLE schema_user1.employees ENABLE ROW LEVEL SECURITY;

CREATE POLICY employees_policy ON schema_user1.employees
USING (current_user = name);

ALTER TABLE schema_user1.employees FORCE ROW LEVEL SECURITY;
\echo 8
CREATE TABLE log_table (
    action VARCHAR(10),
    table_name VARCHAR(50),
    record_id INT,
    old_data JSONB,
    new_data JSONB,
    change_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE OR REPLACE FUNCTION log_changes()
RETURNS TRIGGER AS \$\$
BEGIN
    INSERT INTO log_table(action, table_name, record_id, old_data, new_data)
    VALUES (
        TG_OP,
        TG_TABLE_NAME,
        COALESCE(OLD.id, NEW.id),
        row_to_json(OLD),
        row_to_json(NEW)
    );
    RETURN NEW;
END;
\$\$ LANGUAGE plpgsql;

CREATE TRIGGER employees_changes
AFTER INSERT OR UPDATE OR DELETE ON schema_user1.employees
FOR EACH ROW
EXECUTE FUNCTION log_changes();

CREATE TRIGGER departments_changes
AFTER INSERT OR UPDATE OR DELETE ON schema_user2.departments
FOR EACH ROW
EXECUTE FUNCTION log_changes();

\quit
EOF