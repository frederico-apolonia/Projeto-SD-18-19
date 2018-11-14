## Fase 1
1. Executar `source run.sh`
2. Dentro do run.sh estão várias funções que permitem executar o projeto de uma forma mais rápida
    1. create_missing_dirs: cria as pastas binary e object (caso não existam)
    2. clean_and_compile: compila o data, entry, list e table
    3. create_all_tests: compila os testes fornecidos pelos professores
    4. run_all_tests_valgrind: compila e corre todos os testes com o valgrind
    5. run_test_*: corre cada teste individualmente (data, entry, list e table)

## Fase 2
O `get` poderá inserir "lixo" na tabela em `put`s posteriores do lado do servidor, mas do lado do cliente esse "lixo" nunca aparecerá