/*
 Partição 1: Inicialização da tabela de clientes
 */
DO $$
BEGIN
  INSERT INTO clientes (limite, saldo) VALUES (100000, 0);
  --INSERT INTO clientes (limite, saldo) VALUES (80000, 0);
  INSERT INTO clientes (limite, saldo) VALUES (1000000, 0);
  --INSERT INTO clientes (limite, saldo) VALUES (10000000, 0);
  INSERT INTO clientes (limite, saldo) VALUES (500000, 0);
END;
$$;
