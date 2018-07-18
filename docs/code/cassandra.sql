
drop keyspace dados;

CREATE KEYSPACE dados WITH replication = { 'class': 'SimpleStrategy', 'replication_factor': '1' };

CREATE TABLE IF NOT EXISTS dados.salas_por_nodeid ( 
  updated timestamp,
  sala text,
  id text,
  access_token text,
  PRIMARY KEY (id, access_token)
);

CREATE TABLE IF NOT EXISTS dados.uid_por_salas ( 
  updated timestamp,
  sala text,
  uid bigint,
  PRIMARY key (sala, uid)
);

CREATE TABLE IF NOT EXISTS dados.log_acesso ( 
  timestamp timestamp,
  uid bigint,
  status_code int,
  id text,
  sala text,
  PRIMARY key ((uid), timestamp)
)
WITH CLUSTERING ORDER BY (timestamp DESC);

CREATE TABLE IF NOT EXISTS dados.usuario_por_uid ( 
  updated timestamp,
  nome text,
  matricula text,
  uid bigint,
  PRIMARY key (matricula)
);

-- Insert values to test
insert into dados.salas_por_nodeid (id, access_token, sala, updated)
	values ('abcdef', 'aaaaaaaaaaaaaa', 'B017', toTimestamp(now()));
insert into dados.salas_por_nodeid (id, access_token, sala, updated)
	values ('abcdee', 'bbbbbbbbbbbbbb', 'B015', toTimestamp(now()));

select sala from dados.salas_por_nodeid where id = 'abcdef' and access_token = 'aaaaaaaaaaaaaa';

TRUNCATE TABLE dados.uid_por_salas;

insert into dados.uid_por_salas (sala, uid, updated)
	values ('B017', 898965209, toTimestamp(now()));
insert into dados.uid_por_salas (sala, uid, updated)
	values ('B017', 2286942157, toTimestamp(now()));
insert into dados.uid_por_salas (sala, uid, updated)
	values ('B017', 903156689, toTimestamp(now()));
insert into dados.uid_por_salas (sala, uid, updated)
	values ('B015', 2282748613, toTimestamp(now()));

select * from dados.uid_por_salas where sala='B017';


