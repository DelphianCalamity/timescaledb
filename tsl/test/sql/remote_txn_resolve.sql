-- This file and its contents are licensed under the Timescale License.
-- Please see the included NOTICE for copyright information and
-- LICENSE-TIMESCALE for a copy of the license.

\c :TEST_DBNAME :ROLE_SUPERUSER

CREATE OR REPLACE FUNCTION create_records()
RETURNS VOID
AS :TSL_MODULE_PATHNAME, 'tsl_test_remote_txn_resolve_create_records'
LANGUAGE C;

CREATE OR REPLACE FUNCTION create_records_with_concurrent_heal()
RETURNS VOID
AS :TSL_MODULE_PATHNAME, 'tsl_test_remote_txn_resolve_create_records_with_concurrent_heal'
LANGUAGE C;

SELECT true FROM add_server('loopback', port=>current_setting('port')::integer);
SELECT true FROM add_server('loopback2', port=>current_setting('port')::integer);
SELECT true FROM add_server('loopback3', port=>current_setting('port')::integer);


create table table_modified_by_txns (
    describes text
);

--create records will create 3 records
--1) that is committed
--2) that is prepared but not committed
--3) that is prepared and rolled back
--Thus (1) will be seen right away, (2) will be seen after the heal, (3) will never be seen
SELECT create_records();

SELECT * FROM table_modified_by_txns;
SELECT count(*) FROM pg_prepared_xacts;
SELECT count(*) FROM _timescaledb_catalog.remote_txn;

SELECT _timescaledb_internal.remote_txn_heal_server((SELECT OID FROM pg_foreign_server WHERE srvname = 'loopback'));
SELECT _timescaledb_internal.remote_txn_heal_server((SELECT OID FROM pg_foreign_server WHERE srvname = 'loopback2'));
SELECT _timescaledb_internal.remote_txn_heal_server((SELECT OID FROM pg_foreign_server WHERE srvname = 'loopback3'));

SELECT * FROM table_modified_by_txns;
SELECT count(*) FROM pg_prepared_xacts;
SELECT count(*) FROM _timescaledb_catalog.remote_txn;

--insert one record where the heal function is run concurrently during different steps of the process
--this tests safety when, for example, the heal function is run while the frontend txn is still ongoing.
SELECT create_records_with_concurrent_heal();
SELECT * FROM table_modified_by_txns;
SELECT count(*) FROM pg_prepared_xacts;
SELECT count(*) FROM _timescaledb_catalog.remote_txn;

--test that it is safe to have non-ts prepared-txns with heal
BEGIN;
    INSERT INTO public.table_modified_by_txns VALUES ('non-ts-txn');
PREPARE TRANSACTION 'non-ts-txn';

SELECT _timescaledb_internal.remote_txn_heal_server((SELECT OID FROM pg_foreign_server WHERE srvname = 'loopback'));
SELECT _timescaledb_internal.remote_txn_heal_server((SELECT OID FROM pg_foreign_server WHERE srvname = 'loopback2'));
SELECT _timescaledb_internal.remote_txn_heal_server((SELECT OID FROM pg_foreign_server WHERE srvname = 'loopback3'));

COMMIT PREPARED 'non-ts-txn';
SELECT * FROM table_modified_by_txns;
SELECT count(*) FROM pg_prepared_xacts;
SELECT count(*) FROM _timescaledb_catalog.remote_txn;