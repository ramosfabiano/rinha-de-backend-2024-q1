## Gerando os modelos

Os modelos aqui são automaticamente gerados a partir de uma database *live* usando o utilitário `drogon_ctl`.

1. Levante os serviço *postgres*.
   
```
$ podman-compose up postgres
```

2. Levante o container *drogon*, na mesma rede dos serviços.

```
$ podman run -v ./app:/app:z --network rinha-2024-q1 -it ramosfabiano/drogon:official-20240218 /bin/bash
```

3. Uma vez dentro do container, dispare o `drogon_ctl`:

 ```
[drogon]$ cd /app/src/
[drogon]$ drogon_ctl create model models
```

O diretório `app/src/models` precisa existir, e dentro dele o arquivo `model.json`, com informações de login no banco de dados e tabelas a serem modeladas.


## Referências:

https://github.com/drogonframework/drogon/wiki/ENG-08-3-DataBase-ORM