sudo -u postgres psql << EOF
\c n3250_367228;
CREATE SCHEMA n3250_367228_schema_lab2;
SET search_path = n3250_367228_schema_lab2;
\echo 1
SET datestyle = 'DMY';
CREATE TABLE n3250_367228_schema_lab2.users_info
(id INTEGER PRIMARY KEY, username VARCHAR, date_of_birth DATE, team_id INTEGER, is_leader BOOLEAN);
INSERT INTO users_info (id, username, date_of_birth, team_id, is_leader) VALUES
(1, 'keker', '02/01/1999', 1, FALSE),
(2, 'who?', '01/02/2000', 2, TRUE),
(3, 'ivan', '23/07/2000', 1, TRUE),
(4, 'doter', '06/06/2001', 2, FALSE),
(5, 'tester', '01/01/1970', 3, FALSE),
(6, 'mrcool', '05/09/1999', 3, TRUE),
(7, 'aik', '20/06/2003', 4, TRUE),
(8, 'noname', '04/04/2004', 4, FALSE),
(9, 'abra', '01/01/2002', 5, TRUE),
(10, 'cadabra', '11/9/2001', 5, FALSE),
(11, 'hacker228', '01/04/2009', 228, TRUE),
(12, 'hacker228_2', '01/04/2009', 228, FALSE),
(13, 'neadmin', '01/04/1999', 3, FALSE);
SELECT * FROM users_info;
CREATE TABLE n3250_367228_schema_lab2.teams_info
(id INTEGER PRIMARY KEY, name VARCHAR, leader_id INTEGER);
INSERT INTO teams_info (id, name, leader_id) VALUES
(1, 'SomeShiftHappens', 3),
(2, 'tipo_komanda', 2),
(3, 'admins', 6),
(4, 'theboys', 7),
(5, 'losers', 9),
(228, 'GOD', 11);
SELECT * FROM n3250_367228_schema_lab2.teams_info;
\echo 2
UPDATE teams_info SET leader_id = 1 WHERE teams_info.id = 1;
UPDATE users_info SET is_leader = CASE WHEN id IN (SELECT leader_id FROM teams_info) THEN TRUE ELSE FALSE END;
SELECT * FROM users_info;
SELECT * FROM teams_info;
\echo 3
DELETE FROM teams_info WHERE id = 228;
DELETE FROM users_info USING teams_info WHERE team_id NOT IN (SELECT id FROM teams_info);
SELECT * FROM users_info;
SELECT * FROM teams_info;
\echo 4
SELECT id, username, is_leader FROM users_info;
\echo 5
SELECT id, username FROM users_info WHERE date_of_birth < '01.01.2000';
\echo 6
SELECT * FROM users_info CROSS JOIN teams_info;
\echo 7
SELECT users_info.* FROM users_info JOIN teams_info ON users_info.team_id = teams_info.id WHERE teams_info.name = 'admins';
\echo 8
SELECT SUM(LENGTH(username)) AS sum_name, ROUND(AVG(LENGTH(username)), 2) AS avg_name_len, MIN(date_of_birth) AS min_dob, MAX(date_of_birth) AS max_dob FROM users_info;
\echo 9
SELECT * FROM users_info ORDER by date_of_birth DESC;
\echo 10
SELECT * FROM users_info ORDER by date_of_birth ASC LIMIT 3;
\echo 11
SELECT teams_info.name AS team_name, COUNT(users_info.id) AS member_count FROM teams_info LEFT JOIN users_info ON teams_info.id = users_info.team_id GROUP BY teams_info.id, teams_info.name;
\echo 12
SELECT username FROM users_info WHERE id IN (SELECT leader_id FROM teams_info);
\quit
EOF