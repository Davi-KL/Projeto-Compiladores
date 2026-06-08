#!/usr/bin/env bash
# Executa todos os testes e compara com as saídas esperadas em tests/expected/

LEXER="./bin/lexer"
TESTS_DIR="tests"
EXPECTED_DIR="tests/expected"
PASS=0
FAIL=0

run_test() {
    local nome="$1"
    local entrada="$TESTS_DIR/$nome.txt"
    local esperado="$EXPECTED_DIR/$nome.out"

    if [ ! -f "$esperado" ]; then
        echo "SKIP  $nome  (sem expected/$nome.out)"
        return
    fi

    saida=$("$LEXER" "$entrada" 2>/dev/null | tr -d '\r')
    if diff -q <(echo "$saida") <(tr -d '\r' < "$esperado") > /dev/null 2>&1; then
        echo "PASS  $nome"
        ((PASS++))
    else
        echo "FAIL  $nome"
        diff <(echo "$saida") <(tr -d '\r' < "$esperado")
        ((FAIL++))
    fi
}

for txt in "$TESTS_DIR"/*.txt; do
    nome=$(basename "$txt" .txt)
    run_test "$nome"
done

echo ""
echo "Resultado: $PASS passou(ram), $FAIL falhou(ram)."
[ "$FAIL" -eq 0 ]
