CREATE TABLE IF NOT EXISTS clientes (
  id SERIAL PRIMARY KEY,
  limite INT,
  saldo INT
);

CREATE TABLE transacoes (
  client_id INT,
  valor INT,
  tipo VARCHAR(1),
  descricao VARCHAR(10),
  data_realizacao TIMESTAMPTZ,
  CONSTRAINT client_fk FOREIGN KEY(client_id) REFERENCES clientes(id)
);