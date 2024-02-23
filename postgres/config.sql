ALTER SYSTEM SET listen_addresses = '*' ;
ALTER SYSTEM SET shared_buffers = '192MB' ;
ALTER SYSTEM SET max_connections = 300 ;
ALTER SYSTEM SET log_min_messages = FATAL ;
ALTER SYSTEM SET checkpoint_timeout = 600;
ALTER SYSTEM SET effective_io_concurrency = 2;
