sudo -u postgres psql -e << EOF
\c n3250_367228;
CREATE SCHEMA n3250_367228_schema_lab5;
SET search_path = n3250_367228_schema_lab5;
CREATE EXTENSION IF NOT EXISTS pgcrypto;
\echo 1
CREATE TABLE hashes (
    id SERIAL PRIMARY KEY,
    sha1_hash TEXT NOT NULL,
    md5_hash TEXT NOT NULL
);

INSERT INTO hashes (sha1_hash, md5_hash)
VALUES (
    encode(digest('example data', 'sha1'), 'hex'),
    encode(digest('example data', 'md5'), 'hex')
);

SELECT
    sha1_hash,
    md5_hash,
    decode(sha1_hash, 'hex') AS sha1_bytes,
    decode(md5_hash, 'hex') AS md5_bytes
FROM hashes;

WITH original_hashes AS (
    SELECT
        encode(digest('example data', 'sha1'), 'hex') AS sha1_hash,
        encode(digest('example data', 'md5'), 'hex') AS md5_hash
)
SELECT
    CASE
        WHEN sha1_hash = (SELECT sha1_hash FROM hashes LIMIT 1) THEN 'SHA-1 match'
        ELSE 'SHA-1 mismatch'
    END AS sha1_check,
    CASE
        WHEN md5_hash = (SELECT md5_hash FROM hashes LIMIT 1) THEN 'MD5 match'
        ELSE 'MD5 mismatch'
    END AS md5_check
FROM original_hashes;

\echo 2
CREATE TABLE encrypted_data (
    id SERIAL PRIMARY KEY,
    encrypted_value BYTEA NOT NULL
);

INSERT INTO encrypted_data (encrypted_value)
VALUES (
    pgp_sym_encrypt('sensitive information', 'encryption_key')
);

SELECT
    id,
    pgp_sym_decrypt(encrypted_value, 'encryption_key') AS decrypted_value
FROM encrypted_data;

\quit
EOF