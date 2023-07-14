------------------------------------------------------------------------------------
-- SEQUENCIA DE CRIAÇÃO PARA POSTGRESQL
--------------------------------------------------------
create role esdblogin LOGIN PASSWORD 'esdb1!2023'
   VALID UNTIL 'infinity';

-- Cloud
grant esdblogin to psql; -- Or root or postgres

create DATABASE energyspydb
  with OWNER = esdblogin
       ENCODING = 'UTF8'
       CONNECTION LIMIT = -1;

comment on DATABASE energyspydb
  is 'Banco de dados do sistema EnergySpy.';
