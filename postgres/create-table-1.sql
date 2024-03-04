/*
 Criação das tabelas
*/

CREATE SEQUENCE client_sequence
    START 1
    INCREMENT 2;

CREATE TABLE IF NOT EXISTS clientes (
  id INT PRIMARY KEY DEFAULT nextval('client_sequence'),
  limite INTEGER NOT NULL,
  saldo INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS transacoes (
  id SERIAL PRIMARY KEY,  
  client_id INTEGER NOT NULL REFERENCES clientes(id),
  valor INTEGER NOT NULL,
  tipo VARCHAR(1) NOT NULL,
  descricao VARCHAR(10) NOT NULL,
  realizada_em TIMESTAMPTZ DEFAULT (NOW()),
  saldo_posterior INTEGER DEFAULT (0),
  limite_posterior INTEGER DEFAULT (0)
);

CREATE INDEX transaction_client_index ON transacoes (client_id ASC);
CREATE INDEX transaction_date_index ON transacoes (realizada_em DESC);

/*
 TRIGGER para verificar se existe saldo suficiente para realizar a transação
*/
CREATE OR REPLACE FUNCTION processa_transacao()
RETURNS TRIGGER AS $$
DECLARE
  saldo_posterior INTEGER;
  limite_posterior INTEGER;
BEGIN
  IF NEW.valor <= 0 THEN
    RAISE EXCEPTION 'Valor inválido.';
  END IF;
  IF NEW.tipo NOT IN ('c', 'd') THEN
    RAISE EXCEPTION 'Tipo de transação inválido.';
  END IF;  
  PERFORM pg_advisory_xact_lock(NEW.client_id);
  IF NEW.tipo = 'c' THEN
    UPDATE clientes
    SET saldo = saldo + NEW.valor
    WHERE id = NEW.client_id
    RETURNING saldo,limite INTO saldo_posterior, limite_posterior;
  ELSE 
    IF (SELECT limite + saldo - NEW.valor FROM clientes WHERE id = NEW.client_id) < 0 THEN
      RAISE EXCEPTION 'Saldo+Limite insuficientes.';
    END IF;
    UPDATE clientes
    SET saldo = saldo - NEW.valor
    WHERE id = NEW.client_id
    RETURNING saldo,limite INTO saldo_posterior, limite_posterior;
  END IF;  
  NEW.saldo_posterior = saldo_posterior;
  NEW.limite_posterior = limite_posterior;
  RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER processa_transacao_trigger
BEFORE INSERT ON transacoes
FOR EACH ROW EXECUTE FUNCTION processa_transacao();

