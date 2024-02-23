/*
 Criação das tabelas
*/
CREATE TABLE IF NOT EXISTS clientes (
  id SERIAL PRIMARY KEY,
  limite INTEGER NOT NULL,
  saldo INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS transacoes (
  id SERIAL PRIMARY KEY,  
  client_id INTEGER NOT NULL REFERENCES clientes(id),
  valor INTEGER NOT NULL CHECK (valor > 0),
  tipo VARCHAR(1) NOT NULL CHECK (tipo IN ('c', 'd')),
  descricao VARCHAR(10) NOT NULL,
  realizada_em TIMESTAMPTZ DEFAULT (NOW())
  --saldo_posterior INTEGER NOT NULL
);

CREATE INDEX transaction_client_index ON transacoes (client_id ASC);
CREATE INDEX transaction_date_index ON transacoes (realizada_em DESC);

/*
 TRIGGER para verificar se existe saldo suficiente para realizar a transação
*/
CREATE OR REPLACE FUNCTION processa_transacao()
RETURNS TRIGGER AS $$
BEGIN
  IF NEW.valor <= 0 THEN
    RAISE EXCEPTION 'Valor inválido.';
  END IF;
  IF NEW.tipo NOT IN ('c', 'd') THEN
    RAISE EXCEPTION 'Tipo de transação inválido.';
  END IF;  
  IF NEW.tipo = 'c' THEN
    UPDATE clientes
    SET saldo = saldo + NEW.valor
    WHERE id = NEW.client_id;
  ELSE 
    IF (SELECT limite + saldo - NEW.valor FROM clientes WHERE id = NEW.client_id) < 0 THEN
      RAISE EXCEPTION 'Saldo+Limite insuficientes.';
    END IF;
    UPDATE clientes
    SET saldo = saldo - NEW.valor
    WHERE id = NEW.client_id;
  END IF;  
  --NEW.saldo_posterior = (SELECT saldo FROM clientes WHERE id = NEW.client_id);
  RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER processa_transacao_trigger
BEFORE INSERT ON transacoes
FOR EACH ROW EXECUTE FUNCTION processa_transacao();

/*
 Inicializa a tabela de clientes (mandatório segundo o enunciado do desafio)
*/
DO $$
BEGIN
  INSERT INTO clientes (limite, saldo) VALUES (100000, 0);
  INSERT INTO clientes (limite, saldo) VALUES (80000, 0);
  INSERT INTO clientes (limite, saldo) VALUES (1000000, 0);
  INSERT INTO clientes (limite, saldo) VALUES (10000000, 0);
  INSERT INTO clientes (limite, saldo) VALUES (500000, 0);
END;
$$;
