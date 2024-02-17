# Rinha de Backend

Este pequeno projeto foi inspirado pelo desafio proposto na [Rinha de Backend 2024 Q1](https://github.com/zanfranceschi/rinha-de-backend-2024-q1).

Em resumo, o objetivo é implementar uma API simples, capaz de sobreviver a um teste de estresse usando pouquíssimos recursos.

## Tech Stack

- [Postgres](https://www.postgresql.org/) (banco de dados)
- [Nginx](https://www.nginx.com/) (balanceamento de carga)
- C++ / [Drogon](https://drogon.org/)

## Execução da Aplicação

Desenvolvemos nossa solução com gerenciamento e execução de contêineres realizado através do *podman* e do *podman-compose*.

```bash
$ podman-compose --version
podman-compose version: 1.0.6
['podman', '--version', '']
using podman version: 4.9.0
podman-compose version 1.0.6
podman --version 
podman version 4.9.0
exit code: 0
```

A aplicação completa (incluindo todos os componentes) pode ser iniciada da seguinte forma:

```bash
$ podman-compose -f docker-compose.yml up --build
```

Para terminar a execução:

```bash
$ podman-compose -f docker-compose.yml down
```
