.headers on
.mode columns
.width 30 20

DROP TABLE IF EXISTS identification;
DROP TABLE IF EXISTS rules;
DROP TABLE IF EXISTS log;
DROP TABLE IF EXISTS container;

CREATE TABLE identification (id VARCHAR(50), name VARCHAR(50), image VARCHAR(30));
CREATE TABLE rules (name VARCHAR(50), food VARCHAR(50), mintime INTEGER, amount REAL);
CREATE TABLE log (id VARCHAR(50), date_time VARCHAR(50));
CREATE TABLE container (side VARCHAR(50), pin INTEGER, food VARCHAR(50));

INSERT INTO identification VALUES ("ed9dd8e5", "Dan", "Dan.jpg");
INSERT INTO identification VALUES ("5bb7d8e5", "Ella", "Ella.jpg");
INSERT INTO identification VALUES ("f581d5e5", "Toonces", "Toonces.jpg");
INSERT INTO identification VALUES ("f9bdd4e5", "Hobbes", "Hobbes.jpg");
INSERT INTO identification VALUES ("eaf6d4e5", "Lexi", "Lexi.jpg");
INSERT INTO identification VALUES ("bad1d5e5", "Meli", "Meli.jpg");
INSERT INTO identification VALUES ("N/A Left", "Manual Dispense Left", "Dan.jpg");
INSERT INTO identification VALUES ("N/A Right", "Manual Dispense Right", "Dan.jpg");

INSERT INTO rules VALUES ("Meli", "Purina Naturals", 60, 0.5);
INSERT INTO rules VALUES ("Lexi", "Purina Naturals", 60, 0.5);
INSERT INTO rules VALUES ("Toonces", "Purina DH", 60, 0.6667);
INSERT INTO rules VALUES ("Ella", "Purina DH", 60, 0.6667);
INSERT INTO rules VALUES ("Hobbes", "Purina DH", 60, 0.6667);
INSERT INTO rules VALUES ("Dan", "Purina Naturals", 60, 0.5);

INSERT INTO log VALUES('ed9dd8e5', '0');
INSERT INTO log VALUES('5bb7d8e5', '0');
INSERT INTO log VALUES('f581d5e5', '0');
INSERT INTO log VALUES('f9bdd4e5', '0');
INSERT INTO log VALUES('eaf6d4e5', '0');
INSERT INTO log VALUES('bad1d5e5', '0');
INSERT INTO log VALUES('N/A Left', '0');
INSERT INTO log VALUES('N/A Right', '0');

INSERT INTO container VALUES("left", 18, "Purina DH");
INSERT INTO container VALUES("right", 24, "Purina Naturals");

/* INSERT INTO log VALUES('78b3c0020031c064b0f31000009000', strftime('%s','now'));

SELECT name,food FROM rules WHERE name IN
	(SELECT name FROM identification WHERE id="78b3c0020031c064b0f31000009000");
*/
